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

A `GameSound` is a PCM buffer (`data`, `byte_count`, `sample_rate` `GameSound_SampleRateMin`..`GameSound_SampleRateMax`, `channels` 1 or 2, `bits_per_sample` 8 or 16, 8-bit samples signed) with `volume` 0..`GameSound_VolumeMax`, `pan` -`GameSound_PanMax`..`GameSound_PanMax`, `speed` `GameSound_SpeedMin`..`GameSound_SpeedMax` per mille, `play_count` (0 loops until stopped) and an optional `next` sound played after `tail_ms` of silence. `GameSound_Create` returns the index in a table of ten slots that doubles when full, -1 only when the allocation fails. Setters store the raw value and answer 1; `SetData` refuses a null pointer and keeps only the pointer, so the sample memory belongs to the game for the sound's lifetime; `SetNext` needs two valid handles and cannot clear a chain. The format is checked when a voice is set up (`FUN_08118720`); a rejected format leaves the voice's previous format.

`Play` takes one of `AudioMixer_VoiceCount` voices, stealing the oldest when none is free, and answers 1 for any valid handle. `lead_in_ms` plus `duck_ms` are silence before the sound, during which the music level drops to `duck_level`; with `flags` bit 1 set `Play` queues a 500 ms tone instead of the audio clock hold. `state` is `GameSoundState_*`: `Play`, `Pause`, `Resume` and `Stop` write it along the `next` ring. A sound that ends by itself clears `attached` and `voice` but leaves `state` at Playing, so `IsPlaying` (attached and Playing) is the test for completion and `GetState` is not. `GetVoicePosition` is the voice's frame cursor of the current pass. Nothing is required per frame. `Destroy` detaches the voices and deletes only the object. At game end the runtime teardown resets every voice and destroys every remaining sound before the game heap goes.

## Music

`GameAudio_*` controls the media player: playback list (`InitPlaybackList`, `AppendMediaSelection`, `RemoveEntryAtIndex`, `ClearPlaybackList`), transport (`PlayIndex`, `TogglePlayPause`, `Stop`, `NextTrack`, `PreviousTrack`, `Seek`, the scan functions), settings (repeat `GameRepeatMode_*`, order `GamePlaybackOrder_*`, EQ, speed, gain, volume 0..`GameAudio_VolumeMax`) and queries. Positions are milliseconds. Setters post a games-service event; the UI task applies it (`FUN_080f5d94`) and the bridge predicts the playback state, the entry count and the last volume payload until the queue drains, so `GetPlaybackState` and `GetEntryCount` answer the prediction while the other queries read the player and lag a queued command. `TogglePlayPause` with `wait` set delays 150 ms after pausing. `GameInput_ForwardButtonEvent` acts on releases only: Menu leaves the game, Next and Previous skip, Play toggles. `GameAudio_SnapshotPlayerSettings` runs at launch and `GameAudio_RestorePlayerSettings` at game end: repeat and order come back, speed returns to 1000, the EQ only when set through `SetEQMenuIndexTemporary`, gain to 0, and the playback list is rebuilt when the game changed it; volume is not restored.

## Open points

- `GameSystem_GetHostStatusValue` and `GameSound_SetFlagC` are named after what they touch; their purpose is unverified.
- TODO: name the OpenGLES and Metadata tables.
