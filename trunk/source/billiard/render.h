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
	void DisableAllLights() const;
	void EnableAllLights() const;
	int GetNumLights() const;
	bool IsLightEnabled() const;

	void Resize( int x, int y, int width, int height );

	View* GetView();
	bool SetView( View *v );
	void ResetView();

	void Activate();
	void Deactivate();
	void ClearScreen();
private:
	Renderer();

	void InitGL();
	struct {
		int posx, posy, width, height;
	} Viewport;

	View *CurrentView;
	View* DefaultView;   // this view is created and maintained by the renderer
	Light* Lights[ MAX_LIGHTS ]; // renderer will not manage the memory of lights, so if you created a light, you must delete it yourself
	int LightIdx;

	static Renderer* Curr;
};

#endif
