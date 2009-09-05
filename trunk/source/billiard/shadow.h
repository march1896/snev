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
/*******************************************************************
 * the above shadow volume should be used like this..
 * 1,render the scene, warm up the depth buffer.
 * 2,call like this:
 * 	PreDrawShadow();
 * 	DrawShadowVolume();
 * 	DrawShadow();
 *******************************************************************/

// below is the code fragment of shadow map, 
void PreShadowMap( Light * light );
// draw first pass
void DuringShadowMap();
// draw second pass
void PostShadowMap();
#endif 
