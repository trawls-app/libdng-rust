#include <dng_host.h>
#include <dng_xmp_sdk.h>

extern "C" void dummy() {
  dng_xmp_sdk::InitializeSDK();
  dng_host host;
}
