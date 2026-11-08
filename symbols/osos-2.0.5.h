/* Generated from the symbols file for OSOS 2.0.5. */
#pragma once

#define OSOS_VERSION "2.0.5"
#define OSOS_RUNTIME_BASE 0x08000000
#define OSOS_RUNTIME_FILE_OFFSET 0x0000b798
#define OSOS_NATIVE_HEAP_BASE 0x08b3b1b0

#ifndef __ASSEMBLER__
#include <stdint.h>

struct MediaEntry;
struct MediaItem;
struct MediaList;
struct NativeString;
struct ThreeBandEQ;

void *Heap_AllocateZeroed(uint32_t byte_count);
void Heap_Free(void *allocation);
void *Runtime_OperatorNew(uint32_t byte_count);
int Mutex_Lock(void *mutex);
int Mutex_Unlock(void *mutex);
int HoldSwitch_GetLogicalState(void);
void System_HoldChanged(void *system, int locked);
int File_OpenLegacyPath(void *path, uint32_t fork, int mode, void **out_handle);
int File_ReadLegacyHandle(void *handle, void *buffer, uint32_t bytes, uint32_t flags, uint32_t *out_read);
int File_WriteLegacyHandle(void *handle, const void *buffer, uint32_t bytes, uint32_t flags, uint32_t *out_written);
int File_CloseLegacyHandle(void *handle);
int BufferedFile_GetPosition(void *file, uint64_t *position);
void Preferences_LoadFile(void *preferences);
void *CppString_Init(void *string, const char *text);
void *CppString_InitBytes(void *string, const void *data, uint32_t bytes, void *allocator);
void CppString_Destroy(void *string);
void StringStream_Init(void *stream, void *string, uint32_t mode);
void StringStream_Destroy(void *stream, uint32_t flags, void *storage);
struct NativeString *NativeString_InitEmpty(struct NativeString *string);
struct NativeString *NativeString_Destroy(struct NativeString *string);
char *NativeString_GetUtf8(struct NativeString *string);
void NativeString_AssignUtf16(struct NativeString *string, const uint16_t *text, int characters);
int SongText_CompareUtf16(void *collation, const uint16_t *left, uint32_t left_units, const uint16_t *right, uint32_t right_units);
int StringCache_ReadToken(void *cache, uint32_t token, void *destination, uint32_t *bytes, uint32_t maximum_bytes);
int StringCache_ReleaseToken(void *cache, uint32_t token);
int StringCache_GetTokenView(void *cache, uint32_t token, void **data, uint32_t *bytes);
void ResourceBank_InitOverrides(void *bank);
void ResourceBank_SetResource(void *bank, uint32_t type, uint32_t id, const void *data, uint32_t bytes);
void *ResourceBank_GetResource(void *bank, uint32_t type, uint32_t id, uint32_t *out_bytes);
const void *ResourceBank_NextResource(void *bank, uint32_t *id, uint32_t *bytes);
uint32_t *ResourceNames_FindOrInsertSlot(void *map, const void *name);
void ResourceBlockList_AppendBorrowedBlock(void *list, const void *data, uint32_t bytes, uint32_t metadata);
char *ResourceService_GetString(void *service, uint32_t id);
void *ResourceService_GetBitmap(void *service, uint32_t id);
void *UI_GetResourceBank(void);
void *UI_GetResourceService(void);
void *UI_GetService(uint32_t service_id);
void *UI_GetRootView(void);
void *UI_GetSelectionModel(void);
void *UI_GetControllerRegistry(void);
void *UI_CreateScreenController(void *registry, uint32_t screen_id, uint32_t layout_id, uint32_t transition);
void UI_RegisterController(void *registry, const char *name, void *ordinary_factory, void *resource_factory);
void UI_RegisterViewFactory(uint32_t view_type, void *create);
void UI_LoadLayoutEventTables(void *layout);
void UI_ReadEventMap(void *stream, void *map);
void *ScreenController_CreateNamed(const char *name, uint32_t screen_id, uint32_t layout_id, uint32_t transition);
int ScreenController_HandleEvent(void *controller, void *event);
uint32_t Controller_Activate(void *controller, uint32_t entering);
uint32_t Controller_Deactivate(void *controller, uint32_t leaving);
int MenuItem_SetVisible(uint32_t item_id, uint32_t visible);
void ContextMenuItems_Append(void *items, uint32_t item_id, const char *action, uint32_t flags);
void Wheel_DecodeEvent(void *event);
uint32_t Button_EventToUIKey(void *event);
void *WheelAccumulator_Init(void *state, int threshold, uint8_t accelerate, uint32_t interval);
int WheelAccumulator_Step(void *state, int motion, uint32_t speed);
void WheelAccumulator_Reset(void *accumulator);
void View_InitOptions(const void *descriptor, void *options);
void *View_Construct(void *view, void *unused, void *parent, void *event_target, const void *options);
void View_BindGraphics(void *view, void *context);
void View_GetLocalRect(void *view, int32_t *rect);
void View_InvalidateRect(void *view, const int32_t *rect);
void *GraphicsContext_Construct(void *context);
void GraphicsContext_FillRect(void *context, const int32_t *rect);
void GraphicsContext_SetColor(void *context, const uint8_t *rgba);
void GraphicsContext_SetBackgroundColor(void *context, const uint8_t *rgba);
void GraphicsContext_SetFont(void *context, const uint32_t *font);
void GraphicsContext_SetPen(void *context, int x, int baseline);
void GraphicsContext_SetMode(void *context, uint8_t mode);
void GraphicsContext_DrawText(void *context, const char *text);
int GraphicsContext_TextWidth(void *context, const char *text);
void Font_Construct(uint32_t *font, const char *family, uint32_t size, uint8_t style);
int Font_Ascent(void *context);
int Font_Descent(void *context);
void Surface_FillRect(void *surface, const int32_t *rect, const uint8_t *rgba, uint32_t mode, void *clip);
void GLES_ClearColorx(int red, int green, int blue, int alpha);
uint32_t RGB565_BlendFixed(uint32_t first, uint32_t second, int weight);
int Settings_HandleAction(void *controller, const char *action, uint32_t argument);
int SettingsModel_GetProperty(void *model, uint32_t type, uint32_t property, uintptr_t *out);
int SettingsModel_GetIndexedProperty(void *model, uint32_t type, uint32_t property, uint32_t index, uintptr_t *out);
void SettingsModel_ToggleMainMenuItem(void *model, uint32_t index);
void SettingsModel_ToggleMusicMenuItem(void *model, uint32_t index);
void *SettingsModel_ApplyMusicMenuPreferences(void *model);
void *SettingsModel_ResetMainMenu(void *model);
void *SettingsModel_ResetMusicMenu(void *model);
void *SettingsModel_GetEQPresetPreview(void *model, uint32_t menu_index);
void *SettingsModel_GetEQPresetName(void *model, uint32_t menu_index);
void EQ_LoadPresetCoefficients(struct ThreeBandEQ *eq);
void EQ_ResetAllBiquadStates(struct ThreeBandEQ *eq);
void EQ_ResetBiquadState(struct ThreeBandEQ *eq, uint32_t stage);
void EQ_ProcessBiquadStereoS16(struct ThreeBandEQ *eq, const int16_t *input, int16_t *output, uint32_t frames, uint32_t stage);
void EQ_ProcessThreeBiquadsInPlace(struct ThreeBandEQ *eq, int16_t *input, uint32_t frames, int16_t **output, uint32_t *output_frames);
uint32_t EQ_SettingsIdToDspPreset(uint32_t settings_id);
void Audio_SetPlaybackSource(void *source);
void *MediaPlayer_GetInstance(void);
int MediaPlayer_ClearPreparedTrack(void *player);
int MediaPlayer_RebuildArtwork(void *player);
int MediaItem_IsValid(struct MediaItem *item);
struct MediaItem *MediaItem_InitFromRecord(struct MediaItem *item, void *record, uint8_t flags);
uint32_t MediaItem_HasRecordFlag8F01(struct MediaItem *item);
uint32_t MediaItem_HasKind8(struct MediaItem *item);
uint32_t MediaItem_HasKind8062(struct MediaItem *item);
uint32_t MediaItem_HasKind200000(struct MediaItem *item);
int MediaListView_IsValid(void *list_view);
struct MediaEntry *PlaybackList_GetEntryByIndex(void *list, uint32_t index);
void MediaList_InvalidateOrderCache(struct MediaList *list, uint32_t order_mode);
void *MediaList_CopyEntryAtPosition(void *source, void *destination, void *cursor, uint32_t flags);
struct MediaEntry *MediaList_GetEntryByOrderIndex(struct MediaList *list, int order_mode, uint32_t reverse, uint32_t index);
int MediaList_AddToOnTheGo(void *controller);
int MediaList_DispatchAction(void *controller, const char *action, uint32_t argument);
int OnTheGo_AddBrowser(void *otg, int row, void *browser, int save);
uint32_t MediaListSelection_SelectArtworkSource(void *selection, uint32_t category);
void ArtworkSession_Stop(void *session);
void *AlbumArtworkProvider_Create(uint32_t source_kind, uint32_t selector);
void *Genius_GetInstance(void);
int Genius_MapVisibleRow(void *genius, int row);
void Genius_GetItem(void *genius, uint32_t row, void *item);
int MainMediaMenu_DispatchCommonAction(void *controller, const char *action, uint32_t argument);
int NowPlaying_DispatchAction(void *controller, const char *action, uint32_t argument);
int SongList_DispatchAction(void *controller, const char *action, uint32_t argument);
int NestedPlaylists_DispatchAction(void *controller, const char *action, uint32_t argument);
int Genius_DispatchAction(void *controller, const char *action, uint32_t argument);
int ITunesDB_ImportTrackDataset(void *reader);
int ITunesDB_ImportCachedString(void *reader, uint32_t encoding, uint32_t mhod_type, void *cache, uint32_t *token, uint32_t glyph_field, int collect);
int ITunesDB_SkipImportBytes(void *reader, int bytes);
void TrackData_AcquireStringCaches(void *track_data);
void TrackData_ReleaseStringCaches(void *track_data);
void TrackData_DestroyStorage(void *track_data);
void *MediaBrowser_GetOwner(void *browser);
struct MediaList *MediaBrowser_GetCurrentList(void *browser);
int MediaBrowser_CreateChild(void *owner, void **out);
int MediaBrowser_EvaluateEntryFilters(void *browser, struct MediaEntry *entry, struct MediaList *list_override);
int MediaBrowser_InvalidateEntryMatches(void *browser);
void MediaBrowser_ClearArtists(void *browser);
void MediaBrowser_ClearAlbums(void *browser);
void MediaBrowser_ClearSongs(void *browser);
uint32_t MediaBrowser_GetRecordArtworkId(void *owner, void *record);
int MediaBrowser_AccumulateArtistGroup(void *browser, void *callback, uint32_t field_low, uint32_t field_high, uint32_t index, struct MediaEntry *entry, void *record, int first);
int MediaBrowser_AccumulateAlbumGroup(void *browser, void *callback, uint32_t field_low, uint32_t field_high, uint32_t index, struct MediaEntry *entry, void *record, int first);
uint32_t MediaBrowser_BuildArtists(void *browser);
int MediaBrowser_BuildSongs(void *browser);
int MediaBrowser_ReadArtistName(void *browser, int index, uint16_t *counted_out);
int MediaBrowser_ReadArtistJumpLabel(void *browser, int index, uint16_t *counted_out);
uint32_t MediaBrowser_GetOrderMode(void *browser);
void AlbumGroupVector_Append(void *vector, const void *row);
void *LibraryBrowser_Create(uint32_t ownership);
void *LibraryBrowser_InitBorrowed(void *model, uint8_t item_flags, void *browser);
void *LibraryBrowser_Destroy(void *model);
void LibraryBrowser_CopyAlbumArtistSubtitle(void *model, int index, struct NativeString *out);
void *LibraryModelRef_Init(void *reference, void *model, int synchronized);
void *LibraryModelRef_Assign(void *destination, void *source);
void LibraryModelRef_Release(void *reference);
void *LibraryModelRef_Get(void *reference);
void GameManifest_InitReader(void *reader, const char *manifest_path, uint8_t storage_mode, const char *signature_path);

