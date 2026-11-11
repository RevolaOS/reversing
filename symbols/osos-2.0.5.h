/* SPDX-License-Identifier: GPL-3.0-only */
/* Generated from the symbols file for OSOS 2.0.5. */
#pragma once

#define OSOS_VERSION "2.0.5"
#define OSOS_RUNTIME_BASE 0x08000000
#define OSOS_RUNTIME_FILE_OFFSET 0x0000b798
#define OSOS_NATIVE_HEAP_BASE 0x08b3b1b0

#ifndef __ASSEMBLER__
#include <stdint.h>

struct DirectoryIterator;
struct MediaEntry;
struct MediaItem;
struct MediaList;
struct NativePath;
struct NativeString;
struct ThreeBandEQ;

int ArtworkDatabase_ReadImageThumbs(void *reader, void *image, uint32_t count, uint32_t flags);
void *ArtworkDatabase_FindById(void *database, uint32_t artwork_id);
void ArtworkThumb_Destroy(void *thumb);
void *ArtworkThumb_GetNameCache(void *thumb);
void EQ_LoadPresetCoefficients(struct ThreeBandEQ *eq);
void EQ_ResetAllBiquadStates(struct ThreeBandEQ *eq);
void EQ_ResetBiquadState(struct ThreeBandEQ *eq, uint32_t stage);
void EQ_ProcessBiquadStereoS16(struct ThreeBandEQ *eq, const int16_t *input, int16_t *output, uint32_t frames, uint32_t stage);
void EQ_ProcessThreeBiquadsInPlace(struct ThreeBandEQ *eq, int16_t *input, uint32_t frames, int16_t **output, uint32_t *output_frames);
uint32_t EQ_SettingsIdToDspPreset(uint32_t settings_id);
void Audio_SetPlaybackSource(void *source);
void GameManifest_InitReader(void *reader, const char *manifest_path, uint8_t storage_mode, const char *signature_path);
void *GraphicsContext_ConstructDefault(void *context);
void GraphicsContext_FillRect(void *context, const int32_t *rect);
void GraphicsContext_SetColor(void *context, const uint8_t *rgba);
void GraphicsContext_SetBackgroundColor(void *context, const uint8_t *rgba);
void GraphicsContext_SetFont(void *context, const uint32_t *font);
void GraphicsContext_SetTextPen(void *context, int x, int baseline);
void GraphicsContext_SetDrawMode(void *context, uint8_t mode);
void GraphicsContext_DrawUtf8(void *context, const char *text);
int GraphicsContext_MeasureUtf8(void *context, const char *text);
void GraphicsContext_CopyRect(void *context, void *source, const int32_t *source_rect, const int32_t *rect, uint32_t alpha, uint32_t flags);
void *GraphicsContext_InitWithBitmap(void *context, void *bitmap);
void *GraphicsContext_InitFromBitmapResource(void *context, void *bitmap, const void *resource);
void GraphicsContext_SetOrigin(void *context, int x, int y);
void *Bitmap_Construct(void *bitmap);
void *Bitmap_Destroy(void *bitmap);
void GraphicsContext_Destroy(void *context);
void Font_Construct(uint32_t *font, const char *family, uint32_t size, uint8_t style);
int GraphicsContext_GetFontAscent(void *context);
int GraphicsContext_GetFontDescent(void *context);
void Surface_FillRect(void *surface, const int32_t *rect, const uint8_t *rgba, uint32_t mode, void *clip);
void GLES_ClearColorx(int red, int green, int blue, int alpha);
uint32_t RGB565_BlendFixed(uint32_t first, uint32_t second, int weight);
int IAP_VerifyDeviceSignatureRSA(void *manager, uint32_t channel, uint32_t key_id, void *signature, uint32_t signature_bytes);
int IAP_VerifyDeviceSignatureAlternate(void *authentication, void *signature, uint32_t signature_bytes);
int Wheel_DispatchButtonChanges(int force_center);
void Input_HandleButton(uint32_t button, uint32_t pressed);
void Event_Signal(uint32_t event);
void IRQ_RestoreState(uint32_t cpsr);
uint32_t IRQ_DisableSave(void);
void MediaPlayer_InitializePlaybackList(void *player, uint32_t shuffle, uint32_t repeat);
int MediaPlayer_IsStopped(void *player);
void MediaPlayer_TogglePlayback(void *player);
void *MediaPlayer_GetInstance(void);
int MediaPlayer_ClearPreparedTrack(void *player);
int MediaPlayer_RebuildArtwork(void *player);
int MediaPlayer_GetCurrentItem(void *player, struct MediaItem *item);
int MediaPlayer_GetItemAtIndex(void *player, uint32_t index, struct MediaItem *item);
void MediaPlayer_SetCurrentIndex(void *player, int index, int restart);
int MediaPlayer_PlayIndex(void *player, int index, uint8_t flag, uint32_t start_ms, uint32_t param_5);
void MediaPlayer_RemoveEntryAtIndex(void *player, uint32_t index);
int MediaPlayer_IsRepeatAll(void *player);
int MediaPlayer_IsRepeatOne(void *player);
int MediaPlayer_CreatePlaybackList(void *player, void *browse_context, void **out_list);
int MediaItem_IsValid(struct MediaItem *item);
struct MediaItem *MediaItem_InitFromRecord(struct MediaItem *item, void *record, uint8_t flags);
void MediaItem_Destroy(struct MediaItem *item);
void MediaItem_Assign(struct MediaItem *destination, const struct MediaItem *source);
uint32_t MediaItem_HasRecordFlag8F01(struct MediaItem *item);
uint32_t MediaItem_HasKind8(struct MediaItem *item);
uint32_t MediaItem_HasKind8062(struct MediaItem *item);
uint32_t MediaItem_HasKind200000(struct MediaItem *item);
int MediaListView_IsValid(void *list_view);
uint16_t MediaListView_GetCount(void *list_view);
int MediaListView_RemoveEntryAtIndex(void *list_view, uint32_t index, int order_mode, uint32_t reverse);
int MediaEntry_Remove(struct MediaEntry *entry);
void MediaList_NotifyChanged(struct MediaList *list);
struct MediaEntry *PlaybackList_GetEntryByIndex(void *list, uint32_t index);
void MediaList_InvalidateOrderCache(struct MediaList *list, uint32_t order_mode);
void *MediaList_CopyEntryAtPosition(void *source, void *destination, void *cursor, uint32_t flags);
struct MediaEntry *MediaList_GetEntryByOrderIndex(struct MediaList *list, int order_mode, uint32_t reverse, uint32_t index);
int MediaList_GetEntryOrderIndex(void *entry, uint32_t order_mode, uint32_t reverse);
int MediaList_BuildOrderIndex(struct MediaList *list, uint32_t order);
void MediaList_ResolveOrderMode(struct MediaList *list, uint32_t *order, uint8_t *reverse);
int MediaListController_AddSelectionToOnTheGo(void *controller);
int MediaListController_DispatchAction(void *controller, const char *action, uint32_t argument);
void MediaListController_HandleShowContextualMenu(void *controller);
int MediaListController_MapVisibleRow(void *controller, int row);
int NestedPlaylists_MapVisibleRow(void *controller, int row);
int OnTheGo_AddBrowserSelection(void *otg, int row, void *browser, int save);
uint32_t MediaListSelection_SelectArtworkSource(void *selection, uint32_t category);
void ArtworkSession_Stop(void *session);
void *AlbumArtworkProvider_Create(uint32_t source_kind, uint32_t selector);
void *Genius_GetInstance(void);
int Genius_MapVisibleRow(void *genius, int row);
void Genius_GetItem(void *genius, uint32_t row, void *item);
int ITunesDB_ImportTrackDataset(void *reader);
int ITunesDB_ImportCachedString(void *reader, uint32_t encoding, uint32_t mhod_type, void *cache, uint32_t *token, uint32_t glyph_field, int collect);
int ITunesDB_SkipImportBytes(void *reader, int bytes);
void TrackData_AcquireStringCaches(void *track_data);
void TrackData_ReleaseStringCaches(void *track_data);
void TrackData_DestroyStorage(void *track_data);
void *MediaBrowser_GetOwner(void *browser);
struct MediaList *MediaBrowser_GetCurrentList(void *browser);
int MediaBrowser_CreateRoot(void *system, void **out_browser);
int MediaBrowser_CreateChild(void *owner, void **out);
int MediaBrowser_EvaluateEntryFilters(void *browser, struct MediaEntry *entry, struct MediaList *list_override);
void MediaBrowser_CopyMatchingEntries(void *browser, struct MediaList *source, uint32_t order_mode, uint32_t reverse, struct MediaList *destination, int apply_skip_flags, uint32_t selected_index, struct MediaEntry **selected_out);
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
void *LibraryModelRef_InitCopy(void *reference, const void *source);
void *LibraryModelRef_Assign(void *destination, void *source);
void LibraryModelRef_Release(void *reference);
void *LibraryModelRef_Get(void *reference);
uint32_t MediaPlayer_GetReadAheadBytes(void *player);
int FixedPool_Create(uint32_t slot_bytes, uint32_t argument, void **out_pool);
uint32_t Heap_GetTotalBytes(void);
void *BlockCache_Get(void);
uint32_t BlockCache_GetBlockBytes(void);
uint32_t BlockCache_GetTotalBlocks(void);
int BlockCache_Grow(uint32_t bytes);
void BlockCache_Trim(void);
int BlockCache_PostRebalance(void *cache, uint32_t argument);
int BlockCache_PostWaitPending(void *cache, uint32_t argument);
int BlockCache_PostClientRequest(void *cache, uint32_t argument);
uint32_t BlockCache_AddBlocks(void *cache, uint32_t count);
int BlockCache_PushFreeBlock(void *cache, void *block);
uint32_t BlockCache_GetAssignedBlockCount(void *cache);
uint32_t BlockCache_CountPendingClients(void *cache);
void BlockCache_IncrementPendingClientCount(void *cache);
void BlockList_Erase(void **out_next, void *list, void **node);
int BlockCacheClient_SetDesiredBytes(void *client, uint32_t bytes, uint32_t weight);
void BlockCacheClient_ReturnBlock(void *client, void *block);
int BlockCacheClient_CancelPending(void *client);
int EAppBlockManager_HandleMessage(void *manager, void *message);
void EAppBlocks_Acquire(void *manager);
void EAppBlocks_Release(void *manager);
uint32_t Power_EnterLightSleep(int mode);
int Disk_GetPowerState(uint32_t *out_powered);
void *StorageArbiter_Get(void);
void StorageArbiter_Activate(void *arbiter, void *owner);
void StorageArbiter_SetOwner(void *arbiter, void *client);
void StorageArbiter_Request(void *arbiter, void *client);
void StorageArbiter_Release(void *arbiter, void *client);
int DiskClient_Yield(void *client);
void *ScsiDiskClient_Create(void *memory, void *scsi_context);
void ScsiDiskClient_Request(void *client);
void HardwareEvent_Post(uint32_t source);
void HardwareEvent_PostWithArgument(uint32_t source, uint32_t argument);
int HardwareEvent_MapSource(uint32_t source, uint8_t *out_index);
int HardwareEvent_Dispatch(uint32_t index, uint32_t *argument);
void App_HandleDiskModeRequest(void *app);
void App_PollDiskModeEntry(void *app);
void App_EnterDiskMode(void *app);
void App_ExitDiskMode(void *app);
void System_PrepareDiskMode(void *system);
void App_HandleDiskGranted(void *app);
void App_HandleReloadTimer(void *app);
void App_StartMusicLoading(void *app);
void App_PollMusicLoading(void *app);
void *MusicLoadingTask_Create(void *memory);
int MusicLoadingTask_Run(void *task);
void ResourceBank_InitOverrides(void *bank);
void ResourceBank_SetResource(void *bank, uint32_t type, uint32_t id, const void *data, uint32_t bytes);
void *ResourceBank_GetResource(void *bank, uint32_t type, uint32_t id, uint32_t *out_bytes);
const void *ResourceBank_FirstResource(void *bank, uint32_t type, uint32_t *id, uint32_t *bytes);
const void *ResourceBank_NextResource(void *bank, uint32_t *id, uint32_t *bytes);
void ResourceBank_RemoveHeapResource(void *bank, uint32_t type, uint32_t id);
void *ResourceBank_Destroy(void *bank);
int ResourceOverrides_Find(void *map, uint32_t type, uint32_t id, uint32_t **entry);
int ResourceOverrides_EraseEntry(void *map, uint32_t type, const uint32_t *entry);
uint32_t *ResourceNames_FindOrInsertSlot(void *map, const void *name);
void ResourceBlockList_AppendBorrowedBlock(void *list, const void *data, uint32_t bytes, uint32_t metadata);
char *ResourceService_GetString(void *service, uint32_t id);
void *ResourceService_GetBitmap(void *service, uint32_t id);
void *UI_GetResourceBank(void);
void UI_SetResourceBank(void *bank);
void *UI_GetResourceService(void);
uint32_t ResourceName_GetId(void *name);
void *Heap_AllocateZeroed(uint32_t byte_count);
void Heap_Free(void *allocation);
void *Runtime_OperatorNew(uint32_t byte_count);
int Mutex_Lock(void *mutex);
int Mutex_Unlock(void *mutex);
void *Mutex_Init(void *mutex);
int HoldSwitch_GetLogicalState(void);
int HoldSwitch_ReadHardwareBit(uint32_t *out_raw_bit);
int Platform_InitMicrosecondCounter(void);
void SystemUI_HandleHoldChange(void *system, int locked);
int File_OpenLegacyPath(void *path, uint32_t fork, int mode, void **out_handle);
int File_ReadLegacyHandle(void *handle, void *buffer, uint32_t bytes, uint32_t flags, uint32_t *out_read);
int File_WriteLegacyHandle(void *handle, const void *buffer, uint32_t bytes, uint32_t flags, uint32_t *out_written);
int File_CloseLegacyHandle(void *handle);
int File_SeekLegacyHandle(void *handle, uint64_t offset);
int BufferedFile_GetPosition(void *file, uint64_t *position);
void Preferences_LoadFile(void *preferences);
void Preferences_FlushIfDirty(void *preferences);
void Preferences_MarkDirty(void *preferences);
void Preferences_SetClickerRoute(void *browser, uint32_t route);
uint32_t Preferences_GetClickerRoute(void *browser);
void *PreferencesService_Get(void);
int File_DeleteLegacyPath(void *path);
void Runtime_Abort(void);
void Runtime_ReportUncaughtException(int kind, const char *what);
struct DirectoryIterator *DirectoryIterator_Init(struct DirectoryIterator *iterator, const char *path, uint8_t recursive, uint32_t volume);
int DirectoryIterator_Next(struct DirectoryIterator *iterator, struct NativePath *path, uint8_t *is_directory);
void DirectoryIterator_Destroy(struct DirectoryIterator *iterator);
int Directory_CreateWithParents(const char *path, uint32_t volume);
struct NativePath *NativePath_InitEmpty(struct NativePath *path);
struct NativePath *NativePath_Destroy(struct NativePath *path);
void *Thread_GetCurrentRecord(void);
void *IRQ_Dispatch(uint32_t *frame);
uint32_t Clock_GetMicroseconds(void);
char *SetWarmBootModeTag(uint32_t mode);
void ResetSocAndWait(void);
void Timer_Stop(void *timer);
int Settings_HandleAction(void *controller, const char *action, uint32_t argument);
void *SettingsModel_GetInstance(void);
void SettingsModel_ToggleClicker(void *model);
void SettingsModel_NotifyClicker(void *model);
int SettingsModel_GetProperty(void *model, uint32_t type, uint32_t property, uintptr_t *out);
int SettingsModel_GetIndexedProperty(void *model, uint32_t type, uint32_t property, uint32_t index, uintptr_t *out);
void SettingsModel_ToggleMainMenuItem(void *model, uint32_t index);
void SettingsModel_ToggleMusicMenuItem(void *model, uint32_t index);
void *SettingsModel_ApplyMainMenuPreferences(void *model);
void *SettingsModel_ApplyMusicMenuPreferences(void *model);
void *SettingsModel_ResetMainMenu(void *model);
void *SettingsModel_ResetMusicMenu(void *model);
void *SettingsModel_GetEQPresetPreview(void *model, uint32_t menu_index);
void *SettingsModel_GetEQPresetName(void *model, uint32_t menu_index);
void SettingsModel_ResetAllSettings(void *model);
void SettingsModel_SelectEQMenuIndex(void *model, uint32_t menu_index);
void SettingsModel_SetPreviewIndex(void *model, uint32_t index);
int SettingsEQ_HandleAction(void *controller, const char *action, uint32_t argument);
int SettingsEQ_ApplySelection(void *controller);
int SettingsEQ_ShowPreview(void *controller);
int SettingsMainMenu_HandleAction(void *controller, const char *action, uint32_t argument);
int SettingsEQ_HandleEvent(void *controller, void *event);
int SettingsEQ_SelectCurrentRow(void *controller, uint32_t deferred);
int SettingsBacklight_SelectCurrentRow(void *controller, uint32_t deferred);
int SettingsMusicMenu_HandleAction(void *controller, const char *action, uint32_t argument);
void SettingsModel_AdjustBrightness(void *model, int delta);
void *CppString_Init(void *string, const char *text);
void *CppString_InitBytes(void *string, const void *data, uint32_t bytes, void *allocator);
void CppString_Destroy(void *string);
int String_CompareCString(const char *left, const char *right);
void StringInputStream_Init(void *stream, void *string, uint32_t mode);
void StringInputStream_Destroy(void *stream, uint32_t flags, void *storage);
struct NativeString *NativeString_InitEmpty(struct NativeString *string);
struct NativeString *NativeString_Destroy(struct NativeString *string);
char *NativeString_GetUtf8(struct NativeString *string);
void NativeString_AssignUtf16(struct NativeString *string, const uint16_t *text, int characters);
int SongText_CompareUtf16(void *collation, const uint16_t *left, uint32_t left_units, const uint16_t *right, uint32_t right_units);
int StringCache_ReadToken(void *cache, uint32_t token, void *destination, uint32_t *bytes, uint32_t maximum_bytes);
int StringCache_ReleaseToken(void *cache, uint32_t token);
int StringCache_GetTokenView(void *cache, uint32_t token, void **data, uint32_t *bytes);
int PodcastEpisodes_DispatchAction(void *controller, const char *action, uint32_t argument);
int PodcastEpisodes_BuildAndShowContextualMenu(void *controller);
void MenuList_RebuildVisibleItems(void *menu_list, void *view, int refresh_subscriptions);
int MenuList_SetItemTable(void *menu_list, uint32_t table_id, void *view);
void MenuItems_RegisterResourceType(uint32_t type);
void UI_ShowContextMenuItems(void *controller, void *items);
void *ContextMenuModel_Get(void);
void ContextMenuModel_SetItems(void *model, const void *items);
uint32_t ContextMenuModel_GetRememberedIndex(void *model);
void UI_SetScreenState(void *manager, uint32_t screen_id, uint32_t state0, uint32_t state1, uint32_t state2, uint32_t state3);
int ContextMenuModel_GetProperty(void *model, uint32_t type, uint32_t property, uintptr_t *out);
int ContextMenuModel_GetIndexedProperty(void *model, uint32_t type, uint32_t property, int index, uintptr_t *out);
void UI_LoadLayoutInstances(void *layout);
int ContextMenuController_DispatchAction(void *controller, const char *action, uint32_t argument);
void ScreenController_SwitchLayoutDefault(void *controller);
void *UI_GetLayoutManager(void);
int MenuList_QueryProperty(void *menu_list, uint32_t type, uint32_t property, uint32_t *out);
void *ControllerRegistry_InvokeControllerFactory(void *registry, const char *class_name, const char *controller_name);
void *ControllerRegistry_InvokeScreenFactory(void *registry, const char *class_name, const char *controller_name, uint32_t screen_id, uint32_t layout_id, uint32_t transition);
void *UI_FindServiceById(uint32_t service_id);
void *UI_GetRootView(void);
void *UI_GetSelectionModel(void);
void *UI_GetControllerRegistry(void);
void *UI_CreateScreenController(void *registry, uint32_t screen_id, uint32_t layout_id, uint32_t transition);
void UI_RegisterControllerFactories(void *registry, const char *name, void *ordinary_factory, void *resource_factory);
void UI_RegisterViewFactory(uint32_t view_type, void *create);
void UI_LoadLayoutEventTables(void *layout);
void UIEventMap_ReadStream(void *stream, void *map);
void *ScreenController_CreateNamed(const char *name, uint32_t screen_id, uint32_t layout_id, uint32_t transition);
int ScreenController_HandleEvent(void *controller, void *event);
uint32_t ResourceScreen_Activate(void *controller, uint32_t entering);
uint32_t ResourceScreen_Deactivate(void *controller, uint32_t leaving);
int MenuItem_SetVisible(uint32_t item_id, uint32_t visible);
void ContextMenuItems_Append(void *items, uint32_t item_id, const char *action, uint32_t flags);
void *ContextMenuItems_Init(void *items, uint8_t flag);
void *ContextMenuItems_Destroy(void *items);
void WheelEvent_DecodeMotion(void *event);
uint32_t ButtonEvent_GetUIKeyCode(void *event);
void *WheelAccumulator_Init(void *state, int threshold, uint8_t accelerate, uint32_t interval);
int WheelAccumulator_ConsumeMotion(void *state, int motion, uint32_t speed);
void WheelAccumulator_Reset(void *accumulator);
void View_InitOptionsFromDescriptor(const void *descriptor, void *options);
void View_ResolveBounds(void *view, int32_t *bounds);
void *View_Construct(void *view, void *unused, void *parent, void *event_target, const void *options);
void View_BindGraphicsContext(void *view, void *context);
void View_GetLocalContentRect(void *view, int32_t *rect);
void View_InvalidateRect(void *view, const int32_t *rect);
void View_DrawBackgroundAndBorder(void *view);
void View_Draw(void *view, void *draw_argument, const int32_t *clip);
void View_DrawClipped(void *view, void *draw_argument, const int32_t *clip);
int View_HandleModelNotification(void *view, void *event);
void UI_RegisterBuiltinViewFactories(void);
void ProgressBar_RefreshFromModel(void *view);
void *TextView_CreateFromDescriptor(void *owner, void *parent, void *event_target, const void *descriptor);
int MainMediaMenu_DispatchCommonAction(void *controller, const char *action, uint32_t argument);
int NowPlaying_DispatchAction(void *controller, const char *action, uint32_t argument);
int SongList_DispatchAction(void *controller, const char *action, uint32_t argument);
int NestedPlaylists_DispatchAction(void *controller, const char *action, uint32_t argument);
int Genius_DispatchAction(void *controller, const char *action, uint32_t argument);
void UI_InitializeEventTimer(void *controller, void *timer, uint32_t period, void *event_queue);
void Timer_Start(void *timer);
void UITask_Main(void);
void Timer_SetPeriod(void *timer, uint32_t milliseconds);
int Controller_ExecuteNavigatorAction(void *controller, void *action, int action_context, uint8_t *stop_dispatch);
void Navigator_PopTop(uint32_t flags);
void Navigator_PopToMainScreen(void);
void Navigator_PopToPixoMainScreen(void);
int ExtrasMenu_DispatchAction(void *controller, const char *action, uint32_t argument);
void *UI_ConstructScreenDefinition(void *definition, uint32_t screen_id, int load_layouts);
void UI_LoadScreenLayouts(void *definition);
uint32_t SettingsMenuListController_Deactivate(void *controller, uint32_t leaving);
int ResourceScreen_HandleEvent(void *controller, void *event);
int ScreenController_DispatchNamedEvent(void *controller, void *event);
int BrightnessController_HandleEvent(void *controller, void *event);
int BrightnessController_HandleWheel(void *controller, void *event);
int WheelInput_QuantizeMotion(uint32_t motion, int threshold);
uint32_t WheelEvent_GetSpeed(void *event);
int ScreenController_PopSelf(void *controller, uint32_t flags);
void *ResourceService_GetIndexedResource(void *service, uint32_t type, uint32_t property, uint32_t index);
int MenuList_GetRowProperty(void *list_model, uint32_t type, uint32_t property, int index, void *out);
void MenuItems_RegisterAll(void);
void BitmapView_LoadBitmap(void *view);
void *Object_GetInterface(void *object, uint32_t interface);
uint8_t Layout_GetKind(void *layout);
int UI_InstanceEnterStyle(void *manager, void *instance, int transition_5, int other_kind);
int UI_InstanceLeaveStyle(void *manager, void *instance, int transition_5, int other_kind);
void *UI_GetCompositorLayer(void *layers, uint32_t index);
int UI_EmitControllerEvent(void *controller, void *event_name);
void *ControllerLayoutCache_GetOrInsert(void *definition, uint32_t layout_id);
void *View_FindViewById(void *view, uint32_t id);
void *UI_FindTemplateView(void *owner, uint32_t template_id);
void UI_DisplayPreparedLayout(void *manager, void *layout);
void *Navigator_Get(void);
void Navigator_Push(void *navigator, void *controller, uint32_t flags);
void *UI_GetRootOwner(void);
void TextView_DrawString(void *view, const char *text, void *argument);
void *UI_ResolveTemplateView(void *manager, uint32_t template_id, uint8_t *out_from_root);
void USB_HandleSetupPacket(void *usb);
void *USB_DispatchControlRead(void *usb, uint32_t *inout_length);
void *USB_HandleVendorDeviceRead(void *usb, uint32_t *inout_length);
int USB_DispatchNoDataRequest(void *usb);
int USB_HandleAppleVendorCommand(void *usb, uint32_t setup0, uint32_t setup1);
void *USB_GetControlWriteBuffer(void *usb, uint32_t length);
void USB_StartControlTransfer(void *usb, uint32_t endpoint, void *buffer, uint32_t length);
int USB_IsAccessoryMode(void);
void SCSI_AcquireMedium(void *out_result, void *context, int set_sense);

