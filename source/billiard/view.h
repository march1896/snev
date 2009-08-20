#ifndef _VIEW_H_
#define _VIEW_H_

#include "vector.h"
#include "matrix.h"

class View {
public:
	View( int width, int height );
	~View();
	void Activate();
	void Deactivate();
	bool IsActive();
	void Reset();
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
