#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <windows.h>
#include <gdiplus.h>

#include "sbwnd/sbwnd_proc.h"
#include "sbwnd/sbwnd_create.h"
#include "sbwnd/sbwnd_general.h"
#include "sbwnd/sbwnd_init.h"
#include "res/sbwnd_res.h"

// Checks a window has the current keyboard focus. If not, sends the message where it needs to go.
#define CHK_FOCUS_SEND_MSG(hwnd, msg, lParam, wParam)       if(GetFocus()!=hwnd){SendMessage(GetFocus(),msg,lParam,wParam);return 0;}(void)0
// Background color for string windows while the mouse is over the window
#define STRWND_BG_COLOR		(RGB(0xC0,0xC0,0xC0))
// Background color for string windows while the mouse button is down
#define STRWND_CLK_COLOR	(RGB(0xE0,0xE0,0xE0))

int sbWndCommonProc( HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam ) {
	switch ( msg ) {

		case WM_CREATE: {
			sbWnd *wnd = ( ( CREATESTRUCTW * )lParam )->lpCreateParams;
			SetWindowLongPtrW( hwnd, GWLP_USERDATA, ( LONG_PTR )wnd );
			break;
		}

		case WM_MOUSEMOVE: {
			sbWnd *wnd = ( void * )GetWindowLongPtrW( hwnd, GWLP_USERDATA );
			SbGUIMaster.currentCursorWnd = wnd;
			break;
		}

		case WM_MOUSEWHEEL: {
			sbWnd *wnd = ( void * )GetWindowLongPtrW( hwnd, GWLP_USERDATA );
			if ( SbGUIMaster.currentCursorWnd == wnd )
				return 0;
			PostMessageW( GetHWND( SbGUIMaster.currentCursorWnd ), msg, wParam, lParam );
			break;
		}

		case WM_SIZE:
			EnumChildWindows( hwnd, ChildSizingProc, 0 );
			break;

		case WM_CLOSE:
			DestroyWindow( hwnd );
			break;

		default:
			return DefWindowProcW( hwnd, msg, wParam, lParam );

	}
	return 0;
}

LRESULT CALLBACK BasicWndProc( HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam ) {
	switch( msg ) {

		case WM_LBUTTONDOWN:
			SetFocus( hwnd );
			break;

		case WM_ERASEBKGND: {
			HDC dc = ( void * )wParam;
			RECT r = { 0 };
			GetClientRect( hwnd, &r );
			FillRect( dc, &r, GetStockObject( WHITE_BRUSH ) );
			return 1;
		}

		case WM_PAINT: {
			PAINTSTRUCT ps = { 0 };
			HDC dc = BeginPaint( hwnd, &ps );
			( void )0; // Do nothing. BeginPaint() will remove the update region so no further WM_PAINT messages are sent.
			EndPaint( hwnd, &ps );
			break;
		}

	}
	return sbWndCommonProc( hwnd, msg, wParam, lParam );
}

