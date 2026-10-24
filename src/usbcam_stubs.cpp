// Xbox Live Vision camera (XUsbcam*) imports.
// The ReXGlue v0.9.0 SDK ships xboxkrnl_usbcam.cpp but leaves it out of its
// build, so Poker Smash (links XCAM) fails to link. Provide them here:
// camera always reports "not connected", the rest are no-op stubs.
// Logic adapted from Xenia / ReXGlue xboxkrnl_usbcam.cpp (BSD license).

#include <rex/kernel/xboxkrnl/private.h>
#include <rex/hook.h>
#include <rex/types.h>
#include <rex/system/kernel_state.h>
#include <rex/system/xtypes.h>

using namespace rex;
using namespace rex::kernel;

namespace poker_smash::usbcam {

rex::u32 XUsbcamCreate_entry(rex::u32 buffer, rex::u32 buffer_size,
                             mapped_void unk3_ptr) {
  // Must succeed: some titles abort init if camera alloc "fails".
  return X_STATUS_SUCCESS;
}

rex::u32 XUsbcamGetState_entry() {
  return 0;  // 0 = no camera connected
}

}  // namespace poker_smash::usbcam

REX_EXPORT(__imp__XUsbcamCreate, poker_smash::usbcam::XUsbcamCreate_entry)
REX_EXPORT(__imp__XUsbcamGetState, poker_smash::usbcam::XUsbcamGetState_entry)
REX_EXPORT_STUB(__imp__XUsbcamSetCaptureMode);
REX_EXPORT_STUB(__imp__XUsbcamSetConfig);
REX_EXPORT_STUB(__imp__XUsbcamReadFrame);
REX_EXPORT_STUB(__imp__XUsbcamSetView);
REX_EXPORT_STUB(__imp__XUsbcamDestroy);
