# USB device and disk mode

## Control endpoint

`USBDeviceTask_Run` is the body of the task named "USBDeviceTask". It runs `USB_RunDeviceLoop`, which handles endpoint completions and calls `USB_HandleSetupPacket` for each setup packet. Handlers run on that task and must not block. The device context is `USBDevice_bytes` long; the setup packet sits at `USBDevice_setup` (`USBDevice_request_type`, `USBDevice_request`, `USBDevice_value`, `USBDevice_index`, `USBDevice_length`).

`USB_HandleSetupPacket` sorts requests three ways:

- Device-to-host requests go to `USB_DispatchControlRead`. That passes vendor reads to `USB_HandleVendorDeviceRead` (site `USB_HandleVendorDeviceRead_call`), which answers only `bRequest` 0x40.
- Host-to-device requests with data ask `USB_GetControlWriteBuffer` for a buffer (site `USB_GetControlWriteBuffer_call`). It accepts only class requests 0x21 and 0x22.
- Requests without data go to `USB_DispatchNoDataRequest` (site `USB_DispatchNoDataRequest_call`). Vendor request 0x40 there is `USB_HandleAppleVendorCommand`, which sets the charging current and re-enumerates.

A request nobody accepts stalls. `USB_StartControlTransfer` starts the data stage on an endpoint. `USB_IsAccessoryMode` reports whether the device runs as an accessory rather than mass storage.

## Disk mode

The data volume has one owner at a time, kept by the storage arbiter (`StorageArbiter_Get`, `StorageArbiter_*` offsets). At boot the owner is `DiskClient`, the OS side. USB mass storage uses a `ScsiDiskClient`, created with `ScsiDiskClient_Create`.

1. A SCSI command that needs the medium calls `SCSI_AcquireMedium`. That calls `ScsiDiskClient_Request` and polls `ScsiDiskClient_state` up to 80 times at 100 ms intervals. If the request is not granted, the command fails with NOT READY, medium not present (ASC 0x3a).
2. `StorageArbiter_Request` asks the current owner through its yield slot. For `DiskClient` that is `DiskClient_Yield` (site `DiskClient_Yield_slot`), which posts `HardwareSource_DiskModeRequest` once with `HardwareEvent_Post` and returns 1. A yield slot that returns 0 refuses: the requester's state becomes `ScsiDiskClientState_Refused`, the SCSI side answers medium not present, and the UI stays up.
3. `HardwareEvent_MapSource` and `HardwareEvent_Dispatch` turn the source into `AppMessage_DiskModeRequest`. `SystemUI_DispatchSystemEvent` passes that to `App_HandleUsbConnected` (site `App_HandleUsbConnected_call`), which sets `App_disk_mode_requested` and shows the disk-mode screen.
4. `App_PollDiskModeEntry` then calls `App_EnterDiskMode`, which sets `App_in_disk_mode` and runs `System_PrepareDiskMode`. That logs "Enter Disk Mode", stops playback, calls `System_SaveAll` and frees the databases. The volume is then released to the SCSI client, whose state becomes `ScsiDiskClientState_Granted`.

`DiskClient_OnGranted` posts `HardwareSource_DiskGranted` (`AppMessage_DiskGranted`) when the volume comes back. `App` holds the pointer to the app object. The disk-mode sequence and the refusal were checked in the emulator.