LRESULT CALLBACK TextboxProc( HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam ) {
	switch( msg ) {

		case WM_LBUTTONDOWN:
			SetFocus( hwnd );
			break;

		case WM_SETFOCUS: {
			sbWnd *wnd = ( void * )GetWindowLongPtrW( hwnd, GWLP_USERDATA );
			SBTextboxInfo *info = GetSpecificHandle( wnd );

			RECT r = *info->tbRect;
			int hgt = r.bottom - r.top;

			CreateCaret( hwnd, NULL, 0, hgt );
			SetCaretPos( info->currentCaretPosition, 1 );
			ShowCaret( hwnd );

			break;
		}

		case WM_KILLFOCUS:
			DestroyCaret( );
			break;

		case WM_COMMAND: {
			sbWnd *sbw = ( void * )GetWindowLongPtrW( hwnd, GWLP_USERDATA );
			SBTextboxInfo *info = GetSpecificHandle( sbw );
			CHK_FOCUS_SEND_MSG( hwnd, msg, wParam, lParam );
			switch( LOWORD( wParam ) ) {
				case LARGE_BACK: {
					do
						SendMessage( hwnd, WM_CHAR, VK_BACK, 0 );
					while ( info->tiedownLength > 0 && info->tiedown[ info->tiedownLength - 1 ] != ' ');
					break;
				}
				case PASTE: {
					OpenClipboard( NULL );
					wString s = ( wString )GetClipboardData( CF_UNICODETEXT );
					for ( size_t i = 0; i < wcslen( s ); i++ )
						SendMessage( hwnd, WM_CHAR, s[i], 0 );
					CloseClipboard( );
					break;
				}
				case COPY: {
					/* TODO */
					break;
				}
			}
			break;
		}

		case WM_SIZE: {

			if ( GetFocus( ) == hwnd )
				DestroyCaret( );

			sbWnd *wnd = ( void * )GetWindowLongPtrW( hwnd, GWLP_USERDATA );
			SBTextboxInfo *info = GetSpecificHandle( wnd );
			UpdateTextboxInfo( wnd );

			if ( GetFocus( ) == hwnd ) {
				CreateCaret( hwnd, NULL, 0, info->currentFont->lfHeight );
				SetCaretPos( info->currentCaretPosition, 1 );
				ShowCaret( hwnd );
			}

			break;
		}

		case SBM_CHANGEFONT: {

			if ( GetFocus( ) == hwnd )
				HideCaret( hwnd );

			sbWnd *wnd = ( void * )GetWindowLongPtrW( hwnd, GWLP_USERDATA );
			SBTextboxInfo *info = GetSpecificHandle( wnd );

			int h = info->currentFont->lfHeight;
			memcpy( info->currentFont, SbGUIMaster.currentFont, sizeof( LOGFONTW ) );
			info->currentFont->lfHeight = h;

			InvalidateRect( hwnd, NULL, TRUE );
			UpdateWindow( hwnd );
			if ( GetFocus( ) == hwnd )
				ShowCaret( hwnd );

			break;
		}

		case WM_CHAR: {

			sbWnd *wnd = ( void * )GetWindowLongPtrW( hwnd, GWLP_USERDATA );
			SBTextboxInfo *info = GetSpecificHandle( wnd );

			if ( wParam == VK_RETURN && info->enterAction )
				break;

			HideCaret( hwnd );

			wString str = wcsdup( info->tiedown );
			uint8_t send = 0;

			switch( wParam ) {

				case VK_RETURN:

					ResetTextboxInfo( info );
					send = 1;

					break;

				case VK_BACK:

					if ( info->currentCaretPosition <= 2 )
						break;

					info->tiedown[ --info->tiedownLength ] = 0;

					break;

				default:

					if ( info->canDisplay )
						UpdateTiedown( info, ( wchar_t )wParam );

					break;

			}

			InvalidateRect( hwnd, NULL, TRUE );
			UpdateWindow( hwnd );
			
			SetCaretPos( info->currentCaretPosition, 1 );
			ShowCaret( hwnd );

			if ( send )
				SBWindows.signalFn( wnd, wnd, str );

			free( str );
			
			break;
		}

		case WM_ERASEBKGND: {
			HDC dc = ( void * )wParam;
			RECT r = { 0 };
			GetClientRect( hwnd, &r );
			FillRect( dc, &r, GetStockObject( WHITE_BRUSH ) );
			return 1;
		}

		case WM_PAINT: {

			PAINTSTRUCT ps = { 0 };
			HDC dc = BeginPaint( hwnd, &ps );

			// This is the text rendering machine.

			sbWnd *wnd = ( void * )GetWindowLongPtrW( hwnd, GWLP_USERDATA );
			SBTextboxInfo *info = GetSpecificHandle( wnd );
			wString str = SBTextboxes.getString( wnd );
			LOGFONTW *logfont = info->currentFont;

			HBRUSH prevBrush = GetCurrentObject( dc, OBJ_BRUSH );
			HFONT prevFont = GetCurrentObject( dc, OBJ_FONT );
			HBRUSH brush = GetStockObject( BLACK_BRUSH );
			HFONT font = CreateFontIndirectW( logfont );

			SelectObject( dc, brush );
			SelectObject( dc, font );

			SIZE strSz = { 0 };
			DWORD szs = GetTabbedTextExtentW( dc, str, ( int )wcslen( str ), 0, NULL );
			strSz.cy = HIWORD( szs );
			strSz.cx = LOWORD( szs );
			if ( !( ( strSz.cx + 1 ) >= info->tbRect->right ) )
				info->currentCaretPosition = 2 + strSz.cx;
			else if ( info->canDisplay != 0 ) {
				info->tiedown[ --info->tiedownLength ] = '\0';
				info->canDisplay = 0;
			}

			TabbedTextOutW( dc, 2, 1, str, ( int )wcslen( str ), 0, NULL, 2 );

			SelectObject( dc, prevBrush );
			SelectObject( dc, prevFont );
			DeleteObject( brush );
			DeleteObject( font );

			EndPaint( hwnd, &ps );

			break;
		}

	}
	return sbWndCommonProc( hwnd, msg, wParam, lParam );
}

