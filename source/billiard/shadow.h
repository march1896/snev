#ifndef _SHADOW_H_
#define _SHADOW_H_
/*****************************************
 * this file generate shadow for ball use shadow volume method
 ****************************************/
class Ball;
class Light;

void ConstructShadowModel( Ball* ball );
void DrawShadow();
void PreDrawShadow();
void EndDrawShadow();
void DrawShadowVolume( Ball* ball, Light* light );

void DrawShadowModel( Ball* ball );

#endif 
