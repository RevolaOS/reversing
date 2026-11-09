# OSOS startup

## Relocation

The OSOS file starts with a 0x800-byte IMG1 header. `Startup_CopyImageToRuntimeRegions` copies the IRAM part of the body to `IRAM_BASE` and the DRAM code and initialised data to `OSOS_RUNTIME_BASE`, clears the IRAM BSS and branches to the relocated IRAM entry. Several functions exist twice by name in Ghidra: a veneer in IRAM and the implementation in DRAM. The symbols file names the DRAM implementation.

Startup also builds the MMU section map (see [memory.md](memory.md)).

## SysInfo

The NOR loader passes a boot-info record. `Platform_ImportBootSysInfo` checks its `SysI` magic and copies the 0x120-byte SysInfo into OSOS data; `Platform_GetImportedSysInfo` returns that copy. SysInfo carries, among others, the region flags and the DRAM base and size. `Platform_HasRegionalVolumeLimit` returns bit 0 of its region flags.

## Tasks

`System_CreateAndStartConfiguredTasks` walks a table of 21 task descriptors twice: first it creates every task, then it starts them. The table includes the graphics manager, so UI code can run before startup returns.
