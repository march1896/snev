#include "render.h"

Render::Render()
{
}

Render::~Render()
{
}

Render* Render::getInstance()
{
	static Render _instance;

	return &_instance;
}

void Render::Draw( RenderType Type )
{
}
