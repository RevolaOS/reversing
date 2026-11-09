# UI: screens, menu rows and events

## Screens

A pushed screen is created by `ControllerRegistry_InvokeScreenFactory` from a controller class name, a screen id and a layout id. Screens seen in Settings:

| Screen | Controller | Screen id | Layout id |
|---|---|---|---|
| Settings list | `TSilverSettingsMenuListCntlr` | `RES_SettingsMenu_Main_Screen` | `RES_SettingsMenu_Main_Layout` |
| Settings > Main Menu | `TCSettings_MainMenu` | `RES_Settings_MainMenu_Screen` | `RES_Settings_MainMenu_Layout` |
| Settings > Legal | `TSilverCntlr` | `RES_Settings_Legal_Screen` | `RES_Settings_Legal_Layout` |

Each controller factory builds its screen definition with `UI_ConstructScreenDefinition`, which runs `UI_LoadScreenLayouts`: every block of the screen's `SLst` becomes a prepared layout in `ScreenDefinition_layout_cache`, keyed by layout id. Showing a layout looks it up with `ControllerLayoutCache_GetOrInsert` and aborts on a miss, so the layout must be listed in the screen's `SLst` when the controller is created. Resources come from the resource bank at that moment; there is no boot-time registry, so screens and layouts added with `ResourceBank_SetResource` later work as long as they are in place before the push.

## Menu rows

A list's rows are blocks in an `ITEM` resource (`RES_SettingsMenu_Items` for Settings). Row fields: `MenuRow_id`, `MenuRow_style` (2 = text row without arrow or chosen event), `MenuRow_visible`, `MenuRow_title` (a `SORC` id or property source) and `MenuRow_title_from_model`. `MenuItem_SetVisible` finds a row by id in the menu registry, so it only reaches rows that were in the `ITEM` table when the list loaded.

`MenuList_RebuildVisibleItems` drops rows whose `MenuRow_visible` is 0 and subscribes to the Settings-style sources of rows with `MenuRow_title_from_model` set. The last such row is the list's dynamic row: `MenuList_QueryProperty` reports the row count as the visible rows minus one plus that row's provider's answer to `Property_Count` for its property. A model-backed last row therefore has to answer `Property_Count` with 1, or it disappears; answering N expands it into N rows.

The Main Menu and Music Menu toggle screens have no `ITEM` table: `SettingsModel_GetIndexedProperty` serves their rows from `MainMenuSettingsRows` and `MusicMenuSettingsRows` (`MenuSettingsRow_*`), selected by property id alone.

## Main, Music and Extras menus

The main menu is `RES_MainMenu_Items` on `RES_MainMenus_Main_Screen`, the Music menu `RES_MusicMenu_Items` on `RES_MainMenus_Music_Screen`; their rows are the `RES_MainMenu_*_Item` and `RES_MusicMenu_*_Item` ids, in the order of `MainMenuSettingsRows` and `MusicMenuSettingsRows`. Extras is controller `TCExtrasMenu` on `RES_ExtrasMenu_Screen`; its rows (`RES_ExtrasMenu_*_Item`) are bound in the screen's `CEVT`, and only `HandleLock` and `HandleAddressBook` reach `ExtrasMenu_DispatchAction` (site `Extras_Action_slot`). Its list is bound to `RES_ExtrasMenu_Items` through a `ViewBinding_ItemTable` block in the screen's `Resource_VSlt` resource.

Each `*_Action_slot` is the `ScreenController_action_method` entry of a controller vtable. Main, Music and Now Playing each have two such vtables, so a hook binds both the plain and the `Secondary` slot. Which class each screen creates has not been traced.

Sites `MainMenu_Index_call` and `MusicMenu_Index_call` are `BLX r1` through the selection model, not direct calls; a replacement must call the original through r1.

## Resource blocks