extern uint8_t ResourceNameMap[];
extern void *const ScreenController_vtable[];
extern void *const View_vtable[];
extern const uint8_t EQ_Presets[];
extern const uint32_t EQ_PresetIds[];
extern uint8_t BrowserRegistry[];
extern uint32_t MainMenuSettingsRows[];
extern uint32_t MusicMenuSettingsRows[];
#endif

#define SITE_Preferences_LoadFile_call 0x08058808
#define SITE_ResourceBlockList_AppendBorrowedBlock_call 0x0817a88c
#define SITE_ResourceBank_InitOverrides_call 0x081c7ea8
#define SITE_ResourceBank_NextTemplate_call 0x081af75c
#define SITE_UI_LoadLayoutEvents_call 0x081d63f8
#define SITE_UI_TemplateEventsGet_call 0x081d6218
#define SITE_UI_LayoutEventsGet_call 0x081d62b4
#define SITE_UI_ControllerEventsGet_call 0x081d634c
#define SITE_Settings_Action_slot 0x089a3bf4
#define SITE_Settings_SecondaryAction_slot 0x089a7c3c
#define SITE_Settings_Property_slot 0x08997b60
#define SITE_Settings_IndexedProperty_slot 0x08997b7c
#define SITE_NowPlaying_Action_slot 0x0899be50
#define SITE_NowPlaying_SecondaryAction_slot 0x089a2e00
#define SITE_Music_Action_slot 0x089a940c
#define SITE_Music_SecondaryAction_slot 0x089a507c
#define SITE_Main_Action_slot 0x089a4c9c
#define SITE_Main_SecondaryAction_slot 0x089a8fa4
#define SITE_AlbumSubtitle_slot 0x0898cbf4
#define SITE_GraphicsContext_FillRect_call 0x0825ae9c
#define SITE_TextView_DrawColor_call 0x082872a8
#define SITE_View_BackgroundFill_call 0x082636b4
#define SITE_MarqueeTextView_DrawColor_call 0x081a8f0c
#define SITE_CoverFlow_ClearColor_call 0x08138304
#define SITE_AlbumArt_ClearColor_call 0x08164230
#define SITE_ArtworkReflection_Blend_call 0x0826a890
#define SITE_EQ_Process_slot 0x089ae0e0
#define SITE_EQ_Reset_slot 0x089ae0d4
#define SITE_EQ_SetPreset_Load_call 0x082735dc
#define SITE_EQ_SetRate_Load_call 0x08273610
#define SITE_EQ_DspCount_word 0x082735c8
#define SITE_EQ_TrackGuard_word 0x08173b74
#define SITE_EQ_SettingsCount_word 0x080bb498
#define SITE_EQ_PreviewCount_word 0x081e0f74
#define SITE_EQ_NameCount_word 0x081e0fc0
#define SITE_EQ_ProviderCount_word 0x081e1ae8
#define SITE_EQ_ResourceCount_word 0x08295420
#define SITE_EQ_MenuIds_literal 0x0807bfb0
#define SITE_EQ_SettingsIds_literal 0x080bb4ac
#define SITE_EQ_PreviewTable_literal 0x081e0fa8
#define SITE_EQ_NameTable_literal 0x081e0ff0
#define SITE_EQ_ResourceTable_literal 0x082955dc
#define SITE_HeapRegionBase_literal 0x0804b478
#define SITE_RuntimeHeapBase_literal 0x0807c048
#define SITE_Panic_entry 0x0803930c
#define SITE_Abort_entry 0x0802606c
#define SITE_RegionalVolumeResult_word 0x0804b188
#define SITE_GameManifest_Init_call 0x080f9418
#define SITE_SongList_CancelAppend_call 0x082195ec
#define SITE_NestedPlaylists_CancelAppend_call 0x0822ffb8
#define SITE_Genius_CancelAppend_call 0x0821d26c
#define SITE_NowPlaying_CancelAppend_call 0x0821526c
#define SITE_Albums_CancelAppend_call 0x0821c004
#define SITE_Artists_CancelAppend_call 0x0821fee8
#define SITE_SongList_DispatchAction_entry 0x08218b24
#define SITE_NestedPlaylists_DispatchAction_entry 0x0822ea1c
#define SITE_Genius_DispatchAction_entry 0x0821c0e4
#define SITE_MediaList_DispatchAction_entry 0x0820f5a4
#define SITE_MediaList_OnTheGoAdd_call 0x0820f9cc
#define SITE_ITunesDB_ImportTrackDataset_call 0x08058f1c
#define SITE_ITunesDB_AlbumArtist_slot 0x0808adcc
#define SITE_ITunesDB_SortArtist_slot 0x0808add0
#define SITE_ITunesDB_SortAlbumArtist_slot 0x0808ade8
#define SITE_TrackData_DestroyStorage_call 0x0803e650
#define SITE_MediaBrowser_BuildArtists_entry 0x08048fe8
#define SITE_MediaBrowser_ReadArtistName_entry 0x08047b54
#define SITE_MediaBrowser_ReadArtistJumpLabel_entry 0x08047bc8
#define SITE_MediaBrowser_ArtistFilterCompare_call 0x080d7bd8
#define SITE_AlbumGroup_literal 0x08048fe4
#define SITE_LibraryBrowser_Destroy_entry 0x08133c30
#define SITE_MediaListSelection_SelectArtworkSource_call 0x08169e30
#define SITE_AlbumArtworkProvider_Create_call 0x08138c34
#define SITE_MainMenu_Index_call 0x081d2214
#define SITE_MusicMenu_Index_call 0x081dd6dc
#define SITE_MainMenu_Toggle_call 0x081d225c
#define SITE_MusicMenu_Toggle_call 0x081dd724
#define SITE_MusicMenu_Apply_call 0x081e3f48
#define SITE_SettingsModel_ResetMainMenu_entry 0x081e1024
#define SITE_SettingsModel_ResetMusicMenu_entry 0x081e1134

