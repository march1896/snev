/*	Nehe SDK
	Copyright 2002 Jeff Molofee, Gregory Austwick and Others

	Title:
		 Window Handling class	

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
	mention in any credits would be appreciated, but is not 
	necessary.
*/

#include <windows.h>
#include "win32.h"

Window :: Window()
{
	// clear everything
	width=height=0;
	mx=my=0;
	lbut=rbut=sized=false;
	hwnd=NULL;
	hins=NULL;
	ZeroMemory(keys,sizeof(bool)*256);
}

Window :: ~Window()
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
	Window *win;	// our window class

	// get the right class
	win=(Window *)GetWindowLong(hwnd,GWL_USERDATA);

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

void Window :: DataAccess(int key,int value)
{
	// if called before anything else
	if(!this)
		return;

	// Convert messages to variables
	switch(key)
	{
		case WINMX:			mx=value;	break;
		case WINMY:			my=value;	break;
		case WINLBUT:		lbut=( (value==1) ? true : false);	break;
		case WINRBUT:		rbut=( (value==1) ? true : false);	break;
		case WINKEYUP:		keys[value]=true;	break;
		case WINKEYDOWN:	keys[value]=false;	break;
		case WINSIZED:		sized=true;			break;
		case WINWIDTH:		width=value;		break;
		case WINHEIGHT:		height=value;		break;
	}
}

void Window :: DoMessaging()
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

// register the window
BOOL Window :: Register(char *name)
{
	WNDCLASS	wcls;	// Windows class structure

	wcls.style=CS_HREDRAW | CS_VREDRAW;
	wcls.lpfnWndProc=WndProc;
	wcls.cbClsExtra=0;
	wcls.cbWndExtra=0;
	wcls.hInstance=hins;
	wcls.hIcon=LoadIcon(NULL,IDI_APPLICATION);
	wcls.hCursor=LoadCursor(NULL,IDC_ARROW);
	wcls.hbrBackground=(HBRUSH)GetStockObject(BLACK_BRUSH);
	wcls.lpszMenuName=NULL;
	wcls.lpszClassName=name;

	return RegisterClass(&wcls);
}

bool Window :: Setup(int x,int y,int wid,int hit, char *name)
{
	// get the dimensions
	width=wid;
	height=hit;

	// get the instance
	hins=GetModuleHandle(NULL);

	// register the window
	if(!this->Register(name))
		return false;

	// now create the window
	hwnd=CreateWindow(name,name,WS_POPUP|WS_VISIBLE,
		x,y,width,height,NULL,NULL,hins,NULL);

	// did that work?
	if(!hwnd)
		return false;

	// show the window
	ShowWindow(hwnd,SW_SHOWNORMAL);
	UpdateWindow(hwnd);

	// store a pointer to the class in HWND
	SetWindowLong(hwnd,GWL_USERDATA,(long)this);

	// success
	return true;
}

bool Window :: SetupTitle(int x,int y,int wid,int hit, char *name)
{
	// get the dimensions
	width=wid;
	height=hit;

	// get the instance
	hins=GetModuleHandle(NULL);

	// register the window
	if(!this->Register(name))
		return false;

	// now create the window
	hwnd=CreateWindow(name,name,WS_VISIBLE|WS_OVERLAPPEDWINDOW,
		x,y,width,height,NULL,NULL,hins,NULL);

	// did that work?
	if(!hwnd)
		return false;

	// show the window
	ShowWindow(hwnd,SW_SHOWNORMAL);
	UpdateWindow(hwnd);

	// store a pointer to the class in HWND
	SetWindowLong(hwnd,GWL_USERDATA,(long)this);

	// success
	return true;
}

void Window :: Close()
{
	// anything to do?
	if(!hwnd)
		return;

	// pretend we weren't here
	SetWindowLong(hwnd,GWL_USERDATA,0);
	DestroyWindow(hwnd);
}