LRESULT CALLBACK TextWndProc( HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam ) {
	switch( msg ) {

		case WM_ERASEBKGND: {
			HDC dc = ( void * )wParam;
			RECT r = { 0 };
			GetClientRect( hwnd, &r );
			FillRect( dc, &r, GetStockObject( WHITE_BRUSH ) );
			return 1;
		}

		case WM_PAINT: {
			
			PAINTSTRUCT ps = { 0 };
			HDC dc = BeginPaint( hwnd, &ps );

			sbWnd *wnd = ( void * )GetWindowLongPtrW( hwnd, GWLP_USERDATA );
			SBBasicTextWindowInfo *info = GetSpecificHandle( wnd );
			sbList *text = info->text;
			int h = info->fontSize;

			HBRUSH brush = GetStockObject( BLACK_BRUSH );
			HFONT font = CreateFontIndirectW( info->currentFont );

			HBRUSH prevBrush = SelectObject( dc, brush );
			HFONT prevFont = SelectObject( dc, font );

			RECT r = { 0 };
			GetClientRect( hwnd, &r );
			r.left++;
			r.right -= 11;
			r.bottom = LONG_MAX;

			wchar_t
				finArr[ 32768 ] = { 0 },
				*finArrPtr = finArr;
			int chLeft = 32767;

			for ( uint16_t i = 0; i < Lists.Size( text ); ++i ) {
				wString str = Lists.Get( text, i );
				int chprint = swprintf( finArrPtr, chLeft, L"%s\n", str );
				finArrPtr += chprint;
				chLeft -= chprint;
			}

			if ( finArrPtr != finArr && finArrPtr[-1] == '\n' )
				finArrPtr[-1] = 0;

			SetWindowOrgEx( dc, 0, info->currentOffset, NULL );
			int th = DrawTextExW( dc, finArr, -1, &r, DT_EXPANDTABS | DT_WORDBREAK, NULL );

			info->lineCount = th / info->fontSize;
			UpdateTextWndScrollbarInfo( info, NULL );

			SelectObject( dc, prevBrush );
			SelectObject( dc, prevFont );
			DeleteObject( brush );
			DeleteObject( font );

			EndPaint( hwnd, &ps );
			break;
		}

		case WM_MOUSEWHEEL: {
			int16_t rotate = HIWORD( wParam );
			int rot32 = ( int )rotate / WHEEL_DELTA; // eat my ass windows

			sbWnd *wnd = ( void * )GetWindowLongPtrW( hwnd, GWLP_USERDATA );
			SBBasicTextWindowInfo *info = GetSpecificHandle( wnd );

			if ( info->lineCount <= info->currentMaxLines )
				break;

			if ( rot32 < 0 ) {

				int cur = SBVScrollbarWindows.getCurrentPos( info->scrollbar );
				SBVScrollbarWindows.advance( info->scrollbar );

				if ( SBVScrollbarWindows.getCurrentPos( info->scrollbar ) != cur ) {
					info->currentOffset += info->fontSize * 3;
					InvalidateRect( hwnd, NULL, TRUE );
				}

			} else if ( rot32 > 0 ) {

				int cur = SBVScrollbarWindows.getCurrentPos( info->scrollbar );
				SBVScrollbarWindows.retreat( info->scrollbar );

				int cOff = info->currentOffset;
				cOff -= ( 3 * info->fontSize );
				cOff = cOff < 0 ? 0 : cOff;

				info->currentOffset = cOff;

				if ( SBVScrollbarWindows.getCurrentPos( info->scrollbar ) != cur )
					InvalidateRect( hwnd, NULL, TRUE );
			}
			
			break;
		}

		case WM_SIZE: {

			RECT r = { 0 };
			GetClientRect( hwnd, &r );

			sbWnd *wnd = ( void *)GetWindowLongPtrW( hwnd, GWLP_USERDATA );
			SBBasicTextWindowInfo *info = GetSpecificHandle( wnd );

			if ( info == NULL )
				break;

			info->currentMaxLines = r.bottom / info->fontSize;
			UpdateTextWndScrollbarInfo( info, &r );

			break;
		}

		case WM_LBUTTONDOWN: {
			sbWnd *wnd = ( void * )GetWindowLongPtrW( hwnd, GWLP_USERDATA );
			SBWindows.signalFn( wnd, wnd, NULL );
			SetFocus( hwnd );
			break;
		}
			
		case SBM_DRAWTEXT: {

			sbWnd *wnd = ( void * )GetWindowLongPtrW( hwnd, GWLP_USERDATA );
			SBBasicTextWindowInfo *info = GetSpecificHandle( wnd );
			Lists.Add( info->text, ( void * )wParam );

			InvalidateRect( hwnd, NULL, FALSE );

			break;
		}

		case SBM_CLEARTEXT: {

			sbWnd *wnd = ( void * )GetWindowLongPtrW( hwnd, GWLP_USERDATA );
			SBBasicTextWindowInfo *info = GetSpecificHandle( wnd );
			Lists.Delete( info->text );
			info->text = Lists.New( );

			SBVScrollbarWindows.reset( info->scrollbar );
			SBWindows.hide( info->scrollbar, 1 );
			info->currentOffset = 0;

			InvalidateRect( hwnd, NULL, TRUE );

			break;
		}

		case SBM_CHANGEFONT: {

			sbWnd *wnd = ( void * )GetWindowLongPtrW( hwnd, GWLP_USERDATA );
			SBBasicTextWindowInfo *info = GetSpecificHandle( wnd );

			memcpy( info->currentFont, SbGUIMaster.currentFont, sizeof( LOGFONTW ) );
			info->currentFont->lfHeight = info->fontSize;

			InvalidateRect( hwnd, NULL, TRUE );

			break;
		}

	}
	return sbWndCommonProc( hwnd, msg, wParam, lParam );
}

