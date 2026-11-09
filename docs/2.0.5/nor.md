# NOR loader

The 2.0.5 loader (`apple-loader.bin`) is the plaintext body of the NOR primary image. All NOR modules differ from 2.0.4, so 2.0.4 module offsets do not apply.

## Layout

The loader runs at `IRAM_BASE`. Its first 0x100 bytes are the ARM exception vectors. It contains an EFI firmware volume of 41 images: 40 PE32 and one TE (SecCore). PreEfi and SecCore are linked in place inside the loader. PreEfi loads DxeMain, which loads the other modules.

## Startup

1. The reset vector reaches a short ARM stub at the end of the loader that enters SecCore (ARM).
2. SecCore enters PreEfi in Thumb state at `InitializePreEfiBootMemory`. It initialises hardware, detects the boot mode and the DRAM range and builds the HOB list.
3. `PrepareDramResourcesAndEnterDxe` adds DRAM resource HOBs (`InitializeHobMemoryRange` sets up the list), loads DXE, prepares its stack and enters it through `EnterDxeWithStack`. That stub sets SP, passes the HOB list in r0 and jumps without a return address.
4. DxeMain dispatches drivers (Ata, DxeSst25, SystemConfig, ROM, Aes, IpodFirmwareFS and others) as their protocol dependencies appear, and finally Bds, which loads OSOS.

## Handoff to OSOS

`Bds_SelectAndBootFirmware` picks an image from the firmware directory by name:
- `diag`, `disk` or `scan` for the special boot modes;
- `aupd` when an update is pending;
- otherwise, when there is no `dfup` image and an `rsrc` image exists, `osr3` or else `osos`.

It starts the chosen image with `Bds_LoadAndStartImage(image, flags, argument)`; for OSOS the flags are 0. That function:

1. Reads the 0x800-byte image header into pool memory and verifies it.
2. Allocates pages at the load address given by the directory entry, then reads and decrypts the body through `DRAM_UNCACHED_ALIAS` (load address OR the alias bit).
3. Writes `BOOT_SYSINFO_MAGIC` and a pointer to `BOOT_DESCRIPTOR + BootDescriptor_sysinfo_magic` and `BootDescriptor_sysinfo`, copies SysInfo to that pointer, and stores the flags at `BootDescriptor_flags`.
4. Sets CPSR to 0xd3 (SVC mode, IRQ and FIQ masked) and branches by register to the load address, so OSOS starts in ARM state with interrupts off.

`Platform_ImportBootSysInfo` reads the same descriptor slot. If the header fails verification and the third argument is 0, a second verifier handles the image instead.