extern uint8_t AudioOutputQueue[];
extern const uint8_t EQ_Presets[];
extern const uint32_t EQ_PresetIds[];
extern const uint8_t DisplayLayerOrders[];
extern const uint32_t DisplayBlendRegisters[];
extern void *const Bitmap_vtable[];
extern uint8_t WheelState[];
extern const uint8_t ButtonKeyCodes[5];
extern uint8_t BrowserRegistry[];
extern void *BlockCache;
extern void *MainHeap;
extern uint16_t ATA_Identify[256];
extern uint8_t StorageArbiter[];
extern uint8_t DiskClient[];
extern void *const DiskClient_vtable[];
extern void *const ScsiDiskClient_vtable[];
extern void *const MusicLoadingTask_vtable[];
extern void *App;
extern uint8_t ResourceNameMap[];
extern uint32_t ClickerRoute;
extern uint8_t IRQ_Nesting;
extern uint32_t MainMenuSettingsRows[];
extern uint32_t MusicMenuSettingsRows[];
extern void *const ScreenController_vtable[];
extern void *const View_vtable[];
extern void *const TemplateView_vtable[];
extern void *const SportTimerChosenDispatcher_vtable[];
extern void *const SettingsMenuListController_vtable[];
extern void *const ExtrasMenuController_vtable[];
extern void *const ArtistsController_vtable[];
extern void *const AlbumsController_vtable[];
extern uint8_t Navigator[];
extern void *const TextView_vtable[];
extern void *const SelectedRowTextView_vtable[];
extern void *const MenuList_vtable[];
extern void *const MenuListModel_vtable[];
#endif

