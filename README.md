# reversing

What is known about Apple's iPod Classic firmware, and the symbols the RevolaOS core and extensions are built against.

| Path | Contents |
|---|---|
| `symbols/` | One file per firmware version plus its generated linker script and C header |
| `docs/<version>/` | One file per Apple subsystem |
| `ghidra/` | Headless scripts for import, export and verification, and the exported analysis (names, comments, types) |
| `tools/` | `symbols.py` |

## Symbols

`symbols/osos-2.0.5.toml` is the source of truth for every address RevolaOS code uses. Prose in `docs/` uses the names from it, never raw addresses.

| Table | Entry | Generated as |
|---|---|---|
| `[functions]` | `Name = { at, c, thumb }` | linker symbol (bit 0 set for Thumb) and the prototype `c` |
| `[data]` | `Name = { at, c }` | linker symbol and `extern c` |
| `[sites]` | `Name = { at, kind, in }` | `#define SITE_Name` |
| `[offsets.Struct]` | `field = value` | `#define Struct_field` |
| `[constants]`, `[resources]` | `Name = value` | `#define Name`, `#define RES_Name` |

Site kinds: `call` (a `BL` to redirect), `slot` (a pointer in a vtable or table), `word` (a constant), `entry` (a function start for a jump hook), `literal` (an address held in a literal pool). `in` names the containing function and offset; `verify` checks it.

```sh
python tools/symbols.py generate symbols/osos-2.0.5.toml
python tools/symbols.py check symbols/osos-2.0.5.toml
python tools/symbols.py verify symbols/osos-2.0.5.toml --ghidra <analysis>/2.0.5/osos
```

`generate` writes `osos-2.0.5.ld` and `osos-2.0.5.h` next to the source; both are committed. `check` fails when they are stale. `verify` compares every function and site with the Ghidra export and lists functions Ghidra has not defined yet.

Code links against the linker script with `-Wl,--just-symbols=osos-2.0.5.ld` and includes the header. Functions without a prototype exist only so sites can name them.

Apple firmware bytes are never committed. Analysis is reproduced from the user's own decrypted inputs.

TODO: Ghidra scripts that refresh the symbols file from a project and push names back into it.

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