LRESULT CALLBACK ClickableProc( HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam ) {
	switch( msg ) {
		
		case WM_ERASEBKGND: {
			HDC dc = ( void * )wParam;
			RECT r = { 0 };
			GetClientRect( hwnd, &r );
			FillRect( dc, &r, GetStockObject( WHITE_BRUSH ) );
			return 1;
		}

		case WM_PAINT: {
			PAINTSTRUCT ps = { 0 };
			HDC dc = BeginPaint( hwnd, &ps );
			( void )0; // Do nothing
			EndPaint( hwnd, &ps );
			break;
		}

	}
	return sbWndCommonProc( hwnd, msg, wParam, lParam );
}

LRESULT CALLBACK RestrictedImageProc( HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam ) {
	switch( msg ) {
		
		case WM_ERASEBKGND: {
			HDC dc = ( void * )wParam;
			RECT r = { 0 };
			GetClientRect( hwnd, &r );
			FillRect( dc, &r, GetStockObject( WHITE_BRUSH ) );
			return 1;
		}

		case WM_PAINT: {

			RECT r = { 0 };
			GetClientRect( hwnd, &r );
			uint32_t ih = r.bottom - r.top;
			uint32_t iw = r.right - r.left;

			sbWnd *wnd = ( void * )GetWindowLongPtrW( hwnd, GWLP_USERDATA );
			SBRestrictedImageWindowInfo *info = GetSpecificHandle( wnd );

			if ( !wcscmp( L"none", info->path ) )
				break;
			
			GpGraphics *gph;
			PAINTSTRUCT ps;

			HDC dc = BeginPaint( hwnd, &ps );

			GdipCreateFromHDC( dc, &gph );

			uint32_t w = iw > info->maxWidth ? info->maxWidth : iw;
			uint32_t h = ( uint32_t )( ( double )w / info->aspectRatio );

			if ( h > ih ) {
				h = ih;
				w = ( uint32_t )( ( double )h * info->aspectRatio );
			}

			int left = ( ( iw / 2 ) - ( w / 2 ) ) + r.left;
			int top = ( ( ih / 2 ) - ( h / 2 ) ) + r.top;
			GdipDrawImageRectI( gph, info->img, left, top, w, h );

			GdipDeleteGraphics( gph );
			EndPaint( hwnd, &ps );
			
			break;
		}

	}
	return sbWndCommonProc( hwnd, msg, wParam, lParam );
}

