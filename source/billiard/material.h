#ifndef _MATERIAL_H_
#define _MATERIAL_H_

#include "color.h"

class Material {
public:
	Material();
	Material( const color &colour );
	Material( const color &Amb, const color &Diff, const color &Spec, const float &Shin );
	void SetAmbient( const color &para );
	void SetDiffuse( const color &para );
	void SetSpecular( const color &para );
	void SetShininess( const float &para );
	color GetAmbient() const;
	color GetDiffuse() const;
	color GetSpecular() const;
	float GetShininess() const;
private:
	color Ambient;
	color Diffuse;
	color Specular;
	float Shininess;
};
#endif
