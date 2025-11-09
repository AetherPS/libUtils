# libUtils
A collection of utility libraries for PS4 development.

# Build Requirements
- PS4 Official SDK

# Build Steps
1. Ensure that the PS4 Official SDK is fully installed, including the Visual Studio integration components.
2. Clone the Repository and its submodules: 
    ```bash
    git clone --recurse-submodules https://github.com/AetherPS/libUtils
    ```
3. Build the solution using **Visual Studio**.

# Libraries
- **libUtils**: Utility functions for logging, file system operations, and more.
- **libSysInt**: System Interface Library for jailbreaking or loading PRX.
- **lib7zip**: 7-Zip archive decompression library.
- **libAppCtrl**: Utility for managing the state and retrieving metadata of apps by TitleId.
- **libDetour**: Detouring/Hooking library supporting 64/32 bit hooking and hooking calls with a hook management system.
