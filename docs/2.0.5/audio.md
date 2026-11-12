# Audio

## Effects chain

`Audio_InitEffectsChain` builds a chain of three effect stages in a fixed order: sample-rate converter, three-band EQ, playback speed. Each starts disabled; post-effects gain starts at unity; source and output default to 44.1 kHz, 16-bit stereo.

`Audio_ProcessEffectsAndOutput` runs every enabled stage on the PCM buffer, passing the buffer pointer and frame count from one stage to the next, then applies the post-effects gain and submits the result to the output. `Audio_SetPostEffectsGainQ15` sets that gain in Q15 (unity skips the multiply).

## Output underruns

`Audio_FillOutputBuffer` runs from IRAM each time the output needs another buffer. While audio is active it reads the buffer from `AudioOutputQueue` through `AudioOutputQueue_Read`. When the queue is empty, that function fills the whole buffer with silence (site `AudioOutputQueue_SilenceFill_call`) and adds one to `AudioOutputQueue_underruns` and `AudioOutputQueue_underruns_since_reset`. A queue holding less than a buffer is copied without silence and not counted. In the emulator both counters stood at 221 once a track had started and did not move during ten seconds of steady playback.

## EQ

`Audio_SetEQPreset` forwards a preset to `EQ_SetDspPreset`, which stores it, reloads coefficients with `EQ_LoadPresetCoefficients` and resets every biquad history. Presets 1 to 22 have separate coefficient banks for 44.1 and 48 kHz; preset 0 or any other sample rate bypasses the EQ.

`EQ_ProcessThreeBiquadsInPlace` runs up to three stereo biquads (`EQ_ProcessBiquadStereoS16`) in place. Coefficients are signed Q8.24, accumulation is 64-bit, and each active stage saturates to S16 before the next. Because the post-effects gain runs after the EQ, it cannot undo EQ clipping; precut has to act before the first stage.

The presets are two tables of `EQ_PresetCount` rows, Off first: `EQ_PresetIds` holds each row's settings id, `EQ_Presets` its name and preview image resource ids (`EQPreset_name`, `EQPreset_preview`). Settings ids run from `EQ_SettingsId_First` to `EQ_SettingsId_Last`, with `EQ_SettingsId_Off` and `EQ_SettingsId_Flat` for Off and Flat. `EQ_SettingsIdToDspPreset` maps a settings id to the preset `EQ_SetDspPreset` takes; `EQ_SettingsIdToMenuIndex` and `EQ_MenuIndexToSettingsId` convert between a settings id and its row. The readers hold the tables' addresses as literals (`EQ_MenuIds_literal`, `EQ_SettingsIds_literal`, `EQ_PreviewTable_literal`, `EQ_NameTable_literal`, `EQ_ResourceTable_literal`) and the row count as immediates (`EQ_DspCount_word`, `EQ_PreviewCount_word`, `EQ_NameCount_word`, `EQ_SettingsCount_word`, `EQ_ProviderCount_word`, `EQ_ResourceCount_word`).

The setting is kept as a row index under `Preference_EQMenuIndex`, read and written through the typed preference methods, and as a settings id in `PreferencesFile_eq_settings_id`. `Settings_SetEQPreset` stores the chosen row's settings id and applies it through `MediaPlayer_apply_eq_settings_id_method`; `MediaPlayer_GetEQSettingsId` and `MediaPlayer_GetEQMenuIndex` read it back.

`MediaTrack_ApplyEQ` runs for a track `MediaTrack_IsReady` accepts. It reads the track's own settings id (`MediaItem_GetEQSettingsId`, from `MediaRecord_eq_settings_id`) at `EQ_TrackPreset_call`; a nonzero id replaces the player's unless the player's is Flat. The result goes through `EQ_SettingsIdToDspPreset` to the audio task.

The effect's record is a `ThreeBandEQ`: `ThreeBandEQ_sample_rate`, `ThreeBandEQ_enabled`, `ThreeBandEQ_preset` and three `EQStage` records at `ThreeBandEQ_stages`, each with its coefficients, the four history words per channel and a bypass flag. `ThreeBandEQ_vtable` carries the reset and process methods (`EQ_Reset_slot`, `EQ_Process_slot`); `EQ_SetDspPreset` and `EQ_SetSampleRate` call `EQ_LoadPresetCoefficients` at `EQ_SetPreset_Load_call` and `EQ_SetRate_Load_call`. `AudioMixer_Get` returns the mixer whose `AudioMixer_effects` context holds the EQ at `AudioEffectsContext_eq`.

## EU volume policy

Volume runs from 0 to 256. `Platform_HasRegionalVolumeLimit` tests bit 0 of the imported SysInfo flags. `Platform_UsesVolumeWarningPolicy` tests bit 7 (`VolumePolicy_RegionTest_word`), else a stored override byte; it selects the EU Volume Limit screens (`VolumeLimitEU`, `SettingsMenuEU`), the high-volume warning, exposure counting and the startup clamp. `Volume_GetRegionalCeiling` returns 256 unless bit 0 or the policy is set, then 168 with EU Volume Limit on and 222 with it off (`Volume_EUOffCeiling_word`). `Volume_GetDefaultLimit` returns 222 under the same condition, else 256 (`Volume_EUDefaultLimit_word`). An iPod with bit 0 alone is therefore held at 222 without the screen to change it.

`VolumePolicy_IsWarningAcknowledged` reports acknowledged when the policy test fails (`VolumePolicy_WarningPolicyTest_word`). `Preferences_LoadAndMigrate` clamps the saved volume only under the policy (`Volume_StartupClampTest_word`).

`MediaPlayer_FinishCurrentTrack`, on track changes and stop, resets the warning once exposure passes 18 hours (`VolumeExposure_TrackFinishTest_word` is the policy branch). `MediaPlayer_PostVolumeExposureLimitEvent`, the exposure callback after 20 hours, posts event 5, which lowers the volume. Neither depends on the EU Volume Limit setting.