LRESULT CALLBACK MasterProc( HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam ) {
	switch( msg ) {

		case WM_LBUTTONDOWN:
			SetFocus( hwnd );
			break;

		case WM_ERASEBKGND: {
			HDC dc = ( void * )wParam;
			RECT r = { 0 };
			GetClientRect( hwnd, &r );
			FillRect( dc, &r, GetStockObject( WHITE_BRUSH ) );
			return 1;
		}

		case WM_PAINT: {
			PAINTSTRUCT ps = { 0 };
			HDC dc = BeginPaint( hwnd, &ps );
			( void )0; // Do nothing.
			EndPaint( hwnd, &ps );
			break;
		}

		case WM_COMMAND: {

			if ( HIWORD( wParam ) != 0 ) {
				CHK_FOCUS_SEND_MSG( hwnd, msg, wParam, lParam );
			}

			switch( LOWORD( wParam ) ) {

				case ITEM_FONTS: {

					CHOOSEFONTW chFont = { 0 };

					chFont.lStructSize = sizeof( CHOOSEFONT );
					chFont.lpLogFont = newptr( LOGFONTW );

					if ( ChooseFontW( &chFont ) == TRUE ) {
						memcpy( SbGUIMaster.currentFont, chFont.lpLogFont, sizeof( LOGFONTW ) );
						EnumChildWindows( hwnd, ChildFontChangeProc, 0 );
					}

					break;
				}
			}
			break;
		}

		case WM_DESTROY:
			PostQuitMessage( 0 ); // THERE CAN ONLY BE ONE--DO NOT MAKE MORE THAN ONE MASTER WINDOW, ELSE SUFFER.
			break;

	}
	return sbWndCommonProc( hwnd, msg, wParam, lParam );
}

LRESULT CALLBACK ViewcmdMasterProc( HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam ) {
	switch( msg ) {
		
		case WM_LBUTTONDOWN:
			SetFocus( hwnd );
			break;

		case WM_ERASEBKGND: {
			HDC dc = ( void * )wParam;
			RECT r = { 0 };
			GetClientRect( hwnd, &r );
			FillRect( dc, &r, GetStockObject( WHITE_BRUSH ) );
			return 1;
		}

		case WM_PAINT: {
			PAINTSTRUCT ps = { 0 };
			HDC dc = BeginPaint( hwnd, &ps );
			( void )0; // Do nothing
			EndPaint( hwnd, &ps );
			break;
		}

		/*case WM_CHAR: {
			sbWnd *wnd = ( void * )GetWindowLongPtrW( hwnd, GWLP_USERDATA );
			switch( wParam ) {
				case VK_LEFT:
					SBWindows.signalFn( wnd, wnd, ( void * )SBEVT_PREVPAGE );
					break;
				case VK_RIGHT:
					SBWindows.signalFn( wnd, wnd, ( void * )SBEVT_NEXTPAGE );
					break;
			}
			break;
		}*/

	}
	return sbWndCommonProc( hwnd, msg, wParam, lParam );
}