UI resources are a 32-bit count followed by blocks of `{bytes, kind, data}`, each padded to 4 bytes. `ResourceBlockList_Load` passes the kind word to `ResourceBlockList_AppendBorrowedBlock` as its metadata.

`ResourceBank_SetResource` copies the data into the heap; replacing an override frees the previous copy, so pointers into it become invalid. There is no call to remove an override.

## Screen resources

| Type | Keyed by | Content |
|---|---|---|
| `Resource_SCST` | screen id | controller class name, NUL-terminated |
| `Resource_SLst` | screen id | blocks; word 0 of each is a layout id (the first is the default layout) |
| `Resource_SLyt` | layout id | blocks; word 0 of each is an `SSin` id |
| `Resource_SSin` | instance list id | blocks: `SsinInstance_view_bindings` (a `VCvs` id), `SsinInstance_template`, `SsinInstance_substitute` (loaded as the instance's `VLyt` id), `SsinInstance_flags` |
| `Resource_VCvs` | instance id | blocks whose kind is a view id, each holding option blocks; a `Resource_SORC` option is a 32-bit source id. Kind `ViewBinding_ItemTable` binds a list to an `ITEM` table at `ItemTableBinding_item_table` |
| `Resource_View` | template id | view descriptors, one block per view, kind = view type |
| `Resource_TMLT` | template id | template parameters, copied unchanged when a template is cloned |
| `Resource_VLyt` | view layout id | `ViewLayout_title` is the title source |
| `Resource_SORC` | source id | one `SORC` block of `SourceBinding_service`, `SourceBinding_property` and `SourceBinding_destination` |

A source for a literal string is a `Resource_String` resource (UTF-8, NUL-terminated) bound with service `Service_String`, property = string id, destination `SourceDestination_Title`. Settings model rows use service `Service_Settings` with the property id.

A view descriptor starts with a 12-word frame, then `ViewDescriptor_id` and `ViewDescriptor_type`; `ViewDescriptor_options_bytes` is the shortest descriptor RepriseOS builds for a custom view. In the Legal text page the paragraph descriptor holds a top offset at +0x0c, the view it anchors below at +0x10, an anchor mode at +0x14 and its text source at +0x64. These meanings and the `SsinInstance_flags` bits 16 to 23 (cleared for a full-screen custom view) come from RepriseOS's UI compiler and are not derived from code.

The names in navigator actions resolve through `ResourceName_GetId`, which looks the C++ string up in the global name map (`ResourceNames_FindOrInsertSlot`); an unknown name maps to 0.

## Events

Event tables are resources of type `Resource_TEVT` (template), `Resource_SEVT` (layout) and `Resource_CEVT` (screen controller): a count followed by events. An event is the name, the byte `'1'`, the handler, a 32-bit has-action flag and, if set, the action and its arguments. Strings are a 32-bit length followed by UTF-8. Words in the stream are not aligned, so a writer stores them bytewise.

The stock Settings rows bind `list.pid.<row id>.chosen` and `delayedselected` in the `CEVT` of `RES_SettingsMenu_Main_Screen`; the layout's `SEVT` carries only named controller events. For a binding without an action, the screen controller's action slot receives the handler string. It is first asked with `ActionSupportQuery`, where a nonzero return means supported, and then called with an object pointer as the argument.

`UI_GetLayoutEventTable` loads a layout's tables once through site `UI_LoadLayoutEvents_call`. A hook there can call the original and then read more event streams into `Layout_events` with `UIEventMap_ReadStream`, which is how bindings are added at run time.

## Views

`UI_RegisterViewFactory` maps a view type (the descriptor's `ViewDescriptor_type`) to `create(owner, parent, event_target, descriptor)`. A custom view fills `ViewDescriptor_options_bytes` of options with `View_InitOptionsFromDescriptor`, allocates `View_bytes`, calls `View_Construct` and installs its own vtable: a copy of `View_vtable` (`View_vtable_bytes`, the last four slots null) with `View_draw_method` replaced. `View_destroy_method` is the deleting destructor. A full redraw is `View_InvalidateRect(UI_GetRootView(), root + View_bounds)`.

### Templates

`UI_InitializeViewTemplates` runs once per template owner while the UI starts. It enumerates every `Resource_View` with `ResourceBank_FirstResource` and `ResourceBank_NextResource`, and `UI_CreateTemplateFromResource` builds a live view tree for each one from its `Resource_TMLT`. Nothing is built later. On a ROM-backed bank the enumeration walks only the ROM index and never the overrides, so a View that exists only as an override gets no template. To add one, hook site `ResourceBank_NextTemplate_call`: call the original, and when it returns NULL, yield the extra View resources one at a time. The hook is called again with the last id it returned.

`UI_DisplayPreparedLayout` finds each `SSin` instance's template with `UI_FindTemplateView`. If there is no template, the instance is skipped without an error. Otherwise `UI_ApplyInstanceResources` applies the instance's `Resource_VLyt`, then the layout's `Resource_VSlt`, then the instance's `Resource_VCvs`. Each block finds a child view of the template by id and passes its options to `View_ApplyPropertyBindings`. All screens that use a template share the same views, so bindings set for one screen stay in place until another screen rebinds them.

The stock Legal page binds only its title this way. Its paragraph text comes from the template's view descriptors.

Settings > Brightness is the slider prototype: its template has a `ViewType_Progress` bar (`RES_Settings_Brightness_Bar`) and a `ViewType_Text` value (`RES_Settings_Brightness_Value_Text`). Bound to a Settings property, the bar asks for type `Property_Control` and reads three signed words (`ControlRange_*`); the text asks for `Property_String`.

A row whose `SORC` has a second binding with destination `SourceDestination_InlineValue` shows that source as the blue value beside the title (the Sound Check row, `RES_SettingsMenu_SoundCheck_Item`).

The selection model's current row index comes from its vtable slot `SelectionModel_current_index_method` (see `UI_GetSelectionModel`).

## Input events

`InputEvent_kind` is `InputEvent_ButtonDown`, kinds 7 and 8 for further button phases, `InputEvent_ButtonUp` or `InputEvent_Wheel`. For a wheel event, `WheelEvent_DecodeMotion` fills `InputEvent_wheel_delta`. For a button, `ButtonEvent_GetUIKeyCode` returns the key from `InputEvent_key`: `UIKey_Menu`, `UIKey_Center`, `UIKey_Next`, `UIKey_Previous` or `UIKey_Play`.

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

Transitions are `Transition_Standard` (`standard`), `Transition_ForegroundPush` (`foregroundpush`) and `Transition_ScreenPush` (`screenpush`). A binding may carry both a handler and an action. Pops reach a screen through several paths, so a screen learns that it left from its controller's `ScreenController_deactivate_method`.

`UI_CreateScreenController` reads the class name from the screen's `SCST` and calls the registered factory. `ScreenController_CreateNamed` builds the plain resource screen (`TSilverCntlr`, `ResourceScreenController_bytes`, vtable `ScreenController_vtable`) and stores the controller name. The Settings list controller is `SettingsMenuListController_bytes` with vtable `SettingsMenuListController_vtable`; both vtables are `ScreenController_vtable_bytes` long and are preceded by `CxxVtable_prefix_bytes` of offset-to-top and type info.

## Timers

`UI_InitializeEventTimer` binds a timer (`EventTimer_bytes`) to a recipient and an event queue; NULL selects the calling task's queue. `Timer_SetPeriod` sets milliseconds and `Timer_Start` arms it once. The recipient's vtable slot `EventRecipient_event_method` receives events of kind `Event_Timer`; the timer pointer is `TimerEventPayload_timer` in the payload returned by the event's `Event_payload_method` with `EventPayload_Timer`. Any object with that vtable slot can receive timer events.