#define ITunesReader_file 0x40000
#define ITunesReader_owner 0x40004
#define TrackData_artist_cache 0x118
#define MediaList_flags 0x18c
#define MediaList_order_flags 0x18d
#define MediaList_shuffle_index 0x3b4
#define MediaList_order_index 0x3ac
#define MediaList_jump_index 0x544
#define MediaBrowser_track_data 0xf60
#define MediaBrowser_artists 0xe64
#define MediaBrowser_albums 0xeb0
#define MediaBrowser_artist_tokens 0xe4c
#define MediaBrowser_artist_jump 0xe50
#define MediaBrowser_artist_count 0xe54
#define MediaBrowser_order_flag 0xe3c
#define MediaListSelection_artwork_category 0x83f
#define MediaPlayer_current_item_method 0x15c
#define SongModel_get_item_method 0x22c
#define ScreenController_screen 0xb4
#define ScreenController_settings_model 0xb0
#define ScreenController_event_method 0x30
#define ScreenController_activate_method 0xec
#define ScreenController_deactivate_method 0xf0
#define Model_notify_method 0x58
#define Service_interface_method 0x14
#define Preferences_object 0x378
#define Preferences_get_method 0xdc
#define Preferences_set_method 0xd4
#define View_draw_method 0x74
#define View_bounds 0x80
#define View_graphics_options 0x4c

