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

## Disk power

`Disk_GetPowerState` reports whether the ATA block is powered. Unpowered means the disk is not spinning; powered does not prove it is, because a powered disk can be in standby. It only reads registers: the disk driver chosen once from the hardware version (the hard-disk table on the iPod Classic, checked in the emulator) reports `PowerGate0_ATA` clear in `ClockRegister_PowerGate0`. It takes no lock and sends no ATA command, so it can be polled from any task. The PATA standby command is only sent while the block is powered.

UI timers are described in [ui.md](ui.md).

## Clock and reset

`Clock_GetMicroseconds` returns the 32-bit timer register `ClockRegister_Microseconds`. It counts microseconds and wraps after 2^32 µs, about 71.6 minutes. `Platform_InitMicrosecondCounter` configures the timer and clears and starts it. The OSOS entry code calls it early, so the count starts at OSOS start, not at SoC reset. A second caller runs it from a registered handler; it did not run when the emulator woke from idle sleep. No other tick counter since boot has been found.

`SetWarmBootModeTag` copies an 8-byte tag to `WarmBootTag_address` in IRAM. The modes are `WarmBootMode_None` through `WarmBootMode_RetailOS`, with the tags "--------", "diskmode", "diskscan", "softupdt" and "retailOS". `ResetSocAndWait` stops the watchdog, requests a software reset of the SoC and spins until it happens. `System_CheckVolumeOrRestart`, the first call in `App_ExitDiskMode`, sets `WarmBootMode_RetailOS` and resets when its volume check fails. What the boot code does with each tag, and whether IRAM survives the reset, has not been checked. No cold reset path (watchdog or PMU) has been found.

