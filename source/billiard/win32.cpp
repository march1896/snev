/*	Nehe SDK
	Copyright 2002 Jeff Molofee, Gregory Austwick and Others

	Title:
		 WindowsHandle Handling class	

	Version History:
		v1.00		Copied from my ill-fated tutorial series:)	(GA 29/10/02)
		v1.01		Converted to NeHe style Keyboard routine	(JM,GA 29/10/02)

	Licence:

	This SDK/library is free software, written primarily for
	teaching purposes. You can redistribute and modify it to 
	your heart's content. However, any use of this library is 
	at the user's own risk. There is no warranty. The authors 
	will not be held responsible for ANYTHING that happens as 
	a result of the use of this software. All financial/emotional/health 
	problems or anything else that happens after using this 
	software is therefore the user's own responsibility. 
	Nevertheless, if the user finds it useful, an email or a 
	mention in any credits would be appreciated, but is not necessary.
*/

#include <windows.h>
#include "win32.h"
#include "opengl.h"

#include "log.h"

WindowsHandle :: WindowsHandle()
{
	// clear everything
	//width=height=0;
	WindowRect.left = WindowRect.right = WindowRect.top = WindowRect.bottom = 0;
	mx=my=0;
	lbut=rbut=sized=false;
	hwnd=NULL;
	hInstance=NULL;

	hDC = NULL;
	hRC = NULL;
	fullscreen = false;

	bpp = 32;

	ZeroMemory(keys,sizeof(bool)*256);
}

WindowsHandle :: ~WindowsHandle()
{
	this->Close();
}

#define WINMX		0x0013
#define WINMY		0x0014
#define WINLBUT		0x0015
#define WINRBUT		0x0016
#define WINKEYUP	0x0017
#define WINKEYDOWN	0x0018
#define	WINSIZED	0x0019
#define WINWIDTH	0x0020
#define WINHEIGHT	0x0021

LRESULT CALLBACK WndProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	WindowsHandle *win;	// our WindowsHandle class

	// get the right class
	win=(WindowsHandle *)GetWindowLongPtr(hwnd,GWL_USERDATA);

	// which message
	switch(msg)
	{
		case WM_CLOSE:
			exit(0);
		case WM_DESTROY:
			SetWindowLong(hwnd,GWL_USERDATA,0);	// pretend we weren't here
			return 0;
		case WM_SIZE:
			win->DataAccess(WINSIZED,1);				// say it's been resized
			win->DataAccess(WINWIDTH,LOWORD(lParam));	// get the new width
			win->DataAccess(WINHEIGHT,HIWORD(lParam));	// get the new height
			return 0;
		case WM_KEYDOWN:
			// set the key as being down
			win->DataAccess(WINKEYUP,wParam);
			return 0;
		case WM_KEYUP:
			// set the key as being up
			win->DataAccess(WINKEYDOWN,wParam);
			return 0;
		case WM_MOUSEMOVE:
			// send x and y
			win->DataAccess(WINMX,LOWORD(lParam));
			win->DataAccess(WINMY,HIWORD(lParam));
			return 0;
		case WM_LBUTTONUP:
			// set left mouse button up
			win->DataAccess(WINLBUT,0);
			return 0;
		case WM_RBUTTONUP:
			// set right mouse button up
			win->DataAccess(WINRBUT,0);
			return 0;
		case WM_LBUTTONDOWN:
			// set left mouse button down
			win->DataAccess(WINLBUT,1);
			return 0;
		case WM_RBUTTONDOWN:
			// set right mouse button down
			win->DataAccess(WINRBUT,1);
			return 0;
		case WM_COMMAND:
			// prevent screensaver or monitor power saving
			if(wParam==SC_SCREENSAVE || wParam==SC_MONITORPOWER)
				return 0;
		default:
			return DefWindowProc(hwnd,msg,wParam,lParam);
	};
}

