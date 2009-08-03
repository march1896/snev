#ifndef _OBJECT_H_
#define _OBJECT_H_
class Object
{
public:
	Object();
	virtual ~Object();
private:
};

class VisibleObject: public Object
{
public:
	VisibleObject();
	virtual void Draw();
	virtual ~VisibleObject();
private:
};
#endif
