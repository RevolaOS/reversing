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

The hold switch is not polled. When its GPIO changes, the GPIO handler reads the raw bit and calls `HoldSwitch_NotifyRawChange` (site `HoldSwitch_NotifyRawChange_call`), which posts `HardwareSource_HoldChanged`. `HardwareEvent_Dispatch` then reads `HoldSwitch_GetLogicalState` and, if it differs from its cached state, emits the change that reaches `SystemUI_HandleHoldChange` on the UI task. That handler updates the lock state and releases held keys. `Input_HandleButton` and `PowerController_HandleIdleSleep` read `HoldSwitch_GetLogicalState` directly for each button press or idle check, so key gating does not depend on the event. In the emulator no reads happened during 20 seconds idle. An injected hold state therefore has to replace `HoldSwitch_GetLogicalState` and then post `HardwareSource_HoldChanged`; this has not been tried.

## Wheel touch and injection

`WheelState` also holds the touch state: `WheelState_touch` is the finger position (0 to `WheelPositions` - 1) OR `WheelTouch_Down`, and 0 when the finger is lifted. `WheelState_threshold`, `WheelState_motion`, `WheelState_position`, `WheelState_touch_down_position`, `WheelState_touch_count` and `WheelState_previous_touch` are the accumulator's working fields. `InputWheel_AccumulateMotion` counts a move when the threshold ≤ |delta| < 36, and 12 positions move a list by one row.

To inject input, do what `Wheel_HandleInterrupt` does, then wake the wheel task:
- buttons: write the mask to `WheelState_buttons`;
- touch: write the position OR `WheelTouch_Down` to `WheelState_touch`;
- lift: write 0 to `WheelState_touch`, 6 to `WheelState_threshold` and 0 to `WheelState_motion`.

Then clear `WheelState_skip_packet` and call `Event_Signal(WheelEvent_Signal)`, which is safe from any task and from interrupts. The wheel task handles one step per wake, so steps have to be paced: one per request or per timer tick. A loop on the UI task does not give the wheel task time to run. This was checked in the emulator.

