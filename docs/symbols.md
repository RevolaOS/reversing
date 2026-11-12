# Symbols

`symbols/osos-2.0.5/` is the source of truth for every address RevolaOS code uses. It holds one TOML file per subsystem, such as `media.toml` or `artwork.toml`; `firmware.toml` carries the image identity and memory map. Each file can use every table below, so a subsystem's functions, sites and struct offsets sit together. Names are unique across the directory. Prose in `docs/` uses the names, never raw addresses.

## Tables

| Table | Entry | Generated as |
|---|---|---|
| `[functions]` | `Name = { at, c, thumb }` | linker symbol (bit 0 set for Thumb), the prototype `c`, and `#define ADDR_Name` for patch declarations |
| `[data]` | `Name = { at, c }` | linker symbol, `extern c` and `#define ADDR_Name` |
| `[sites]` | `Name = { at, kind, in }` | `#define SITE_Name` |
| `[offsets.Struct]` | `field = value` | `#define Struct_field` |
| `[constants]`, `[resources]` | `Name = value` | `#define Name`, `#define RES_Name` |

| Site kind | What is at the address |
|---|---|
| `call` | a `BL`, or a `BLX` through a register, to redirect |
| `slot` | a pointer in a vtable or table |
| `word` | a constant |
| `entry` | a function start for a jump hook |
| `literal` | an address held in a literal pool |

`in` names the containing function and offset; `verify` checks it.

## Commands

```sh
python tools/symbols.py generate symbols/osos-2.0.5
python tools/symbols.py check symbols/osos-2.0.5
python tools/symbols.py verify symbols/osos-2.0.5
```

| Command | Does |
|---|---|
| `generate` | writes `osos-2.0.5.ld` and `osos-2.0.5.h` next to the directory; both are committed |
| `check` | fails when they are stale |
| `verify` | compares every function and site with `ghidra/analysis/2.0.5/osos` (or `--analysis <dir>`) and lists functions Ghidra has not defined yet |
| `push`, `refresh` | move names between the files and a Ghidra project, see [Ghidra](ghidra.md) |

CI runs `check` and `verify`.

## Using the output

Code links against the linker script with `-Wl,--just-symbols=osos-2.0.5.ld` and includes the header. A function without a prototype gets only its linker symbol and `ADDR_` define; add `c` before calling it from C. The revola repository copies both files into `firmware/` unchanged.
