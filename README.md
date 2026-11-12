# reversing

What is known about Apple's iPod Classic firmware, and the symbols the RevolaOS core and extensions are built against.

| Path | Contents |
|---|---|
| `symbols/` | One directory per firmware version plus its generated linker script and C header |
| `docs/` | Workflow pages and one directory per firmware version, one file per subsystem |
| `ghidra/` | Headless scripts, the program manifest and the exported analysis: names, prototypes, comments, types |
| `tools/` | `symbols.py` (symbols), `ghidra.py` (Ghidra project) |

## Workflow

Names and addresses start in Ghidra and end in `symbols/osos-2.0.5/`, from which the linker script and header are generated. Requires Python 3.11+, Ghidra 12.1.4, JDK 21 and the `revola` CLI from the installer repository.

```sh
python tools/ghidra.py import --inputs <decrypted 2.0.5 inputs>
python tools/symbols.py refresh symbols/osos-2.0.5 --ghidra build/ghidra
python tools/symbols.py push symbols/osos-2.0.5 --ghidra build/ghidra
python tools/symbols.py generate symbols/osos-2.0.5
python tools/symbols.py check symbols/osos-2.0.5
python tools/symbols.py verify symbols/osos-2.0.5
```

Apple firmware bytes are never committed. Analysis is reproduced from the user's own decrypted inputs.

## Documentation

| Page | Contents |
|---|---|
| [Symbols](docs/symbols.md) | The symbols files and what is generated from them |
| [Ghidra](docs/ghidra.md) | Project, push and refresh, exported analysis |
| [Apple firmware 2.0.5](docs/2.0.5/README.md) | One page per subsystem |
| [STYLE.md](STYLE.md) | Principles and rules for every file |

## Credits

RevolaOS started from the groundwork of [RepriseOS](https://github.com/lgreitis/osos-lab) by lgreitis: the boot chain, the installer's device and bundle code, and the reverse engineering of Apple's 2.0.5 firmware. You can support lgreitis on [Ko-fi](https://ko-fi.com/lgreitis).

## License

GPL-3.0-only. See COPYING and NOTICE.
