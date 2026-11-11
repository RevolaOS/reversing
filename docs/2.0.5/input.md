# Input: click wheel buttons

`Wheel_HandleInterrupt` decodes each button packet from the wheel controller into the held-button mask at `WheelState_buttons` in `WheelState`:

| Bit | Button |
|---|---|
| `WheelButton_Center` | Center |
| `WheelButton_Next` | Next |
| `WheelButton_Previous` | Previous |
| `WheelButton_Play` | Play/Pause |
| `WheelButton_Menu` | Menu |

`Wheel_RunTask` polls the controller once when it starts, then on each packet lets `Wheel_DispatchButtonChanges` compare the mask with `WheelState_reported_buttons` and call `Input_HandleButton` for every change. `Input_HandleButton` maps the button to a UI key through `ButtonKeyCodes` and may swap Menu with Play and Next with Previous for a rotated layout; the mask always reports the physical button.

Reading `WheelState_buttons` from any task tells which buttons are held without touching the controller. A button held from power-on is visible there by the time `Preferences_LoadFile_call` runs (seen in the emulator).

## Hold switch

The hold switch is not polled. When its GPIO changes, the GPIO handler reads the raw bit and calls `HoldSwitch_NotifyRawChange` (site `HoldSwitch_NotifyRawChange_call`), which posts `HardwareSource_HoldChanged`. `HardwareEvent_Dispatch` then reads `HoldSwitch_GetLogicalState` and, if it differs from its cached state, emits `AppMessage_HoldOn` or `AppMessage_HoldOff`. `SystemUI_DispatchSystemEvent` passes both to `SystemUI_HandleHoldChange` on the UI task (site `SystemUI_HandleHoldChange_call`) and marks them handled, so screen controllers never see a hold change. That handler updates the lock state and releases held keys. `Input_HandleButton` and `PowerController_HandleIdleSleep` read `HoldSwitch_GetLogicalState` directly for each button press or idle check, so key gating does not depend on the event. `HoldSwitch_GetLogicalState` reads the switch over the PMU bus through `HoldSwitch_ReadHardwareBit` on every call. Its callers ran on different tasks in the emulator: `Input_HandleButton` on the wheel task, `HardwareEvent_Dispatch` on its own task after a hold change and on the UI task after waking from sleep, and `PowerController_HandleIdleSleep` on the UI task. Callers through its one-instruction thunk and `HoldSwitch_ReadSetting` both reach it, so an override goes at site `HoldSwitch_GetLogicalState_entry`. In the emulator no reads happened during 20 seconds idle. An injected hold state therefore has to replace `HoldSwitch_GetLogicalState` and then post `HardwareSource_HoldChanged`; this has not been tried.

## Wheel touch and injection

`WheelState` also holds the touch state: `WheelState_touch` is the finger position (0 to `WheelPositions` - 1) OR `WheelTouch_Down`, and 0 when the finger is lifted. `WheelState_threshold`, `WheelState_motion`, `WheelState_position`, `WheelState_reported_position`, `WheelState_touch_count` and `WheelState_previous_touch` are the accumulator's working fields. `InputWheel_AccumulateMotion` counts a move when the threshold ≤ |delta| < 36, and menu lists consume motion with threshold 5 and a carried remainder. `WheelState_notify_pending` marks a queued wheel notification. The UI updates `WheelState_reported_position` when it consumes motion; lifting also updates it, discarding any motion the UI has not consumed.

To inject input, do what `Wheel_HandleInterrupt` does, then wake the wheel task:
- buttons: write the mask to `WheelState_buttons`;
- touch: write the position OR `WheelTouch_Down` to `WheelState_touch`;
- lift: write 0 to `WheelState_touch`, 6 to `WheelState_threshold` and 0 to `WheelState_motion`.

Then clear `WheelState_skip_packet` and call `Event_Signal(WheelEvent_Signal)`, which is safe from any task and from interrupts. The wheel task handles one step per wake, so steps have to be paced: one per request or per timer tick. A loop on the UI task does not give the wheel task time to run. This was checked in the emulator.

