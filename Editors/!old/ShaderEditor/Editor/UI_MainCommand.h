#ifndef UI_MainCommandH
#define UI_MainCommandH

enum{
	COMMAND_INITIALIZE,			// p1 - D3DWindow, p2 - TPanel
	COMMAND_DESTROY,
	COMMAND_QUIT,
	COMMAND_EDITOR_PREF,
	COMMAND_CHANGE_ACTION,
    COMMAND_IMAGE_EDITOR,
	COMMAND_CHECK_TEXTURES,
	COMMAND_REFRESH_TEXTURES,
	COMMAND_RELOAD_TEXTURES,
	COMMAND_CHANGE_AXIS,
	COMMAND_CHANGE_SNAP,
    COMMAND_CHANGE_SETTINGS,
    COMMAND_UNLOAD_TEXTURES,
    COMMAND_EVICT_OBJECTS,
    COMMAND_EVICT_TEXTURES,
    COMMAND_CHECK_MODIFIED,
	COMMAND_EXIT,
	COMMAND_SHOW_PROPERTIES,
	COMMAND_UPDATE_PROPERTIES,	// p1 - forced update if needed
    COMMAND_REFRESH_PROPERTIES,
	COMMAND_ZOOM_EXTENTS,
    COMMAND_RENDER_FOCUS,
    COMMAND_RENDER_RESIZE,		
    COMMAND_RENDER_WIRE,		// p1 - toggle to wireframe
    COMMAND_UPDATE_CAPTION,
	COMMAND_BREAK_LAST_OPERATION,
	COMMAND_UPDATE_TOOLBAR,
    COMMAND_TOGGLE_SAFE_RECT,
    COMMAND_TOGGLE_GRID,
	COMMAND_UPDATE_GRID,
    COMMAND_GRID_NUMBER_OF_SLOTS,
    COMMAND_GRID_SLOT_SIZE,
    
    COMMAND_REFRESH_UI_BAR,
    COMMAND_RESTORE_UI_BAR,
    COMMAND_SAVE_UI_BAR,

    COMMAND_MUTE_SOUND,

    // ����� ������ ����������
    COMMAND_CLEAR,
    COMMAND_LOAD,
    COMMAND_RELOAD,
    COMMAND_SAVE,
    COMMAND_SAVEAS,
    COMMAND_SAVE_BACKUP,

    COMMAND_CREATE_SOUND_LIB,

    // sound
	COMMAND_SOUND_EDITOR,
	COMMAND_SYNC_SOUNDS,
    
	COMMAND_UNDO,
	COMMAND_REDO,

    COMMAND_MAIN_LAST
};
//------------------------------------------------------------------------------

#endif //UI_MainCommandH



