#include "control.h"
#include "win32.h"


Control::Control( WindowsHandle *wh ):
	winHandle( wh )
{
}

bool Control::IsPressed( int KeyValue ) {
	return winHandle->GetKey( KeyValue );
}

bool Control::IsMLeftPressed() {
	bool left, right;
	winHandle->GetMouseButton( left, right );
	return left;
}

bool Control::IsMRightPressed() {
	bool left, right;
	winHandle->GetMouseButton( left, right );
	return right;
}

void Control::GetMousePosition( int &x, int &y ) {
	winHandle->GetMousePosition( x, y );	
}

void Control::GetMousePressed( bool &left, bool &right ) {
	winHandle->GetMouseButton( left, right );
}