LRESULT CALLBACK StringProc( HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam ) {
	switch( msg ) {
		
		case WM_ERASEBKGND: {

			sbWnd *wnd = ( void * )GetWindowLongPtrW( hwnd, GWLP_USERDATA );
			SBStringWindowInfo *info = GetSpecificHandle( wnd );
			if ( info == NULL )
				return 1;
			
			int state = info->mouseState;

			HDC dc = ( void * )wParam;
			RECT r = { 0 };
			GetClientRect( hwnd, &r );

			COLORREF col = RGB( 0xFF, 0xFF, 0xFF );
			if ( info->clickable && state & 0x01 ) // in-area bit
				col = STRWND_BG_COLOR;
			if ( info->clickable && state & 0x10 ) // click bit
				col = STRWND_CLK_COLOR;

			HBRUSH br = CreateSolidBrush( col );
			FillRect( dc, &r, br );
			DeleteObject( br );

			return 1;
		}

		case WM_PAINT: {

			PAINTSTRUCT ps = { 0 };
			HDC dc = BeginPaint( hwnd, &ps );

			sbWnd *wnd = ( void * )GetWindowLongPtrW( hwnd, GWLP_USERDATA );
			SBStringWindowInfo *info = GetSpecificHandle( wnd );
			
			int state = info->mouseState;
			if ( info->clickable && state & 0x01 ) // in-area bit
				SetBkColor( dc, STRWND_BG_COLOR );
			if ( info->clickable && state & 0x10 ) // click bit
				SetBkColor( dc, STRWND_CLK_COLOR );

			HFONT font = CreateFontIndirectW( info->currentFont );
			HBRUSH brush = GetStockObject( BLACK_BRUSH );

			HBRUSH prevBrush = SelectObject( dc, brush );
			HFONT prevFont = SelectObject( dc, font );

			TabbedTextOutW( dc, 2, 1, info->str, wcslen( info->str ), 0, NULL, 1 );

			SelectObject( dc, prevBrush );
			SelectObject( dc, prevFont );

			DeleteObject( font );
			DeleteObject( brush );

			EndPaint( hwnd, &ps );
			break;
		}

		case SBM_CHANGEFONT: {

			sbWnd *wnd = ( void * )GetWindowLongPtrW( hwnd, GWLP_USERDATA );
			SBStringWindowInfo *info = GetSpecificHandle( wnd );

			memcpy( info->currentFont, SbGUIMaster.currentFont, sizeof( LOGFONTW ) );
			info->currentFont->lfHeight = info->fontSize;

			sbWnd_Layout newSize = { 0 };
			memcpy( &newSize, wnd->lout, sizeof( sbWnd_Layout ) );
			newSize.dims.intDims[2] = SBWindows.getStringWidth( info->str, info->fontSize ) + 4;
			SBWindows.changeLayout( wnd, &newSize );

			InvalidateRect( hwnd, NULL, TRUE );
			break;
		}

		case WM_MOUSEMOVE: {
			
			sbWnd *wnd = ( void * )GetWindowLongPtrW( hwnd, GWLP_USERDATA );
			SBStringWindowInfo *info = GetSpecificHandle( wnd );

			if ( !info->clickable )
				break;

			int prevState = info->mouseState;
			info->mouseState |= 0x01; // Set in-area bit
			int curState = info->mouseState;

			TRACKMOUSEEVENT me = { 0 };
			me.cbSize = sizeof( TRACKMOUSEEVENT );
			me.hwndTrack = hwnd;
			me.dwFlags = TME_LEAVE | TME_HOVER;
			me.dwHoverTime = HOVER_DEFAULT;

			TrackMouseEvent( &me );
			
			if ( ( curState - prevState ) != 0 )
				InvalidateRect( hwnd, NULL, TRUE );

			break;
		}

		case WM_MOUSELEAVE: {
			sbWnd *wnd = ( void * )GetWindowLongPtrW( hwnd, GWLP_USERDATA );
			SBStringWindowInfo *info = GetSpecificHandle( wnd );
			info->mouseState = 0; // Reset all bits
			InvalidateRect( hwnd, NULL, TRUE );
			break;
		}

		case WM_LBUTTONDOWN: {
			sbWnd *wnd = ( void * )GetWindowLongPtrW( hwnd, GWLP_USERDATA );
			SBStringWindowInfo *info = GetSpecificHandle( wnd );

			if ( !info->clickable )
				break;
			info->mouseState |= 0x10; // Set click bit

			InvalidateRect( hwnd, NULL, TRUE );
			break;
		}

		case WM_LBUTTONUP: {
			sbWnd *wnd = ( void * )GetWindowLongPtrW( hwnd, GWLP_USERDATA );
			SBStringWindowInfo *info = GetSpecificHandle( wnd );

			if ( !info->clickable )
				break;
			info->mouseState &= ~(0x10); // Reset click bit

			InvalidateRect( hwnd, NULL, TRUE );
			UpdateWindow( hwnd ); // Cannot assume out function will return quickly

			SBWindows.signalFn( wnd, wnd, NULL );
			break;
		}

	}
	return sbWndCommonProc( hwnd, msg, wParam, lParam );
}

