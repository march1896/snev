#include "material.h"

Material::Material():
	Ambient( 0.2, 0.2, 0.2, 1.0 ),
	Diffuse( 0.8, 0.8, 0.8, 1.0 ),
	Specular( 0.0, 0.0, 0.0, 1.0 ),
	Shininess( 0.0 )
{
}

Material::Material( const color &colour ):
	Ambient( colour ),
	Diffuse( colour ),
	Specular( 0.0, 0.0, 0.0, 1.0 ),
	Shininess( 0.0 )
{
}

Material::Material( const color &Amb, const color &Diff, const color &Spec, const float &Shin ):
	Ambient( Amb ),
	Diffuse( Diff ),
	Specular( Spec ),
	Shininess( Shin )
{
}

void Material::SetAmbient( const color &para ) { 	Ambient = para; 	}
void Material::SetDiffuse( const color &para ) { 	Diffuse = para; 	}
void Material::SetSpecular( const color &para ) { 	Specular = para; 	}
void Material::SetShininess( const float &para ) { 	Shininess = para; 	}
color Material::GetAmbient() const { 	return Ambient; 	}
color Material::GetDiffuse() const { 	return Diffuse; 	}
color Material::GetSpecular() const { 	return Specular; 	}
float Material::GetShininess() const { 	return Shininess; 	}
