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
	mention in any credits would be appreciated, but is not 
	necessary.
*/

#ifndef _WIN32WINDOW_H_
#define _WIN32WINDOW_H_

#include <windows.h>

class WindowsHandle {
	RECT		WindowRect;				// Grabs Rectangle Upper Left / Lower Right Values
	
	int		mx,my;			// mouse position
	bool		lbut,rbut;		// buttons positions
	bool		keys[256];		// key positions
	bool		sized;			// whether WindowsHandle has been sized
	HWND		hwnd;			// Windows handle
	HINSTANCE	hInstance;			// instance

	HDC		hDC;              // Private GDI Device Context
	HGLRC		hRC;              // Permanent Rendering Context
	
	int 		bpp; 		// Bits per pixel
	bool 		fullscreen;
	// Helper functions
	BOOL Register(char *name);
	void DoMessaging();
public:
	WindowsHandle();
	~WindowsHandle();
	// Setup function
	//	Params:
	//		(x,y):		position of WindowsHandle
	//		(wid,hit):	dimensions of WindowsHandle
	//		name:		name of WindowsHandle
	bool Setup(int x,int y,int wid,int hit,char *name);
	// Setup function with title
	//	Params:
	//		(x,y):		position of WindowsHandle
	//		(wid,hit):	dimensions of WindowsHandle
	//		name:		name of WindowsHandle
	bool SetupTitle(int x,int y,int wid,int hit,char *name);

	bool InitWindow(char* name, int x, int y, int wid, int hit, bool fullscreenflag);
	void KillWindow();
	// Close WindowsHandle
	void Close();
	// Has the WindowsHandle been sized?
	bool BeenSized() { bool temp=sized; sized=false; return temp; };
	// Get Windows Handle
	HWND GetHandle() const		{ return hwnd; };
	// Get Windows Instance
	HINSTANCE GetInstance() const { return hInstance; };
	// Get Dimensions
	//	Params:
	//		wid,hit:	dimensions of WindowsHandle
	void GetDimensions(int &wid,int &hit) const	{
		wid=WindowRect.right - WindowRect.left;	
		hit=WindowRect.bottom - WindowRect.top;			// return the dimensions
	};
	// Get Mouse Position
	//	Params:
	//		(x,y): position of WindowsHandle
	void GetMousePosition(int &x,int &y) {
		this->DoMessaging();			// get latest mouse position
		x=mx;	y=my;					// and return it
	};
	// Get Mouse Buttons
	//	Params:
	//		(left,right): state of mouse buttons
	void GetMouseButton(bool &left,bool &right) {
		this->DoMessaging();			// get latest mouse button status
		left=lbut; right=rbut;		// and return it
	};
	// Get Key Value
	//	Params:
	//		the key to test
	bool GetKey(int val) { 
		this->DoMessaging();	// get latest keys
		if(val<0 || val>255)	// is it a sensible value
			return false; 
		return keys[val];		// return the status
	};
	// Get the values of all the keys
	bool *GetKeys()	{ 
		this->DoMessaging();	// get latest keys
		return (bool *)keys;	// return the key array
	};

	//
	//	Data access procedure
	//
	void DataAccess(int key,int value);
};


#endif
