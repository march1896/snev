#include "light.h"

Light::Light(): position(), ambient(), diffuse(), specular() {}
Light::Light( vector4 pos, color amb, color diff, color spec ): position( pos ), ambient( amb ), diffuse( diff ), specular( spec ) {}
Light::Light( const Light& para ) {
	this->position = para.getPos();
	this->ambient = para.getAmbient();
	this->diffuse = para.getDiffuse();
	this->specular = para.getSpecular();
}
Light::~Light() {}

void Light::setPos( const vector4 &pos ) 	{ 	this->position = pos; 	}
void Light::setAmbient( const color &amb ) 	{ 	this->ambient = amb; 	}
void Light::setDiffuse( const color &diff ) 	{ 	this->diffuse = diff; 	}
void Light::setSpecular( const color &spec ) 	{ 	this->specular = spec; 	}

vector4 Light::getPos() const		{ 	return this->position; 	}
color 	Light::getAmbient() const	{ 	return this->ambient; 	}
color 	Light::getDiffuse() const 	{ 	return this->diffuse; 	}
color 	Light::getSpecular() const	{ 	return this->specular; 	}

Light Light::operator=( const Light& para ) {
	if ( this == &para ) return *this;
	this->position = para.getPos();
	this->ambient = para.getAmbient();
	this->diffuse = para.getDiffuse();
	this->specular = para.getSpecular();
	return *this;
}

