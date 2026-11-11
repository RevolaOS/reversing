# Power and sleep

## Sleep

`Power_EnterLightSleep(mode)` puts the device into light sleep and returns after wake with a set of wake flags. Two controller paths call it:

| Caller | Mode | Trigger |
|---|---|---|
| `PowerController_HandleIdleSleep` | 0 | idle timeout; checks the hold switch and wake flags afterwards |
| `PowerController_HandleRequestedSleep` | 1 | explicit sleep request |

Light sleep is entered only on battery or USB power. On the power adapter the iPod stays awake, so code that relies on a sleep hook (site `Sleep_Idle_call` or `Sleep_Request_call`) never runs there. Seen in the emulator. TODO: confirm on hardware.

## Saving state

`System_SaveAll` flushes the media database state, the event log and `Preferences_FlushIfDirty`, which writes `iPod_Control/Device/Preferences` only when its dirty flag is set. It has three callers, USB disk mode among them, so site `Preferences_FlushIfDirty_call` fires on every full save.

## Preferences and the clicker

The object `Preferences_FlushIfDirty` writes is reached as `App` → `App_browser` → `MediaBrowser_preferences`. Its dirty byte is `PreferencesFile_dirty`: `Preferences_MarkDirty` sets it, and `Preferences_FlushIfDirty` clears it once the file is open, whatever the write returns. A setting change starts no timer and sends no notification; the file is written only by the next flush.

Settings change preferences through the preferences service (`Service_Preferences`, returned by `PreferencesService_Get` and held at `SettingsModel` + `Preferences_object`). `Preferences_set_method` takes `(service, property, Service_Preferences, value)`, where the value `Preference_Toggle` cycles the setting. `Preferences_get_method` takes `(service, property, Service_Preferences)` and returns the value.

`Preference_Clicker` holds `Clicker_Both`, `Clicker_Off`, `Clicker_Headphones` or `Clicker_Speaker`. Setting it stores the choice in the service and calls `Preferences_SetClickerRoute`. That writes the route bits (`ClickerRoute_Speaker`, `ClickerRoute_Headphones`; Both is 3, Off 0) to `PreferencesFile_clicker_route` and to `ClickerRoute`, and marks the file dirty only if the bits changed. The click code reads `ClickerRoute` on every click (watched in the emulator), not the file copy. Storing `PreferencesFile_clicker_route` directly therefore changes what the next write puts in the file without touching the clicks or the dirty byte. Stock Settings labels 0x608f "Speaker" and 0x6090 "Headphones". RepriseOS found the routing the other way round on the device, and the names here follow the routing; this has not been checked here.

`SettingsModel_ToggleClicker` (`ToggleSetting_Clicker` in `Settings_HandleAction`) sets `Preference_Toggle`: Both becomes Off, and Off, Headphones and Speaker become Both. It then calls `SettingsModel_NotifyClicker` for `SettingsProperty_ClickerImage`, `SettingsProperty_ClickerValue` and `SettingsProperty_ClickerDescription`. `SettingsModel_GetProperty` answers all three for every value. The value is the Off, Speaker, Headphones or On string (Both shows "On"), the description is the matching `RES_Settings_Clicker*_Description`, and the image is `RES_Settings_ClickerOn_Image` when route bit `ClickerRoute_Speaker` is set and `RES_Settings_ClickerOff_Image` otherwise. In the emulator, 0x6090 showed Headphones with the On picture and 0x608f showed Speaker with the Off picture, without a crash.

## Disk power

`Disk_GetPowerState` reports whether the ATA block is powered. Unpowered means the disk is not spinning; powered does not prove it is, because a powered disk can be in standby. It only reads registers: the disk driver chosen once from the hardware version (the hard-disk table on the iPod Classic, checked in the emulator) reports `PowerGate0_ATA` clear in `ClockRegister_PowerGate0`. It takes no lock and sends no ATA command, so it can be polled from any task. The PATA standby command is only sent while the block is powered.

UI timers are described in [ui.md](ui.md).

## Clock and reset

`Clock_GetMicroseconds` returns the 32-bit timer register `ClockRegister_Microseconds`. It counts microseconds and wraps after 2^32 µs, about 71.6 minutes. `Platform_InitMicrosecondCounter` configures the timer and clears and starts it. The OSOS entry code calls it early, so the count starts at OSOS start, not at SoC reset. A second caller runs it from a registered handler; it did not run when the emulator woke from idle sleep. No other tick counter since boot has been found.

`SetWarmBootModeTag` copies an 8-byte tag to `WarmBootTag_address` in IRAM. The modes are `WarmBootMode_None` through `WarmBootMode_RetailOS`, with the tags "--------", "diskmode", "diskscan", "softupdt" and "retailOS". `ResetSocAndWait` stops the watchdog, requests a software reset of the SoC and spins until it happens. `System_CheckVolumeOrRestart`, the first call in `App_ExitDiskMode`, sets `WarmBootMode_RetailOS` and resets when its volume check fails. What the boot code does with each tag, and whether IRAM survives the reset, has not been checked. No cold reset path (watchdog or PMU) has been found.

