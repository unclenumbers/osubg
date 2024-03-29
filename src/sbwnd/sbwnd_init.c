#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <windows.h>
#include <gdiplus/gdiplus.h>

#include "sbwnd/sbwnd_init.h"
#include "sbwnd/sb_ds.h"
#include "sbwnd/sbwnd_create.h"
#include "sbwnd/sbwnd_general.h"
#include "res/sbwnd_res.h"
#include "sbwnd/sbwnd_proc.h"

SbGUIMasterStruct SbGUIMaster;

void initGUI( void ) {
	initDS( );
	initWindowFunctions( );
	SetUpSbGUIMaster( );
	RegisterClasses( );
	CreateTestWindow( );
}

void initWindowFunctions( void ) {
	SBBasicWindows.create = BasicWindowCreator;
	SBTextboxes.create = TextboxCreator;
	SBTextboxes.getString = getString_SbTextbox;
	SBTextboxes.sendString = sendString_SbTextbox;
	SBTextboxes.setEnterAction = setEnterAction_SbTextbox;
	SBTextboxes.getEnterAction = getEnterAction_SbTextbox;
	SBBasicTextWindows.create = BasicTextWindowCreator;
	SBBasicTextWindows.draw = draw_SbBasicTextWindow;
	SBBasicTextWindows.clear = clear_SbBasicTextWindow;
	SBClickableWindows.create = ClickableWindowCreator;
	SBRestrictedImageWindows.create = RestrictedImageWindowCreator;
	SBRestrictedImageWindows.updateImage = updateImage_SbRestrictedImageWindow;
	SBMasterWindows.create = MasterWindowCreator;
	SBViewcmdMasterWindows.create = ViewcmdMasterWindowCreator;
	SBStringWindows.create = StringWindowCreator;
	SBStringWindows.changeString = changeString_SbStringWindow;
	SBProgressBarWindows.create = ProgressBarWindowCreator;
	SBProgressBarWindows.advance = advance_SbProgressBarWindow;
	SBDldcmdMasterWindows.create = DldcmdMasterWindowCreator;
	SBVScrollbarWindows.create = VScrollbarWindowCreator;
	SBVScrollbarWindows.advance = advance_SbVScrollbarWindow;
	SBVScrollbarWindows.retreat = retreat_SbVScrollbarWindow;
	SBVScrollbarWindows.setMaxIncrement = setMaxIncrement_SbVScrollbarWindow;
	SBVScrollbarWindows.getMaxIncrement = getMaxIncrement_SbVScrollbarWindow;
	SBVScrollbarWindows.getCurrentPos = getCurrentPos_SbVScrollbarWindow;
	SBVScrollbarWindows.reset = reset_SbVScrollbarWindow;
	SBVScrollbarWindows.setPos = setPos_SbVScrollbarWindow;
	SBWindows.destroy = destroy_AllTypes;
	SBWindows.setSignalFn = SetSignalFn_AllTypes;
	SBWindows.appendReference = appendReference_AllTypes;
	SBWindows.getReference = getReference_AllTypes;
	SBWindows.getStringWidth = getStringWidth_AllTypes;
	SBWindows.signalFn = CallSignalFn_AllTypes;
	SBWindows.changeLayout = changeLayout_AllTypes;
	SBWindows.addOffset = addOffset_AllTypes;
	SBWindows.toSurface = toSurface_AllTypes;
	SBWindows.focus = focus_AllTypes;
	SBWindows.getLayout = getLayout_AllTypes;
	SBWindows.getParent = getParent_AllTypes;
	SBWindows.getID = getID_AllTypes;
	SBWindows.getType = getType_AllTypes;
	SBWindows.setCreateMode = setCreateMode_AllTypes;
	SBWindows.show = show_AllTypes;
	SBWindows.hide = hide_AllTypes;
	SBWindows.update = update_AllTypes;
}

