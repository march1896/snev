#ifndef _CONTROL_H_
#define _CONTROL_H_

class WindowsHandle;

class Control {
public:
	Control( WindowsHandle *wh );

	bool IsPressed( int KeyValue );
	bool IsMLeftPressed();
	bool IsMRightPressed();
	void GetMousePosition( int &x, int &y );
	void GetMousePressed( bool &left, bool &right );
private:
	// Control Data
	Control();
	WindowsHandle *winHandle;
};


#endif