#define ADDR_ArtworkDatabase_Import 0x08059000
#define ADDR_ArtworkDatabase_ReadImageThumbs 0x0808b578
#define ADDR_ArtworkDatabase_FindById 0x08045534
#define ADDR_ArtworkDatabase_FindByDbid 0x08045668
#define ADDR_ArtworkThumb_Destroy 0x0803e16c
#define ADDR_ArtworkThumb_GetNameCache 0x080475cc
#define ADDR_EQ_LoadPresetCoefficients 0x082734c0
#define ADDR_EQ_ResetAllBiquadStates 0x08273630
#define ADDR_EQ_ResetBiquadState 0x08273594
#define ADDR_EQ_ProcessBiquadStereoS16 0x08273658
#define ADDR_EQ_ProcessThreeBiquadsInPlace 0x082737fc
#define ADDR_EQ_SetDspPreset 0x082735c4
#define ADDR_EQ_SetSampleRate 0x082735f4
#define ADDR_EQ_SettingsIdToDspPreset 0x080b28a0
#define ADDR_EQ_SettingsIdToMenuIndex 0x080bb460
#define ADDR_MediaTrack_ApplyEQ 0x08173b48
#define ADDR_Audio_SetPlaybackSource 0x081a64c4
#define ADDR_Audio_InitEffectsChain 0x0818ced8
#define ADDR_Audio_ProcessEffectsAndOutput 0x0818cbe0
#define ADDR_Audio_SetPostEffectsGainQ15 0x0818cec0
#define ADDR_Audio_SetEQPreset 0x0818ca70
#define ADDR_Audio_FillOutputBuffer 0x22007840
#define ADDR_AudioOutputQueue_Read 0x22006b84
#define ADDR_Platform_UsesVolumeWarningPolicy 0x08047450
#define ADDR_Volume_GetRegionalCeiling 0x081558c0
#define ADDR_Volume_GetDefaultLimit 0x08155ad0
#define ADDR_VolumePolicy_IsWarningAcknowledged 0x08155a88
#define ADDR_MediaPlayer_FinishCurrentTrack 0x081731d8
#define ADDR_MediaPlayer_PostVolumeExposureLimitEvent 0x080cfe74
#define ADDR_DisplayLayer_Commit 0x08114be4
#define ADDR_DisplayPipe_ConfigureWindow 0x08143c30
#define ADDR_DisplayPipe_SetWindowSource 0x081445cc
#define ADDR_DisplayPipe_SetWindowEnable 0x081440b8
#define ADDR_DisplayPipe_SetWindowBlendMode 0x0814472c
#define ADDR_DisplayPipe_SetWindowAlpha 0x08144410
#define ADDR_DisplayPipe_SetOrder 0x0814482c
#define ADDR_GameManifest_InitReader 0x0825e510
#define ADDR_GameLoader_LoadVerifiedManifest 0x080f9374
#define ADDR_GraphicsContext_ConstructDefault 0x0825b008
#define ADDR_GraphicsContext_FillRect 0x0825ae34
#define ADDR_GraphicsContext_SetColor 0x08259b28
#define ADDR_GraphicsContext_SetBackgroundColor 0x08259b20
#define ADDR_GraphicsContext_SetFont 0x0825aba8
#define ADDR_GraphicsContext_SetTextPen 0x0825aba0
#define ADDR_GraphicsContext_SetDrawMode 0x082596fc
#define ADDR_GraphicsContext_DrawUtf8 0x08259648
#define ADDR_GraphicsContext_MeasureUtf8 0x08259d10
#define ADDR_GraphicsContext_CopyRect 0x08259568
#define ADDR_GraphicsContext_InitWithBitmap 0x0825aefc
#define ADDR_GraphicsContext_InitFromBitmapResource 0x0825af2c
#define ADDR_GraphicsContext_SetOrigin 0x0825aef0
#define ADDR_Bitmap_Construct 0x08103e8c
#define ADDR_Bitmap_Destroy 0x08103ee0
#define ADDR_GraphicsContext_Destroy 0x0825b038
#define ADDR_Font_Construct 0x0826c0f4
#define ADDR_GraphicsContext_GetFontAscent 0x08298784
#define ADDR_GraphicsContext_GetFontDescent 0x082987b8
#define ADDR_Surface_FillRect 0x08069a5c
#define ADDR_GLES_ClearColorx 0x082c7820
#define ADDR_RGB565_BlendFixed 0x0826a9a8
#define ADDR_ArtworkReflection_GenerateRGB565 0x0826a7c0
#define ADDR_IAP_VerifyDeviceSignatureRSA 0x0812dca8
#define ADDR_IAP_VerifyDeviceSignatureAlternate 0x081592fc
#define ADDR_IAP_HandleDeviceAuthenticationSignature 0x081892b8
#define ADDR_IAP_SetAccessoryAuthState 0x08185e18
#define ADDR_Wheel_HandleInterrupt 0x0805f688
#define ADDR_Wheel_RunTask 0x0805f738
#define ADDR_Wheel_DispatchButtonChanges 0x0807443c
#define ADDR_Input_HandleButton 0x08056900
#define ADDR_HoldSwitch_NotifyRawChange 0x080c75f8
#define ADDR_Event_Signal 0x0802cfa8
#define ADDR_IRQ_RestoreState 0x083924d8
#define ADDR_IRQ_DisableSave 0x083924e0
#define ADDR_InputWheel_AccumulateMotion 0x08093718
#define ADDR_MediaPlayer_InitializePlaybackList 0x08170b6c
#define ADDR_MediaPlayer_IsStopped 0x08172568
#define ADDR_MediaPlayer_TogglePlayback 0x08170304
#define ADDR_MediaPlayer_GetInstance 0x08172824
#define ADDR_MediaPlayer_ClearPreparedTrack 0x0817190c
#define ADDR_MediaPlayer_RebuildArtwork 0x08170238
#define ADDR_MediaPlayer_GetCurrentItem 0x0816f790
#define ADDR_MediaPlayer_GetItemAtIndex 0x0816ec38
#define ADDR_MediaPlayer_SetCurrentIndex 0x0816f058
#define ADDR_MediaPlayer_PlayIndex 0x08171e7c
#define ADDR_MediaPlayer_RemoveEntryAtIndex 0x081700bc
#define ADDR_MediaPlayer_IsRepeatAll 0x08172ec8
#define ADDR_MediaPlayer_IsRepeatOne 0x08172efc
#define ADDR_MediaPlayer_CreatePlaybackList 0x081719d4
#define ADDR_MediaItem_IsValid 0x0829a7dc
#define ADDR_MediaItem_InitFromRecord 0x08266d20
#define ADDR_MediaItem_Destroy 0x08266da0
#define ADDR_MediaItem_Assign 0x08266da4
#define ADDR_MediaItem_HasRecordFlag8F01 0x0829af84
#define ADDR_MediaItem_HasKind8 0x0829a9d4
#define ADDR_MediaItem_HasKind8062 0x0829aad8
#define ADDR_MediaItem_HasKind200000 0x0829afb4
#define ADDR_MediaListView_IsValid 0x0829d030
#define ADDR_MediaListView_GetCount 0x0829c858
#define ADDR_MediaListView_RemoveEntryAtIndex 0x0827a8c8
#define ADDR_MediaEntry_Remove 0x0803db30
#define ADDR_MediaList_NotifyChanged 0x080564b8
#define ADDR_PlaybackList_GetEntryByIndex 0x0829cab8
#define ADDR_MediaList_InvalidateOrderCache 0x080d1928
#define ADDR_MediaList_CopyEntryAtPosition 0x0803bbf0
#define ADDR_MediaList_GetEntryByOrderIndex 0x08047870
#define ADDR_MediaList_GetEntryOrderIndex 0x080497ac
#define ADDR_MediaList_BuildOrderIndex 0x080d438c
#define ADDR_MediaList_ResolveOrderMode 0x080a9ab0
#define ADDR_MediaListController_AddSelectionToOnTheGo 0x0820f8d4
#define ADDR_MediaListController_DispatchAction 0x0820f5a4
#define ADDR_MediaListController_HandleShowContextualMenu 0x082102b0
#define ADDR_MediaListController_MapVisibleRow 0x082573a8
#define ADDR_NestedPlaylists_MapVisibleRow 0x0822fa4c
#define ADDR_OnTheGo_AddBrowserSelection 0x081095f8
#define ADDR_MediaListSelection_SelectArtworkSource 0x08169c98
#define ADDR_MediaListSelection_SetList 0x08169db4
#define ADDR_ArtworkSession_Stop 0x0820a76c
#define ADDR_ArtworkSource_CreateProvider 0x08138b84
#define ADDR_AlbumArtworkProvider_Create 0x08140030
#define ADDR_Genius_GetInstance 0x08121a4c
#define ADDR_Genius_MapVisibleRow 0x08122180
#define ADDR_Genius_GetItem 0x08121a7c
#define ADDR_ITunesDB_ImportLibrary 0x08058afc
#define ADDR_ITunesDB_ImportTrackDataset 0x0808a488
#define ADDR_ITunesDB_ImportCachedString 0x080cdee0
#define ADDR_ITunesDB_SkipImportBytes 0x08069000
#define ADDR_TrackData_AcquireStringCaches 0x08051370
#define ADDR_TrackData_ReleaseStringCaches 0x08061284
#define ADDR_TrackData_DestroyStorage 0x080cfaf4
#define ADDR_TrackData_Destroy 0x0803e58c
#define ADDR_MediaBrowser_GetOwner 0x08046ee0
#define ADDR_MediaBrowser_GetCurrentList 0x08046e00
#define ADDR_MediaBrowser_CreateRoot 0x0804d6e4
#define ADDR_MediaBrowser_CreateChild 0x0804d80c
#define ADDR_MediaBrowser_EvaluateEntryFilters 0x080d7a98
#define ADDR_MediaBrowser_CopyMatchingEntries 0x0803bf78
#define ADDR_MediaBrowser_InvalidateEntryMatches 0x0804ec24
#define ADDR_MediaBrowser_ClearArtists 0x0804e850
#define ADDR_MediaBrowser_ClearAlbums 0x0804e794
#define ADDR_MediaBrowser_ClearSongs 0x0804ebe8
#define ADDR_MediaBrowser_GetRecordArtworkId 0x080464b0
#define ADDR_MediaBrowser_AccumulateArtistGroup 0x080c6bf0
#define ADDR_MediaBrowser_AccumulateAlbumGroup 0x080bf518
#define ADDR_MediaBrowser_BuildArtists 0x08048fe8
#define ADDR_MediaBrowser_BuildSongs 0x0807b858
#define ADDR_MediaBrowser_ReadArtistName 0x08047b54
#define ADDR_MediaBrowser_ReadArtistJumpLabel 0x08047bc8
#define ADDR_MediaBrowser_GetOrderMode 0x0804af48
#define ADDR_AlbumGroupVector_Append 0x083d8b74
#define ADDR_LibraryBrowser_Create 0x0825012c
#define ADDR_LibraryBrowser_InitBorrowed 0x08133be8
#define ADDR_LibraryBrowser_Destroy 0x08133c30
#define ADDR_LibraryBrowser_CopyAlbumArtistSubtitle 0x08133168
#define ADDR_LibraryModelRef_Init 0x0839576c
#define ADDR_LibraryModelRef_InitCopy 0x083957d4
#define ADDR_LibraryModelRef_Assign 0x08395818
#define ADDR_LibraryModelRef_Release 0x083937cc
#define ADDR_LibraryModelRef_Get 0x083ccd1c
#define ADDR_Library_LoadDatabases 0x0805d6c8
#define ADDR_MediaPlayer_GetReadAheadBytes 0x0822c92c
#define ADDR_FixedPool_Create 0x08053074
#define ADDR_Heap_GetTotalBytes 0x0809cc58
#define ADDR_BlockCache_Get 0x08180830
#define ADDR_BlockCache_GetBlockBytes 0x0817fd4c
#define ADDR_BlockCache_GetTotalBlocks 0x08180628
#define ADDR_BlockCache_Grow 0x080ca238
#define ADDR_BlockCache_Trim 0x080cd16c
#define ADDR_BlockCache_PostRebalance 0x0817fd68
#define ADDR_BlockCache_PostWaitPending 0x0817fdd0
#define ADDR_BlockCache_PostClientRequest 0x0817fd9c
#define ADDR_BlockCache_AddBlocks 0x081804e8
#define ADDR_BlockCache_PushFreeBlock 0x081801a4
#define ADDR_BlockCache_GetAssignedBlockCount 0x08180690
#define ADDR_BlockCache_CountPendingClients 0x08180840
#define ADDR_BlockCache_IncrementPendingClientCount 0x08180700
#define ADDR_BlockCache_DecrementPendingClientCount 0x08180644
#define ADDR_BlockList_Erase 0x083cc960
#define ADDR_BlockCacheClient_SetDesiredBytes 0x081f1f50
#define ADDR_BlockCacheClient_ReturnBlock 0x081f2888
#define ADDR_BlockCacheClient_CancelPending 0x081f2988
#define ADDR_EAppBlockManager_HandleMessage 0x0819d67c
#define ADDR_EAppBlocks_Acquire 0x0819d870
#define ADDR_EAppBlocks_Release 0x0819d40c
#define ADDR_Power_EnterLightSleep 0x0805d0c0
#define ADDR_PowerController_HandleIdleSleep 0x0819b28c
#define ADDR_PowerController_HandleRequestedSleep 0x0819bb18
#define ADDR_System_SaveAll 0x080638d8
#define ADDR_Disk_GetPowerState 0x082b3630
#define ADDR_StorageArbiter_Get 0x0807bf3c
#define ADDR_StorageArbiter_Activate 0x0815dde4
#define ADDR_StorageArbiter_SetOwner 0x0815dc00
#define ADDR_StorageArbiter_Request 0x0815dcec
#define ADDR_StorageArbiter_Release 0x0815dc58
#define ADDR_DiskClient_OnGranted 0x081d8248
#define ADDR_DiskClient_Yield 0x081d8258
#define ADDR_ScsiDiskClient_Create 0x08130648
#define ADDR_ScsiDiskClient_Request 0x08130620
#define ADDR_HardwareEvent_Post 0x0807c5b4
#define ADDR_HardwareEvent_PostWithArgument 0x0807c5bc
#define ADDR_HardwareEvent_MapSource 0x080d9f18
#define ADDR_HardwareEvent_Dispatch 0x081dc214
#define ADDR_App_HandleDiskModeRequest 0x0810b6e8
#define ADDR_App_PollDiskModeEntry 0x081089ec
#define ADDR_App_EnterDiskMode 0x08106854
#define ADDR_App_ExitDiskMode 0x0810b5e0
#define ADDR_System_PrepareDiskMode 0x0805df34
#define ADDR_SystemUI_DispatchSystemEvent 0x0810acd0
#define ADDR_App_HandleDiskGranted 0x08108bf0
#define ADDR_App_HandleReloadTimer 0x081084d0
#define ADDR_App_StartMusicLoading 0x0810b974
#define ADDR_App_PollMusicLoading 0x0810a514
#define ADDR_MusicLoadingTask_Create 0x081aec48
#define ADDR_MusicLoadingTask_Run 0x081aec04
#define ADDR_System_CheckVolumeOrRestart 0x081dc1d0
#define ADDR_ResourceBank_InitOverrides 0x08111f00
#define ADDR_ResourceBank_SetResource 0x08111e20
#define ADDR_ResourceBank_GetResource 0x08111d74
#define ADDR_ResourceBank_FirstResource 0x081120f4
#define ADDR_ResourceBank_NextResource 0x08112000
#define ADDR_ResourceBank_RemoveHeapResource 0x08111fbc
#define ADDR_ResourceBank_Destroy 0x081124a4
#define ADDR_ResourceOverrides_Find 0x0812252c
#define ADDR_ResourceOverrides_EraseEntry 0x08122574
#define ADDR_ResourceNames_FindOrInsertSlot 0x083d242c
#define ADDR_ResourceBlockList_AppendBorrowedBlock 0x0817a7ec
#define ADDR_ResourceBlockList_Load 0x0817a82c
#define ADDR_ResourceService_GetString 0x08268d94
#define ADDR_ResourceService_GetBitmap 0x08268d84
#define ADDR_UI_GetResourceBank 0x0819579c
#define ADDR_UI_SetResourceBank 0x081957c0
#define ADDR_UI_GetResourceService 0x08268d34
#define ADDR_UI_InitResourceProviders 0x081c7e68
#define ADDR_UI_InitializeViewTemplates 0x081af5b4
#define ADDR_ResourceName_GetId 0x08286e74
#define ADDR_Heap_AllocateZeroed 0x0805232c
#define ADDR_Heap_Free 0x080521c0
#define ADDR_Runtime_OperatorNew 0x082a17e0
#define ADDR_Mutex_Lock 0x08258b34
#define ADDR_Mutex_Unlock 0x08258b38
#define ADDR_Mutex_Init 0x08258b3c
#define ADDR_HoldSwitch_GetLogicalState 0x0807c010
#define ADDR_HoldSwitch_ReadHardwareBit 0x082dc010
#define ADDR_Platform_InitMicrosecondCounter 0x083619c4
#define ADDR_SystemUI_HandleHoldChange 0x08108c14
#define ADDR_Platform_HasRegionalVolumeLimit 0x0804b17c
#define ADDR_File_OpenLegacyPath 0x0804f784
#define ADDR_File_ReadLegacyHandle 0x0804f838
#define ADDR_File_WriteLegacyHandle 0x0804ff68
#define ADDR_File_CloseLegacyHandle 0x0804f6e4
#define ADDR_File_SeekLegacyHandle 0x0804f88c
#define ADDR_BufferedFile_GetPosition 0x08037f98
#define ADDR_Preferences_LoadFile 0x0809c69c
#define ADDR_Preferences_LoadAndMigrate 0x080587f4
#define ADDR_Startup_CopyImageToRuntimeRegions 0x22004700
#define ADDR_Platform_ImportBootSysInfo 0x08360668
#define ADDR_Platform_GetImportedSysInfo 0x08360620
#define ADDR_Platform_GetHeapRegion 0x0804b448
#define ADDR_Memory_InitPlatformHeapBounds 0x0806bf34
#define ADDR_System_CreateAndStartConfiguredTasks 0x0805d5f4
#define ADDR_Preferences_FlushIfDirty 0x08063760
#define ADDR_Preferences_MarkDirty 0x080537bc
#define ADDR_Preferences_SetClickerRoute 0x0805bef0
#define ADDR_Preferences_GetClickerRoute 0x080465fc
#define ADDR_PreferencesService_Get 0x08167a54
#define ADDR_File_DeleteLegacyPath 0x0804f99c
#define ADDR_Runtime_Abort 0x0802606c
#define ADDR_Runtime_ReportUncaughtException 0x0825de34
#define ADDR_DirectoryIterator_Init 0x081e57dc
#define ADDR_DirectoryIterator_Next 0x081e54e8
#define ADDR_DirectoryIterator_Destroy 0x081e5928
#define ADDR_Directory_CreateWithParents 0x08090840
#define ADDR_NativePath_InitEmpty 0x0826fcc4
#define ADDR_NativePath_Destroy 0x0826fcf4
#define ADDR_Thread_Create 0x0804b4f4
#define ADDR_Thread_GetCurrentRecord 0x0804b744
#define ADDR_IRQ_Entry 0x22003080
#define ADDR_IRQ_Dispatch 0x22008150
#define ADDR_Clock_GetMicroseconds 0x0804b7ac
#define ADDR_SetWarmBootModeTag 0x082b3a44
#define ADDR_ResetSocAndWait 0x0835e934
#define ADDR_Timer_Stop 0x081219d0
#define ADDR_Settings_HandleAction 0x0821daf4
#define ADDR_Settings_CollectPropertyResources 0x08295174
#define ADDR_SettingsModel_GetInstance 0x081e0ff4
#define ADDR_SettingsModel_Construct 0x081e4010
#define ADDR_SettingsModel_ToggleClicker 0x081e300c
#define ADDR_SettingsModel_NotifyClicker 0x081e22f8
#define ADDR_SettingsModel_GetProperty 0x081e11f4
#define ADDR_SettingsModel_GetIndexedProperty 0x081e30ac
#define ADDR_SettingsModel_ToggleMainMenuItem 0x081e3460
#define ADDR_SettingsModel_ToggleMusicMenuItem 0x081e36ac
#define ADDR_SettingsModel_ApplyMainMenuPreferences 0x081e2a50
#define ADDR_SettingsModel_ApplyMusicMenuPreferences 0x081e2e48
#define ADDR_SettingsModel_ResetMainMenu 0x081e1024
#define ADDR_SettingsModel_ResetMusicMenu 0x081e1134
#define ADDR_SettingsModel_LoadPreferences 0x081e3f00
#define ADDR_SettingsModel_GetEQPresetPreview 0x081e0f74
#define ADDR_SettingsModel_GetEQPresetName 0x081e0fc0
#define ADDR_MainMenuSettings_HandleSelection 0x081d2200
#define ADDR_MusicMenuSettings_HandleSelection 0x081dd6c8
#define ADDR_SettingsModel_ResetAllSettings 0x081e1e6c
#define ADDR_SettingsController_ConfirmResetAll 0x08212820
#define ADDR_SettingsModel_SelectEQMenuIndex 0x081e4008
#define ADDR_SettingsModel_SetPreviewIndex 0x081e11d4
#define ADDR_SettingsEQ_HandleAction 0x081345e4
#define ADDR_SettingsEQ_ApplySelection 0x081347b0
#define ADDR_SettingsEQ_ShowPreview 0x08134690
#define ADDR_SettingsMainMenu_HandleAction 0x081d2144
#define ADDR_SettingsEQ_HandleEvent 0x08398e78
#define ADDR_SettingsEQ_SelectCurrentRow 0x08134750
#define ADDR_SettingsBacklight_SelectCurrentRow 0x08100d88
#define ADDR_SettingsMusicMenu_HandleAction 0x081dd60c
#define ADDR_SettingsModel_AdjustBrightness 0x081e2ee8
#define ADDR_CppString_Init 0x083cf82c
#define ADDR_CppString_InitBytes 0x083cf87c
#define ADDR_CppString_Destroy 0x083cf7d4
#define ADDR_String_CompareCString 0x0838886c
#define ADDR_StringInputStream_Init 0x0829e7b8
#define ADDR_StringInputStream_Destroy 0x0829e81c
#define ADDR_NativeString_InitEmpty 0x0826de38
#define ADDR_NativeString_Destroy 0x0826de7c
#define ADDR_NativeString_GetUtf8 0x0829bac0
#define ADDR_NativeString_AssignUtf16 0x0826cfa0
#define ADDR_SongText_CompareUtf16 0x0803a574
#define ADDR_StringCache_ReadToken 0x080a94ec
#define ADDR_StringCache_ReleaseToken 0x080bb0c8
#define ADDR_StringCache_GetTokenView 0x08093598
#define ADDR_PodcastEpisodes_DispatchAction 0x0823077c
#define ADDR_PodcastEpisodes_BuildAndShowContextualMenu 0x082309d0
#define ADDR_MenuList_RebuildVisibleItems 0x081946fc
#define ADDR_MenuList_SetItemTable 0x08194d1c
#define ADDR_MenuItems_RegisterResourceType 0x08194df0
#define ADDR_UI_ShowContextMenuItems 0x08129d78
#define ADDR_ContextMenuModel_Get 0x081debd0
#define ADDR_ContextMenuModel_SetItems 0x081decd0
#define ADDR_ContextMenuModel_GetRememberedIndex 0x081dec00
#define ADDR_UI_SetScreenState 0x08176af8
#define ADDR_ContextMenuModel_GetProperty 0x081dec28
#define ADDR_ContextMenuModel_GetIndexedProperty 0x081ded30
#define ADDR_UI_LoadLayoutInstances 0x081d64cc
#define ADDR_ContextMenuController_DispatchAction 0x081de6f8
#define ADDR_ContextMenuController_ForwardAction 0x081dea30
#define ADDR_ScreenController_SwitchLayoutDefault 0x08129994
#define ADDR_UI_GetLayoutManager 0x081747ec
#define ADDR_MenuList_QueryProperty 0x08194c2c
#define ADDR_ControllerRegistry_InvokeControllerFactory 0x081d58dc
#define ADDR_ControllerRegistry_InvokeScreenFactory 0x081d5a40
#define ADDR_UI_FindServiceById 0x081c7d44
#define ADDR_UI_GetRootView 0x08113600
#define ADDR_UI_GetSelectionModel 0x0817dde4
#define ADDR_UI_GetControllerRegistry 0x081d5950
#define ADDR_UI_CreateScreenController 0x081d59e0
#define ADDR_UI_RegisterControllerFactories 0x081d5ac8
#define ADDR_UI_RegisterViewFactory 0x081d70d8
#define ADDR_UI_LoadLayoutEventTables 0x081d61b0
#define ADDR_UI_GetLayoutEventTable 0x081d63e0
#define ADDR_UIEventMap_ReadStream 0x082a191c
#define ADDR_ScreenController_CreateNamed 0x0812a0d8
#define ADDR_ScreenController_HandleEvent 0x0812a39c
#define ADDR_ResourceScreen_Activate 0x08398954
#define ADDR_ResourceScreen_Deactivate 0x08398b6c
#define ADDR_MenuItem_SetVisible 0x08194ff0
#define ADDR_ContextMenuItems_Append 0x08162390
#define ADDR_ContextMenuItems_Init 0x081624e8
#define ADDR_ContextMenuItems_Destroy 0x0816250c
#define ADDR_WheelEvent_DecodeMotion 0x08289490
#define ADDR_ButtonEvent_GetUIKeyCode 0x0829b978
#define ADDR_WheelAccumulator_Init 0x081b0efc
#define ADDR_WheelAccumulator_ConsumeMotion 0x081b0e4c
#define ADDR_WheelAccumulator_Reset 0x081b0e7c
#define ADDR_View_InitOptionsFromDescriptor 0x0829b090
#define ADDR_View_ResolveBounds 0x08264f9c
#define ADDR_View_Construct 0x08265bf8
#define ADDR_View_BindGraphicsContext 0x08264bdc
#define ADDR_View_GetLocalContentRect 0x08299014
#define ADDR_View_InvalidateRect 0x082655a0
#define ADDR_View_DrawBackgroundAndBorder 0x08263530
#define ADDR_View_Draw 0x0826576c
#define ADDR_View_DrawClipped 0x08265388
#define ADDR_View_HandleModelNotification 0x08265a2c
#define ADDR_ProgressBar_CreateFromResource 0x081d5fac
#define ADDR_UI_RegisterBuiltinViewFactories 0x081d7110
#define ADDR_ProgressBar_RefreshFromModel 0x082053a4
#define ADDR_TextView_CreateFromDescriptor 0x0817b238
#define ADDR_TextView_DrawTextWithColor 0x08287218
#define ADDR_MarqueeTextView_Draw 0x081a8dc8
#define ADDR_CoverFlowView_UpdateActiveState 0x0813818c
#define ADDR_GLAlbumArtView_UpdateActiveState 0x081641b8
#define ADDR_MainMediaMenu_DispatchCommonAction 0x08223d20
#define ADDR_NowPlaying_DispatchAction 0x08212dbc
#define ADDR_NowPlaying_BuildAndShowContextualMenu 0x0821505c
#define ADDR_SongList_DispatchAction 0x08218b24
#define ADDR_SongList_BuildAndShowContextualMenu 0x08219478
#define ADDR_NestedPlaylists_DispatchAction 0x0822ea1c
#define ADDR_NestedPlaylists_BuildAndShowContextualMenu 0x0822fdb4
#define ADDR_Genius_DispatchAction 0x0821c0e4
#define ADDR_Genius_BuildAndShowContextualMenu 0x0821d0cc
#define ADDR_ArtistsController_ShowContextMenu 0x0821fe9c
#define ADDR_UI_InitializeEventTimer 0x0810635c
#define ADDR_Timer_Start 0x08121268
#define ADDR_UITask_Create 0x081391b8
#define ADDR_UITask_Main 0x08139118
#define ADDR_Timer_SetPeriod 0x0812195c
#define ADDR_Controller_ExecuteNavigatorAction 0x08129998
#define ADDR_Navigator_PopTop 0x082896b8
#define ADDR_Navigator_PopToScreen 0x08289728
#define ADDR_Navigator_PopToMainScreen 0x08289714
#define ADDR_Navigator_PopToPixoMainScreen 0x08289768
#define ADDR_ExtrasMenu_DispatchAction 0x0811a664
#define ADDR_UI_ConstructScreenDefinition 0x0813ca44
#define ADDR_UI_LoadScreenLayouts 0x0813c91c
#define ADDR_SettingsMenuListController_Deactivate 0x083a4a90
#define ADDR_ResourceScreen_HandleEvent 0x08398bdc
#define ADDR_ScreenController_DispatchNamedEvent 0x0812a070
#define ADDR_BrightnessController_HandleEvent 0x081ec9d4
#define ADDR_BrightnessController_HandleWheel 0x081ec864
#define ADDR_WheelInput_QuantizeMotion 0x08289794
#define ADDR_WheelEvent_GetSpeed 0x082894e8
#define ADDR_ScreenController_PopSelf 0x081034f0
#define ADDR_ResourceService_GetIndexedResource 0x08138dd8
#define ADDR_MenuList_GetRowProperty 0x08194ecc
#define ADDR_MenuList_HandleNotification 0x08195120
#define ADDR_MenuItems_RegisterAll 0x081951cc
#define ADDR_BitmapView_LoadBitmap 0x08100020
#define ADDR_Object_GetInterface 0x0826c594
#define ADDR_UI_TransitionToLayout 0x0817518c
#define ADDR_Layout_GetKind 0x081d6ad4
#define ADDR_UI_AnimateInstanceIn 0x081774c8
#define ADDR_UI_AnimateInstanceOut 0x08177d84
#define ADDR_UI_AnimateSharedInstance 0x08176b28
#define ADDR_UI_InstanceEnterStyle 0x08179d04
#define ADDR_UI_InstanceLeaveStyle 0x0817a040
#define ADDR_UI_SlideLayerIn 0x081786c8
#define ADDR_UI_SlideLayerOut 0x08178924
#define ADDR_UI_MoveViewToLayer 0x08179684
#define ADDR_UI_HoldViewInBackgroundLayer 0x08179dbc
#define ADDR_UI_SlideStatusBarChildren 0x08178e2c
#define ADDR_UI_SlideLayerFromPane 0x081790ec
#define ADDR_UI_GetCompositorLayer 0x0826dfec
#define ADDR_SportTimerChosenDispatcher_Activate 0x0821abc4
#define ADDR_UI_EmitControllerEvent 0x0812a150
#define ADDR_ScreenController_ShowLayout 0x08398a94
#define ADDR_ControllerLayoutCache_GetOrInsert 0x0813ca2c
#define ADDR_UI_CreateTemplateFromResource 0x081df580
#define ADDR_View_FindViewById 0x08292580
#define ADDR_UI_FindTemplateView 0x081af514
#define ADDR_UI_DisplayPreparedLayout 0x08174464
#define ADDR_UI_ApplyInstanceResources 0x081781c0
#define ADDR_View_ApplyPropertyBindings 0x08175b7c
#define ADDR_Navigator_Get 0x08104d00
#define ADDR_Navigator_Push 0x0810514c
#define ADDR_UI_GetRootOwner 0x0813e204
#define ADDR_TextView_DrawString 0x082883f8
#define ADDR_TextView_DrawTextLayout 0x082872f8
#define ADDR_MenuList_RestoreState 0x0827d77c
#define ADDR_MenuList_SelectRow 0x0827d1b4
#define ADDR_UI_ResolveTemplateView 0x0817481c
#define ADDR_CapacityView_CreateFromDescriptor 0x081d587c
#define ADDR_CapacityView_LoadSegments 0x081370f4
#define ADDR_CapacityView_Draw 0x08137738
#define ADDR_USB_HandleSetupPacket 0x0828a404
#define ADDR_USB_DispatchControlRead 0x0828a7d4
#define ADDR_USB_HandleVendorDeviceRead 0x0828a27c
#define ADDR_USB_DispatchNoDataRequest 0x0828a780
#define ADDR_USB_HandleAppleVendorCommand 0x0828a054
#define ADDR_USB_GetControlWriteBuffer 0x0828a8f8
#define ADDR_USB_StartControlTransfer 0x0828a3b4
#define ADDR_USB_RunDeviceLoop 0x0828afd8
#define ADDR_USB_ContinueControlWrite 0x0828acd4
#define ADDR_USB_SelectConfigurationOrder 0x0828b668
#define ADDR_USBDeviceTask_Run 0x0813e598
#define ADDR_USB_IsAccessoryMode 0x0813e560
#define ADDR_SCSI_AcquireMedium 0x080ed98c
#define ADDR_AudioOutputQueue 0x22010398
#define ADDR_EQ_Presets 0x089d4b58
#define ADDR_EQ_PresetIds 0x083e4110
#define ADDR_DisplayLayerOrders 0x083f35ec
#define ADDR_DisplayBlendRegisters 0x083f35e0
#define ADDR_Bitmap_vtable 0x08989298
#define ADDR_WheelState 0x089d2b8c
#define ADDR_ButtonKeyCodes 0x089d2ec5
#define ADDR_BrowserRegistry 0x08a1a6dc
#define ADDR_BlockCache 0x089d380c
#define ADDR_MainHeap 0x089d2c78
#define ADDR_ATA_Identify 0x08ae087c
#define ADDR_StorageArbiter 0x08a7e8f8
#define ADDR_DiskClient 0x08a7e920
#define ADDR_DiskClient_vtable 0x08996b78
#define ADDR_ScsiDiskClient_vtable 0x0898c96c
#define ADDR_MusicLoadingTask_vtable 0x08994174
#define ADDR_App 0x089d2cb4
#define ADDR_ResourceNameMap 0x08ae02c4
#define ADDR_ClickerRoute 0x089d2bdc
#define ADDR_IRQ_Nesting 0x22008af4
#define ADDR_MainMenuSettingsRows 0x08a815f0
#define ADDR_MusicMenuSettingsRows 0x08a81898
#define ADDR_ScreenController_vtable 0x0899d3fc
#define ADDR_View_vtable 0x089ad8ec
#define ADDR_TemplateView_vtable 0x08997824
#define ADDR_SportTimerChosenDispatcher_vtable 0x089a72dc
#define ADDR_SettingsMenuListController_vtable 0x089a3ae0
#define ADDR_ExtrasMenuController_vtable 0x0899cb30
#define ADDR_ArtistsController_vtable 0x089a40e4
#define ADDR_AlbumsController_vtable 0x089a3514
#define ADDR_Navigator 0x08a822cc
#define ADDR_TextView_vtable 0x089af07c
#define ADDR_SelectedRowTextView_vtable 0x08993504
#define ADDR_MenuList_vtable 0x08992c60
#define ADDR_MenuListModel_vtable 0x08992c70

