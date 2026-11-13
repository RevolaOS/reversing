# Games

The eApp host: how a game image imports firmware services, and what each service does. Names refer to `games.toml`.

## Import resolution

`GameLoader_ResolveImportsAndEntryPoints` walks the image's import list (`GameImageHeader_imports`). Each `GameImport` record names a module, carries its interface id and holds `function_count` stubs (`ldr pc, [pc, #imm]` words) followed by as many literal words. The loader finds the module with `GameAPI_FindModuleByName` in `GameAPI_ModuleRegistry`, requires `function_count` to equal the matching `GameAPIInterface_function_count`, and writes the module's function pointers into the literal words. A record named `GameImport_EndOfListName` ends the list; each record ends with `GameImport_Tail`.

A `GameAPIModule` record is the 32-byte name, counts, `next`, the function pointers, one `GameAPIInterface` (the 16-byte id and the count) and `GameAPIModule_Tail`. The ids are `GameAPI_<Module>_Id0..3`, the counts `GameAPI_<Module>_Count`.

| Module | Count | Functions |
|---|---|---|
| miscTBD | 13 | `GameSystem_*` |
| DebugUtil | 5 | `GameDebug_*` |
| MemoryAlloc | 3 | `GameMemory_*` |
| InputEvents | 2 | `GameInput_*` |
| Settings | 1 | `GameSettings_GetValue` |
| AsyncFileIO | 17 | `GameFile_*`, `GameDirectory_*`, `GameSaveFile_*` |
| SoundEffect | 42 | `GameSound_*` |
| OpenGLES | 170 | GL ES, then EGL and `GameGraphics_*` (TODO: name the GL table) |
| Audio | 54 | `GameAudio_*` |
| Metadata | 169 | `GameMetadata_*` (TODO: name the table) |
| Users | 10 | `GameUser_*` |

The trailing comment on every function entry in `games.toml` gives its module and index, in table order.

## Host objects

Most entries are shims. miscTBD and the file services go through the game runtime (`GameRuntime_file_service` is the AsyncFileIO worker, `GameRuntime_game_heap` the game heap). Audio goes through a media-player bridge that posts games-service events; the UI task applies them and the bridge predicts the result until then. SoundEffect objects live in a table indexed by the handle `GameSound_Create` returns. Users is the games service's player vector.

## Settings

`GameSettings_GetValue` looks the name up in `GameSettings_Table` (`GameSetting` records): `TimeFormat` and `Language` read the preference first, `HoldSwitch` calls `HoldSwitch_ReadSetting`. Unknown names answer -0x32, a null buffer -0x31.

## Files

A game drives AsyncFileIO with a `GameFileRequest` whose `type` is one of `GameFileRequest_*`; the Queue function for that type answers 1 when queued. Paths are relative to the game's directory of the given kind (`GameDirectory_Package`, `UserData`, `Stats`, `SharedData`; see `GameFiles_BuildPath`). Opening for writing, deleting and renaming need a kind other than Package or 4. The worker task (`eAppAsyncIO`) performs the operation, stores `result`, `transferred`, `size` and `handle`, writes the size (file size, or entry count of a listing) to `completion` +8 when set, and links the request into the list the next frame input carries in `GameFrameInput_completed_requests` (`GameRuntime_AppendCompletedRequest`; the request's own `next` is not cleared, so the game zeroes it before queueing). At teardown `GameFileService_Drain` queues a `GameFileRequest_Complete` barrier, waits for it, and drops every handle. `ReadFile` and `WriteFile` open, transfer and close in one request. An `Open` that fails still returns a handle and keeps its `FileHandle` registered on the path until the game queues `Close` for it; a leaked read handle makes a later write open of the same path fail in `FileHandle_OpenNative` with a sharing conflict that leaves `FileHandle_status` unwritten, so the request can report success with a garbage size and a dead handle. Close every handle an `Open` returns, success or not. Seek takes `offset` and `whence` 0, 1 or 2. Directory lists stop at 500 entries with result 0x14.

`GameSaveFile_*` is a write-behind store of four files (`GameSave_MaxSlots`, `GameSave_MaxBytes` together): `Open` binds a slot to a path, `Write` replaces the contents in memory, `Read` copies from memory, and the files are written at `System_SaveAll` and in `GameRuntime_LoadManifestAndLaunch` before the next game's init entry, not when a game ends. Nothing is read from disk on `Open`.

`GameDebug_*` opens up to ten plain files by path and reads or writes them synchronously.

## Sound effects

A `GameSound` is a PCM buffer (`data`, `byte_count`, `sample_rate` 8000..48000, `channels` 1 or 2, `bits_per_sample` 8 or 16) with `volume` 0..0x7fff, `pan` -0x7fff..0x7fff, `speed` 500..2000 per mille, `play_count` (0 loops) and an optional `next` sound played after `tail_ms` of silence. `Play` takes a mixer voice; while the sound plays the music level drops to `duck_level` over `duck_ms`. `state` is `GameSoundState_*`.

## Music

`GameAudio_*` controls the media player: playback list (`InitPlaybackList`, `AppendMediaSelection`, `RemoveEntryAtIndex`, `ClearPlaybackList`), transport (`PlayIndex`, `TogglePlayPause`, `Stop`, `NextTrack`, `PreviousTrack`, `Seek`, the scan functions), settings (repeat, order, EQ, speed, gain, volume 0..255) and queries. `GetPlaybackState` answers `GamePlaybackState_*`, predicted from queued commands when one is pending. `SetEQMenuIndexTemporary` is undone when the game ends; `GameAudio_RestorePlayerSettings` restores everything the bridge saved at launch.

## Open points

- `GameSystem_GetHostStatusValue`, `GameAudio_SetListOption`, `GameAudio_SetPendingItem`, `GameAudio_GetPlayerFlag93F`, `GameAudio_GetCurrentEntryFlag` and `GameSound_SetFlagC` are named after what they touch; their purpose is unverified.
- TODO: name the OpenGLES and Metadata tables.
