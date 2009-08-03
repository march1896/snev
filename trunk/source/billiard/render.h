#ifndef _RENDER_H_
#define _RENDER_H_
class Render
{
public:
	enum RenderType
	{
		BALL,
		TABLE
	};
	void Draw( RenderType Type );
	static Render* getInstance();
private:
	Render();
	~Render();
};
#endif
