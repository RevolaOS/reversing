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
