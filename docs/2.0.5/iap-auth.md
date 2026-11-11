# iAP accessory authentication

Accessories that want authentication-gated iAP lingos must prove they carry an Apple authentication coprocessor. The exchange runs in the General Lingo: the iPod requests the accessory certificate and a signature over a nonce, verifies the signature and, on success, marks the accessory authenticated.

## Handshake

The General Lingo (`0x00`) carries the authentication exchange over the dock serial or USB transport:

| iPod request | Accessory response | Purpose |
|---|---|---|
| `GetDevAuthenticationInfo` (`0x14`) | `RetDevAuthenticationInfo` (`0x15`) | Supply the accessory certificate |
| `GetDevAuthenticationSignature` (`0x17`) | `RetDevAuthenticationSignature` (`0x18`) | Supply the signature over the challenge |

The iPod reports the result with `AckDevAuthenticationStatus` (`0x19`). Replacing the signature verifiers does not synthesize these responses or skip the handshake. An accessory that never reaches signature verification does not gain authentication from this bypass.

## Verification gate

`IAP_HandleDeviceAuthenticationSignature` handles `RetDevAuthenticationSignature` (0x18). It looks up the accessory's record (one per connected accessory, up to three), picks a verifier by the record's verifier kind and acknowledges with `AckDevAuthenticationStatus` (0x19):

| Kind | Verifier |
|---|---|
| 1 | `IAP_VerifyDeviceSignatureRSA` |
| 2 | `IAP_VerifyDeviceSignatureAlternate` |

Both verifiers return 0 on success. On 0 the handler calls `IAP_SetAccessoryAuthState` with state 6 (authenticated), which unlocks the gated lingos. Any other result refuses them.

Each verifier has exactly one caller, `IAP_HandleDeviceAuthenticationSignature`, and is entered by `BL`. The call sites are `IAP_VerifyDeviceSignatureRSA_call` and `IAP_VerifyDeviceSignatureAlternate_call`. Replacing both calls with handlers returning 0 bypasses signature verification. The accessory still has to answer the certificate and signature requests.

The RSA verifier takes `(manager, channel, key_id, signature, signature_bytes)`, with the byte count on the stack. The alternate verifier takes `(authentication, signature, signature_bytes)`, all in registers. Both return their status in r0.

Call-site hooks leave the verifier entries intact and restore the original calls when unbound. Unlike two-word entry patches, they do not require proving that the accessory task cannot be executing either verifier during core boot. Restoring verification affects subsequent handshakes; it does not revoke an accessory's existing authenticated state.
