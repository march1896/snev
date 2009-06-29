// *****************************************************************************
//
// Layout.h
//
// Layout module - layouts are generated in GUI layout tool, loaded in the game
//                 as layout resources.  This module provides routines to
//                 display a layout, as well as accessors to be used by
//                 custom-draw layout menus.
//
// (c) 2003 Visual Concepts
// by Isaac Gartner
//    Nick Jones
//
// *****************************************************************************


#ifdef VCFETARGET
#define MAX_PATH 274
#endif

#ifdef VCFETARGET
typedef u32 SPREADSHEET_DRAWSTYLE;
#else
#include "spreadsheet.h"
#endif

///////////////////////////////////////////////////////////////////////////////
//
// External struct declarations
//
///////////////////////////////////////////////////////////////////////////////

struct MARKSCENE;
struct MARKSCENETEXT;
struct PROCESS_INSTANCE;

///////////////////////////////////////////////////////////////////////////////
//
// Layout type declarations
//
///////////////////////////////////////////////////////////////////////////////

struct LAYOUT;

enum LAYOUT_OBJECT_TYPE {
	SCENE,
	SPREADSHEET,
	TEMPLATE,
	ANIMATION,
};

typedef const void* LAYOUT_USERDATA;

//base class for all layout objects
struct LAYOUT_OBJECT {
	LAYOUT_OBJECT*                 Next;            // linked list pointer
	LAYOUT_OBJECT_TYPE             Type;
	string_crc                     NameHash;
	vector4                        Offset;
};

struct CALLBACK_OBJECT
{
	u32					Item;	
	u32					Func;
	LAYOUT_USERDATA		Param;
	LAYOUT_USERDATA		Param2;
    LAYOUT_USERDATA*    UserList;
	CALLBACK_OBJECT*	Next;
};

//Options for camera to be used when rendering a LAYOUT_SCENE
enum LAYOUT_CAMERA_TYPE {
	ORTHOGRAPHIC,
	PERSPECTIVE,
};

//Options for scene animation
enum ANIMATION_MODE {
	NONE,
	ONE_SHOT,
	LOOPING,
	MULTISTATE,
	USER,
};

enum LAYOUT_TRI_STATE_VALUE {
	DEFAULT,
	TRUE,
	FALSE
};

enum LAYOUT_SCENE_STATE {
	START,
	SECTION2,
	SECTION3,
	END
};

enum LAYOUT_BONE_MODIFIER_TYPE {
    X_TRANSLATE,
    Y_TRANSLATE,
    Z_TRANSLATE,
};

struct LAYOUT_BONE_MODIFIER;

//A scene within a layout
struct LAYOUT_SCENE extends LAYOUT_OBJECT {
	
	//
	// Tool-specified data
	//
	
	char*                            SceneFilename;
	float                            LoopStartTime     	=  0;
	float                            LoopEndTime       	=  0;
	float                            AnimTimeInit       =  0;
	bool							 RenderZPrePass:1  	=  false;
	bool                             IsMarkScene:1     	=  false;
	bool							 Enabled:1		   	=  true;
	LAYOUT_TRI_STATE_VALUE           UseSceneLights:2  	=  DEFAULT;
	LAYOUT_CAMERA_TYPE               Camera:1           =  ORTHOGRAPHIC;
	ANIMATION_MODE                   AnimationMode:3    =  ONE_SHOT;
	LAYOUT_SCENE_STATE               State:2            =  START;	// run-time

	CALLBACK_OBJECT					 Callback;			//head of the callback list
	CALLBACK_OBJECT*				 MaterialCallback  =  null;
	CALLBACK_OBJECT*				 TextCallback	   =  null;
	CALLBACK_OBJECT*				 LocatorCallback   =  null;

	// Bone modifier data
	LAYOUT_BONE_MODIFIER*            BoneModifierList  =  null;
	uint                             BoneModifierCount =  0;

	// Run-time data
	VCSCENE*                         Scene;
	MARKSCENE*                       MarkScene;
	float							 StartTime		   	=  0;
	float							 EndTime		   	=  0;
	float                            Time;

};

struct LAYOUT_SPREADSHEET extends LAYOUT_OBJECT {
	uint                             Width;
	uint                             Height;
	SPREADSHEET_DRAWSTYLE            DrawStyle;
};

struct LAYOUT_TEMPLATE    extends LAYOUT_OBJECT {
	char*                            TemplateFilename;
	const void*                      Context;
	LAYOUT*                          Layout;
};