void SetUpSbGUIMaster( void ) {
	GdiplusStartupInput in = {
		.GdiplusVersion = 1
	};
	GdiplusStartupOutput out;
	ULONG_PTR tok;
	GdiplusStartup( &tok, &in, &out );
	SbGUIMaster.gdipToken = tok;
	SbGUIMaster.large_id = 0;
	SbGUIMaster.createMode = SBWND_CREATEMODE_SHOW;
	SbGUIMaster.WindowClassNameArray = newarr( wString, NUM_CLASSES );
	LOGFONTW *consolasFont = newptr( LOGFONTW );
	wcscpy( consolasFont->lfFaceName, L"Consolas" );
	consolasFont->lfQuality = CLEARTYPE_QUALITY;
	consolasFont->lfCharSet = DEFAULT_CHARSET;
	SbGUIMaster.currentFont = consolasFont;
	SbGUIMaster.sbAccelTable = LoadAccelerators( GetModuleHandleW( NULL ), MAKEINTRESOURCE( TBL_ONE ) );
	SbGUIMaster.TopLevelWindows = Lists.New( );
	SbGUIMaster.masterWnd = NULL;
	SbGUIMaster.scrWidth = GetSystemMetrics( SM_CXSCREEN );
	SbGUIMaster.scrHeight = GetSystemMetrics( SM_CYSCREEN );
}

