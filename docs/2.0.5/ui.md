# UI: screens, menu rows and events

## Screens

A pushed screen is created by `ControllerRegistry_InvokeScreenFactory` from a controller class name, a screen id and a layout id. Screens seen in Settings:

| Screen | Controller | Screen id | Layout id |
|---|---|---|---|
| Settings list | `TSilverSettingsMenuListCntlr` | `RES_SettingsMenu_Main_Screen` | `RES_SettingsMenu_Main_Layout` |
| Settings > Main Menu | `TCSettings_MainMenu` | `RES_Settings_MainMenu_Screen` | `RES_Settings_MainMenu_Layout` |
| Settings > Legal | `TSilverCntlr` | `RES_Settings_Legal_Screen` | `RES_Settings_Legal_Layout` |

## Menu rows

A list's rows are blocks in an `ITEM` resource (`RES_SettingsMenu_Items` for Settings). Row fields: `MenuRow_id`, `MenuRow_style` (2 = text row without arrow or chosen event), `MenuRow_visible`, `MenuRow_title` (a `SORC` id or property source) and `MenuRow_title_from_model`. `MenuItem_SetVisible` finds a row by id in the menu registry, so it only reaches rows that were in the `ITEM` table when the list loaded.

The Main Menu and Music Menu toggle screens have no `ITEM` table: `SettingsModel_GetIndexedProperty` serves their rows from `MainMenuSettingsRows` and `MusicMenuSettingsRows` (`MenuSettingsRow_*`), selected by property id alone.

## Resource blocks

UI resources are a 32-bit count followed by blocks of `{bytes, kind, data}`, each padded to 4 bytes. `ResourceBlockList_Load` passes the kind word to `ResourceBlockList_AppendBorrowedBlock` as its metadata.

## Events

Event tables are resources of type `Resource_TEVT` (template), `Resource_SEVT` (layout) and `Resource_CEVT` (screen controller): a count followed by events. An event is the name, the byte `'1'`, the handler, a 32-bit has-action flag and, if set, the action and its arguments. Strings are a 32-bit length followed by UTF-8. Words in the stream are not aligned, so a writer stores them bytewise.

The stock Settings rows bind `list.pid.<row id>.chosen` and `delayedselected` in the `CEVT` of `RES_SettingsMenu_Main_Screen`; the layout's `SEVT` carries only named controller events. For a binding without an action, the screen controller's action slot receives the handler string. It is first asked with `ActionSupportQuery`, where a nonzero return means supported, and then called with an object pointer as the argument.

`UI_GetLayoutEventTable` loads a layout's tables once through site `UI_LoadLayoutEvents_call`. A hook there can call the original and then read more event streams into `Layout_events` with `UIEventMap_ReadStream`, which is how bindings are added at run time.

## Timers

`UI_InitializeEventTimer` binds a timer (`EventTimer_bytes`) to a recipient and an event queue; NULL selects the calling task's queue. `Timer_SetPeriod` sets milliseconds and `Timer_Start` arms it once. The recipient's vtable slot `EventRecipient_event_method` receives events of kind `Event_Timer`; the timer pointer is `TimerEventPayload_timer` in the payload returned by the event's `Event_payload_method` with `EventPayload_Timer`. Any object with that vtable slot can receive timer events.