struct LAYOUT_ANIMATION extends LAYOUT_OBJECT {
	int                              StartFrame		= 0;
	int                              EndFrame		= 0;
	int                            	 LoopStartFrame = 0;
	int                            	 LoopEndFrame   = 0;
	ANIMATION_MODE                   Mode    		= ONE_SHOT;
};


// Data-types for specifying callbacks for dynamic scene attributes

struct LAYOUT_TEXT_DATA {

	const void * 			LayoutContext;
	LAYOUT_USERDATA 		User;
    LAYOUT_USERDATA         User2;
    LAYOUT_USERDATA*        UserList;
	char * 					Buffer;
	uint					BufferSize;
	PROCESS_INSTANCE * 		Instance;
	VCTEXT * 				TextContext;

	const MARKSCENETEXT * 	MarkSceneText;
};

struct LAYOUT_MATERIAL_DATA {

	const void * 			LayoutContext;
	LAYOUT_USERDATA 		User;
    LAYOUT_USERDATA         User2;
    LAYOUT_USERDATA*        UserList;
	VCMATERIAL * 			Material;
	PROCESS_INSTANCE * 		Instance;
};

struct LAYOUT_NODE_DATA {

	const void * 			LayoutContext;
	LAYOUT_USERDATA 		User;
    LAYOUT_USERDATA         User2;
    LAYOUT_USERDATA*        UserList;
	matrix * 				Matrix;
	float 					Visibility;	// called Visibility by MarkScene, but many people use it as Alpha... what really is this value? -TLM
	PROCESS_INSTANCE * 		Instance;
};

struct LAYOUT_SCENE_DATA {

	LAYOUT_USERDATA			User;
    LAYOUT_USERDATA   		User2;
    LAYOUT_USERDATA*        UserList;
	LAYOUT_SCENE * 			LayoutScene;
	PROCESS_INSTANCE * 		Instance;
    vector4*                Offset;
};

struct LAYOUT_SPREADSHEET_DATA {

	const LAYOUT_SPREADSHEET * LayoutSpreadSheet;

};

typedef void ( LAYOUT_TEXT_FUNC )			 ( const LAYOUT_TEXT_DATA*        );
typedef void ( LAYOUT_MATERIAL_FUNC )		 ( const LAYOUT_MATERIAL_DATA*    );
typedef void ( LAYOUT_CUSTOM_DRAW_FUNC )	 ( const LAYOUT_NODE_DATA*        );
typedef void ( LAYOUT_SCENE_CALLBACK_FUNC )  ( const LAYOUT_SCENE_DATA*       );
typedef void ( LAYOUT_SPREADSHEET_FUNC )	 ( const LAYOUT_SPREADSHEET_DATA* );

struct LAYOUT_FUNC_DEFINITION
{
	string_crc crc;
	void* func;
};

// **********

enum LAYOUT_CUSTOM_DRAW_SCENE {
	FLIPCHIP,
	LOGOCOIN,
	LOCATOR,
	NONE
};

struct LAYOUT_DYNAMIC_TEXT_NODE
{
	string_crc 					   SceneNameHash;
	string_crc 					   NodeNameHash;
	LAYOUT_TEXT_FUNC * 			   Func;
	LAYOUT_USERDATA 			   Context;
    LAYOUT_USERDATA 			   Context2;
    LAYOUT_USERDATA*               UserList;
};

struct LAYOUT_CUSTOM_DRAW_NODE
{
	string_crc                     SceneNameHash;
	const char*                    NodeName;
	LAYOUT_CUSTOM_DRAW_FUNC *      Func;
	LAYOUT_CUSTOM_DRAW_SCENE       SceneType      = NONE;
	LAYOUT_USERDATA                Context        = null;
	LAYOUT_USERDATA                Context2       = null;
	LAYOUT_USERDATA*               UserList       = null;
};

struct LAYOUT_DYNAMIC_MATERIAL
{
	string_crc                     SceneNameHash;
	const char*                    MaterialName;
	LAYOUT_MATERIAL_FUNC*          Func;
	LAYOUT_USERDATA                Context          = null;
	LAYOUT_USERDATA                Context2         = null;
	LAYOUT_USERDATA*               UserList         = null;
};


struct LAYOUT_SCENE_CALLBACK
{
	string_crc					   SceneNameHash;
	LAYOUT_SCENE_CALLBACK_FUNC*    Func;
	LAYOUT_USERDATA                Context          = null;
	LAYOUT_USERDATA                Context2         = null;
	LAYOUT_USERDATA*               UserList         = null;
};

struct LAYOUT_BONE_MODIFIER
{
	string_crc                     SceneNameHash;
	string_crc                     ObjectNameHash;
	string_crc                     NodeNameHash;
	vector4                        Offset;
};

