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
#include "timer.h"

//
//	Timer class
//

Timer :: Timer()
{
	__int64 frequency;	// The performance counter frequency

	// check for performance counter
	if(QueryPerformanceFrequency((LARGE_INTEGER *) &frequency))
	{
		// Use the performance counter to get the present time
		QueryPerformanceCounter((LARGE_INTEGER *) &perf_start);
		// say this computer has a performance counter
		performance=true;
		// calculate the resolution from the frequency
		resolution=(float) ( ((double)1.0f)/((double)frequency) );
	}else{	
		// No performance counter available
		performance=false;
		// Get the present time
		mm_start=timeGetTime();
		// calculate the resolution
		resolution=1.0f/1000.0f;
	}
}

float Timer :: GetTime()
{
	// get the time from the available counter
	if(performance)
	{
		__int64 time;			// the current time

		// get the time
		QueryPerformanceCounter((LARGE_INTEGER *) &time);
		// calculate it in milliseconds
		return ( (float)(time-perf_start)*resolution )*1000.0f;
	}else
		return ( (float)(timeGetTime()-mm_start)*resolution )*1000.0f;
}

//
//	Frame class
//

Frame :: Frame()
{
	// allocate a new timer
	time=new Timer;
	// get the present time
	lasttime=time->GetTime();
	// clear everything else
	fps=0;
	length=0.0f;
}

void Frame :: Update()
{
	// get the the present time
	float now=time->GetTime();

	// calculate the length of the last frame
	length=now-lasttime;
	// make sure it's sensible
	if(length<=0.0f)
		length=1.0f;

	// and the frames per second
	fps=(int)(1000.0f/length);

	// and save the last time
	lasttime=now;
}
