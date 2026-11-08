# reversing

What is known about Apple's iPod Classic firmware, and the symbols the RevolaOS core and extensions are built against.

| Path | Contents |
|---|---|
| `symbols/` | One file per firmware version: name, address, ARM or Thumb state, C prototype. Source of truth for every address used in code |
| `docs/<version>/` | One file per Apple subsystem |
| `ghidra/` | Headless scripts for import, export and verification, and the exported analysis (names, comments, types) |

Addresses appear only in the symbols files. Prose uses symbol names.

Apple firmware bytes are never committed. Analysis is reproduced from the user's own decrypted inputs.

TODO: generator for the linker script and C header, and the Ghidra sync scripts.

## What drives development

1. **RAM first.** 64 MiB in total, about 20 MiB of heap shared with Apple's music database. Every pull request reports resident and peak bytes from the emulator heap profile. An increase needs a stated reason. A disabled extension costs zero bytes. Reuse what exists before adding anything.
2. **Performance second.** No work in audio callbacks beyond the DSP itself. No redraw of unchanged pixels. No disk reads for things that can be computed. No per-draw checks when a one-time hook can decide. Hot paths bind directly; the dispatcher runs only when two handlers share a site.
3. **Optimise before merge.** Measure, remove what measurement does not justify, and stop when the next step costs more clarity than it saves. "Works" is not the bar.
4. **Complexity matches need.** No abstraction for one caller, no layer that only forwards. Defer anything with one user, delete anything with none. A person must be able to reason about the code.
5. **Clean structure.** One concern per directory. Extensions never reach into core internals. Apple addresses come only from the generated symbols header.
6. **Measure, do not guess.** Emulator heap profile for every RAM claim; hardware for audio, cache and storage behaviour.

## Core or extension

Something is core only if the loader, SDK or settings store needs it, if it must act before the loader can run, or if the boot cannot recover without it. Everything else is an extension, however small. One extension per concern. If one feature cannot work without another, it is a module of that extension, not a dependency.


## License

GPL-3.0-only. See COPYING and NOTICE.
