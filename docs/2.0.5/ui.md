# UI: screens, menu rows and events

## Screens

A pushed screen is created by `ControllerRegistry_InvokeScreenFactory` from a controller class name, a screen id and a layout id. Screens seen in Settings:

| Screen | Controller | Screen id | Layout id |
|---|---|---|---|
| Settings list | `TSilverSettingsMenuListCntlr` | `RES_SettingsMenu_Main_Screen` | `RES_SettingsMenu_Main_Layout` |
| Settings > Main Menu | `TCSettings_MainMenu` | `RES_Settings_MainMenu_Screen` | `RES_Settings_MainMenu_Layout` |
| Settings > Legal | `TSilverCntlr` | `RES_Settings_Legal_Screen` | `RES_Settings_Legal_Layout` |

Each controller factory builds its screen definition with `UI_ConstructScreenDefinition`, which runs `UI_LoadScreenLayouts`: every block of the screen's `SLst` becomes a prepared layout in `ScreenDefinition_layout_cache`, keyed by layout id. Showing a layout goes through `ScreenController_switch_layout_method`, which calls `ScreenController_show_layout_method` (`ScreenController_ShowLayout` for the base class; each controller class has its own copy). That method looks the layout up with `ControllerLayoutCache_GetOrInsert` and aborts if the entry is NULL. The lookup inserts a NULL entry for an unknown id, so the layout must be listed in the screen's `SLst` when the controller is created. Resources come from the resource bank at that moment; there is no boot-time registry, so screens and layouts added with `ResourceBank_SetResource` later work as long as they are in place before the push.

## Menu rows

A list's rows are blocks in an `ITEM` resource (`RES_SettingsMenu_Items` for Settings). Row fields: `MenuRow_id`, `MenuRow_style`, `MenuRow_visible`, `MenuRow_title` (a `SORC` id or property source) and `MenuRow_title_from_model`. Styles seen in stock tables:
- `MenuRowStyle_Text`: text without an arrow.
- `MenuRowStyle_Submenu`: text with an arrow.
- `MenuRowStyle_InlineValue`: a title plus a second `SORC` binding with destination `SourceDestination_InlineValue` (Shuffle, Sound Check).
- `MenuRowStyle_ModelLayout`: the model picks a style for each expanded row (see below).

`MenuItems_RegisterAll` builds the menu registry once while the UI starts, before `UI_InitResourceProviders`. It maps each row id to its row data by loading every ROM `ITEM` table through `ResourceBlockList_Load`, so an `AppendBorrowedBlock` hook runs during this load too. `MenuItem_SetVisible` looks the id up there and writes `MenuRow_visible` in that row data. If the value changed, it calls `Model_notify_method` on `UI_GetResourceService()` twice: once with type `Property_ItemChanged` and once with `Notify_None`, both with the item id as the property. `MenuList_HandleNotification` rebuilds an open list (`MenuList_RebuildVisibleItems`) when a `Property_ItemChanged` names one of its items. A list that is not open reads `MenuRow_visible` the next time it is built. It only reaches rows that were registered: rows appended after the registry is built, or `ITEM` tables that exist only as overrides, are not found, and the call returns 0. A row id is not unique: 47 stock rows have `MenuRow_id` 0, most of them the only row of a one-row `ITEM` table. Such rows cannot be addressed by id.

`MenuList_RebuildVisibleItems` drops rows whose `MenuRow_visible` is 0 and subscribes to the Settings-style sources of rows with `MenuRow_title_from_model` set. The last such row is the list's dynamic row: `MenuList_QueryProperty` reports the row count as the visible rows minus one plus that row's provider's answer to `Property_Count` for its property. A model-backed last row therefore has to answer `Property_Count` with 1, or it disappears; answering N expands it into N rows.

Rows of an expanded dynamic row are read with `ResourceService_GetIndexedResource(service, type, property, index)`, where `index` runs from 0 to N-1. It calls the provider's indexed slot (`Settings_IndexedProperty_slot` for Settings) with `*out` cleared, then tries parent services, then falls back to the plain property. `MenuList_GetRowProperty` maps a list position to the `ITEM` row and the index within it. `Property_ItemId` returns `MenuRow_id`. For a `MenuRowStyle_ModelLayout` row it asks `Property_Layout` for each index, and the answer is that row's style; a row that gets no answer is drawn as `MenuRowStyle_Text`. The list also asks the plain slot for `Property_LayoutSet`: a pointer to a zero-terminated list of the styles it must build (`{3, 4, 0}` for the Main and Music Menu lists). `MenuRowStyle_Check` (flush left) and `MenuRowStyle_CheckIndented` ask `Property_Drawable`, `Property_Bitmap` and `Property_String` for each index. A non-NULL `Property_Bitmap` answer is a `BMap` resource pointer, drawn as the checkmark. Without one, a check row looks like a flush-left text row. `MenuRowStyle_Text` rows ask only `Property_String`. Each row style is an object that maps its cells to `SORC` destinations. The text style maps two cells to destinations 8 and 9, and the check styles add a bitmap cell for `SourceDestination_RowBitmap`. A check row asks for the drawable and bitmap only when its `SORC` has a binding with that destination, as `RES_Settings_MainMenu_Item` does (emulator). Which function reads the map has not been traced. `MenuRowStyle_TwoLine` draws the `SourceDestination_InlineValue` source as a grey second line under the title. Music > Songs (`TSilverMediaListCntlr_Songs`) has three layouts:
- `ITEM` 0x1e: `MenuRowStyle_Text`.
- `ITEM` 0x1f: `MenuRowStyle_TwoLine`, with the title at destination 1 and the artist at destination 0xa.
- `ITEM` 0x20: style 13.

In the emulator (20k library) the two-line Songs list asked only for the title, the inline value and a drawable/bitmap at destination 0x14. It never asked for its destination-8 binding. A Settings-backed two-line row has not been tried.

Settings > Main Menu is `TCSettings_MainMenu` on `RES_Settings_MainMenu_Screen`. The screen's `Resource_VSlt` binds `RES_SettingsList_View` to `RES_Settings_MainMenu_Items`, a single `MenuRowStyle_ModelLayout` row (`RES_Settings_MainMenu_Item`) whose source is `Service_Settings` property 0x8909 (0x890a and `RES_Settings_MusicMenu_Item` for Music). Nothing in the class names the property; it comes from the `ITEM` row. `SettingsModel_GetProperty` answers `Property_Count` for 0x8909 and 0x890a with fixed counts (34 and 13, including Reset) and `Property_LayoutSet` with the style list. `SettingsModel_GetIndexedProperty` answers from `MainMenuSettingsRows` and `MusicMenuSettingsRows` (`MenuSettingsRow_*`):
- `Property_Layout`: `MenuSettingsRow_layout_kind`.
- `Property_String`: the title.
- `Property_Bitmap`:
  - `RES_Menu_Checked_Bitmap` for a visible row, `RES_Menu_CheckedSelected_Bitmap` when that row is the selection model's current row, and nothing for a hidden row.
  - The Reset row (index 33 and 12) shows its own icon when selected.

