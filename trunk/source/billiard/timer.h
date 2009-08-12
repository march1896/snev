/*	Nehe SDK
	Copyright 2002 Jeff Molofee, Gregory Austwick and Others

	Title:
		Timer class

	Version History:
		v1.00			New file (5/11/02)


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


#ifndef TIMER_H
#define TIMER_H

#include <windows.h>

// load in the multimedia library
//#ifdef _MSC_VER
//#pragma comment(lib,"winmm.lib")
//#endif

class Timer {
	bool	performance;		// whether the performance timer is available
	float	resolution;			// timer resolution
	DWORD	mm_start;			// multimedia timer start
	__int64	perf_start;			// performance timer start
public:
	Timer();
	// get time in milliseconds
	float GetTime();
};

class Frame {
	Timer	*time;		// a timer object
	float	lasttime;	// last time called
	int		fps;		// frames per second
	float	length;		// length of frame
public:
	Frame();
	// Update frame counter *call once a frame*
	void Update();
	// Get frames per second
	int GetFPS()		{ return fps; };
	// get the time of the last frame
	float GetLength()	{ return length; };
};

#endif
