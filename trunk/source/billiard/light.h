#ifndef _LIGHT_H_
#define _LIGHT_H_

#include "color.h"
#include "vector.h"
#include "matrix.h"

class Light {
public:
	Light();
	Light( vector4 pos, color amb, color diff, color spec );
	Light( const Light& para );
	~Light();

	void setPos( const vector4 &pos );
	void setAmbient( const color &para );
	void setDiffuse( const color &para );
	void setSpecular( const color &para );
	vector4 getPos() const;
	color getAmbient() const;
	color getDiffuse() const;
	color getSpecular() const;
	Light operator=( const Light& para ); 
private:
	vector4 position;
	color ambient;
	color diffuse;
	color specular;
};
#endif
