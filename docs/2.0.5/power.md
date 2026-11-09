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

`Disk_GetPowerState` reports whether the ATA block is powered. Unpowered means the disk is not spinning; powered does not prove it is, because a powered disk can be in standby. The PATA standby command is only sent while the block is powered.

## Timers

Native UI timers deliver events to a controller on the UI task. `UI_InitializeEventTimer` binds a timer to a controller and event queue, and `Timer_Start` arms it once; the handler re-arms it for periodic work. The controller receives event kind 0x0f carrying the timer pointer.

TODO: measure the units of the timer period.