void WindowsHandle :: DataAccess(int key,int value)
{
	// if called before anything else
	if(!this) {
		return;
	}

	// Convert messages to variables
	switch(key)
	{
		case WINMX:		mx=value;	break;
		case WINMY:	 	my=value;	break;
		case WINLBUT:		lbut=( (value==1) ? true : false);	break;
		case WINRBUT:		rbut=( (value==1) ? true : false);	break;
		case WINKEYUP:		keys[value]=true;	break;
		case WINKEYDOWN:	keys[value]=false;	break;
		case WINSIZED:		sized=true;			break;
		//case WINWIDTH:		width=value;		break;
		//case WINHEIGHT:		height=value;		break;
		case WINWIDTH:		WindowRect.right = WindowRect.left + value;		break;
		case WINHEIGHT:		WindowRect.bottom = WindowRect.top + value;		break;
	}
}

void WindowsHandle :: DoMessaging()
{
	MSG msg;	// message info

	// are there any messages
	while(PeekMessage(&msg,hwnd,0,0,PM_NOREMOVE))
	{
		// get the message off the queue
		if(!GetMessage(&msg,hwnd,0,0))
			return;

		// and send it off again
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

// register the WindowsHandle
BOOL WindowsHandle :: Register(char *name)
{
	WNDCLASS	wcls;	// Windows class structure

	wcls.style=CS_HREDRAW | CS_VREDRAW;
	//wcls.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;	// Redraw On Size, And Own DC For Window.
	wcls.lpfnWndProc=WndProc;
	wcls.cbClsExtra=0;
	wcls.cbWndExtra=0;
	wcls.hInstance=hInstance;
	wcls.hIcon=LoadIcon(NULL,IDI_APPLICATION);
	wcls.hCursor=LoadCursor(NULL,IDC_ARROW);
	wcls.hbrBackground=(HBRUSH)GetStockObject(BLACK_BRUSH);
	//wcls.hbrBackground= NULL;
	wcls.lpszMenuName=NULL;
	wcls.lpszClassName=name;

	return RegisterClass(&wcls);
}

bool WindowsHandle :: Setup(int x,int y,int wid,int hit, char *name)
{
	// get the dimensions
	//width=wid;
	//height=hit;
	WindowRect.left = x;
	WindowRect.right = x + wid;
	WindowRect.top = y;
	WindowRect.bottom = y + hit;


	// get the instance
	hInstance=GetModuleHandle(NULL);

	// register the WindowsHandle
	if(!this->Register(name))
		return false;

	// now create the WindowsHandle
	hwnd=CreateWindow(name,name,WS_POPUP|WS_VISIBLE,
			x,y,wid,hit,NULL,NULL,hInstance,NULL);

	// did that work?
	if(!hwnd)
		return false;

	// show the WindowsHandle
	ShowWindow(hwnd,SW_SHOWNORMAL);
	UpdateWindow(hwnd);

	// store a pointer to the class in hwnd
	SetWindowLong(hwnd,GWL_USERDATA,(long)this);

	// success
	return true;
}


bool WindowsHandle :: SetupTitle(int x,int y,int wid,int hit, char *name)
{
	// get the dimensions
	//width=wid;
	//height=hit;
	WindowRect.left = x;
	WindowRect.right = x + wid;
	WindowRect.top = y;
	WindowRect.bottom = y + hit;

	// get the instance
	hInstance=GetModuleHandle(NULL);

	// register the WindowsHandle
	if(!this->Register(name))
		return false;

	// now create the WindowsHandle
	hwnd=CreateWindow(name,name,WS_VISIBLE|WS_OVERLAPPEDWINDOW,
			x,y,wid,hit,NULL,NULL,hInstance,NULL);

	// did that work?
	if(!hwnd)
		return false;

	// show the WindowsHandle
	ShowWindow(hwnd,SW_SHOWNORMAL);
	UpdateWindow(hwnd);

	// store a pointer to the class in hwnd
	SetWindowLong(hwnd,GWL_USERDATA,(long)this);

	// success
	return true;
}

bool WindowsHandle::InitWindow( char* name, int x, int y, int wid, int hit, bool fullscreenflag )
{
	DWORD		dwExStyle;				// Window Extended Style
	DWORD		dwStyle;				// Window Style
	WNDCLASS	wc;	// Windows class structure

	fullscreen = fullscreenflag;
	WindowRect.left = x;
	WindowRect.right = x + wid;
	WindowRect.top = y;
	WindowRect.bottom = y + hit;
	hInstance = GetModuleHandle( NULL );

	//if ( !this->Register( name ) ) return false;
	
	wc.style 		= CS_HREDRAW | CS_VREDRAW | CS_OWNDC;	// Redraw On Size, And Own DC For Window.
	wc.lpfnWndProc		= (WNDPROC) WndProc;					// WndProc Handles Messages
	wc.cbClsExtra		= 0;									// No Extra Window Data
	wc.cbWndExtra		= 0;									// No Extra Window Data
	wc.hInstance		= hInstance;							// Set The Instance
	wc.hIcon	 	= LoadIcon(NULL, IDI_WINLOGO);			// Load The Default Icon
	wc.hCursor		= LoadCursor(NULL, IDC_ARROW);			// Load The Arrow Pointer
	wc.hbrBackground=(HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.hbrBackground	= NULL;									// No Background Required For GL
	wc.lpszMenuName		= NULL;									// We Don't Want A Menu
	wc.lpszClassName	= "OpenGL";								// Set The Class Name

	if (!RegisterClass(&wc))									// Attempt To Register The Window Class
	{
		MessageBox(NULL,"Failed To Register The Window Class.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;											// Return FALSE
	}
	
	
	if (fullscreen)												// Attempt Fullscreen Mode?
	{
		DEVMODE dmScreenSettings;								// Device Mode
		memset(&dmScreenSettings,0,sizeof(dmScreenSettings));	// Makes Sure Memory's Cleared
		dmScreenSettings.dmSize=sizeof(dmScreenSettings);		// Size Of The Devmode Structure
		dmScreenSettings.dmPelsWidth	= wid;				// Selected Screen Width
		dmScreenSettings.dmPelsHeight	= hit;				// Selected Screen Height
		dmScreenSettings.dmBitsPerPel	= bpp;					// Selected Bits Per Pixel
		dmScreenSettings.dmFields=DM_BITSPERPEL|DM_PELSWIDTH|DM_PELSHEIGHT;

		// Try To Set Selected Mode And Get Results.  NOTE: CDS_FULLSCREEN Gets Rid Of Start Bar.
		if (ChangeDisplaySettings(&dmScreenSettings,CDS_FULLSCREEN)!=DISP_CHANGE_SUCCESSFUL)
		{
			// If The Mode Fails, Offer Two Options.  Quit Or Use Windowed Mode.
			if (MessageBox(NULL,"The Requested Fullscreen Mode Is Not Supported By\nYour Video Card. Use Windowed Mode Instead?","NeHe GL",MB_YESNO|MB_ICONEXCLAMATION)==IDYES)
			{
				fullscreen=FALSE;		// Windowed Mode Selected.  Fullscreen = FALSE
			}
			else
			{
				// Pop Up A Message Box Letting User Know The Program Is Closing.
				MessageBox(NULL,"Program Will Now Close.","ERROR",MB_OK|MB_ICONSTOP);
				return FALSE;									// Return FALSE
			}
		}
	}

	if (fullscreen)												// Are We Still In Fullscreen Mode?
	{
		dwExStyle=WS_EX_APPWINDOW;								// Window Extended Style
		dwStyle=WS_POPUP;										// Windows Style
		ShowCursor(FALSE);										// Hide Mouse Pointer
	}
	else
	{
		dwExStyle=WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;			// Window Extended Style
		dwStyle=WS_OVERLAPPEDWINDOW;							// Windows Style
	}

	AdjustWindowRectEx(&WindowRect, dwStyle, FALSE, dwExStyle);		// Adjust Window To True Requested Size

	if (!(hwnd=CreateWindowEx(	dwExStyle,							// Extended Style For The Window
					"OpenGL",							// Class Name
					name,								// Window Title
					dwStyle |							// Defined Window Style
					WS_CLIPSIBLINGS |					// Required Window Style
					WS_CLIPCHILDREN,					// Required Window Style
					WindowRect.left, WindowRect.top,								// Window Position
					WindowRect.right-WindowRect.left,	// Calculate Window Width
					WindowRect.bottom-WindowRect.top,	// Calculate Window Height
					NULL,								// No Parent Window
					NULL,								// No Menu
					hInstance,							// Instance
					NULL)))								// Dont Pass Anything To WM_CREATE
	{
		MessageBox(NULL,"Can't Create hWnd Context.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return false;								// Return FALSE
	}

	SetWindowLongPtr( hwnd, GWL_USERDATA, (LONG_PTR)this);

	static	PIXELFORMATDESCRIPTOR pfd=				// pfd Tells Windows How We Want Things To Be
	{
		sizeof(PIXELFORMATDESCRIPTOR),				// Size Of This Pixel Format Descriptor
		1,											// Version Number
		PFD_DRAW_TO_WINDOW |						// Format Must Support Window
		PFD_SUPPORT_OPENGL |						// Format Must Support OpenGL
		PFD_DOUBLEBUFFER,							// Must Support Double Buffering
		PFD_TYPE_RGBA,								// Request An RGBA Format
		bpp,										// Select Our Color Depth
		0, 0, 0, 0, 0, 0,							// Color Bits Ignored
		0,											// No Alpha Buffer
		0,											// Shift Bit Ignored
		0,											// No Accumulation Buffer
		0, 0, 0, 0,									// Accumulation Bits Ignored
		16,											// 16Bit Z-Buffer (Depth Buffer)  
		0,											// No Stencil Buffer
		0,											// No Auxiliary Buffer
		PFD_MAIN_PLANE,								// Main Drawing Layer
		0,											// Reserved
		0, 0, 0										// Layer Masks Ignored
	};
	
	if (!(hDC=GetDC(hwnd)))							// Did We Get A Device Context?
	{
		KillWindow();								// Reset The Display
		MessageBox(NULL,"Can't Create A GL Device Context.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}

	GLuint		PixelFormat;			// Holds The Results After Searching For A Match
	
	if (!(PixelFormat=ChoosePixelFormat(hDC,&pfd)))	// Did Windows Find A Matching Pixel Format?
	{
		KillWindow();								// Reset The Display
		MessageBox(NULL,"Can't Find A Suitable PixelFormat.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}

	if(!SetPixelFormat(hDC,PixelFormat,&pfd))		// Are We Able To Set The Pixel Format?
	{
		KillWindow();								// Reset The Display
		MessageBox(NULL,"Can't Set The PixelFormat.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}

	if (!(hRC=wglCreateContext(hDC)))				// Are We Able To Get A Rendering Context?
	{
		KillWindow();								// Reset The Display
		MessageBox(NULL,"Can't Create A GL Rendering Context.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}

	if(!wglMakeCurrent(hDC,hRC))					// Try To Activate The Rendering Context
	{
		KillWindow();								// Reset The Display
		MessageBox(NULL,"Can't Activate The GL Rendering Context.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}

	ShowWindow(hwnd,SW_SHOW);						// Show The Window
	SetForegroundWindow(hwnd);						// Slightly Higher Priority
	SetFocus(hwnd);									// Sets Keyboard Focus To The Window

	return true;									// Success
}

void WindowsHandle::KillWindow()								// Properly Kill The Window
{
	if (fullscreen)										// Are We In Fullscreen Mode?
	{
		ChangeDisplaySettings(NULL,0);					// If So Switch Back To The Desktop
		ShowCursor(TRUE);								// Show Mouse Pointer
	}

	if (hRC)											// Do We Have A Rendering Context?
	{
		if (!wglMakeCurrent(NULL,NULL))					// Are We Able To Release The DC And RC Contexts?
		{
			MessageBox(NULL,"Release Of DC And RC Failed.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		}

		if (!wglDeleteContext(hRC))						// Are We Able To Delete The RC?
		{
			MessageBox(NULL,"Release Rendering Context Failed.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		}
		hRC=NULL;										// Set RC To NULL
	}

	if (hDC && !ReleaseDC(hwnd,hDC))					// Are We Able To Release The DC
	{
		MessageBox(NULL,"Release Device Context Failed.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		hDC=NULL;										// Set DC To NULL
	}

	if (hwnd && !DestroyWindow(hwnd))					// Are We Able To Destroy The Window?
	{
		MessageBox(NULL,"Could Not Release hwnd.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		hwnd=NULL;										// Set hWnd To NULL
	}

	if (!UnregisterClass("OpenGL",hInstance))			// Are We Able To Unregister Class
	{
		MessageBox(NULL,"Could Not Unregister Class.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		hInstance=NULL;									// Set hInstance To NULL
	}
}


void WindowsHandle :: Close()
{
	// anything to do?
	if(!hwnd)
		return;

	// pretend we weren't here
	SetWindowLong(hwnd,GWL_USERDATA,0);
	DestroyWindow(hwnd);
}