struct LAYOUT_INIT {
	LAYOUT_DYNAMIC_TEXT_NODE*      DynamicText      = null;
	LAYOUT_CUSTOM_DRAW_NODE*       CustomDrawNodes  = null;
	LAYOUT_DYNAMIC_MATERIAL*       DynamicMaterials = null;
	LAYOUT_USERDATA                Context          = null;
	LAYOUT_USERDATA                Context2         = null;
	LAYOUT_USERDATA*               UserData         = null;
	LAYOUT_SCENE_CALLBACK*		   SceneCallbacks   = null;
	LAYOUT_BONE_MODIFIER*          BoneModifiers    = null;
};

struct LAYOUT {
	string_crc					   Status					= "uninitialized";
	string_crc                     LayoutNameHash;
	LAYOUT_OBJECT*                 ObjectList               = null;
	LAYOUT_SPREADSHEET_FUNC*       DrawSpreadsheetCallback  = null;
	LAYOUT_DYNAMIC_TEXT_NODE*      DynamicText              = null;
	LAYOUT_CUSTOM_DRAW_NODE*       CustomDrawNodes          = null;
	LAYOUT_DYNAMIC_MATERIAL*       DynamicMaterials         = null;
	LAYOUT_SCENE_CALLBACK*		   SceneCallbacks    		= null;
	LAYOUT_BONE_MODIFIER*          BoneModifiers            = null;

	float                          DeltaTime;

	// Context is a data structure that is defined by each layout.
	// For example, the Context structure for a layout that displays
	// player stats will be defined to contain a pointer to a player.
	// When the layout gets draw, the Context gets passed down to the
	// text callbacks so they know which player's data to display
	void*                          Context;
};

///////////////////////////////////////////////////////////////////////////////
//
// Public functions
//
///////////////////////////////////////////////////////////////////////////////

#ifdef LAYOUTVIEWER
void Layout_SetDrawSpreadsheetCallback( LAYOUT* Layout, LAYOUT_SPREADSHEET_FUNC* CallbackFunction );
#endif


void               LayoutScene_SetEnabled           ( LAYOUT_SCENE* LayoutScene, bool Enabled );
float              LayoutScene_GetTime              ( LAYOUT_SCENE* LayoutScene );
void               LayoutScene_SetTime              ( LAYOUT_SCENE* LayoutScene, float Time );
VCSCENE*           LayoutScene_GetScene             ( LAYOUT_SCENE* LayoutScene );

LAYOUT_ANIMATION * Layout_GetAnimation				( LAYOUT * Layout, string_crc NameHash );

void 			   Layout_AdvanceState				( LAYOUT* Layout );

void 			   Layout_StartSceneAnimation		( LAYOUT* Layout, string_crc SceneHash, string_crc AnimHash );
void 			   Layout_SetSceneAnimationState	( LAYOUT* Layout, string_crc SceneHash, LAYOUT_SCENE_STATE State );
void               Layout_SetSceneTime              ( LAYOUT* Layout, string_crc NameHash, float Time );

void 			   Layout_StretchView				( VCVIEW* View, bool usePerspective );

void               Layout_Update                    ( LAYOUT* Layout, float deltaTime );
void               Layout_Draw                      ( LAYOUT* Layout, PROCESS_INSTANCE * Instance );
void               Layout_DrawWithCurrentView		( LAYOUT* Layout, PROCESS_INSTANCE * Instance, const vector4 * Offset );
void               Layout_DrawWithOffset    		( LAYOUT* Layout, PROCESS_INSTANCE * Instance, const vector4 * Offset );

LAYOUT_OBJECT*     Layout_GetObject                 ( LAYOUT* Layout, string_crc NameHash );
VCSCENE*           Layout_GetVCScene                ( LAYOUT* Layout, string_crc NameHash );
LAYOUT_SCENE*      Layout_GetScene                  ( LAYOUT* Layout, string_crc NameHash );
void               Layout_SetSceneEnable            ( LAYOUT* Layout, string_crc NameHash, bool Enable);
void               Layout_SetObjectOffset           ( LAYOUT* Layout, string_crc NameHash, vector4* Offset);
vector4*           Layout_GetObjectOffset           ( LAYOUT* Layout, string_crc NameHash );

void               Layout_Init                      ( LAYOUT* Layout, LAYOUT_INIT* LayoutInit );
void               Layout_Deinit                    ( LAYOUT* Layout );

void 			   Layout_DrawSceneExternal			( VCSCENE * Scene, const vector4 * Offset, bool usePerspective );

void               Layout_SetupParallelView         ( VCVIEW * View, const vector4 * Offset, bool setupLights );

