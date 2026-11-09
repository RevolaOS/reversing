# Memory

The CPU is an ARM926EJ-S (ARMv5TEJ): no `MOVW`/`MOVT`, no `LDREX`/`STREX`, no execute-never bit, separate I and D caches with a write buffer. Code that patches instructions must clean the D-cache by address, drain the write buffer and invalidate the I-cache.

DRAM is 64 MiB from `DRAM_BASE` to `DRAM_END`. IRAM starts at `IRAM_BASE`.

## MMU section map

OSOS builds a 16 KiB first-level table at `MMU_TRANSLATION_TABLE` during startup, maps 1 MiB sections and then enables the MMU and caches.

| Virtual | Physical | Attributes |
|---|---|---|
| first 1 MiB | `IRAM_BASE` | uncached; exception vectors run from IRAM |
| `IRAM_BASE`, 1 MiB | same | uncached |
| DRAM (base and size from SysInfo) | same | cached, buffered |
| DRAM `\| DRAM_UNCACHED_ALIAS` | DRAM | uncached |
| `MMIO_BASE`, 128 MiB | same | uncached |

Every section has access permission 3 (read/write in all modes), so Apple code in cached DRAM can be patched in place. DRAM size and base come from SysInfo (see [startup.md](startup.md)).

## OSOS image in DRAM

| Region | Start | End |
|---|---|---|
| code and read-only data | `OSOS_RUNTIME_BASE` | data start |
| initialised data | after code | BSS start |
| BSS | after data | `OSOS_NATIVE_HEAP_BASE` |
| heap | `OSOS_NATIVE_HEAP_BASE` | end of DRAM |

A file offset in the OSOS image maps to `offset - OSOS_RUNTIME_FILE_OFFSET + OSOS_RUNTIME_BASE` for the DRAM part.

## Heap

`Memory_InitPlatformHeapBounds` asks `Platform_GetHeapRegion` for the heap and hands it to the memory pool. `Platform_GetHeapRegion` returns the base from the literal at site `HeapRegionBase_literal` and the size as SysInfo RAM base plus RAM size minus that base. A second literal, site `RuntimeHeapBase_literal`, holds the same base. Both hold `OSOS_NATIVE_HEAP_BASE`. Rewriting both moves the start of the heap; the end always follows SysInfo.

The RevolaOS core is placed on the first 4 KiB page above `OSOS_NATIVE_HEAP_BASE`, and both heap-base literals are repointed to the end of the core rounded up to 4 KiB.

`Heap_AllocateZeroed` and `Heap_Free` allocate from and free to this heap.

## Interrupts and interworking

The IRQ vector enters `IRQ_Entry` in IRAM. It switches to SVC mode, pushes an `IRQFrame_bytes` frame on the interrupted task's stack (`IRQFrame_spsr`, r0 to r12, `IRQFrame_lr`, `IRQFrame_pc`), counts the nesting in `IRQ_Nesting` and, on the first level, moves the stack to `IRQ_STACK_TOP`. It then calls `IRQ_Dispatch(frame)`, which returns the stack to resume. Tasks run in SVC mode, so the frame's LR is the task's own. Site `IRQ_Dispatch_entry` hooks every interrupt with interrupts off.

IRAM runs from `IRAM_BASE` to `IRAM_END`; OSOS's loaded IRAM sections end well below that, and the boot descriptor lives near the top.

Some Apple routines return with `mov pc, lr`, which stays in ARM state: `IRQ_RestoreState` and anything that tail-calls it, such as `Event_Signal`. A Thumb caller comes back in ARM state and crashes, so call them from an ARM function that is not a tail call. `IRQ_DisableSave` masks interrupts and returns the previous CPSR for `IRQ_RestoreState`. `Clock_GetMicroseconds` reads the free-running microsecond counter in a single load and is safe in interrupts.