#define SITE_ArtworkDatabase_Import_call 0x0804d458
#define SITE_ArtworkDatabase_ReadImageThumbs_call 0x08074f34
#define SITE_ArtworkDatabase_FindById_PhotoAlbum_call 0x08074d44
#define SITE_ArtworkDatabase_FindByDbid_0804581c_call 0x0804581c
#define SITE_ArtworkDatabase_FindByDbid_08046494_call 0x08046494
#define SITE_AudioOutputQueue_SilenceFill_call 0x22006e80
#define SITE_EQ_Process_slot 0x089ae0e0
#define SITE_EQ_Reset_slot 0x089ae0d4
#define SITE_EQ_SetPreset_Load_call 0x082735dc
#define SITE_EQ_SetRate_Load_call 0x08273610
#define SITE_EQ_DspCount_word 0x082735c8
#define SITE_EQ_TrackGuard_word 0x08173b74
#define SITE_EQ_SettingsCount_word 0x080bb498
#define SITE_EQ_ProviderCount_word 0x081e1ae8
#define SITE_EQ_MenuIds_literal 0x0807bfb0
#define SITE_EQ_SettingsIds_literal 0x080bb4ac
#define SITE_EQ_PreviewTable_literal 0x081e0fa8
#define SITE_EQ_NameTable_literal 0x081e0ff0
#define SITE_EQ_ResourceTable_literal 0x082955dc
#define SITE_VolumePolicy_RegionTest_word 0x0804745c
#define SITE_Volume_EUOffCeiling_word 0x081558f4
#define SITE_Volume_EUDefaultLimit_word 0x08155af0
#define SITE_VolumePolicy_WarningPolicyTest_word 0x08155a90
#define SITE_Volume_StartupClampTest_word 0x08058a30
#define SITE_VolumeExposure_TrackFinishTest_word 0x081732dc
#define SITE_MediaPlayer_PostVolumeExposureLimitEvent_word 0x080cfe74
#define SITE_GameManifest_InitReader_call 0x080f9418
#define SITE_GraphicsContext_FillRect_call 0x0825ae9c
#define SITE_ArtworkReflection_Blend_call 0x0826a890
#define SITE_IAP_VerifyDeviceSignatureRSA_call 0x081893a8
#define SITE_IAP_VerifyDeviceSignatureAlternate_call 0x081893fc
#define SITE_HoldSwitch_NotifyRawChange_call 0x08055b44
#define SITE_MediaPlayer_ToggleStopped_call 0x08170360
#define SITE_AlbumSubtitle_slot 0x0898cbf4
#define SITE_Albums_CancelAppend_call 0x0821c004
#define SITE_MediaListController_DispatchAction_entry 0x0820f5a4
#define SITE_MediaListController_HandleShowContextualMenu_entry 0x082102b0
#define SITE_MediaList_OnTheGoAdd_call 0x0820f9cc
#define SITE_ITunesDB_ImportTrackDataset_call 0x08058f1c
#define SITE_ITunesDB_AlbumArtist_word 0x0808adcc
#define SITE_ITunesDB_SortArtist_word 0x0808add0
#define SITE_ITunesDB_SortAlbumArtist_word 0x0808ade8
#define SITE_TrackData_DestroyStorage_call 0x0803e650
#define SITE_MediaBrowser_BuildArtists_entry 0x08048fe8
#define SITE_MediaBrowser_ReadArtistName_entry 0x08047b54
#define SITE_MediaBrowser_ReadArtistJumpLabel_entry 0x08047bc8
#define SITE_MediaBrowser_ArtistFilterCompare_call 0x080d7bd8
#define SITE_AlbumGroup_literal 0x08048fe4
#define SITE_LibraryBrowser_Destroy_entry 0x08133c30
#define SITE_MediaListSelection_SelectArtworkSource_call 0x08169e30
#define SITE_AlbumArtworkProvider_Create_call 0x08138c34
#define SITE_BlockCache_ImportCost_word 0x0805ddd4
#define SITE_BlockCache_ReserveNative_word 0x0805ddd8
#define SITE_BlockCache_FloorTest_word 0x0805dddc
#define SITE_BlockCache_FloorSet_word 0x0805dde0
#define SITE_MediaPlayer_ReadAheadMinTest_word 0x0822c944
#define SITE_MediaPlayer_ReadAheadMinSet_word 0x0822c948
#define SITE_MediaList_GetEntryOrderIndex_entry 0x080497ac
#define SITE_TrackData_ArtworkId_word 0x0808a868
#define SITE_FixedPool_SlotRoundAdd_word 0x080530b4
#define SITE_FixedPool_SlotRoundMask_word 0x080530b8
#define SITE_EAppBlocks_Acquire_call 0x0819d6d0
#define SITE_EAppBlocks_Release_call 0x0819d6d8
#define SITE_Sleep_Idle_call 0x0819b32c
#define SITE_Sleep_Request_call 0x0819bba8
#define SITE_Preferences_FlushIfDirty_call 0x0806392c
#define SITE_DiskClient_Yield_slot 0x08996ba0
#define SITE_App_HandleDiskModeRequest_call 0x0810ad54
#define SITE_SystemUI_HandleHoldChange_call 0x0810af14
#define SITE_MusicLoadingTask_Run_slot 0x0899417c
#define SITE_DiskModeExit_PopToMainScreen_call 0x0810b65c
#define SITE_ResourceBlockList_AppendBorrowedBlock_call 0x0817a88c
#define SITE_ResourceBank_InitOverrides_call 0x081c7ea8
#define SITE_ResourceBank_NextTemplate_call 0x081af75c
#define SITE_Preferences_LoadFile_call 0x08058808
#define SITE_HeapRegionBase_literal 0x0804b478
#define SITE_RuntimeHeapBase_literal 0x0807c048
#define SITE_Panic_entry 0x0803930c
#define SITE_Abort_entry 0x0802606c
#define SITE_RegionalVolumeResult_word 0x0804b188
#define SITE_Runtime_ReportUncaughtException_entry 0x0825de34
#define SITE_IRQ_Dispatch_entry 0x22008150
#define SITE_HoldSwitch_GetLogicalState_entry 0x0807c010
#define SITE_Settings_Action_slot 0x089a3bf4
#define SITE_Settings_SecondaryAction_slot 0x089a7c3c
#define SITE_SettingsMainMenu_Action_slot 0x089a00f4
#define SITE_SettingsMainMenu_SecondaryAction_slot 0x08996280
#define SITE_SettingsMusicMenu_Action_slot 0x089a05bc
#define SITE_SettingsMusicMenu_SecondaryAction_slot 0x08997094
#define SITE_Settings_Property_slot 0x08997b60
#define SITE_Settings_IndexedProperty_slot 0x08997b7c
#define SITE_EQ_PreviewCount_word 0x081e0f74
#define SITE_EQ_NameCount_word 0x081e0fc0
#define SITE_EQ_ResourceCount_word 0x08295420
#define SITE_MainMenu_Index_call 0x081d2214
#define SITE_MusicMenu_Index_call 0x081dd6dc
#define SITE_MainMenu_Toggle_call 0x081d225c
#define SITE_MusicMenu_Toggle_call 0x081dd724
#define SITE_MusicMenu_Apply_call 0x081e3f48
#define SITE_SettingsModel_ResetMainMenu_entry 0x081e1024
#define SITE_SettingsModel_ResetMusicMenu_entry 0x081e1134
#define SITE_SettingsModel_ResetAllSettings_call 0x08212844
#define SITE_TrackCount_0803b2d8_word 0x0803b2d8
#define SITE_TrackCount_0803bf9c_word 0x0803bf9c
#define SITE_TrackCount_0803c9bc_word 0x0803c9bc
#define SITE_TrackCount_0804789c_word 0x0804789c
#define SITE_TrackCount_0804793c_word 0x0804793c
#define SITE_TrackCount_08047970_word 0x08047970
#define SITE_TrackCount_08047c7c_word 0x08047c7c
#define SITE_TrackCount_08048e08_word 0x08048e08
#define SITE_TrackCount_08048e48_word 0x08048e48
#define SITE_TrackCount_08049070_word 0x08049070
#define SITE_TrackCount_08049368_word 0x08049368
#define SITE_TrackCount_08049938_word 0x08049938
#define SITE_TrackCount_080499c4_word 0x080499c4
#define SITE_TrackCount_0804a180_word 0x0804a180
#define SITE_TrackCount_08051eb8_word 0x08051eb8
#define SITE_TrackCount_080562f0_word 0x080562f0
#define SITE_TrackCount_080586c0_word 0x080586c0
#define SITE_TrackCount_080587b0_word 0x080587b0
#define SITE_TrackCount_08059788_word 0x08059788
#define SITE_TrackCount_0805cb04_word 0x0805cb04
#define SITE_TrackCount_0805cf70_word 0x0805cf70
#define SITE_TrackCount_0806294c_word 0x0806294c
#define SITE_TrackCount_080637d4_word 0x080637d4
#define SITE_TrackCount_08063834_word 0x08063834
#define SITE_TrackCount_0807b8b8_word 0x0807b8b8
#define SITE_TrackCount_0807b8f0_word 0x0807b8f0
#define SITE_TrackCount_0807c87c_word 0x0807c87c
#define SITE_TrackCount_08082248_word 0x08082248
#define SITE_TrackCount_08082280_word 0x08082280
#define SITE_TrackCount_080822a8_word 0x080822a8
#define SITE_TrackCount_0808232c_word 0x0808232c
#define SITE_TrackCount_08089058_word 0x08089058
#define SITE_TrackCount_0808905c_word 0x0808905c
#define SITE_TrackCount_0808909c_word 0x0808909c
#define SITE_TrackCount_080890a4_word 0x080890a4
#define SITE_TrackCount_080890a8_word 0x080890a8
#define SITE_TrackCount_080890b0_word 0x080890b0
#define SITE_TrackCount_08092e24_word 0x08092e24
#define SITE_TrackCount_08092e28_word 0x08092e28
#define SITE_TrackCount_080931b0_word 0x080931b0
#define SITE_TrackCount_080931b4_word 0x080931b4
#define SITE_TrackCount_0809cdc0_word 0x0809cdc0
#define SITE_TrackCount_0809cdc4_word 0x0809cdc4
#define SITE_TrackCount_0809cdd4_word 0x0809cdd4
#define SITE_TrackCount_0809cddc_word 0x0809cddc
#define SITE_TrackCount_0809cde0_word 0x0809cde0
#define SITE_TrackCount_0809cde8_word 0x0809cde8
#define SITE_TrackCount_0809f69c_word 0x0809f69c
#define SITE_TrackCount_080b3460_word 0x080b3460
#define SITE_TrackCount_080b3dcc_word 0x080b3dcc
#define SITE_TrackCount_080b3dec_word 0x080b3dec
#define SITE_TrackCount_080bf914_word 0x080bf914
#define SITE_TrackCount_080bf9b8_word 0x080bf9b8
#define SITE_TrackCount_080d16b4_word 0x080d16b4
#define SITE_TrackCount_080d16b8_word 0x080d16b8
#define SITE_TrackCount_080d43cc_word 0x080d43cc
#define SITE_TrackCount_081087e8_word 0x081087e8
#define SITE_TrackCount_0810897c_word 0x0810897c
#define SITE_TrackCount_0816ebc4_word 0x0816ebc4
#define SITE_TrackCount_0829c884_word 0x0829c884
#define SITE_PodcastEpisodes_CancelAppend_call 0x08230a10
#define SITE_PodcastEpisodes_Action_slot 0x089a67a0
#define SITE_UI_LoadLayoutEvents_call 0x081d63f8
#define SITE_UI_TemplateEventsGet_call 0x081d6218
#define SITE_UI_LayoutEventsGet_call 0x081d62b4
#define SITE_UI_ControllerEventsGet_call 0x081d634c
#define SITE_ScreenController_Deactivate_slot 0x0899d4ec
#define SITE_SettingsMenuListController_Deactivate_slot 0x089a3bd0
#define SITE_NowPlaying_Action_slot 0x0899be50
#define SITE_NowPlaying_SecondaryAction_slot 0x089a2e00
#define SITE_Music_Action_slot 0x089a940c
#define SITE_Music_SecondaryAction_slot 0x089a507c
#define SITE_Main_Action_slot 0x089a4c9c
#define SITE_Main_SecondaryAction_slot 0x089a8fa4
#define SITE_TextView_DrawColor_call 0x082872a8
#define SITE_View_BackgroundFill_call 0x082636b4
#define SITE_MarqueeTextView_DrawColor_call 0x081a8f0c
#define SITE_UI_DisplayPreparedLayout_call 0x08175208
#define SITE_ContextMenuModel_Property_slot 0x08997648
#define SITE_ContextMenuModel_IndexedProperty_slot 0x08997664
#define SITE_ContextMenu_ScreenActionCompare_call 0x081de7b0
#define SITE_CoverFlow_ClearColor_call 0x08138304
#define SITE_AlbumArt_ClearColor_call 0x08164230
#define SITE_SongList_CancelAppend_call 0x082195ec
#define SITE_NestedPlaylists_CancelAppend_call 0x0822ffb8
#define SITE_Genius_CancelAppend_call 0x0821d26c
#define SITE_NowPlaying_CancelAppend_call 0x0821526c
#define SITE_Artists_CancelAppend_call 0x0821fee8
#define SITE_SongList_DispatchAction_entry 0x08218b24
#define SITE_NestedPlaylists_DispatchAction_entry 0x0822ea1c
#define SITE_Genius_DispatchAction_entry 0x0821c0e4
#define SITE_UITask_StackShift_word 0x081391dc
#define SITE_Extras_Action_slot 0x0899cc44
#define SITE_Artists_Action_slot 0x089a41f8
#define SITE_Albums_Action_slot 0x089a3628
#define SITE_TextView_DrawTextWithColor_call 0x08288514
#define SITE_TextView_DrawTextLayout_call 0x082884a8
#define SITE_TextView_DrawString_entry 0x082883f8
#define SITE_USB_ContinueControlWrite_call 0x0828b1e0
#define SITE_USB_HandleVendorDeviceRead_call 0x0828a820
#define SITE_USB_DispatchNoDataRequest_call 0x0828a458
#define SITE_USB_GetControlWriteBuffer_call 0x0828a434

