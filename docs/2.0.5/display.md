# Display pipe

There is no single framebuffer. The display controller at `DISPLAY_PIPE_BASE` composites up to three layers, and a screenshot is the composite of the enabled layers. These register facts come from the emulator's display model; hardware blending has not been compared.

Each layer has a slot at `DisplayPipe_slot0 + layer × DisplayPipe_slot_step`:

- `DisplaySlot_stride`: bytes per line.
- `DisplaySlot_format`: the format is bits 15 to 8. `DisplayFormat_ARGB8888` stores bytes B, G, R, A; `DisplayFormat_RGB565` is 16-bit.
- `DisplaySlot_source`: the pixel address.
- `DisplaySlot_size`: width << 16 | height, signed.
- `DisplaySlot_position`: x << 16 | y, signed.

`DisplayPipe_control` enables the layers (`DisplayLayerEnable_Layer0`, `DisplayLayerEnable_Layer1`, `DisplayLayerEnable_Layer2`). The low byte of `DisplayPipe_order` picks one of the six back-to-front orders in `DisplayLayerOrders`. ARGB layers blend source-over. The Settings screen enables only layer 0: 320 × 240 ARGB at a stride of 1280 bytes. Decoding it matched the emulator screenshot pixel for pixel.
