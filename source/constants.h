#pragma once

/* Plugin IDs
 * 
 * See https://plugincafe.maxon.net/c4dpluginid_cp for used IDs
 * 
 */

// IM_SelectInstances
static const Int32 PID_IM_SELECT = 1050045;
static const char* PICON_IM_SELECT = "im_select.tif";


// IM_CreateInstances
static const Int32 PID_IM_CREATE = 1050047;
static const char* PICON_IM_CREATE = "im_create.tif";

// IM_RenameInstances
static const Int32 PID_IM_RENAME = 1050049;
static const char* PICON_IM_RENAME = "im_rename.tif";


// IM_SplitInstances
static const Int32 PID_IM_SPLIT = 1050046;
static const char* PICON_IM_SPLIT = "im_split.tif";


// IM_MakeEditable
static const Int32 PID_IM_MAKEEDITABLE = 1050105;
static const char* PICON_IM_MAKEEDITABLE = "im_makeeditable.tif";


// IM_SwapInstances
static const Int32 PID_IM_SWAP = 1050048;
static const char* PICON_IM_SWAP = "im_swap.tif";

// IM_FrameSelectedObjects
static const Int32 PID_IM_FRAMEOBJECTS = 1050104;
static const char* PICON_IM_FRAMEOBJECTS = "im_frameobjects.tif";

// IM_FrameSelectedElements
static const Int32 PID_IM_FRAMEELEMENTS = 1050103;
static const char* PICON_IM_FRAMEELEMENTS = "im_frameelements.tif";

// InstanceMan
static const Int32 PID_IM = 1050043;
static const char* PICON_IM = "im_settings.tif";

// Separator
static const Int32 PID_IM_SEPARATOR = 1050106;
static const char* PNAME_IM_SEPARATOR = "#$09--";

// Settings IDs
enum INSTANCEMANIDS : Int32
{
	DEFAULT_INSTANCEMODE = 0,
	INSTANCEMODE,
	SWAPTARGET,
};

// Cinema 4D Commands
#define COMMAND_FRAMEOBJECTS 12151
#define COMMAND_FRAMEELEMENTS 13038
