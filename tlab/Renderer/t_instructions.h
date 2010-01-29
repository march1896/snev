#ifndef _TRD_INSTRUCTIONS_
#define _TRD_INSTRUCTIONS_

/* how to maintain the length of a enum, it's a problem:TODO */
#define ENUM_SIZE_HOLDER 0x5FFFFFFF
#pragma enum(4)
typedef enum _INS_OP {
	OP_LOCAL = 0,
	OP_SET1F,
	OP_SET2F,
	OP_SET3F,
	OP_SET4F,
	OP_SET3X3F,
	OP_SET4X4F,
	OP_END,
	OP_HOLDER = ENUM_SIZE_HOLDER;
} INS_OP;

typedef U8* INS_PARAM;

typedef struct _INSSINGLE {
	INS_OP 					op_code;
	INS_PARAM 				op_param;
} INSSINGLE;

typedef struct _INSCONTEXT {
	U8* 					buffer; /* instructions buffer */
	U8* 					end; 	/* the end of the buffer */
	U8* 					pc; 	/* pc point to the current instruction */
} INSCONTEXT;

/* construct a single instruction, and save it to global context */
void 	INSSINGLE_Init( INSSINGLE* _single );
void 	INSSINGLE_Destroy( INSSINGLE* _single );
void 	INSSINGLE_Construct( INSSINGLE* _single, INS_OP _op_code, INS_PARAM _op_param );

/* operation for a instructions context */
void 	INSCONTEXT_Init( INSCONTEXT* cont );
void 	INSCONTEXT_Destroy( INSCONTEXT* cont );

/* add a single instruction from the global context's current instruction */
INSSINGLE* INSCONTEXT_GetNext( INSCONTEXT* cont );
void 	INSCONTEXT_ConstructNext( INSCONTEXT* cont, INS_OP _op_code, INS_PARAM _op_param );

/* flush the operations, and make change for the pipeline data */
void 	INS_Compile( INSCONTEXT* cont );
#endif // _TRD_INSTRUCTIONS_