Its index bounds and the Reset index are fixed. `ResourceService_GetBitmap(UI_GetResourceService(), id)` returns such a `BMap` resource pointer.

The `chosen` binding of `RES_Settings_MainMenu_Item` runs `HandleMainMenu`, which `SettingsMainMenu_HandleAction` passes to `MainMenuSettings_HandleSelection`. That reads the index from `SelectionModel_current_index_method`; For index 33, it calls `ScreenController_emit_event_method` (`UI_EmitControllerEvent`) with `GotoScreen_ConfirmCancelResetMenu`. That posts `controller.GotoScreen_ConfirmCancelResetMenu`, which the layout's `SEVT` turns into a `navigator.PushScreen` of the Reset confirmation. Any other index goes to `SettingsModel_ToggleMainMenuItem` through `MainMenu_Toggle_call`, with no bounds check. `SettingsMainMenu_HandleAction` sits in `SettingsMainMenu_Action_slot` and `SettingsMainMenu_SecondaryAction_slot`, and Music's `SettingsMusicMenu_HandleAction` (`HandleMusicMenu`, Reset at index 12) in the two `SettingsMusicMenu_*Action_slot`s. Both return 1 for their action, including the `ActionSupportQuery` probe, which they answer without acting, and 0 for anything else. Only `SettingsModel_GetIndexedProperty`, `SettingsModel_GetProperty` and these handlers treat the list position as a row index. The toggle screens bind no `selected` or `delayedselected` events. The checkmark highlight compares the list position with the selection model's current row. The toggle flips `MenuSettingsRow_visible` and continues only if the list view is on screen. It then calls `MenuItem_SetVisible` for the row's `RES_MainMenu_*_Item`, notifies `Notify_All` for `RES_Settings_MainMenu_Item` (the Music toggle notifies `RES_Settings_MusicMenu_Item` and redraws the list), mirrors one special row into a second table, and stores the visibility bit in the preference record. When preferences load, `SettingsModel_LoadPreferences` calls `SettingsModel_ApplyMainMenuPreferences` and `SettingsModel_ApplyMusicMenuPreferences` (site `MusicMenu_Apply_call`). They rebuild the row tables, read each stock row's preference bit and call `MenuItem_SetVisible`, for 33 and 12 rows; they never touch indexes beyond those.

Settings > EQ is `TCSettings_EQ` on `RES_Settings_EQ_Screen`. Its `VSlt` binds the same list view to `RES_Settings_EQ_Items`, one `MenuRowStyle_Text` row (`RES_Settings_EQ_Item`) whose source is `Service_Settings` 0x890f (23 presets). The `CEVT` binds that row's `chosen` event to `HandleEQ` and `selected` to `ShowSetting_EQ`, once for all expanded rows. `SettingsEQ_HandleAction` dispatches both:
- `SettingsEQ_ApplySelection` reads the current index, calls `SettingsModel_SelectEQMenuIndex` and pops the screen with `ScreenController_pop_self_method` (`ScreenController_PopSelf(controller, 0x10)`, which calls `Navigator_PopTop`).
- `SettingsEQ_ShowPreview` passes the index to `SettingsModel_SetPreviewIndex`, which stores it and notifies `Notify_All` for preview property 0x891a.

The current preset is highlighted from the controller's event slot. When `UI_TransitionToLayout` has shown the layout, the controller receives an `Event_Notification` whose `Notification_code` (at `Event_notification`) is `Notification_LayoutShown`; `UI_DisplayPreparedLayout` sends `Notification_LayoutChanged`. `SettingsEQ_HandleEvent` then calls `SettingsEQ_SelectCurrentRow`, unless the controller's deferred flag is set. That function finds the list with `SelectionModel_find_view_method(RES_SettingsList_View)`, takes its `Interface_List` with `Object_GetInterface`, and calls `ListInterface_select_method(list, index)` with the current preset. Backlight does the same (`SettingsBacklight_SelectCurrentRow`). `ListInterface_select_method` is `MenuList_SelectRow`, which sets the row, redraws and restarts the timer that sends `delayedselected`. The layout display paths also call `MenuList_RestoreState`, which re-applies the list's saved row: the row whose `prID` property matches the saved one, or the saved index. A select made at the wrong moment is therefore replaced, with row 0 on a first visit. A clone of the Settings list controller should select right after the stock `ScreenController_activate_method` returns, not on `Notification_LayoutShown`; this was confirmed in the emulator. Right after the push the layout is not shown yet and the list view may not exist.

### Row style catalogue

The row-style table holds one object per style, 2 to 27 and 31. Every style draws the title (destination 1); the object maps the style's other cells to `SORC` destinations. The table below comes from rendering each style in Settings > Main Menu in the emulator. A row with `MenuRowStyle_ModelLayout` was answered with each style in turn, a checked row tested the checkmark, the selected row tested the chevron, and the second binding was repointed to destination 10 to test the second line. A combination counts as supported only if it rendered like a stock row.

| Style | Height | Second line (destination 10) | Checkmark (destination 12) | Chevron when selected | Stock example |
|---|---|---|---|---|---|
| `MenuRowStyle_Text` (2) | one line | no | no | no | Settings > EQ |
| `MenuRowStyle_Check` (3) | one line | no | yes, right | no | Main Menu rows |
| `MenuRowStyle_CheckIndented` (4) | one line, indented | no | yes, right | no | Main Menu rows |
| `MenuRowStyle_Submenu` (5) | one line | no | no | yes | Extras, Artists |
| `MenuRowStyle_TwoLine` (6) | two lines | not drawn from a Settings source | no | no | Songs |
| `MenuRowStyle_TwoLineSubmenu` (7) | two lines | yes | no | yes | Genres |
| 8, 10 | two lines | yes | no | no | Podcast episodes (10) |
| 9, 11 | two lines | yes | no | yes | none |
| `MenuRowStyle_InlineValue` (12) | one line | value on the right | no | no | Settings > Shuffle |
| `MenuRowStyle_Artwork` (13) | tall | yes | broken: drawn in the empty artwork slot | no | Songs (artwork layout), Movies |
| `MenuRowStyle_ArtworkSubmenu` (14) | tall | yes | broken: drawn in the empty artwork slot | yes | Albums, TV Shows, Games |
| 15, 16 | tall, indented | no; the left area stays empty | no | 16 only | none |
| 17 | tall | yes | no | no | none |
| 18 | tall | yes | no | yes | none |
| 19 | one line | no | no | a fixed refresh icon on every row | Genius |
| 20 | one line | no | no | yes | Genius |
| 21, 23 | tall | yes | broken: drawn in the empty left slot | no | none |
| 22, 24 | tall | yes | broken: drawn in the empty left slot | yes | Podcasts (24) |
| 25, 26, 27, 31 | — | — | — | — | crash the firmware when built this way |