#define ArtworkDatabase_image_count 0x8
#define ArtworkDatabase_first_image 0xc
#define ArtworkDatabase_last_image 0x10
#define ArtworkDatabase_image_pool 0x2c
#define ArtworkImage_next 0x0
#define ArtworkImage_id 0xc
#define ArtworkImage_dbid 0x10
#define ArtworkImage_thumb_count 0x22
#define ArtworkImage_thumbs 0x24
#define ArtworkReader_database 0x10
#define ArtworkThumb_next 0x0
#define ArtworkThumb_image 0x4
#define ArtworkThumb_format 0x8
#define ArtworkThumb_dimensions 0xc
#define ArtworkThumb_padding 0x10
#define ArtworkThumb_offset 0x18
#define ArtworkThumb_size 0x1c
#define ArtworkThumb_file 0x20
#define ArtworkThumb_bytes 0x24
#define AudioOutputQueue_underruns_since_reset 0x28
#define AudioOutputQueue_underruns 0x2c
#define DisplayPipe_control 0x8
#define DisplayPipe_video_mode 0x28
#define DisplayPipe_video_strides 0x2c
#define DisplayPipe_video_source_start 0x30
#define DisplayPipe_video_source_end 0x34
#define DisplayPipe_video_plane0 0x38
#define DisplayPipe_video_plane1 0x3c
#define DisplayPipe_video_plane2 0x40
#define DisplayPipe_video_plane3 0x44
#define DisplayPipe_video_scale 0x4c
#define DisplayPipe_video_position 0x50
#define DisplayPipe_video_size 0x54
#define DisplayPipe_slot0 0x58
#define DisplayPipe_slot_step 0x18
#define DisplayPipe_order 0xd4
#define DisplaySlot_stride 0x0
#define DisplaySlot_format 0x4
#define DisplaySlot_source 0x8
#define DisplaySlot_size 0xc
#define DisplaySlot_position 0x14
#define BootDescriptor_flags 0x10
#define BootDescriptor_sysinfo_magic 0x18
#define BootDescriptor_sysinfo 0x1c
#define GraphicsContext_bytes 0x44
#define GraphicsContext_mode 0x10
#define GraphicsContext_color 0x11
#define GraphicsContext_background_color 0x15
#define GraphicsContext_bitmap 0x1c
#define GraphicsContext_font 0x20
#define GraphicsContext_font_flags 0x28
#define GraphicsContext_origin_x 0x2c
#define GraphicsContext_origin_y 0x30
#define GraphicsContext_clip_top 0x34
#define GraphicsContext_clip_left 0x38
#define GraphicsContext_clip_bottom 0x3c
#define GraphicsContext_clip_right 0x40
#define BMap_format 0x0
#define BMap_stride 0x4
#define BMap_depth 0x6
#define BMap_bounds 0x8
#define BMap_payload_bytes 0x18
#define BMap_payload 0x1c
#define FontResource_family 0x0
#define FontResource_size 0x4
#define FontResource_style 0x8
#define FontResource_bytes 0xc
#define Bitmap_pixels 0x4
#define Bitmap_bounds 0x98
#define Bitmap_owns_pixels 0xb4
#define Bitmap_bytes 0xb8
#define WheelState_notify_pending 0x0
#define WheelState_reported_buttons 0x18
#define WheelState_buttons 0x20
#define WheelState_skip_packet 0x1
#define WheelState_threshold 0x4
#define WheelState_motion 0x8
#define WheelState_position 0xc
#define WheelState_reported_position 0x10
#define WheelState_touch_count 0x14
#define WheelState_previous_touch 0x1c
#define WheelState_touch 0x24
#define InputEvent_kind 0x4
#define InputEvent_wheel_delta 0xc
#define InputEvent_wheel_speed 0x14
#define InputEvent_key 0x10
#define ITunesReader_file 0x40000
#define ITunesReader_owner 0x40004
#define MediaBrowser_preferences 0xf00
#define MediaBrowser_library 0xf64
#define MediaBrowser_track_data 0xf60
#define MediaBrowser_artists 0xe64
#define MediaBrowser_albums 0xeb0
#define MediaBrowser_artist_tokens 0xe4c
#define MediaBrowser_artist_jump 0xe50
#define MediaBrowser_artist_count 0xe54
#define MediaBrowser_order_flag 0xe3c
#define MediaEntry_list 0x0
#define MediaEntry_parent 0x4
#define MediaEntry_next 0x8
#define MediaEntry_previous 0xc
#define MediaEntry_id 0x10
#define MediaEntry_flags 0x1c
#define MediaEntry_song_record 0x20
#define MediaEntry_song_shuffle_rank 0x24
#define MediaEntry_song_next_record_entry 0x28
#define MediaEntry_group_first 0x20
#define MediaEntry_group_last 0x24
#define MediaEntry_groups 0x2c
#define MediaEntry_tracks 0x2e
#define MediaEntry_size 0x30
#define MediaList_track_data 0xc
#define MediaList_root 0x40
#define MediaList_order_mode 0xc8
#define MediaList_repeat_mode 0x184
#define MediaList_flags 0x18c
#define MediaList_order_flags 0x18d
#define MediaList_shuffle_index 0x3b4
#define MediaList_order_index 0x3ac
#define MediaList_jump_index 0x544
#define MediaListSelection_artwork_category 0x83f
#define MediaOrder_capacity 0x4
#define MediaOrder_bytes 0x8
#define MediaOrder_count 0xc
#define MediaOrder_entries 0x10
#define MediaItem_entry 0x4
#define MediaItem_record 0x8
#define MediaItem_browser 0xc
#define MediaItem_generation 0x10
#define MediaItem_flags 0x14
#define MediaItem_bytes 0x18
#define MediaItem_title_method 0x50
#define MediaItem_album_method 0x54
#define MediaItem_artist_method 0x58
#define MediaItem_display_artist_method 0x5c
#define MediaItem_genre_method 0x60
#define MediaItem_composer_method 0x64
#define MediaItem_grouping_method 0x68
#define MediaItem_duration_method 0xa0
#define MediaListView_list 0x4
#define MediaListView_browser 0x8
#define MediaListView_generation 0xc
#define MediaRecord_entries 0x24
#define MediaRecord_media_kind 0xbc
#define MediaPlayer_current_track 0x54
#define MediaPlayer_prepared_track 0x58
#define MediaPlayer_playback_view 0x60
#define MediaPlayer_source_view 0x74
#define MediaPlayer_artwork_session 0x88
#define MediaPlayer_playback_source 0x948
#define MediaPlayer_play_index_method 0xe0
#define MediaPlayer_remove_entry_method 0x130
#define MediaPlayer_current_index_method 0x158
#define MediaPlayer_current_item_method 0x15c
#define MediaPlayer_item_at_index_method 0x170
#define MediaPlayer_current_index 0x5c
#define MediaPlayer_mutex 0xa4c
#define SongModel_get_item_method 0x22c
#define TrackData_artist_cache 0x118
#define TrackData_artwork_id 0xf0
#define TrackData_dbid 0x110
#define BlockCache_free_blocks 0x4
#define BlockCache_free_count 0x14
#define BlockCache_clients 0x28
#define BlockCache_block_bytes 0x30
#define BlockCache_total_blocks 0x34
#define BlockCache_trimmed 0x38
#define BlockCache_task_queue 0x3c
#define BlockCache_lock 0x148
#define BlockCache_core_mutex 0x180
#define BlockCache_pending_cond 0x19c
#define BlockCache_assigned_cache 0x1bc
#define BlockCache_pending_count 0x1c0
#define BlockCacheClient_cache 0x4
#define BlockCacheClient_blocks 0x8
#define BlockCacheClient_block_count 0x18
#define BlockCacheClient_assigned_blocks 0x1c
#define BlockCacheClient_mutex 0x24
#define BlockCacheClient_weight 0x40
#define BlockCacheClient_flags 0x44
#define BlockCacheClient_desired_bytes 0x48
#define EAppBlockManager_client 0x14
#define EAppBlockManager_block_list 0x28
#define EAppBlockManager_block_count 0x2c
#define EAppBlockManager_request_blocks 0x34
#define FixedPool_magic 0x0
#define FixedPool_slot_size 0x4
#define FixedPool_slots 0x8
#define FixedPool_chunks 0xc
#define FixedPoolChunk_magic 0x0
#define FixedPoolChunk_pool 0x4
#define FixedPoolChunk_next 0x8
#define FixedPoolChunk_slots 0xc
#define FixedPoolChunk_free 0x10
#define FixedPoolChunk_first 0x14
#define FixedPoolChunk_last 0x18
#define FixedPoolChunk_free_list 0x1c
#define FixedPoolChunk_bytes 0x20
#define MainHeap_free_bytes 0x0
#define MainHeap_total_bytes 0x4
#define MainHeap_used_bytes 0x8
#define App_browser 0x30
#define App_disk_mode_requested 0x4cc
#define App_library_load_started 0x4d2
#define App_in_disk_mode 0x4d3
#define StorageArbiter_active 0x10
#define StorageArbiter_clients 0x14
#define StorageArbiter_owner 0x20
#define StorageArbiter_pending 0x24
#define ScsiDiskClient_context 0x8
#define ScsiDiskClient_wanted 0xc
#define ScsiDiskClient_state 0xd
#define ResourceBlock_length 0x0
#define ResourceBlock_kind 0x4
#define ResourceBlock_data 0x8
#define ResourceBlockList_type 0x8
#define ResourceBlockList_id 0xc
#define ResourceBank_kind 0x0
#define ResourceBank_rom_index 0x4
#define ResourceBank_overrides 0x18
#define ResourceOverride_length 0x0
#define ResourceOverride_data 0x4
#define ResourceOverride_bytes 0x8
#define Preferences_object 0x378
#define Preferences_get_method 0xdc
#define Preferences_set_method 0xd4
#define PreferencesFile_clicker_route 0xb00
#define PreferencesFile_dirty 0xb94
#define DirectoryIterator_bytes 0x44
#define NativePath_bytes 0x8
#define ThreadRecord_kernel_thread 0x0
#define ThreadRecord_context 0x8
#define ThreadRecord_name 0xc
#define ThreadRecord_stack_base 0x40
#define ThreadRecord_bytes 0x44
#define IRQFrame_spsr 0x0
#define IRQFrame_r0 0x4
#define IRQFrame_lr 0x38
#define IRQFrame_pc 0x3c
#define IRQFrame_bytes 0x40
#define MenuSettingsRow_item 0x0
#define MenuSettingsRow_title 0x4
#define MenuSettingsRow_layout_kind 0x8
#define MenuSettingsRow_preference 0xc
#define MenuSettingsRow_visible 0x10
#define MenuSettingsRow_bytes 0x14
#define CppString_bytes 0x4
#define NativeString_data 0x4
#define NativeString_bytes 0x8
#define StringInputStream_bytes 0x7c
#define Model_notify_method 0x58
#define ScreenController_push_method 0xd0
#define ScreenController_below 0x14
#define ScreenController_selection_model 0x44
#define ScreenController_pop_method 0xbc
#define ScreenController_switch_layout_method 0x120
#define ScreenController_emit_event_method 0x11c
#define ScreenController_pop_self_method 0xb8
#define ScreenController_named_event_method 0x110
#define ScreenController_show_layout_method 0x124
#define ScreenController_action_method 0x114
#define ScreenController_vtable_bytes 0x128
#define ScreenController_screen 0xb4
#define ScreenController_settings_model 0xb0
#define ScreenController_event_method 0x30
#define ScreenController_activate_method 0xec
#define ScreenController_deactivate_method 0xf0
#define Service_interface_method 0x14
#define View_model 0x38
#define View_property 0x44
#define View_refresh_method 0xd4
#define View_destroy_method 0x4
#define View_bytes 0xa4
#define View_options_bytes 0x5c
#define View_vtable_bytes 0x114
#define View_draw_method 0x74
#define View_bounds 0x80
#define View_id 0x4c
#define View_find_view_method 0xe4
#define View_linked_view_method 0xfc
#define View_flags 0x48
#define View_children 0xa8
#define View_child_count 0xac
#define View_child_items 0xb0
#define EventTimer_period_ms 0x4
#define EventTimer_recipient 0x24
#define EventTimer_event_queue 0x28
#define EventTimer_bytes 0x2c
#define Event_kind 0x4
#define Event_notification 0x8
#define Event_payload_method 0x14
#define TimerEventPayload_timer 0x8
#define EventRecipient_event_method 0x30
#define MenuRow_id 0x30
#define MenuRow_style 0x60
#define MenuRow_visible 0x64
#define MenuRow_title 0x68
#define MenuRow_title_from_model 0x94
#define Layout_screen_list_record 0x4
#define Layout_id 0x8
#define Layout_instances 0x10
#define Layout_instances_end 0x14
#define Layout_events 0x38
#define Layout_screen 0x58
#define Layout_kind 0xc
#define LayoutInstance_id 0x8
#define LayoutScreen_id 0x4
#define ResourceScreenController_screen_definition 0xb0
#define ResourceScreenController_bytes 0xec
#define SettingsMenuListController_screen_definition 0xc0
#define SettingsMenuListController_bytes 0xfc
#define ScreenDefinition_layout 0x30
#define ScreenDefinition_transition 0x36
#define ScreenDefinition_screen 0x4
#define ScreenDefinition_layout_cache 0xc
#define SsinInstance_view_bindings 0x0
#define SsinInstance_template 0x4
#define SsinInstance_layout_events_id 0x8
#define SsinInstance_flags 0x10
#define SsinInstance_enter_style 0xc
#define SsinInstance_leave_style 0xe
#define SsinInstance_shared_style 0x10
#define ViewDescriptor_frame 0x0
#define ViewDescriptor_top_offset 0xc
#define ViewDescriptor_top_reference_view 0x10
#define ViewDescriptor_top_anchor_mode 0x14
#define ViewDescriptor_id 0x30
#define ViewDescriptor_type 0x4c
#define ViewDescriptor_options_bytes 0x5c
#define TextViewDescriptor_source 0x64
#define TextViewDescriptor_font 0x68
#define TextViewDescriptor_color 0x70
#define ProgressDescriptor_maximum 0x5c
#define ProgressDescriptor_minimum 0x60
#define ProgressDescriptor_source 0x64
#define ProgressDescriptor_style 0x6c
#define WheelAccumulator_bytes 0x34
#define ItemTableBinding_item_table 0x4
#define ViewLayout_title 0x24
#define SourceBinding_service 0x0
#define SourceBinding_property 0x4
#define SourceBinding_destination 0x8
#define SelectionModel_current_index_method 0x168
#define SelectionModel_find_view_method 0xf8
#define ListInterface_select_method 0x174
#define Notification_code 0x4
#define ControlRange_value 0x0
#define ControlRange_minimum 0x4
#define ControlRange_maximum 0x8
#define ControlRange_bytes 0xc
#define MenuList_item_blocks 0x18
#define LayoutManager_screen 0x20
#define LayoutManager_layout 0x28
#define LayoutManager_kind 0x2c
#define LayoutManager_saved_transition 0x80
#define TemplateView_enter_styles 0xfc
#define TemplateView_leave_styles 0x107
#define TemplateView_shared_styles 0x112
#define TemplateResource_has_styles 0x4
#define TemplateResource_styles 0x6
#define TemplateStyle_enter 0x0
#define TemplateStyle_leave 0x2
#define TemplateStyle_shared 0x8
#define TemplateStyle_bytes 0xc
#define ContextMenuModel_items 0x18
#define ContextMenuModel_remembered_item 0x30
#define ContextMenuItems_bytes 0x18
#define ContextMenuItem_item 0x0
#define ContextMenuItem_action 0x4
#define ContextMenuItem_flags 0x8
#define ContextMenuItem_bytes 0xc
#define MediaListController_show_context_menu_method 0x148
#define MediaListController_map_visible_row_method 0x130
#define MediaListController_model 0xb4
#define SongListController_context_row 0xd4
#define SongListController_layout_cache 0xd8
#define SongListController_layout 0x108
#define GeniusController_context_row 0xd4
#define GeniusController_layout_cache 0xd8
#define GeniusController_layout 0x108
#define NestedPlaylistsController_context_row 0xe8
#define NestedPlaylistsController_layout_cache 0xf0
#define NestedPlaylistsController_layout 0x120
#define ArtistsController_layout_cache 0xd4
#define ArtistsController_layout 0x104
#define AlbumsController_layout_cache 0xd8
#define AlbumsController_layout 0x108
#define NowPlayingController_layout_cache 0xec
#define NowPlayingController_layout 0x11c
#define Navigator_top_controller 0x4
#define TextView_context 0xa4
#define TextView_text 0xf8
#define ScreenLayoutEntry_layout 0x0
#define ScreenLayoutEntry_kind 0x4
#define ScreenLayoutEntry_refresh 0x8
#define PodcastEpisodesController_layout_cache 0xdc
#define PodcastEpisodesController_layout 0x100
#define USBDevice_controller 0x34
#define USBDevice_interfaces 0x38
#define USBDevice_configuration_set 0x5c
#define USBDevice_setup 0x90
#define USBDevice_request_type 0x90
#define USBDevice_request 0x91
#define USBDevice_value 0x92
#define USBDevice_index 0x94
#define USBDevice_length 0x96
#define USBDevice_max_packet 0xa0
#define USBDevice_ep0_buffer 0xb4
#define USBDevice_host_to_device 0xb8
#define USBDevice_data_length 0xbc
#define USBDevice_configuration 0xc4
#define USBDevice_bytes 0xd0

