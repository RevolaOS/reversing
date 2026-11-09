# NOR loader

The 2.0.5 loader (`apple-loader.bin`) is the plaintext body of the NOR primary image. All NOR modules differ from 2.0.4, so 2.0.4 module offsets do not apply.

## Layout

The loader runs at `IRAM_BASE`. Its first 0x100 bytes are the ARM exception vectors. It contains an EFI firmware volume of 41 images: 40 PE32 and one TE (SecCore). PreEfi and SecCore are linked in place inside the loader. PreEfi loads DxeMain, which loads the other modules.

## Startup

1. The reset vector reaches a short ARM stub at the end of the loader that enters SecCore (ARM).
2. SecCore enters PreEfi in Thumb state at `InitializePreEfiBootMemory`. It initialises hardware, detects the boot mode and the DRAM range and builds the HOB list.
3. `PrepareDramResourcesAndEnterDxe` adds DRAM resource HOBs (`InitializeHobMemoryRange` sets up the list), loads DXE, prepares its stack and enters it through `EnterDxeWithStack`. That stub sets SP, passes the HOB list in r0 and jumps without a return address.
4. DxeMain dispatches drivers (Ata, DxeSst25, SystemConfig, ROM, Aes, IpodFirmwareFS and others) as their protocol dependencies appear, and finally Bds, which loads OSOS.

TODO: re-check the Bds image loader and OSOS handoff (load address, uncached alias, boot-info copies, entry mode) against the 2.0.5 Bds module.
