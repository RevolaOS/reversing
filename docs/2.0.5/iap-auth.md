# iAP accessory authentication

Accessories that want authentication-gated iAP lingos must prove they carry an Apple authentication coprocessor. The exchange runs in the General Lingo: the iPod requests the accessory certificate and a signature over a nonce, verifies the signature and, on success, marks the accessory authenticated.

## Verification gate

`IAP_HandleDevAuthenticationSignature` handles `RetDevAuthenticationSignature` (0x18). It looks up the accessory's record (one per connected accessory, up to three), picks a verifier by the record's verifier kind and acknowledges with `AckDevAuthenticationStatus` (0x19):

| Kind | Verifier |
|---|---|
| 1 | `IAP_VerifyDeviceSignatureRSA` |
| 2 | `IAP_VerifyDeviceSignatureAlternate` |

Both verifiers return 0 on success. On 0 the handler calls `IAP_SetAccessoryAuthState` with state 6 (authenticated), which unlocks the gated lingos. Any other result refuses them.

Each verifier has exactly one caller, `IAP_HandleDevAuthenticationSignature`, and is entered by `BL`. An entry hook on both that returns 0 therefore authenticates every accessory and changes nothing else. The accessory still has to answer the certificate and signature requests, but any bytes pass.
