# Storage

## Files

| Function | Use |
|---|---|
| `File_OpenLegacyPath` | open a path; returns 0 and a handle |
| `File_ReadLegacyHandle` | read into a buffer, reports bytes read |
| `File_WriteLegacyHandle` | write from a buffer, reports bytes written |
| `File_CloseLegacyHandle` | close |
| `File_DeleteLegacyPath` | delete a file or an empty directory |

There is no rename in this API. `File_OpenLegacyPath` accepts `Dir\name`, `\Dir\name` and `/Dir/name` on the data volume. A missing file returns -43.

## When files can be read

| Site | Disk reads |
|---|---|
| `ResourceBank_InitOverrides_call` (in `UI_InitResourceProviders`) | fail: the data volume is not readable yet |
| `Preferences_LoadFile_call` (in `Preferences_LoadAndMigrate`) | work: first point where file reads succeed |

Seen in the emulator. TODO: confirm on hardware with a spinning disk.