LRESULT CALLBACK ProgressBarProc( HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam ) {
	switch ( msg ) {

		case WM_ERASEBKGND: {
			HDC dc = ( void * )wParam;
			RECT r = { 0 };
			GetClientRect( hwnd, &r );
			FillRect( dc, &r, GetStockObject( WHITE_BRUSH ) );
			return 1;
		}
		
		case WM_PAINT: {
			
			PAINTSTRUCT ps = { 0 };
			HDC dc = BeginPaint( hwnd, &ps );
			
			sbWnd *wnd = ( void * )GetWindowLongPtrW( hwnd, GWLP_USERDATA );
			SBProgressBarWindowInfo *info = GetSpecificHandle( wnd );
			RECT r = { 0 };
			GetClientRect( hwnd, &r );

			int w = r.right - r.left;
			r.right = r.left + ( w * info->cur / info->total );

			FillRect( dc, &r, GetStockObject( BLACK_BRUSH ) );

			EndPaint( hwnd, &ps );
			break;
		}

	}
	return sbWndCommonProc( hwnd, msg, wParam, lParam );
}

LRESULT CALLBACK DldcmdMasterProc( HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam ) {
	switch( msg ) {
		
		case WM_LBUTTONDOWN:
			SetFocus( hwnd );
			break;

		case WM_ERASEBKGND: {
			HDC dc = ( void * )wParam;
			RECT r = { 0 };
			GetClientRect( hwnd, &r );
			FillRect( dc, &r, GetStockObject( WHITE_BRUSH ) );
			return 1;
		}

		case WM_PAINT: {
			PAINTSTRUCT ps = { 0 };
			HDC dc = BeginPaint( hwnd, &ps );
			( void )0; // Do nothing
			EndPaint( hwnd, &ps );
			break;
		}

	}
	return sbWndCommonProc( hwnd, msg, wParam, lParam );
}

LRESULT CALLBACK VScrollbarProc( HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam ) {
	switch ( msg ) {

		case WM_ERASEBKGND: {
			HDC dc = ( void * )wParam;
			RECT r = { 0 };
			GetClientRect( hwnd, &r );
			FillRect( dc, &r, GetStockObject( WHITE_BRUSH ) );
			return 1;
		}

		case WM_PAINT: {

			PAINTSTRUCT ps = { 0 };
			HDC dc = BeginPaint( hwnd, &ps );

			sbWnd *wnd = ( void * )GetWindowLongPtrW( hwnd, GWLP_USERDATA );
			SBVScrollbarWindowInfo *info = GetSpecificHandle( wnd );
			RECT r = { 0 };
			GetClientRect( hwnd, &r );

			HPEN prevPen = SelectObject(
				dc,
				CreatePen( PS_SOLID, 1, RGB( 0, 0, 0 ) )
			);

			int
				len = info->incDist * info->maxInc + 2 + r.right,
				x1 = r.right * 1/6 + 1,
				x2 = r.right * 5/6 + 1,
				bottom = len <= r.bottom
					?	len
					:	r.bottom - 2;

			MoveToEx( dc, x1, 1, NULL );
			LineTo( dc, x2, 1 );
			MoveToEx( dc, x1, bottom, NULL );
			LineTo( dc, x2, bottom );

			int
				maxDist = r.bottom - r.right - 4,
				incLen = info->maxInc * info->incDist - 1,
				dist = incLen <= maxDist
					?	info->cur * info->incDist + 2
					:	info->cur * maxDist/info->maxInc + 2;

			Rectangle(
				dc,
				0, dist,
				r.right, dist + r.right
			);

			HPEN pen = SelectObject( dc, prevPen );
			DeleteObject( pen );

			EndPaint( hwnd, &ps );
			break;
		}

	}
	return sbWndCommonProc( hwnd, msg, wParam, lParam );
}

