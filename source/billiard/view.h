#ifndef _VIEW_H_
#define _VIEW_H_

#include "vector.h"
#include "matrix.h"

// Attention!!!
// In OpenGL, the first operation comes last, eg. if you want to first rotate a object then translate 
// you will call gl function in this order
//  		glTranslatef( ... );
//  		glRotatef( ... );
// But, here we must use the normal order, that is, the first operation comes first;

class View {
public:
	View( int width, int height );
	~View();
	void Activate();
	void Deactivate();
	bool IsActive();
	void Reset();

	void Perspective( float fovy, float aspect, float near, float far );
	void Ortho2D( float left, float right, float bottom, float top );

	void Translate(float x,float y,float z);
	void Translate(vector3 vec)			{ this->Translate(vec.getX(),vec.getY(),vec.getZ()); };
	void Rotate(float angle,float x,float y,float z);
	void Rotate(float angle,vector3 vec) { this->Rotate(angle,vec.getX(),vec.getY(),vec.getZ()); };
	void Scale(float x,float y,float z);
	void Scale(vector3 vec)				{ this->Scale(vec.getX(),vec.getY(),vec.getX()); };
	void LookAt(vector3 pos,vector3 at,vector3 up);
private:
	View();
	matrix44 Projection;
	matrix44 Modelview;
	float AspectRatio;
	bool isActive;
};
#endif
