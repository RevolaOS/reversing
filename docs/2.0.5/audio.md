# Audio

## Effects chain

`Audio_InitEffectsChain` builds a chain of three effect stages in a fixed order: sample-rate converter, three-band EQ, playback speed. Each starts disabled; post-effects gain starts at unity; source and output default to 44.1 kHz, 16-bit stereo.

`Audio_ProcessEffectsAndOutput` runs every enabled stage on the PCM buffer, passing the buffer pointer and frame count from one stage to the next, then applies the post-effects gain and submits the result to the output. `Audio_SetPostEffectsGainQ15` sets that gain in Q15 (unity skips the multiply).

## EQ

`Audio_SetEQPreset` forwards a preset to `EQ_SetDspPreset`, which stores it, reloads coefficients with `EQ_LoadPresetCoefficients` and resets every biquad history. Presets 1 to 22 have separate coefficient banks for 44.1 and 48 kHz; preset 0 or any other sample rate bypasses the EQ.

`EQ_ProcessThreeBiquadsInPlace` runs up to three stereo biquads (`EQ_ProcessBiquadStereoS16`) in place. Coefficients are signed Q8.24, accumulation is 64-bit, and each active stage saturates to S16 before the next. Because the post-effects gain runs after the EQ, it cannot undo EQ clipping; precut has to act before the first stage.
