# Audio

## Effects chain

`Audio_InitEffectsChain` builds a chain of three effect stages in a fixed order: sample-rate converter, three-band EQ, playback speed. Each starts disabled; post-effects gain starts at unity; source and output default to 44.1 kHz, 16-bit stereo.

`Audio_ProcessEffectsAndOutput` runs every enabled stage on the PCM buffer, passing the buffer pointer and frame count from one stage to the next, then applies the post-effects gain and submits the result to the output. `Audio_SetPostEffectsGainQ15` sets that gain in Q15 (unity skips the multiply).

The submission is two calls: `AudioOutputQueue_Get` returns `AudioOutputQueue`, allocating its 128 KiB ring the first time, and `AudioOutputQueue_Write` copies the frames into it, failing without a copy when the ring has no room. `AudioOutputQueue_Get_call` is the last point at which the buffer and frame count are still in registers (r6 and r4, with the context in r9), so an extension that processes the final PCM hooks there. The context describes the format it hands over in `AudioEffectsContext_output_rate`, `AudioEffectsContext_output_bits` and `AudioEffectsContext_output_channels`; a hook checks for 16-bit stereo before touching the samples.

## Output underruns

`Audio_FillOutputBuffer` runs from IRAM each time the output needs another buffer. While audio is active it reads the buffer from `AudioOutputQueue` through `AudioOutputQueue_Read`. When the queue is empty, that function fills the whole buffer with silence (site `AudioOutputQueue_SilenceFill_call`) and adds one to `AudioOutputQueue_underruns` and `AudioOutputQueue_underruns_since_reset`. A queue holding less than a buffer is copied without silence and not counted. In the emulator both counters stood at 221 once a track had started and did not move during ten seconds of steady playback.

## EQ

`Audio_SetEQPreset` forwards a preset to `EQ_SetDspPreset`, which stores it, reloads coefficients with `EQ_LoadPresetCoefficients` and resets every biquad history. Presets 1 to 22 have separate coefficient banks for 44.1 and 48 kHz; preset 0 or any other sample rate bypasses the EQ.

`EQ_ProcessThreeBiquadsInPlace` runs up to three stereo biquads (`EQ_ProcessBiquadStereoS16`) in place. Coefficients are signed Q8.24, accumulation is 64-bit, and each active stage saturates to S16 before the next. Because the post-effects gain runs after the EQ, it cannot undo EQ clipping; precut has to act before the first stage.

The presets are two tables of `EQ_PresetCount` rows, Off first: `EQ_PresetIds` holds each row's settings id, `EQ_Presets` its name and preview image resource ids (`EQPreset_name`, `EQPreset_preview`). Settings ids run from `EQ_SettingsId_First` to `EQ_SettingsId_Last`, with `EQ_SettingsId_Off` and `EQ_SettingsId_Flat` for Off and Flat. `EQ_SettingsIdToDspPreset` maps a settings id to the preset `EQ_SetDspPreset` takes; `EQ_SettingsIdToMenuIndex` and `EQ_MenuIndexToSettingsId` convert between a settings id and its row. The readers hold the tables' addresses as literals (`EQ_MenuIds_literal`, `EQ_SettingsIds_literal`, `EQ_PreviewTable_literal`, `EQ_NameTable_literal`, `EQ_ResourceTable_literal`) and the row count as immediates (`EQ_DspCount_word`, `EQ_PreviewCount_word`, `EQ_NameCount_word`, `EQ_SettingsCount_word`, `EQ_ProviderCount_word`, `EQ_ResourceCount_word`).

The setting is kept as a row index under `Preference_EQMenuIndex`, read and written through the typed preference methods, and as a settings id in `PreferencesFile_eq_settings_id`. `Settings_SetEQPreset` stores the chosen row's settings id and applies it through `MediaPlayer_apply_eq_settings_id_method`; `MediaPlayer_GetEQSettingsId` and `MediaPlayer_GetEQMenuIndex` read it back.

`MediaTrack_ApplyEQ` runs for a track `MediaTrack_IsReady` accepts. It reads the track's own settings id (`MediaItem_GetEQSettingsId`, from `MediaRecord_eq_settings_id`) at `EQ_TrackPreset_call`; a nonzero id replaces the player's unless the player's is Flat. The result goes through `EQ_SettingsIdToDspPreset` to the audio task.

