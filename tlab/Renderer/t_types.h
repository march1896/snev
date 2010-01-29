#ifndef _TRD_TYPEDEF_
#define _TRD_TYPEDEF_
/* typedefs for basic type */
typedef char 			U8;
typedef short 			I16;
typedef unsigned short 	U16;
typedef int 			I32;
typedef unsigned int 	U32;
typedef float 			F32;

struct Mtx2;
struct Mtx3;
struct Mtx4;
typedef Mtx2* 			Mtx2Ptr;
typedef Mtx3* 			Mtx3Ptr;
typedef Mtx4* 			Mtx4Ptr;

struct Vector2; 
struct Vector3;
struct Vector4;
typedef struct Vector2 	Vec2;
typedef struct Vector3 	Vec3;
typedef struct Vector4 	Vec4;
typedef Vec2* 			Vec2Ptr;
typedef Vec3* 			Vec3Ptr;
typedef Vec4* 			Vec4Ptr;

struct DISPLAYLIST;
typedef DISPLAYLIST* 	DISPLAYLISTPtr;

#endif //_TRD_TYPEDEF_