void UpdateTiedown( SBTextboxInfo *info, wchar_t val ) {
	if ( ( info->tiedownLength + 1 ) >=  info->tiedownCapacity )
		DoubleWStringCapacity( info );
	info->tiedown[ info->tiedownLength++ ] = val;
	return;
}

void UpdateTextboxInfo( sbWnd *wnd ) {
	if ( wnd->type != TEXTBOX_WINDOW ) return;
	SBTextboxInfo *info = GetSpecificHandle( wnd );

	RECT *r = newptr( RECT );
	GetClientRect( GetHWND( wnd ), r );
	++r->top;
	--r->bottom;
	++r->left;
	--r->right;
	info->tbRect = r;

	int h = r->bottom - r->top;
	info->currentFont->lfHeight = h;

	return;
}

void ResetTextboxInfo( SBTextboxInfo *info ) {
	info->currentCaretPosition = 2;
	info->tiedownCapacity = 10;
	info->tiedownLength = 0;
	info->canDisplay = 1;
	if ( wcslen( info->tiedown ) > 0 )
		free( info->tiedown );
	info->tiedown = newwstr( 10 );
	return;
}

void DoubleWStringCapacity( SBTextboxInfo *info ) {
	wString dblStr = newwstr( ( info->tiedownCapacity * 2 ) );
	wcscpy( dblStr, info->tiedown );
	free( info->tiedown );
	info->tiedownCapacity *= 2;
	info->tiedown = dblStr;
}

void UpdateTextWndScrollbarInfo( SBBasicTextWindowInfo *info, RECT *wndr ) {
	// Setting max increment
	int exLines = ( int )info->lineCount - ( int )info->currentMaxLines;

	if ( exLines <= 0 && info->currentOffset == 0 ) {
		SBVScrollbarWindows.reset( info->scrollbar );
		SBWindows.hide( info->scrollbar, 0 );
		return;
	}
	SBWindows.show( info->scrollbar, 0 );

	int nMaxInc = ( exLines - 1 ) / 3 + 1;
	if (
		SBVScrollbarWindows.getMaxIncrement( info->scrollbar )
		!= nMaxInc
	)
		SBVScrollbarWindows.setMaxIncrement( info->scrollbar, nMaxInc );

	if ( wndr == NULL )
		return;

	// Setting position--only happens if rectangle is provided (window is resized)
	int
		frameTop = info->currentOffset / info->fontSize, // Last line offscreen
		frameBottom = frameTop + info->currentMaxLines; // Last possible line on screen

	if (
		frameBottom >= info->lineCount
		&& info->currentMaxLines > info->prevMaxLines
		&& frameBottom >= ( info->lineCount + 3 )
	)
		info->currentOffset -= 3 * info->fontSize;

	info->prevMaxLines = info->currentMaxLines;
	return;
}

BOOL CALLBACK ChildSizingProc( IN HWND hwnd, IN LPARAM lParam ) {
	sbWnd *wnd = ( void * )GetWindowLongPtrW( hwnd, GWLP_USERDATA );

	dimension finDims[4] = { 0 };
	sbWndEvaluateLayout( wnd->lout, finDims );
	
	SetWindowPos(
		hwnd, 0,
		finDims[0],
		finDims[1],
		finDims[2],
		finDims[3],
		SWP_NOZORDER
	);
	return 1;
}

BOOL CALLBACK ChildFontChangeProc( IN HWND hwnd, IN LPARAM lParam) {
	SendMessageW( hwnd, SBM_CHANGEFONT, 0, 0 );
	return TRUE;
}

BOOL CALLBACK ChildDestroyProc( IN HWND hwnd, IN LPARAM lParam ) {
	DestroyWindow( hwnd );
	return TRUE;
}
