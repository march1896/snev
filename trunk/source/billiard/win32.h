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

#ifndef _WIN32WINDOW_H_
#define _WIN32WINDOW_H_

#include <windows.h>

class Window {
	int			width,height;	// dimensions
	int			mx,my;			// mouse position
	bool		lbut,rbut;		// buttons positions
	bool		keys[256];		// key positions
	bool		sized;			// whether window has been sized
	HWND		hwnd;			// Windows handle
	HINSTANCE	hins;			// instance
	// Helper functions
	BOOL Register(char *name);
	void DoMessaging();
public:
	Window();
	~Window();
	// Setup function
	//	Params:
	//		(x,y):		position of window
	//		(wid,hit):	dimensions of window
	//		name:		name of window
	bool Setup(int x,int y,int wid,int hit,char *name);
	// Setup function with title
	//	Params:
	//		(x,y):		position of window
	//		(wid,hit):	dimensions of window
	//		name:		name of window
	bool SetupTitle(int x,int y,int wid,int hit,char *name);
	// Close window
	void Close();
	// Has the window been sized?
	bool BeenSized() { bool temp=sized; sized=false; return temp; };
	// Get Windows Handle
	HWND GetHandle() const		{ return hwnd; };
	// Get Windows Instance
	HINSTANCE GetInstance() const { return hins; };
	// Get Dimensions
	//	Params:
	//		wid,hit:	dimensions of window
	void GetDimensions(int &wid,int &hit) const	{
					wid=width;	hit=height;			// return the dimensions
				};
	// Get Mouse Position
	//	Params:
	//		(x,y): position of window
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
