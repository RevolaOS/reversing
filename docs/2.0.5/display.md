# Display pipe

There is no single framebuffer. The display controller at `DISPLAY_PIPE_BASE` composites RGB layers and a scaled YUV video window, and a screenshot is the composite of the enabled windows. These register facts come from the emulator's display model; hardware blending has not been compared.

Each layer has a slot at `DisplayPipe_slot0 + layer × DisplayPipe_slot_step`:

- `DisplaySlot_stride`: bytes per line.
- `DisplaySlot_format`: the format is bits 15 to 8. `DisplayFormat_ARGB8888` stores bytes B, G, R, A; `DisplayFormat_RGB565` is 16-bit.
- `DisplaySlot_source`: the pixel address.
- `DisplaySlot_size`: width << 16 | height, signed.
- `DisplaySlot_position`: x << 16 | y, signed.

`DisplayPipe_control` enables the windows: `DisplayLayerEnable_Layer0` to `DisplayLayerEnable_Layer4` for the five RGB slots and `DisplayLayerEnable_Video` for the video window. `DisplayPipe_SetWindowEnable` sets one of them by window index 0 to 5, where 5 is the video window. ARGB layers blend source-over. The Settings screen enables only layer 0: 320 × 240 ARGB at a stride of 1280 bytes. Decoding it matched the emulator screenshot pixel for pixel.

## Stacking and blending

Three blend registers, listed in `DisplayBlendRegisters`, hold a global alpha in the low byte (`DisplayPipe_SetWindowAlpha`) and a blend mode in bits 12 to 15 and 28 to 31 (`DisplayPipe_SetWindowBlendMode`, modes 0 to 4). Window 0 uses column 0, windows 1 to 3 share column 1, and the video window uses column 2. `DisplayPipe_SetOrder` writes `DisplayPipe_order` and keeps the order index; the row of `DisplayLayerOrders` for that index maps each column to a blend register. Window 4 has no blend register. During main-menu preview changes the order is 1, which gives the video window the first blend register; layer 0 then has transparent pixels where the video picture belongs, so the video window lies behind layer 0. Which blend register is the back one in general has not been confirmed.

## Video window

The video window shows a planar YUV picture through a scaler. `DisplayPipe_ConfigureWindow` (window 5) and `DisplayPipe_SetWindowSource` program it:

| Register | Contents |
|---|---|
| `DisplayPipe_video_mode` | `DisplayVideoMode_YUV420` for `DisplayFormat_YUV420`, else 0 |
| `DisplayPipe_video_strides` | luma stride in bytes, chroma stride << 16 |
| `DisplayPipe_video_source_start`, `DisplayPipe_video_source_end` | source crop, x << 16 \| y |
| `DisplayPipe_video_plane0` to `DisplayPipe_video_plane3` | plane addresses; for `DisplayFormat_YUV420`: luma, chroma, 0, chroma |
| `DisplayPipe_video_scale` | horizontal << 16 \| vertical step, 0x1000 = 1:1 |
| `DisplayPipe_video_position`, `DisplayPipe_video_size` | on-screen x << 16 \| y and width << 16 \| height |

The scaler filter tables at `DISPLAY_PIPE_BASE` + 0x180, + 0x31c and + 0x364 are loaded with it. Formats 9 and 10 use the same registers with two and four plane addresses and have not been seen.

`DisplayLayer_Commit` applies a UI layer to its window: it releases the previous surface, takes the new one, splits a YUV surface into planes and calls the configure, source and enable methods. While the main-menu preview pane changes and while menus slide, the preview background goes to the video window. In the emulator that picture was a 320 × 240 YUV 4:2:0 grey gradient (stride 320, chroma stride 160, chroma near 0x88), the Podcasts pane background, and the control word went from 0x41118141 to 0x411181c1. Settled screens draw the background into layer 0 and turn the video window off.
