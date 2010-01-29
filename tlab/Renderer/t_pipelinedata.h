#ifndef _TRD_PIPELINEDATA_
#define _TRD_PIPELINEDATA_

#include "t_types.h"
#include "t_vector.h"
#include "t_matrix.h"

typedef struct _PIPELINEDATA {
	Vector4 				pos;

	Vector4 				color;

	Vector4 				normal;
	Vector4 				mat_ambient;
	Vector4 				mat_diffuse;
	Vector4 				mat_specular;
	F32 					mat_shin;

	Vector4 				coord;

	I32 					x, y;
} PIPELINEDATA;

typedef struct _ASSEM_POINT {
	U32 					a;  /* a is the index of the points in pipeline data */
} ASSEM_POINT;

typedef struct _ASSEM_LINE {
	U32 					a; /* a, b are the index of the points in pipeline data */
	U32 					b;
} ASSEM_LINE;

typedef struct _ASSEM_TRIANGLE {
	U32 					a;
	U32 					b;
	U32 					c;
} ASSEM_TRIANGLE;

typedef struct _ASSEMBLYDATA {
	ASSEM_POINT* 			pt_buffer;
	ASSEM_POINT* 			pt_end;

	ASSEM_LINE* 			line_buffer;
	ASSEM_LINE* 			line_end;

	ASSEM_TRIANGLE* 		tri_buffer;
	ASSEM_TRIANGLE* 		tri_end;
} ASSEMBLYDATA;

typedef struct _DISPLAYLIST {
	PIPELINEDATA* 			pt_buffer;
	PIPELINEDATA* 			pt_curr;
	PIPELINEDATA* 			pt_end;

	ASSEMBLYDATA 			assembled_data;

	struct _DISPLAYLIST* 	Next;
} DISPLAYLIST;

/* functions for assembly pipeline data, maybe these should be private functions */
void ASSEMBLY_Init( ASSEMBLYDATA* ass );
void ASSEMBLY_Destroy( ASSEMBLYDATA* ass );

/* */
void DISPLAYLIST_Init( DISPLAYLIST* dpl );
void DISPLAYLIST_Destroy( DISPLAYLIST* dpl );
void DISPLAYLIST_Assemble( DISPLAYLIST* dpl );

void DISPLAYLIST_AddVertex( DISPLAYLIST* dpl );

void DISPLAYLIST_SetPos( DISPLAYLIST* dpl, const Vector4* pos );
void DISPLAYLIST_SetColor( DISPLAYLIST* dpl, const Vector4* color );
void DISPLAYLIST_SetNormal( DISPLAYLIST* dpl, const Vector4* normal );
void DISPLAYLIST_SetDiffuse( DISPLAYLIST* dpl, const Vector4* diffuse );
void DISPLAYLIST_SetAmbient( DISPLAYLIST* dpl, const Vector4* ambient );
void DISPLAYLIST_SetSpecular( DISPLAYLIST* dpl, const Vector4* specular );
void DISPLAYLIST_SetShin( DISPLAYLIST* dpl, F32 shin );
void DISPLAYLIST_SetTexCoord( DISPLAYLIST* dpl, const Vector4* texcoord );
#endif // _TRD_PIPELINEDATA_