Supported:
- One line: 2, 5 (chevron), 12 (value on the right), 20 (chevron).
- Two lines: 7 (chevron), 8, 9 (chevron), 10, 11 (chevron).
- Tall two lines: 13, 14 (chevron), 17, 18 (chevron), 21 to 24 (22 and 24 chevron).

The chevron is not a `SORC` cell. When the row is drawn, its views ask the list model, through `ResourceService_GetIndexedResource`, for `Property_Bitmap` with property `RES_Menu_Chevron_Bitmap` (and `Property_Visible` for the style's own record), and `MenuList_GetRowProperty` passes the query on to the row's provider. Stock providers do not answer it, so the lookup falls back to the resource bank and finds the 10 × 15 chevron bitmap (seen in the emulator). A provider that claims such a query without answering hides the chevron. Single selection is the list highlight on the current row (see the EQ list), so it works with any style. A checkmark (multiple selection) renders correctly only in 3 and 4, and neither has a second line or a chevron; the tall styles draw destination 12 in their artwork slot instead. Style 6 drew its second line from the music library in Songs but not from a Settings source; why has not been traced. Destinations 8, 9, 13 to 20 and 32 were not given sources here; Artists and Songs carry destination-8 bindings that are never asked for.

## Settings preview pane

Every row of `RES_SettingsMenu_Main_Screen` has its own layout. The row's `delayedselected` binding runs a `ShowSetting_*` handler and the action `navigator.SwitchLayout` with that row's layout name, so the pane follows the highlighted row through layout switches. Each layout has four instances:
- the status bar (`RES_StatusBarWhite_Template`);
- the background (`RES_FullScreenImage_Template`);
- the pane, `RES_SettingsPreview_Template` for most rows;
- the list (`RES_SettingsList_Template`).

The pane has a caption (`RES_SettingsPreview_Caption_Text`), the dark rounded box behind the icon (`RES_SettingsPreview_Background_View`, bound in the template to `RES_SettingsPreview_Background_Image`), an icon (`RES_Settings_Icon_View`) and a detail line (`RES_Settings_InfoDetail_View`). The pane instance's `Resource_VLyt` (`SsinInstance_substitute`) binds each of them for that row. Each row's `VLyt` only shows or hides the box with `ViewOption_Visible`; Legal hides it. About's caption is a `Service_String` string, its icon is `Service_Settings` property 0x891d and its detail line is property 0x8905. A clone binds its own caption and bitmap by giving its pane instance a `VLyt` with its own sources.

A `VLyt`, `VSlt` or `VCvs` block's kind is a view id, and its data is a block list of options:
- `ViewOption_Visible`: a byte, 1 to show and 0 to hide.
- `ViewOption_TopEdge`, `ViewOption_LeftEdge`, `ViewOption_BottomEdge` and `ViewOption_RightEdge`: one edge constraint each, in the descriptor frame's triple format.
- `Resource_SORC`: a source id.

The 30 stock `VSlt` resources hold only `ViewBinding_ItemTable` blocks: `{view id, ITEM id}`. Template views keep whatever was last applied, so a view a `VLyt` leaves out keeps the previous screen's caption or icon. A clone should bind every pane view.

An image view (`BitmapView_LoadBitmap`) asks its source for `Property_Bitmap`, or `Property_Drawable` when that is its type. A `Property_Bitmap` answer is a pointer to a `BMap` resource (`BMap_format`, `BMap_stride` in bytes, `BMap_depth`, `BMap_bounds` as `{top, left, bottom, right}`, `BMap_payload_bytes`, `BMap_payload`). Formats seen in stock bitmaps:
- `BMapFormat_BGRA8888`: depth 32, stride = width × 4, bytes B, G, R, A per pixel (little-endian `0xAARRGGBB`), not premultiplied.
- `BMapFormat_Indexed8`: depth 8; the payload is a 32-bit palette count, that many 4-byte entries in the same B, G, R, A order, then one byte per pixel at the stride.
- `BMapFormat_RGB565`: depth 16.
- `BMapFormat_Indexed16`: depth 16; the payload is a palette as for `BMapFormat_Indexed8`, then 16-bit indices at the stride (`RES_SettingsPreview_Background_Image`).
- `BMapFormat_Mask8` and `BMapFormat_Mask4`: depth 8 or 4, no palette; each pixel is a coverage value. `BitmapView_Draw` draws formats 2, 4 and 8 in the view's tint colour with `GraphicsDrawMode_Text`. The Settings pane icons (`RES_Settings_Brightness_Image` and the other `Settings_*_Image` resources) are `BMapFormat_Mask8`, 78 × 121 at a stride of 80.

A format of 0 picks one from the depth. The byte order comes from stock palettes: the selected checkmark's edge pixels blend white into the blue selection bar only when read as B, G, R. 

In `RES_SettingsMenu_Main_Screen`'s `CEVT`, About and the rows with values (Shuffle, Repeat, Sound Check, Clicker, Sort By) handle `chosen` in place with `ShowSetting_*` or `ToggleSetting_*`. Volume Limit (`GotoVolumeLimit_or_Lock_or_EU_Screen`) and Backlight (`HandleSetting_BacklightTimer`) also use handlers. Main Menu, Music Menu, Brightness, Audiobooks, Radio Regions, EQ, Date & Time, Language, Legal and Reset Settings push a screen with `navigator.PushScreen`. These handlers in `Settings_HandleAction` work on the controller's Settings model and the stored preferences, not on the screen id, the `ITEM` table or the selection index. Two rows change visibility with state through `MenuItem_SetVisible`: `RES_SettingsMenu_VolumeLimit_Item` and `RES_SettingsMenu_RadioRegions_Item`. A stock Settings layout keeps its own status bar `VCvs`, so switching to it shows the title "Settings". Its list instance is bound through the `VSlt` of whichever screen shows it.

Dropping a stock row while a list loads and binding its id in another screen's `CEVT` is safe for the bindings themselves: events are named by row id, and preview layouts are chosen by name, not by position. The handler strings (`ToggleSetting_*`, `ShowSetting_*`) are understood only by the Settings list controller's action slot, so another screen needs that controller class or its own handler. `MenuItem_SetVisible` still writes the stock row data, which the other list shares.

### Switching layouts

`navigator.SwitchLayout(name)` looks the name up with `ResourceName_GetId` and calls `ScreenController_switch_layout_method(controller, layout_id, 0, 1)`; `navigator.PushLayout` passes 1 as the third argument. Calling the slot directly with a layout id does the same thing. The layout must be in the screen's `SLst`, or `ScreenController_ShowLayout` aborts. To make a name resolve, write the id into the slot that `ResourceNames_FindOrInsertSlot(ResourceNameMap, &name)` returns for a `CppString` key. The map keeps its own copy of the key, so the string can be destroyed afterwards. The Settings list controller is built anew on every push, and its screen definition reads the `SLst` each time, so an `SLst` override only has to be in place before the push. The `ShowSetting_*` handlers only notify the Settings model (Shuffle: `Notify_All` for the pane properties 0x891d and 0x8905) so that views bound to those properties re-read them while the row is highlighted. A layout switch applies the new pane `VLyt` anyway, so a row without a handler leaves nothing stale as long as its `VLyt` binds every pane view.

## Main, Music and Extras menus

The main menu is `RES_MainMenu_Items` on `RES_MainMenus_Main_Screen`, the Music menu `RES_MusicMenu_Items` on `RES_MainMenus_Music_Screen`; their rows are the `RES_MainMenu_*_Item` and `RES_MusicMenu_*_Item` ids, in the order of `MainMenuSettingsRows` and `MusicMenuSettingsRows`. Extras is controller `TCExtrasMenu` on `RES_ExtrasMenu_Screen`; its rows (`RES_ExtrasMenu_*_Item`) are bound in the screen's `CEVT`, and only `HandleLock` and `HandleAddressBook` reach `ExtrasMenu_DispatchAction` (site `Extras_Action_slot`). Its list is bound to `RES_ExtrasMenu_Items` through a `ViewBinding_ItemTable` block in the screen's `Resource_VSlt` resource.

Each `*_Action_slot` is the `ScreenController_action_method` entry of a controller vtable. Main, Music and Now Playing each have two such vtables, so a hook binds both the plain and the `Secondary` slot. Which class each screen creates has not been traced.

Sites `MainMenu_Index_call` and `MusicMenu_Index_call` are `BLX r1` through the selection model, not direct calls; a replacement must call the original through r1.

## Resource blocks

UI resources are a 32-bit count followed by blocks of `{bytes, kind, data}`, each padded to 4 bytes. `ResourceBlockList_Load` passes the kind word to `ResourceBlockList_AppendBorrowedBlock` as its metadata.

`ResourceBank_SetResource` copies the data into the heap; replacing an override frees the previous copy, so pointers into it become invalid.

## Resource lifetime

### Override storage

`UITask_Main` builds one ROM-backed bank per language plus the UI bank (`ResourceBank_kind` = `ResourceBankKind_Rom`), installs the UI bank with `UI_SetResourceBank`, and then runs `MenuItems_RegisterAll`. `UI_SetResourceBank` destroys the bank it replaces, but nothing else calls it, so the UI bank and its overrides last until reset. `ResourceBank_GetResource` looks in `ResourceBank_overrides` first and then in the ROM index at `ResourceBank_rom_index`.

`ResourceBank_overrides` maps a type to a per-type map, which maps an id to an entry of `ResourceOverride_bytes`: `ResourceOverride_length` and `ResourceOverride_data`. Both maps are the firmware's sorted array map: 8-byte {key, value} slots searched by bisection. The slot array grows by 4 slots when full and shrinks by 4 when 4 are free. A heap block is the request rounded up to 4, plus an 8-byte header, rounded up to 8, and at least 16 bytes. One override of n bytes costs:
- the copy: max(16, round8(round4(n) + 8)) bytes;
- the entry: 16 bytes;
- an 8-byte slot in the per-type map.

The first override of a type also allocates the per-type map (0x28 bytes, a 48-byte block), its slot array, and a slot in the outer map.

### Removing an override

No function removes an override from a ROM-backed bank. A heap-only bank (`ResourceBankKind_Heap`) keeps the same kind of map at `ResourceBank_rom_index`, and `ResourceBank_RemoveHeapResource` removes from it. It finds the entry with `ResourceOverrides_Find`, removes the slot with `ResourceOverrides_EraseEntry` and frees the copy, but not the entry. `ResourceOverrides_EraseEntry(map, type, entry)` removes the slot whose value is `entry`. When the per-type map becomes empty, it also removes that map from the outer map and frees it. It returns 1 if a slot was removed. Both helpers take the map itself, so the same steps work on `ResourceBank_overrides`: find the entry, erase it, then `Heap_Free` the copy (`ResourceOverride_data`) and the entry. `Heap_Free` ends in the same free routine that `ResourceBank_SetResource` uses. This sequence has not been run in the emulator. Afterwards `ResourceBank_GetResource` returns the ROM resource again for a stock id, and NULL for an id that exists only as an override.

`ResourceBank_Destroy` on a ROM-backed bank frees the ROM index and the outer map object. It frees no per-type maps, entries or copies.

`ResourceBank_SetResource` with 0 bytes stores a 0-length override. Its copy is still a 16-byte block, the same as for 4 bytes. `ResourceBank_GetResource` then returns a non-NULL pointer and a length of 0. `ResourceBlockList_Load` reads a length of 0, or a 4-byte count of 0, as a list with no blocks. `UI_LoadLayoutInstances` aborts on an `SLyt` with no blocks. `UI_ConstructScreenDefinition` checks only that the `SCST` pointer is non-NULL. How `UIEventMap_ReadStream` handles an empty event table has not been traced.

### Pointers into resources

| Holder | Points into | Released |
|---|---|---|
| Prepared layout in `ScreenDefinition_layout_cache` | the `SLst` block (`Layout_screen_list_record`) and every `SSin` block of its instances (`Layout_instances`) | when the screen definition is destroyed, which deletes every prepared layout |
| `Layout_events` | nothing; `TEVT`, `SEVT` and `CEVT` are copied into a string and parsed | not applicable |
| A `ResourceBlockList` | every block it loaded, including blocks lent through `ResourceBlockList_AppendBorrowedBlock` | when the list is destroyed; it frees block data only if it owns it |
| A list view's `MenuList_item_blocks` | its `ITEM` blocks and any rows lent into them | when another table id is bound, or the list is destroyed |
| The menu registry | the row data of every row loaded while `MenuItems_RegisterAll` ran | never |
| The context-menu model (`ContextMenuModel_Get`) | the action strings | when the next context menu is shown |
| Views bound by `View_ApplyPropertyBindings` | `Ui32` (destination 0x15) and `BMap` (destinations 0x1a to 0x1e) resources fetched from the bound service | when the view is bound again |

`UI_ApplyInstanceResources` reads `VLyt`, `VSlt` and `VCvs` into temporary lists each time a layout is displayed. `UI_CreateTemplateFromResource` copies the `TMLT` block, and `View_Construct` copies the descriptor's options. Which type-specific view factories keep a descriptor pointer has not been checked.

The layout manager (`UI_GetLayoutManager`) keeps the ids of the last displayed screen and layout in `LayoutManager_screen` and `LayoutManager_layout`. When the next layout is displayed, `UI_DisplayPreparedLayout` and `ScreenController_ShowLayout` rebuild a temporary layout from `LayoutManager_layout`, which reads its `SLyt` and `SSin` by id again. A second, unnamed path applies the `VLyt`, `VSlt` and `VCvs` of that layout and screen again. If that `SLyt` is missing or empty, `UI_LoadLayoutInstances` aborts. A screen's `SLst`, `SLyt` and `SSin` overrides can therefore be replaced or removed only after its controllers are destroyed and another layout has been displayed.

### Lent menu rows

The menu registry maps a row id to its row data. `MenuItems_RegisterAll` fills it once, from `UITask_Main`, and `MenuItems_RegisterResourceType` is its only writer. Rows lent while it loads the ROM `ITEM` tables are registered by pointer, and no entry is ever removed. `MenuItem_SetVisible` writes `MenuRow_visible` through that pointer.

List views come from templates built once at UI start. A list view's `MenuList` loads its default table when the template is built. After that, `MenuList_SetItemTable` (called for a `ViewBinding_ItemTable` block in `UI_ApplyInstanceResources`) builds a new block list only if the bound table id differs from the one it holds. Otherwise it keeps the old list. Opening Main, Music, Extras or Settings reloads the table, and runs an `AppendBorrowedBlock` hook again, only if that list view last showed a different table. Which screens share a list template has not been traced. Closing a screen releases nothing.

A freed lent row can still be read through:
- the menu registry, if the row was lent while `MenuItems_RegisterAll` ran: `MenuItem_SetVisible` for its id writes into it;
- every list view whose `MenuList_item_blocks` still holds the table: `MenuList_RebuildVisibleItems`, `MenuList_GetRowProperty` and `MenuList_QueryProperty` read it. `MenuList_HandleNotification` rebuilds such a list on a `Property_ItemChanged` for one of its items without checking whether it is on screen.

Replacing or removing an `ITEM` override has the same effect: a list view that holds that table id keeps pointers into the freed copy, and binding the same id again does not reload it.

### Context menu rows

`ContextMenuItems_Append` stores the action pointer without copying the string. The stock builders keep the item vector on the stack. `UI_ShowContextMenuItems` copies the 12-byte items, pointers included, into `ContextMenuModel_items` of the context-menu model, a singleton that is never destroyed. `ContextMenuController_DispatchAction` reads the action string from there when a row is chosen. The model keeps the pointers until the next `UI_ShowContextMenuItems` replaces the vector.

### Reusing ids

State keyed by id that outlives an override:
- `ResourceNameMap`: a name keeps its id. Only `ResourceName_GetId` and the code that fills the map at start use it, and nothing removes a name.
- The menu registry: entries keyed by row id, never removed.
- List views: `MenuList_SetItemTable` compares only the table id.
- Templates: one per `View` id, built once at UI start.
- `LayoutManager_screen` and `LayoutManager_layout`: the last displayed screen and layout.

Layout event tables belong to the prepared layout and go away with its screen definition. State keyed by id in the Settings model has not been examined.

## Screen resources

| Type | Keyed by | Content |
|---|---|---|
| `Resource_SCST` | screen id | controller class name, NUL-terminated |
| `Resource_SLst` | screen id | blocks; word 0 of each is a layout id (the first is the default layout) |
| `Resource_SLyt` | layout id | blocks; word 0 of each is an `SSin` id |
| `Resource_SSin` | instance list id | blocks: `SsinInstance_view_bindings` (a `VCvs` id), `SsinInstance_template`, `SsinInstance_substitute` (the id of the instance's `VLyt` and `TEVT`), `SsinInstance_flags` |
| `Resource_VCvs` | instance id | blocks whose kind is a view id, each holding option blocks; a `Resource_SORC` option is a 32-bit source id. Kind `ViewBinding_ItemTable` binds a list to an `ITEM` table at `ItemTableBinding_item_table` |
| `Resource_VCrv` | instance id (the `VCvs` id) | one block of four edge constraints (`ViewDescriptor` frame layout) that `UI_ApplyInstanceResources` applies to the template's container in the `VCvs` pass; a block of another size aborts. Without it the template keeps its own frame |
| `Resource_View` | template id | view descriptors, one block per view, kind = view type |
| `Resource_TMLT` | template id | template parameters, copied unchanged when a template is cloned |
| `Resource_VLyt` | view layout id | `ViewLayout_title` is the title source |
| `Resource_SORC` | source id | one `SORC` block of `SourceBinding_service`, `SourceBinding_property` and `SourceBinding_destination` |

A source for a literal string is a `Resource_String` resource (UTF-8, NUL-terminated) bound with service `Service_String`, property = string id, destination `SourceDestination_Title`. Settings model rows use service `Service_Settings` with the property id.

A view descriptor starts with a 12-word frame, then `ViewDescriptor_id` and `ViewDescriptor_type`. A text view's own fields start at `ViewDescriptor_options_bytes`. For a text view (`TextView_CreateFromDescriptor`) the descriptor holds a `SORC` id at `TextViewDescriptor_source`, a `FONT` id at `TextViewDescriptor_font` and a `COLR` id at `TextViewDescriptor_color`. The view binds the source entry whose destination is 1 to the service it names, whatever that service is. This binding happens once, when the template is built, and afterwards the view changes only when its model notifies it or when a `VCvs` binding is applied again. The frame holds four edge constraints of three words each: an offset, a reference view id and a mode byte. They are, in order, the left, top, bottom and right edges (`View_InitOptionsFromDescriptor`, resolved by `View_ResolveBounds`). The top edge's words are `ViewDescriptor_top_offset`, `ViewDescriptor_anchor` and `ViewDescriptor_anchor_mode`. A mode from `AnchorMode_Top` to `AnchorMode_Right` places the edge at that edge of the reference view plus the offset, so a Legal paragraph with `AnchorMode_Bottom` sits below the previous paragraph. Modes 0 and 5 to 10 select fixed constraints from a table; their meanings have not been mapped. The `VCvs` pass of `UI_ApplyInstanceResources` reads bits 16 to 23 of `SsinInstance_flags`. If they are nonzero, as on the stock Legal page, the template's bottom edge follows its content, so the page can be taller than the screen. If they are zero, as for the status bar, the bottom edge follows the parent.

The names in navigator actions resolve through `ResourceName_GetId`, which looks the C++ string up in the global name map (`ResourceNames_FindOrInsertSlot`); an unknown name maps to 0.

## Events

Event tables are resources of type `Resource_TEVT` (template), `Resource_SEVT` (layout) and `Resource_CEVT` (screen controller): a count followed by events. An event is the name, the byte `'1'`, the handler, a 32-bit has-action flag and, if set, the action and its arguments. Strings are a 32-bit length followed by UTF-8. Words in the stream are not aligned, so a writer stores them bytewise.

The stock Settings rows bind `list.pid.<row id>.chosen` and `delayedselected` in the `CEVT` of `RES_SettingsMenu_Main_Screen`; the layout's `SEVT` carries only named controller events. For a binding without an action, the screen controller's action slot receives the handler string. It is first asked with `ActionSupportQuery`, where a nonzero return means supported, and then called with an object pointer as the argument.

`UI_LoadLayoutEventTables` reads a `Resource_TEVT` for every instance, keyed by `SsinInstance_substitute`, then the layout's `SEVT`, then the controller's `CEVT`. A missing `TEVT`, `SEVT` or `CEVT` calls `Runtime_Abort`, so a cloned instance with a new substitute id needs a `TEVT` stored under it; a stock one can be copied. `UI_GetLayoutEventTable` loads a layout's tables once through site `UI_LoadLayoutEvents_call`. A hook there can call the original and then read more event streams into `Layout_events` with `UIEventMap_ReadStream`, which is how bindings are added at run time. The stream is a `StringInputStream_bytes` object, initialised with `StringInputStream_Init` from a `CppString_bytes` string in `StringInputStream_ResourceMode` and destroyed with `StringInputStream_Destroy(stream, StringInputStream_DestroyComplete, NULL)`, which does not free the storage. This is how `UI_LoadLayoutEventTables` reads its own tables.

## Views

`UI_RegisterViewFactory` maps a view type (the descriptor's `ViewDescriptor_type`) to `create(owner, parent, event_target, descriptor)`. A custom view fills `View_options_bytes` of options with `View_InitOptionsFromDescriptor`, allocates `View_bytes`, calls `View_Construct` and installs its own vtable: a copy of `View_vtable` (`View_vtable_bytes`, the last four slots null) with `View_draw_method` replaced. `View_destroy_method` is the deleting destructor. Stock factories allocate with `Runtime_OperatorNew` and return the new view. The factories live in a map keyed by view type with no fixed size. The first registration for a type wins, so a built-in type cannot be replaced. `UITask_Main` registers the built-in types (`UI_RegisterBuiltinViewFactories`) before `UI_InitResourceProviders`. A type used by a template must be registered before `UI_InitializeViewTemplates` builds that template.

### Drawing

`View_DrawClipped` calls `View_draw_method` as `draw(view, draw_argument, clip)`, where `clip` is the part of the view to redraw. The base method, `View_Draw`, draws the view's shape layer and then `View_DrawBackgroundAndBorder(view)`. No graphics context is passed in. A draw method builds its own: it initialises `GraphicsContext_bytes` with `GraphicsContext_ConstructDefault`, then `View_BindGraphicsContext` attaches the window bitmap, puts the origin at the view's top-left corner and clips to the visible part. `View_GetLocalContentRect` then gives the drawable area in those local coordinates. Rects are `{top, left, bottom, right}`. `View_bounds` is in screen coordinates, the same space for every view, not relative to the parent; the window's origin is subtracted only when drawing. The context needs no release: its destructor does nothing, and stock draw code lets it go out of scope. A replacement draw method can call `View_Draw` first to keep the stock background.

Every text goes through `TextView_DrawString`, which draws one run with the view's graphics context (`TextView_context`) and its UTF-8 text (`TextView_text` for plain text views). It calls `TextView_DrawTextWithColor` (site `TextView_DrawTextWithColor_call`) or, for wrapped text, `TextView_DrawTextLayout` (site `TextView_DrawTextLayout_call`). Both callees take their later arguments on the stack, so hook site `TextView_DrawString_entry` rather than the calls. The context carries `GraphicsContext_color` and the clip `GraphicsContext_clip_top`, `GraphicsContext_clip_left`, `GraphicsContext_clip_bottom` and `GraphicsContext_clip_right` in screen coordinates. A view's children are at `View_children` (`View_child_count`, `View_child_items`). `Navigator_Get` returns the navigator, whose top controller is at `Navigator_top_controller`.

Text uses `FONT` resources of three words (`FontResource_family`, an `FSTR` id naming the family, `FontResource_size` in the firmware's own units, and `FontResource_style`, 1 for bold). `Font_Construct(font, family, size, style)` turns them into a two-word handle that `GraphicsContext_SetFont` copies to `GraphicsContext_font`. Stock text uses Helvetica: the status-bar titles and the preview detail line use `RES_Font_BoldSmall`, the preview caption and the Brightness value `RES_Font_Bold`, and the Legal paragraphs `RES_Font_RegularSmall`. A live text view's handle sits in its context, at `TextView_context` plus `GraphicsContext_font`. Which font list rows use has not been traced.

To draw a string, set a font with `Font_Construct` (the family is a C string such as "Helvetica"; an unknown family gives a zero handle) and `GraphicsContext_SetFont`, set the colour, call `GraphicsContext_SetDrawMode(context, GraphicsDrawMode_Text)` as `TextView_DrawTextWithColor` does, then `GraphicsContext_SetTextPen(context, x, baseline)` and `GraphicsContext_DrawUtf8(context, text)`. The pen y is the baseline, because `GraphicsContext_DrawUtf8` subtracts the font's ascent itself. A fresh context's mode draws no text; drawing with `GraphicsDrawMode_Text` was confirmed in the emulator.

`GraphicsContext_CopyRect(context, source, source_rect, rect, alpha, flags)` copies from a context that has a bitmap attached into `rect`, offset by both contexts' origins (`GraphicsContext_origin_x`, `GraphicsContext_origin_y`) and clipped to the destination's clip (`GraphicsContext_clip_top` to `GraphicsContext_clip_right`). To draw a `BMap`, build the source with `GraphicsContext_InitFromBitmapResource(context, bitmap, resource)`, where `bitmap` is `Bitmap_bytes` of storage set up by `Bitmap_Construct`. `BitmapView_Draw` draws with alpha 0xff and flags 0. Both can live on the stack. Afterwards call `Bitmap_Destroy` (the non-deleting destructor: it frees pixels only when `Bitmap_owns_pixels` is set, which a resource-backed bitmap never has, and tears down an embedded member). `GraphicsContext_Destroy` does nothing.

`View_InvalidateRect` draws nothing itself. It takes a rect in the same screen coordinates as `View_bounds`, clips it to the view and its parent and queues a window invalidation, and only when the view is visible and drawing is enabled. It keeps no lock, so call it from the UI task, for example from an action handler or a UI timer. A full redraw is `View_InvalidateRect(UI_GetRootView(), root + View_bounds)`.

A view bound to a model (`View_model`, `View_property`) is refreshed through `View_refresh_method` when `View_HandleModelNotification` receives an `Event_ModelChange` for its property, with either the view's own type or `Notify_All`. Descriptor words +0x34 and +0x38 bind a second service and property of type `Property_Visible`, which shows or hides the view. A view whose service is not registered when the template is built stays unbound until a `VCvs` binding sets a source. The Settings service is registered in time: no resource rebinds `RES_Settings_Brightness_Bar`, yet it follows the Settings model. This is inferred, not traced.

### Templates

`UI_InitializeViewTemplates` runs once per template owner while the UI starts. It enumerates every `Resource_View` with `ResourceBank_FirstResource` and `ResourceBank_NextResource`, and `UI_CreateTemplateFromResource` builds a live view tree for each one from its `Resource_TMLT`. Nothing is built later. On a ROM-backed bank the enumeration walks only the ROM index and never the overrides, so a View that exists only as an override gets no template. To add one, hook site `ResourceBank_NextTemplate_call`: call the original, and when it returns NULL, yield the extra View resources one at a time. The hook is called again with the last id it returned. The template then builds its child views from the View resource it loads again by id through the resource provider, so each yielded View must also be stored with `ResourceBank_SetResource`; a View that is only yielded produces a template with no children.

`UI_DisplayPreparedLayout` finds each `SSin` instance's template with `UI_ResolveTemplateView`, which tries `UI_FindTemplateView` on the manager's template owner and then a search of the root view tree. If the new layout's instance has no template, it is skipped without an error. When it leaves a layout, it rebuilds the old layout from its resources by id. For every old instance that the new layout does not share, it calls the template view's slot 0x130 without checking for NULL. A layout whose template can no longer be resolved at that moment therefore crashes with a data abort reading address 0, for example when the idle timeout switches to the Charging screen. An `SSin` instance with template 0 is skipped on display but crashes when the layout is left, so a clone must remove an unwanted instance, never zero its template. Otherwise `UI_ApplyInstanceResources` applies the instance's `Resource_VLyt`, then the screen's `Resource_VSlt` (keyed by `ScreenDefinition_screen`), then the instance's `Resource_VCvs`. Each block looks up a view in the template tree with `View_FindViewById` (`View_find_view_method`; the id is in `View_id`) and passes its options to `View_ApplyPropertyBindings`. Apart from an item-table block, nothing checks the result of that lookup. A block whose kind is not the id of a view in the template therefore crashes on a NULL pointer. All screens that use a template share the same views, so bindings set for one screen stay in place until another screen rebinds them.

The stock Legal page binds only its title this way. Its paragraph text comes from the template's view descriptors.

Settings > Brightness is the slider prototype. Its controller class is `TCSettings_Brightness`, and its template has five image views, a `ViewType_Progress` bar (`RES_Settings_Brightness_Bar`) and a `ViewType_Text` value (`RES_Settings_Brightness_Value_Text`). A progress descriptor holds `ProgressDescriptor_maximum` (100 for Brightness), `ProgressDescriptor_minimum` (also the initial value), a `SORC` id at `ProgressDescriptor_source` and `ProgressDescriptor_style`; `ProgressBar_CreateFromResource` decodes it. `ProgressBar_RefreshFromModel` asks the bound service for `Property_Control`. The provider stores a pointer to a `ControlRange_bytes` record (`ControlRange_value`, `ControlRange_minimum`, `ControlRange_maximum`) in `*out`; the bar copies the three words at once. The text asks for `Property_String`. Turning the wheel reaches `BrightnessController_HandleEvent`, which calls `BrightnessController_HandleWheel`. That converts the motion to steps with `WheelInput_QuantizeMotion(motion, 16)` and passes them to `SettingsModel_AdjustBrightness`. The model clamps the value to 0 to 100 and notifies `Property_Control` for the bar and `Notify_All` for the text.

The disk usage bar is a `ViewType_Capacity` view (`RES_Capacity_View`). It is in `RES_SettingsPreview_Capacity_Template` with style 0 (used and free) and in `RES_About_Capacity_Template` with style 1 (Audio, Video, Photos, Other and free), the style being the descriptor byte at `ViewDescriptor_options_bytes`. It has no source. `CapacityView_Draw` reads the volume's total and free bytes and the library's per-category totals directly in code, then draws each segment from bitmaps that `CapacityView_LoadSegments` loads: a left cap, a one-pixel fill stretched to the segment's width, and a right cap. Style 0's pieces are 13 px high with 9-px caps, style 1's are 30 px high with 16-px caps, and the colours are in the bitmaps. Style 1 has five colour sets, each of three consecutive bitmaps (left cap, fill, right cap) starting at `RES_Capacity_Free_Segment` (grey #d2d5d8), `RES_Capacity_Audio_Segment` (blue #5b93d0), `RES_Capacity_Video_Segment` (purple #6e76d2), `RES_Capacity_Photos_Segment` (yellow #e0b038) and `RES_Capacity_Other_Segment` (orange #de894e); the fills are `BMapFormat_BGRA8888` with a faint dark edge at the top and bottom. The About legend is separate: four groups of a colour swatch image and two text views, each with its own `SORC`. To show other numbers in this style, draw the same pieces on a canvas with `GraphicsContext_CopyRect`.

A row whose `SORC` has a second binding with destination `SourceDestination_InlineValue` shows that source as the blue value beside the title (the Sound Check row, `RES_SettingsMenu_SoundCheck_Item`).

The selection model's current row index comes from its vtable slot `SelectionModel_current_index_method` (see `UI_GetSelectionModel`).

## Input events

`InputEvent_kind` is `InputEvent_ButtonDown`, kinds 7 and 8 for further button phases, `InputEvent_ButtonUp` or `InputEvent_Wheel`. For a wheel event, `WheelEvent_DecodeMotion` fills `InputEvent_wheel_delta`. For a button, `ButtonEvent_GetUIKeyCode` returns the key from `InputEvent_key`: `UIKey_Menu`, `UIKey_Center`, `UIKey_Next`, `UIKey_Previous` or `UIKey_Play`.

A screen controller receives events in `ScreenController_event_method` as `handle(controller, event)`; a nonzero return means handled. The base handlers (`ScreenController_HandleEvent`, and `ResourceScreen_HandleEvent` for resource screens) pass every event to `ScreenController_named_event_method` (`ScreenController_DispatchNamedEvent`). That method turns the event into a name such as `button.menu.up` and runs the matching `SEVT` and `CEVT` bindings. A timer event for the controller's own timer then goes to the timer slot. A subclass handles its own kinds first and falls back to this base code, which is how `BrightnessController_HandleEvent` takes wheel events on a screen without a list. A hook on the event slot that calls the original for keys it does not handle therefore keeps all layout and controller bindings. On a list screen the list view consumes the wheel, and the controller sees only the list's notifications: kind 0x1f, carrying `LstS` and then `LstD` (the `selected` and `delayedselected` events). This was checked in the emulator on Settings > EQ. On a screen whose views take no input, the wheel reaches the controller's event slot, as on Brightness.

`WheelAccumulator_Init(state, threshold, accelerate, interval)` needs `WheelAccumulator_bytes` of storage that stays valid until it is unregistered. Stock values are 5, 1, 350 for menu lists and 8, 0, 350 for a lock-screen code entry. `WheelAccumulator_ConsumeMotion(state, motion, speed)` returns signed steps. `motion` is `InputEvent_wheel_delta` and `speed` is `WheelEvent_GetSpeed(event)` (`InputEvent_wheel_speed`); menu lists pass speed 0. `WheelInput_QuantizeMotion(motion, threshold)` uses one shared accumulator and needs no state; Brightness uses it with threshold 16.

## Navigation

`Controller_ExecuteNavigatorAction` runs the action of an event binding on the screen controller:

| Action | Arguments | Effect |
|---|---|---|
| `navigator.PushScreen` | screen name, layout name, transition | `UI_CreateScreenController`, then the controller's `ScreenController_push_method` |
| `navigator.PushController` | controller name | registered controller, then `ScreenController_push_method` |
| `navigator.PopTopScreen` | | the top controller's `ScreenController_pop_method`, else `Navigator_PopTop` |
| `navigator.PopToScreen` | screen name | `Navigator_PopToScreen` |
| `navigator.PopToMainScreen` | | `Navigator_PopToMainScreen` |
| `navigator.PopToPixoMainScreen` | | `Navigator_PopToPixoMainScreen` |
| `navigator.SwitchLayout` | layout name | `ScreenController_switch_layout_method` |
| `navigator.PushLayout` | layout name | `ScreenController_switch_layout_method` with push set |

Transitions are `Transition_Standard` (`standard`), `Transition_ForegroundPush` (`foregroundpush`) and `Transition_ScreenPush` (`screenpush`). `navigator.PushScreen` passes its third argument through this name lookup, and a missing or unknown name gives `Transition_Standard`. The factories store the value in `ScreenDefinition_transition`. When `UI_DisplayPreparedLayout` replaces one layout with another, instances whose `SsinInstance_template` appears in both layouts stay in place, and only the others are taken down and brought in. That is why a half-screen Settings push animates only the list: the status bar, background and pane templates are shared.

Each `SLst` block also gives the layout a kind (`ScreenLayoutEntry_kind`, kept as `Layout_kind`): 4 for the Settings lists (main, EQ, Main Menu), 8 for Brightness, 9 for Legal. `UI_TransitionToLayout` picks the animation as follows:
- A transition saved by the leaving screen's deactivate replaces the requested one.
- `Transition_Instant`, or either layout of kind 10, displays the new layout at once.
- Some pairs of old and new kinds override the request: 4 with 1, 5 or 9 and 3 with 5 (in either order) become `Transition_ForegroundPush`; 1, 5 or 9 with 9, and 1 with 5, become 4; and 1 with 8 becomes `Transition_Standard`. Other pairs keep the requested transition.
- `Transition_ForegroundPush` and `Transition_ScreenPush` take a snapshot of the whole screen and slide it, so everything moves.
- With the other values there is no snapshot, and only the instances that are not shared move.

So Settings (4) to Legal (9) always slides the whole screen, while Settings (4) to Brightness (8) keeps the requested `Transition_Standard` and moves only the list. A binding may carry both a handler and an action. Pops reach a screen through several paths, so a screen learns that it left from its controller's `ScreenController_deactivate_method`. Its second argument is 0 when another screen is pushed on top and nonzero when the screen is popped (checked in the emulator with Settings and About). Each class's deactivate function appears in exactly one vtable: `ScreenController_Deactivate_slot` holds `ResourceScreen_Deactivate`, and `SettingsMenuListController_Deactivate_slot` holds `SettingsMenuListController_Deactivate`. Vtables are in writable DRAM, so a slot can be patched in place, but the patch then runs for every controller of that class (Settings lists include About), and it must call the original for controllers it does not own.

`UI_CreateScreenController` reads the class name from the screen's `SCST` and calls the registered factory. `ScreenController_CreateNamed` builds the plain resource screen (`TSilverCntlr`, `ResourceScreenController_bytes`, vtable `ScreenController_vtable`) and stores the controller name. The Settings list controller is `SettingsMenuListController_bytes` with vtable `SettingsMenuListController_vtable`; both vtables are `ScreenController_vtable_bytes` long and are preceded by `CxxVtable_prefix_bytes` of offset-to-top and type info.

## Timers

`UI_InitializeEventTimer` binds a timer (`EventTimer_bytes`) to a recipient and an event queue; NULL selects the calling task's queue. `Timer_SetPeriod` sets milliseconds and `Timer_Start` arms it once. The recipient's vtable slot `EventRecipient_event_method` receives events of kind `Event_Timer`; the timer pointer is `TimerEventPayload_timer` in the payload returned by the event's `Event_payload_method` with `EventPayload_Timer`. Any object with that vtable slot can receive timer events.
