# Apple firmware 2.0.5

iPod Classic (7th generation) running Apple OSOS 2.0.5. Names refer to `symbols/osos-2.0.5/`; NOR names refer to the analysis in `ghidra/analysis/2.0.5/NOR`.

| File | Subsystem |
|---|---|
| [memory.md](memory.md) | CPU, DRAM, IRAM, MMU section map, OSOS image layout, heap, interrupts |
| [nor.md](nor.md) | NOR loader: EFI volume, PreEfi, DXE |
| [startup.md](startup.md) | OSOS startup: relocation, SysInfo, task start |
| [storage.md](storage.md) | File API and when the data volume becomes readable |
| [power.md](power.md) | Sleep, state saving, disk power |
| [ui.md](ui.md) | Screens, menu rows, resource blocks, events, timers |
| [input.md](input.md) | Click wheel buttons, touch and input injection |
| [usb.md](usb.md) | USB control endpoint, disk mode |
| [display.md](display.md) | Display pipe layers |
| [audio.md](audio.md) | Effects chain, EQ, post-effects gain |
| [iap-auth.md](iap-auth.md) | iAP accessory authentication |
| [games.md](games.md) | eApp host: import modules, files, sound effects, music control |
