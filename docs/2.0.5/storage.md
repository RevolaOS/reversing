# Storage

## Files

| Function | Use |
|---|---|
| `File_OpenLegacyPath` | open a path; returns 0 and a handle |
| `File_ReadLegacyHandle` | read into a buffer, reports bytes read |
| `File_WriteLegacyHandle` | write from a buffer, reports bytes written |
| `File_CloseLegacyHandle` | close |
| `File_DeleteLegacyPath` | delete a file or an empty directory |
| `Directory_CreateWithParents` | create a directory and any missing parents; returns 0, also when it already exists |

There is no rename in this API. `File_OpenLegacyPath` accepts `Dir\name`, `\Dir\name` and `/Dir/name` on the data volume. A missing file returns `FileError_NotFound`; other open errors do not establish absence.

`Directory_CreateWithParents` takes `Dir/sub` or `/Dir/sub` (`/` only) and volume 0 for the data volume. It locks the volume and creates each path prefix in turn; Apple calls it before writing `iPod_Control/Device/radio`.

A positive-length read starting at EOF returns `FileError_EndOfFile` with zero bytes. A read spanning EOF can succeed with a short byte count. These indicate truncation when checking a known report length; other read errors must leave the pending report unchanged for retry.

## Write completion

`File_CloseLegacyHandle` returns zero without exposing the underlying close result. `File_WriteLegacyHandle` with flags bit 0 set invokes a native flush but discards its result; the returned byte count is not proof that data reached stable storage. Reading the file back in the same boot checks its contents through the running filesystem and caches, not survival of a power interruption.

The FAT path can suppress a directory-entry update failure while leaving metadata dirty. The ATA command dispatcher can also report a successful flush completion after the physical driver returned an error. Durability requires checking native metadata updates and physical device-flush completion.

These paths use filesystem and device locks. They are unsuitable for panic-time writes when the interrupted task may own those locks. Write reports after a controlled restart and retain the pending recovery record until a later boot can confirm the report and settings.

## When files can be read

| Site | Disk reads |
|---|---|
| `ResourceBank_InitOverrides_call` (in `UI_InitResourceProviders`) | fail: the data volume is not readable yet |
| `Preferences_LoadFile_call` (in `Preferences_LoadAndMigrate`) | work: first point where file reads succeed |

Seen in the emulator. TODO: confirm on hardware with a spinning disk.
