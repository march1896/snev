#ifndef _RENDER_H_
#define _RENDER_H_

#include "light.h"
#include "vector.h"
#include "opengl.h"

class View;
class WindowsHandle;

//#define MAX_LIGHTS GL_MAX_LIGHTS
#define MAX_LIGHTS 4
//This number should less or equal than GL_MAX_LIGHTS

class Renderer
{
public:
	Renderer( WindowsHandle* winhandle );
	Renderer( int x, int y, int width, int height );
	~Renderer();

	bool AddLight( Light *lit );
	bool RemoveLight( Light *lit );
	int GetNumLights();
	View* GetDefaultView();
private:
	Renderer();

	void InitGL();
	// if a renderer is constructed, the position and the dimension could not be changed
	struct {
		int posx, posy, width, height;
	} Viewport;

	View *defaultview;
	Light* Lights[ MAX_LIGHTS ];
	int LightIdx;
};

#endif