The effect's record is a `ThreeBandEQ`: `ThreeBandEQ_sample_rate`, `ThreeBandEQ_enabled`, `ThreeBandEQ_preset` and three `EQStage` records at `ThreeBandEQ_stages`, each with its coefficients, the four history words per channel and a bypass flag. `ThreeBandEQ_vtable` carries the reset and process methods (`EQ_Reset_slot`, `EQ_Process_slot`); `EQ_SetDspPreset` and `EQ_SetSampleRate` call `EQ_LoadPresetCoefficients` at `EQ_SetPreset_Load_call` and `EQ_SetRate_Load_call`. `AudioMixer_Get` returns the mixer whose `AudioMixer_effects` context holds the EQ at `AudioEffectsContext_eq`.

## MP4 seek

`MediaPlayer_Seek` reaches the stream object through `MediaStream_seek_method` (`MediaStream_Seek_slot`). `MediaStream_Seek` clamps the target to `MediaStream_duration_ms` less `MediaStream_SeekEndMarginMs`, resets the track slots and calls the parser at `MediaStream_parser` through `MediaParser_seek_to_time_method`; for MP4 files that is `MP4Parser_SeekToTime` (`MP4Parser_SeekToTime_slot`), which rejects targets past `MP4Parser_duration_ms`, seeks the primary track with `MP4Parser_SeekPrimaryTrack` and then the audio track at `MP4Parser_audio_track` with `MP4Parser_SeekAudioTrack`.

`MP4Parser_SeekAudioTrack` has no index: it advances the `MP4Track` one sample per pass, adding each `time_to_sample` delta to `MP4Track_presentation_time` and `MP4Track_decode_time`, until the presentation time reaches the target in the track's `MP4Track_timescale`; a target behind the current sample restarts from sample 1. Each pass reads the sample size, sample-to-chunk and edit tables through windows that `MP4Parser_LoadSampleSizeWindow`, `MP4Parser_LoadSampleToChunkWindow` and `MP4Parser_LoadChunkOffsetWindow` refill from the file (`MP4_SampleSizeWindow`, `MP4_SampleToChunkWindow`, `MP4_ChunkOffsetWindow` entries at a time) with `MP4Parser_ReadBigEndianWords`. `MP4Parser_LoadTrackEdit` keeps `MP4Track_edit_media_start`, `MP4Track_edit_media_end` and `MP4Track_edit_duration` for the current edit, converting durations with `Runtime_UDiv64`. `MP4Parser_ReadSamples` is the playback read path, split into `MP4Parser_ReadAudioTrackSamples` and `MP4Parser_ReadPrimaryTrackSamples`.

Both the seek walk and the audio read path compare `MP4Track_decode_time` with the edit bounds after sign-extending only its low word (`MP4Parser_SeekEditEndTest_word`, `MP4Parser_ReadEditStartTest_word`, `MP4Parser_ReadEditEndTest_word`), so a file with an edit list stops at 2^31 ticks: 13:31:35 at 44.1 kHz, 12:25:39 at 48 kHz. A seek replacement enters at `MP4Parser_SeekAudioTrack_entry`, which only the audio task runs.

## EU volume policy

Volume runs from 0 to 256. `Platform_HasRegionalVolumeLimit` tests bit 0 of the imported SysInfo flags. `Platform_UsesVolumeWarningPolicy` tests bit 7 (`VolumePolicy_RegionTest_word`), else a stored override byte; it selects the EU Volume Limit screens (`VolumeLimitEU`, `SettingsMenuEU`), the high-volume warning, exposure counting and the startup clamp. `Volume_GetRegionalCeiling` returns 256 unless bit 0 or the policy is set, then 168 with EU Volume Limit on and 222 with it off (`Volume_EUOffCeiling_word`). `Volume_GetDefaultLimit` returns 222 under the same condition, else 256 (`Volume_EUDefaultLimit_word`). An iPod with bit 0 alone is therefore held at 222 without the screen to change it.

`VolumePolicy_IsWarningAcknowledged` reports acknowledged when the policy test fails (`VolumePolicy_WarningPolicyTest_word`). `Preferences_LoadAndMigrate` clamps the saved volume only under the policy (`Volume_StartupClampTest_word`).

`MediaPlayer_FinishCurrentTrack`, on track changes and stop, resets the warning once exposure passes 18 hours (`VolumeExposure_TrackFinishTest_word` is the policy branch). `MediaPlayer_PostVolumeExposureLimitEvent`, the exposure callback after 20 hours, posts event 5, which lowers the volume. Neither depends on the EU Volume Limit setting.
