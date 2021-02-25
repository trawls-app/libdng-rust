To build the DNG SDK, you also need to download and install the Adobe XMP SDK. It is available from:

https://www.adobe.com/devnet/xmp.html

Rename the "XMP-Toolkit-SDK-<version>" folder to "xmp_sdk", and place it next to the dng_sdk folder (i.e. as a sibling of this read-me file).

Note that the current XMP SDK also requires you to download some additional source code.  See the ReadMe.txt files inside the XMP SDK's third-party folder for details.

This version of the DNG SDK requires libc++, and the XMP Toolkit SDK CC-2020.01 requires some modification.

The following changes are required to the XMP Toolkit SDK CC-2020.01:

1. File: xmp_sdk/build/XMP_Config.cmake
Line 30: set(INCLUDE_CPP_DOM_SOURCE FALSE)

2. File: xmp_sdk/XMPCore/build/CMakeListsCommon.txt
Line 120: if(WIN32 AND XMP_BUILD_STATIC)

3. After generating the XCode project files using CMake, you need to open the XCode projects and switch the "C++ standard Library" to "libc++".

4. After generating the VisualStudio projects files using CMake, change the runtime libraries from "MD" to "MT" for release targets, and "MDd" to "MTd" for debug targets.

After making these changes, open the project files and build the static library targets for both XMPCore and XMPFiles, both Release and Debug.