#define AlbumArtist_order_mode 0x2d
#define MainMenuSettings_row_count 0x22
#define MusicMenuSettings_row_count 0xd
#define Property_String 0x53747220
#define Property_Count 0x564d6178
#define Notify_None 0x4e6f6e65
#define Notify_All 0x2a2a2a2a
#define Resource_BMap 0x424d6170
#define Resource_Draw 0x44726177
#define Service_Settings 0x8900
#define Service_NowPlaying 0x7f00
#define ActionSupportQuery 0xdeadbeef

#define RES_Settings_Off_String 0x0dad0bda
#define RES_Settings_On_String 0x0dad0bdb
#define RES_Settings_Speaker_String 0x0dad0bd3
#define RES_Settings_Headphones_String 0x0dad0bd4
#define RES_Settings_Both_String 0x0dad0bd5
#define RES_Settings_ClickerOff_Description 0x0dad0bd6
#define RES_Settings_ClickerSpeaker_Description 0x0dad0bd7
#define RES_Settings_ClickerHeadphones_Description 0x0dad0bd8
#define RES_Settings_ClickerBoth_Description 0x0dad0bd9
#define RES_Settings_ClickerOn_Image 0x0dad0c03
#define RES_Settings_ClickerOff_Image 0x0dad0c04
#define RES_Settings_Brightness_Image 0x0dad0bff
#define RES_Settings_MainMenu_Item 0x0dad0c91
#define RES_Settings_MusicMenu_Item 0x0dad0c94
#define RES_Menu_Checked_Bitmap 0x0dad0c08
#define RES_Menu_Reset_Bitmap 0x0dad0125
#define RES_CoverFlow_Backside_Border_Color 0x0dad05d8
#define RES_NowPlaying_White_Background_View 0x0dad08c5
#define RES_NowPlaying_Idle_Template 0x0dad0925
#define RES_NowPlaying_ChapterArt_BackgroundImage_View 0x0dad092d
#define RES_RentalInfo_White_Background_View 0x0dad09ae
#define RES_Search_Input_Text 0x0dad0ddc
