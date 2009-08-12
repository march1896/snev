#include "view.h"

void View :: Reset()
{
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void View :: Translate(float x,float y,float z)
{
	// translate by (x,y,z)
	glTranslatef(x,y,z);
}

void View :: Rotate(float angle,float x,float y,float z)
{
	// rotate axis (x,y,z), angle
	glRotatef(angle,x,y,z);
}

void View :: Scale(float x,float y,float z)
{
	// scale by (x,y,z)
	glScalef(x,y,z);
}

void View :: Pos2D(int x,int y)
{
	glRasterPos2i(x,y);
}

void View :: Pos2D(float x,float y)
{
	glRasterPos2f(x,y);
}

void View :: LookAt(vector3 pos,vector3 at,vector3 up)
{
	gluLookAt(pos.getX(),pos.getY(),pos.getZ(),at.getX(),at.getY(),at.getZ(),up.getX(),up.getY(),up.getZ());
}

void View :: Save()
{
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
}

void View :: Restore()
{
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
}