#define DISPLAY_PIPE_BASE 0x38900000
#define DisplayLayerEnable_Layer0 0x40
#define DisplayLayerEnable_Layer1 0x20
#define DisplayLayerEnable_Layer2 0x10
#define DisplayLayerEnable_Layer3 0x8
#define DisplayLayerEnable_Layer4 0x4
#define DisplayLayerEnable_Video 0x80
#define DisplayFormat_RGB565 0x3
#define DisplayFormat_ARGB8888 0x7
#define DisplayFormat_YUV420 0x8
#define DisplayVideoMode_YUV420 0x100
#define DRAM_BASE 0x8000000
#define DRAM_END 0xc000000
#define DRAM_UNCACHED_ALIAS 0x80000000
#define IRAM_BASE 0x22000000
#define BOOT_DESCRIPTOR 0x2203ff00
#define BOOT_SYSINFO_MAGIC 0x53797349
#define MMIO_BASE 0x38000000
#define MMU_TRANSLATION_TABLE 0x2200c000
#define MMU_UNMAPPED_ADDRESS 0x10000000
#define IRAM_END 0x22040000
#define IRQ_STACK_TOP 0x8b3b1b0
#define GraphicsDrawMode_Text 0x21
#define BMapFormat_Mask4 0x4
#define BMapFormat_Mask8 0x8
#define BMapFormat_Indexed8 0x64
#define BMapFormat_Indexed16 0x65
#define BMapFormat_RGB565 0x565
#define BMapFormat_BGRA8888 0x1888
#define WheelButton_Center 0x1
#define WheelButton_Next 0x2
#define WheelButton_Previous 0x4
#define WheelButton_Play 0x8
#define WheelButton_Menu 0x10
#define InputEvent_ButtonDown 0x6
#define InputEvent_ButtonUp 0xd
#define InputEvent_Wheel 0x15
#define UIKey_Play 0x10
#define UIKey_Previous 0x59
#define UIKey_Next 0x5a
#define UIKey_Menu 0x6e
#define UIKey_Center 0x6f
#define WheelPositions 0x60
#define WheelTouch_Down 0x40000000
#define WheelEvent_Signal 0x5
#define AlbumArtist_order_mode 0x2d
#define MediaOrderMode_Database 0x1
#define MediaOrderMode_Shuffle 0x2
#define MediaOrderMode_Playback 0x33
#define MediaListFlags_Shuffled 0x1
#define MediaListFlags_PlaybackList 0x80
#define MediaListOrderFlags_Reversed 0x4
#define MediaEntryFlags_Group 0x100
#define MediaRepeat_All 0x1
#define MediaRepeat_One 0x2
#define HardwareSource_DiskModeRequest 0x8
#define HardwareSource_DiskGranted 0x9
#define HardwareSource_HoldChanged 0x6
#define AppMessage_DiskModeRequest 0x63800003
#define AppMessage_DiskGranted 0x63800004
#define AppMessage_HoldOn 0x63800013
#define AppMessage_HoldOff 0x63800014
#define ScsiDiskClientState_Refused 0x0
#define ScsiDiskClientState_Pending 0x1
#define ScsiDiskClientState_Granted 0x2
#define Resource_BMap 0x424d6170
#define Resource_Draw 0x44726177
#define Resource_SORC 0x534f5243
#define Resource_ITEM 0x4954454d
#define Resource_TEVT 0x54455654
#define Resource_SEVT 0x53455654
#define Resource_CEVT 0x43455654
#define Resource_SCST 0x53435354
#define Resource_SLst 0x534c7374
#define Resource_SLyt 0x534c7974
#define Resource_SSin 0x5353696e
#define Resource_VCvs 0x56437673
#define Resource_VCrv 0x56437276
#define Resource_VLyt 0x564c7974
#define Resource_View 0x56696577
#define Resource_TMLT 0x544d4c54
#define Resource_String 0x53747220
#define Resource_VSlt 0x56536c74
#define ResourceBankKind_Heap 0x1
#define ResourceBankKind_Rom 0x2
#define ClockRegister_Microseconds 0x3c7000b4
#define Service_Preferences 0x6000
#define Preference_Toggle 0x60bf
#define Preference_Clicker 0x608c
#define Clicker_Both 0x608d
#define Clicker_Off 0x608e
#define Clicker_Headphones 0x608f
#define Clicker_Speaker 0x6090
#define ClickerRoute_Speaker 0x1
#define ClickerRoute_Headphones 0x2
#define ClockRegister_PowerGate0 0x3c500048
#define PowerGate0_ATA 0x20
#define WarmBootTag_address 0x2203ff00
#define WarmBootMode_None 0x0
#define WarmBootMode_DiskMode 0x1
#define WarmBootMode_DiskScan 0x2
#define WarmBootMode_SoftwareUpdate 0x3
#define WarmBootMode_RetailOS 0x4
#define MainMenuSettings_row_count 0x22
#define MusicMenuSettings_row_count 0xd
#define SettingsProperty_ClickerValue 0x8912
#define SettingsProperty_ClickerDescription 0x8914
#define SettingsProperty_ClickerImage 0x8925
#define StringInputStream_ResourceMode 0xc
#define StringInputStream_DestroyComplete 0x2
#define AnchorMode_Top 0x1
#define AnchorMode_Left 0x2
#define AnchorMode_Bottom 0x3
#define AnchorMode_Right 0x4
#define Property_String 0x53747220
#define Property_Count 0x564d6178
#define Notify_None 0x4e6f6e65
#define Notify_All 0x2a2a2a2a
#define Service_Settings 0x8900
#define Service_NowPlaying 0x7f00
#define Service_ContextMenu 0x8180
#define ActionSupportQuery 0xdeadbeef
#define Event_Timer 0xf
#define EventPayload_Timer 0xd00
#define Transition_Standard 0x0
#define Transition_ForegroundPush 0x2
#define Transition_ScreenPush 0x3
#define CxxVtable_prefix_bytes 0x8
#define ViewBinding_ItemTable 0x3f1
#define Service_String 0x80
#define SourceDestination_Title 0x1
#define Property_Control 0x436e746c
#define Property_Visible 0x56566973
#define Interface_List 0x1700
#define Interface_Template 0x3b80
#define Event_Notification 0x16
#define Notification_LayoutShown 0x2502
#define Notification_LayoutChanged 0x2503
#define ViewOption_TopEdge 0x3e8
#define ViewOption_LeftEdge 0x3e9
#define ViewOption_BottomEdge 0x3ea
#define ViewOption_RightEdge 0x3eb
#define ViewOption_Visible 0x3ed
#define Property_Minimum 0x564d696e
#define Property_Layout 0x4c4c6179
#define Property_LayoutSet 0x4c4c6180
#define Property_Bitmap 0x424d6170
#define Property_Drawable 0x44726177
#define Property_ItemId 0x4c69746d
#define Property_ItemChanged 0x4c697445
#define MenuRowStyle_ModelLayout 0x1
#define MenuRowStyle_Text 0x2
#define MenuRowStyle_Check 0x3
#define MenuRowStyle_CheckIndented 0x4
#define MenuRowStyle_Submenu 0x5
#define MenuRowStyle_TwoLine 0x6
#define MenuRowStyle_TwoLineSubmenu 0x7
#define MenuRowStyle_Artwork 0xd
#define MenuRowStyle_ArtworkSubmenu 0xe
#define MenuRowStyle_InlineValue 0xc
#define Event_ModelChange 0x13
#define SourceDestination_InlineValue 0xa
#define SourceDestination_RowBitmap 0xc
#define SourceDestination_RowStatusBitmap 0x13
#define SourceDestination_RowStatusSelectedBitmap 0x14
#define ViewType_Progress 0x70676f67
#define ViewType_Text 0x74657874
#define ViewType_Capacity 0x63706374
#define UITask_stack_bytes 0x20000
#define Transition_Instant 0x6
#define Transition_Hidden 0x1
#define LayoutKind_NowPlaying 0x1
#define LayoutKind_CoverFlow 0x2
#define LayoutKind_Menu 0x3
#define LayoutKind_SplitList 0x4
#define LayoutKind_List 0x5
#define LayoutKind_MenuPane 0x6
#define LayoutKind_Clock 0x7
#define LayoutKind_FullScreen 0x8
#define LayoutKind_Page 0x9
#define LayoutKind_System 0xa

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
#define RES_SettingsMenu_Legal_Item 0x0dad0c7b
#define RES_SettingsMenu_About_Item 0x0dad0c6b
#define RES_SettingsMenu_Shuffle_Item 0x0dad0c6c
#define RES_SettingsMenu_Repeat_Item 0x0dad0c6d
#define RES_SettingsMenu_MainMenu_Item 0x0dad0c6e
#define RES_SettingsMenu_MusicMenu_Item 0x0dad0c6f
#define RES_SettingsMenu_VolumeLimit_Item 0x0dad0c70
#define RES_SettingsMenu_Backlight_Item 0x0dad0c71
#define RES_SettingsMenu_Brightness_Item 0x0dad0c72
#define RES_SettingsMenu_Audiobooks_Item 0x0dad0c73
#define RES_SettingsMenu_RadioRegions_Item 0x0dad0c74
#define RES_SettingsMenu_EQ_Item 0x0dad0c75
#define RES_SettingsMenu_Clicker_Item 0x0dad0c77
#define RES_SettingsMenu_DateTime_Item 0x0dad0c78
#define RES_SettingsMenu_SortBy_Item 0x0dad0c79
#define RES_SettingsMenu_Language_Item 0x0dad0c7a
#define RES_SettingsMenu_ResetSettings_Item 0x0dad0c7c
#define RES_SettingsMenu_Items 0x00000041
#define RES_SettingsMenu_SoundCheck_Item 0x0dad0c76
#define RES_SettingsMenu_Main_Screen 0x0dad0c6a
#define RES_SettingsMenu_Main_Layout 0x0dad0c7f
#define RES_Settings_MainMenu_Screen 0x0dad0c90
#define RES_Settings_MainMenu_Layout 0x0dad0c92
#define RES_Settings_MusicMenu_Screen 0x0dad0c93
#define RES_Settings_MusicMenu_Layout 0x0dad0c95
#define RES_Settings_MusicMenu_Items 0x00000043
#define RES_Settings_Legal_Screen 0x0dad0cb3
#define RES_Settings_Legal_Layout 0x0dad0cb4
#define RES_Settings_EQ_Items 0x00000047
#define RES_Settings_EQ_Screen 0x0dad0cae
#define RES_Settings_EQ_Layout 0x0dad0cb0
#define RES_Settings_EQ_Item 0x0dad0caf
#define RES_Settings_MainMenu_Items 0x00000042
#define RES_SettingsPreview_Template 0x0dad0c2e
#define RES_SettingsPreview_Caption_Text 0x0dad0618
#define RES_SettingsPreview_Background_View 0x0dad0c2f
#define RES_SettingsPreview_Background_Image 0x0dad0bf5
#define RES_Settings_Brightness_Screen 0x0dad0c98
#define RES_Settings_Brightness_Screen_Default 0x0dad0c99
#define RES_Settings_Brightness_Template 0x0dad0c5a
#define RES_Settings_Brightness_Bar 0x0dad0c60
#define RES_Settings_Brightness_Value_Text 0x0dad0c61
#define RES_SettingsMenus_Brightness_Layout 0x0dad0c84
#define RES_SettingsMenus_Legal_Layout 0x0dad0c8e
#define RES_SettingsInfo_Template_Brightness_Layout 0x0dad0c39
#define RES_SettingsInfo_Template_Legal_Layout 0x0dad0c40
#define RES_Settings_Icon_View 0x0dad0c30
#define RES_Settings_InfoDetail_View 0x0dad0c31
#define RES_Menu_Checked_Bitmap 0x0dad0c08
#define RES_Font_Bold 0x0dad0000
#define RES_Font_BoldSmall 0x0dad0001
#define RES_Font_RegularSmall 0x0dad0020
#define RES_Menu_CheckedSelected_Bitmap 0x0dad0c09
#define RES_Menu_Playing_Bitmap 0x0dad072c
#define RES_Menu_PlayingSelected_Bitmap 0x0dad072d
#define RES_Menu_Chevron_Bitmap 0x0dad0125
#define RES_About_Capacity_Template 0x0dad0b4e
#define RES_Capacity_View 0x0dad0b4f
#define RES_Capacity_Free_Segment 0x0dad0aef
#define RES_Capacity_Audio_Segment 0x0dad0af2
#define RES_Capacity_Video_Segment 0x0dad0af5
#define RES_Capacity_Photos_Segment 0x0dad0af8
#define RES_Capacity_Other_Segment 0x0dad0afb
#define RES_Capacity_Audio_Swatch_Bitmap 0x0dad0ae4
#define RES_Capacity_Video_Swatch_Bitmap 0x0dad0ae5
#define RES_Capacity_Photos_Swatch_Bitmap 0x0dad0ae6
#define RES_Capacity_Other_Swatch_Bitmap 0x0dad0ae7
#define RES_SettingsPreview_Capacity_Template 0x0dad07c6
#define RES_SettingsList_Template 0x0dad01b2
#define RES_SettingsList_View 0x0dad01b3
#define RES_CoverFlow_Backside_Border_Color 0x0dad05d8
#define RES_NowPlaying_White_Background_View 0x0dad08c5
#define RES_NowPlaying_Idle_Template 0x0dad0925
#define RES_NowPlaying_ChapterArt_BackgroundImage_View 0x0dad092d
#define RES_RentalInfo_White_Background_View 0x0dad09ae
#define RES_Search_Input_Text 0x0dad0ddc
#define RES_Notes_List_Screen_Alt 0x0dad09fa
#define RES_Notes_List_Screen_Alt_Default 0x0dad09fb
#define RES_Settings_Legal_Template 0x0dad0041
#define RES_StatusBarWhite_Template 0x0dad01a1
#define RES_StatusBarWhite_Text 0x0dad01a6
#define RES_StatusBar_Template 0x0dad020c
#define RES_StatusBar_Title_Text 0x0dad020e
#define RES_FullScreenImage_Template 0x0dad06b4
#define RES_About_Top_Template 0x0dad0b24
#define RES_About_Screen 0x0dad0b6c
#define RES_About_Accessory_Screen 0x0dad0b70
#define RES_Background_Template 0x0dad01d8
#define RES_Stopwatch_Template 0x0dad0d1d
#define RES_Stopwatch_Screen 0x0dad0d46
#define RES_Stopwatch_Screen_Stopped 0x0dad0d47
#define RES_Stopwatch_Menu_Screen 0x0dad0d49
#define RES_MainMenu_Items 0x0000001a
#define RES_MusicMenu_Items 0x0000001b
#define RES_MainMenus_Main_Screen 0x0dad076e
#define RES_MainMenus_Main_Layout 0x0dad0798
#define RES_MainMenus_Music_Screen 0x0dad07c8
#define RES_ContextMenu_Screen 0x0dad01c9
#define RES_ContextMenu_Layout 0x0dad01ca
#define RES_Calendar_Empty_Layout 0x0dad0397
#define RES_Calendar_Empty_Instances 0x00000026
#define RES_Calendar_Empty_Template 0x0dad0372
#define RES_Calendar_Empty_Text 0x0dad0374
#define RES_ContextMenu_Items 0x00000001
#define RES_ContextMenu_Item 0x0dad01cb
#define RES_ContextMenu_StartGenius_String 0x0dad0117
#define RES_ContextMenu_BrowseArtist_String 0x0dad0118
#define RES_ContextMenu_BrowseAlbum_String 0x0dad0119
#define RES_ContextMenu_BrowseCompilation_String 0x0dad011a
#define RES_ContextMenu_AddToOnTheGo_String 0x0dad011b
#define RES_ContextMenu_RemoveFromOnTheGo_String 0x0dad011c
#define RES_ContextMenu_Cancel_String 0x0dad011d
#define RES_ContextMenu_Title_String 0x0dad011e
#define RES_ExtrasMenu_Screen 0x0dad01cc
#define RES_ExtrasMenu_Layout 0x0dad01d6
#define RES_ExtrasMenu_Stopwatch_Layout 0x0dad01ed
#define RES_MainMenu_Music_Item 0x0dad076f
#define RES_MainMenu_CoverFlow_Item 0x0dad0770
#define RES_MainMenu_GeniusMixes_Item 0x0dad0771
#define RES_MainMenu_Playlists_Item 0x0dad0772
#define RES_MainMenu_Artists_Item 0x0dad0773
#define RES_MainMenu_Albums_Item 0x0dad0774
#define RES_MainMenu_Compilations_Item 0x0dad0775
#define RES_MainMenu_Songs_Item 0x0dad0776
#define RES_MainMenu_Radio_Item 0x0dad0792
#define RES_MainMenu_Genres_Item 0x0dad0777
#define RES_MainMenu_Composers_Item 0x0dad0778
#define RES_MainMenu_Audiobooks_Item 0x0dad0779
#define RES_MainMenu_Search_Item 0x0dad077a
#define RES_MainMenu_Photos_Item 0x0dad0781
#define RES_MainMenu_Podcasts_Item 0x0dad0784
#define RES_MainMenu_iTunesU_Item 0x0dad0785
#define RES_MainMenu_Videos_Item 0x0dad077b
#define RES_MainMenu_VideoPlaylists_Item 0x0dad077c
#define RES_MainMenu_Movies_Item 0x0dad077d
#define RES_MainMenu_TVShows_Item 0x0dad077e
#define RES_MainMenu_MusicVideos_Item 0x0dad077f
#define RES_MainMenu_Rentals_Item 0x0dad0780
#define RES_MainMenu_Extras_Item 0x0dad0786
#define RES_MainMenu_Clocks_Item 0x0dad0787
#define RES_MainMenu_Alarms_Item 0x0dad0788
#define RES_MainMenu_Calendars_Item 0x0dad0789
#define RES_MainMenu_Contacts_Item 0x0dad078a
#define RES_MainMenu_Games_Item 0x0dad078b
#define RES_MainMenu_Notes_Item 0x0dad078c
#define RES_MainMenu_ScreenLock_Item 0x0dad078d
#define RES_MainMenu_Stopwatch_Item 0x0dad078e
#define RES_MainMenu_ShuffleSongs_Item 0x0dad0794
#define RES_MainMenu_Sleep_Item 0x0dad0796
#define RES_MusicMenu_CoverFlow_Item 0x0dad07c9
#define RES_MusicMenu_GeniusMixes_Item 0x0dad07ca
#define RES_MusicMenu_Playlists_Item 0x0dad07cb
#define RES_MusicMenu_Artists_Item 0x0dad07cc
#define RES_MusicMenu_Albums_Item 0x0dad07cd
#define RES_MusicMenu_Compilations_Item 0x0dad07ce
#define RES_MusicMenu_Songs_Item 0x0dad07cf
#define RES_MusicMenu_Radio_Item 0x0dad07d0
#define RES_MusicMenu_Genres_Item 0x0dad07d1
#define RES_MusicMenu_Composers_Item 0x0dad07d2
#define RES_MusicMenu_Audiobooks_Item 0x0dad07d3
#define RES_MusicMenu_Search_Item 0x0dad07d4
#define RES_ExtrasMenu_Clocks_Item 0x0dad01cd
#define RES_ExtrasMenu_Calendars_Item 0x0dad01ce
#define RES_ExtrasMenu_Contacts_Item 0x0dad01cf
#define RES_ExtrasMenu_Alarms_Item 0x0dad01d0
#define RES_ExtrasMenu_Games_Item 0x0dad01d1
#define RES_ExtrasMenu_Notes_Item 0x0dad01d2
#define RES_ExtrasMenu_ScreenLock_Item 0x0dad01d3
#define RES_ExtrasMenu_Stopwatch_Item 0x0dad01d4
#define RES_MediaLists_PodcastEpisodes_Screen 0x0dad07f8
#define RES_MediaLists_Songs_Screen 0x0dad07e9
#define RES_MediaLists_NestedPlaylists_Screen 0x0dad0808
#define RES_MediaLists_Genius_Screen 0x0dad0844
#define RES_MediaLists_GeniusPlaylist_Screen 0x0dad0848
#define RES_MediaLists_Artists_Screen 0x0dad07f0
#define RES_MediaLists_Artists_Screen_Default 0x0dad07f1
#define RES_MediaLists_Albums_Screen 0x0dad07ed
#define RES_NowPlaying_Screen 0x0dad096c
#define RES_ExtrasMenu_Items 0x00000002