void RegisterClasses( void ) {
	HINSTANCE inst = GetModuleHandleW( NULL );
	WNDCLASSEXW
		txtbox = { 0 },
		basic = { 0 },
		text = { 0 },
		clickable = { 0 },
		restrictedImage = { 0 },
		master = { 0 },
		viewmaster = { 0 },
		string = { 0 },
		progbar = { 0 },
		dldmaster = { 0 },
		vscrollbar = { 0 };

	SbGUIMaster.WindowClassNameArray[ BASIC_WINDOW ] = L"basic";
	SbGUIMaster.WindowClassNameArray[ TEXTBOX_WINDOW ] = L"textbox";
	SbGUIMaster.WindowClassNameArray[ TEXT_WINDOW ] = L"text";
	SbGUIMaster.WindowClassNameArray[ CLICKABLE_WINDOW ] = L"clickable";
	SbGUIMaster.WindowClassNameArray[ RESTRICTED_IMAGE_WINDOW ] = L"restrictedImage";
	SbGUIMaster.WindowClassNameArray[ MASTER_WINDOW ] = L"master";
	SbGUIMaster.WindowClassNameArray[ VIEWCMD_MASTER_WINDOW ] = L"viewmaster";
	SbGUIMaster.WindowClassNameArray[ STRING_WINDOW ] = L"string";
	SbGUIMaster.WindowClassNameArray[ PROGRESS_BAR_WINDOW ] = L"progbar";
	SbGUIMaster.WindowClassNameArray[ DLDCMD_MASTER_WINDOW ] = L"dldmaster";
	SbGUIMaster.WindowClassNameArray[ VSCROLLBAR_WINDOW ] = L"vscrollbar";

	HBRUSH white = CreateSolidBrush( RGB( 0xFF, 0xFF, 0xFF ) );

	txtbox.cbSize = sizeof( WNDCLASSEXW );
	txtbox.lpfnWndProc = TextboxProc;
	txtbox.hInstance = inst;
	txtbox.lpszClassName = SbGUIMaster.WindowClassNameArray[ TEXTBOX_WINDOW ];
	txtbox.hCursor = LoadCursor( NULL, IDC_IBEAM );
	txtbox.cbWndExtra = sizeof( sbWnd * );

	basic.cbSize = sizeof( WNDCLASSEXW );
	basic.lpfnWndProc = BasicWndProc;
	basic.hInstance = inst;
	basic.hCursor = LoadCursor( NULL, IDC_ARROW );
	basic.hbrBackground = white;
	basic.lpszClassName = SbGUIMaster.WindowClassNameArray[ BASIC_WINDOW ];
	basic.cbWndExtra = sizeof( sbWnd * );

	text.cbSize = sizeof( WNDCLASSEXW );
	text.lpfnWndProc = TextWndProc;
	text.hInstance = inst;
	text.lpszClassName = SbGUIMaster.WindowClassNameArray[ TEXT_WINDOW ];
	text.hCursor = LoadCursor( NULL, IDC_ARROW );
	text.hbrBackground = white;
	text.cbWndExtra = sizeof( sbWnd * );

	clickable.cbSize = sizeof( WNDCLASSEXW );
	clickable.lpfnWndProc = ClickableProc;
	clickable.hInstance = inst;
	clickable.lpszClassName = SbGUIMaster.WindowClassNameArray[ CLICKABLE_WINDOW ];
	clickable.hCursor = LoadCursor( NULL, IDC_HAND );
	clickable.cbWndExtra = sizeof( sbWnd * );

	restrictedImage.cbSize = sizeof( WNDCLASSEXW );
	restrictedImage.lpfnWndProc = RestrictedImageProc;
	restrictedImage.hInstance = inst;
	restrictedImage.lpszClassName = SbGUIMaster.WindowClassNameArray[ RESTRICTED_IMAGE_WINDOW ];
	restrictedImage.hCursor = LoadCursor( NULL, IDC_ARROW );
	restrictedImage.hbrBackground = white;
	restrictedImage.cbWndExtra = sizeof( sbWnd * );

	master.cbSize = sizeof( WNDCLASSEXW );
	master.lpfnWndProc = MasterProc;
	master.hInstance = inst;
	master.lpszClassName = SbGUIMaster.WindowClassNameArray[ MASTER_WINDOW ];
	master.hbrBackground = white;
	/*master.hIcon = LoadImageW(
		inst,
		MAKEINTRESOURCEW( LGSB_ICO ),
		IMAGE_ICON,
		0, 0, 0
	);
	master.hIconSm = LoadImageW(
		inst,
		MAKEINTRESOURCEW( SMSB_ICO ),
		IMAGE_ICON,
		0, 0, 0
	);*/
	master.lpszMenuName = MAKEINTRESOURCEW( MENU_ONE );
	master.hCursor = LoadCursor( NULL, IDC_ARROW );
	master.cbWndExtra = sizeof( sbWnd * );

	viewmaster.cbSize = sizeof( WNDCLASSEXW );
	viewmaster.lpfnWndProc = ViewcmdMasterProc;
	viewmaster.hInstance = inst;
	viewmaster.lpszClassName = SbGUIMaster.WindowClassNameArray[ VIEWCMD_MASTER_WINDOW ];
	viewmaster.hbrBackground = white;
	viewmaster.cbWndExtra = sizeof( sbWnd * );

	string.cbSize = sizeof( WNDCLASSEXW );
	string.lpfnWndProc = StringProc;
	string.hInstance = inst;
	string.lpszClassName = SbGUIMaster.WindowClassNameArray[ STRING_WINDOW ];
	string.cbWndExtra = sizeof( sbWnd * );

	progbar.cbSize = sizeof( WNDCLASSEXW );
	progbar.lpfnWndProc = ProgressBarProc;
	progbar.hInstance = inst;
	progbar.lpszClassName = SbGUIMaster.WindowClassNameArray[ PROGRESS_BAR_WINDOW ];
	progbar.hbrBackground = white;
	progbar.cbWndExtra = sizeof( sbWnd * );

	dldmaster.cbSize = sizeof( WNDCLASSEXW );
	dldmaster.lpfnWndProc = DldcmdMasterProc;
	dldmaster.hInstance = inst;
	dldmaster.lpszClassName = SbGUIMaster.WindowClassNameArray[ DLDCMD_MASTER_WINDOW ];
	dldmaster.hbrBackground = white;
	dldmaster.cbWndExtra = sizeof( sbWnd * );

	vscrollbar.cbSize = sizeof( WNDCLASSEXW );
	vscrollbar.lpfnWndProc = VScrollbarProc;
	vscrollbar.hInstance = inst;
	vscrollbar.lpszClassName = SbGUIMaster.WindowClassNameArray[ VSCROLLBAR_WINDOW ];
	vscrollbar.cbWndExtra = sizeof( sbWnd * );

	RegisterClassExW( &basic );
	RegisterClassExW( &txtbox );
	RegisterClassExW( &text );
	RegisterClassExW( &clickable );
	RegisterClassExW( &restrictedImage );
	RegisterClassExW( &master );
	RegisterClassExW( &viewmaster );
	RegisterClassExW( &string );
	RegisterClassExW( &progbar );
	RegisterClassExW( &dldmaster );
	RegisterClassExW( &vscrollbar );
}

void CreateTestWindow( void ) {
	SBWindows.setCreateMode( SBWND_CREATEMODE_HIDE );
	sbWnd_Layout l = { 0 };
	
	SbGUIMaster.testWnd = SBBasicWindows.create( L"testwindow", &l );
	SBWindows.setCreateMode( SBWND_CREATEMODE_SHOW );
}

void shutdownSB( void ) {
	HINSTANCE inst = GetModuleHandleW( NULL );
	for ( int i = 0; i < NUM_CLASSES; ++i )
		UnregisterClassW( SbGUIMaster.WindowClassNameArray[i], inst );

	DestroyAcceleratorTable( SbGUIMaster.sbAccelTable );
	free( SbGUIMaster.WindowClassNameArray );
	free( SbGUIMaster.currentFont );
	SBWindows.destroy( SbGUIMaster.testWnd );
	Lists.Delete( SbGUIMaster.TopLevelWindows );
	GdiplusShutdown( SbGUIMaster.gdipToken );
	return;
}