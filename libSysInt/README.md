# libSysInt - System Interface Library

A C++ library for system integration functionality, targeting C++14 standard. This library provides a unified interface for system-level operations, with support for both Fusion Driver and GoldHEN backends.

## Requirements

- PlayStation®4 SDK v9.00
- Microsoft Visual Studio (or Visual Studio Build Tools) with C++ workload installed

## API Reference

The library provides several key functions for system-level operations:

### Jailbreak Management

#### `bool Jailbreak(uint64_t authId, bool nullSandboxPath)`
Attempts to jailbreak the current process using either Fusion Driver (preferred) or GoldHEN.

**NOTE:** This function only supports setting the AuthId or NullSandboxPath if using the Fusion Driver.

Parameters:
- `authId`: The authorization ID to use for the jailbreak
- `nullSandboxPath`: Whether to null the sandbox path

Returns:
- `true` if jailbreak was successful
- `false` if jailbreak failed

#### `bool RestoreJail()`
Restores the jail state using the same backend that was used for jailbreaking.

Returns:
- `true` if jail was successfully restored
- `false` if restoration failed or no backup was stored

### SPRX Management

#### `int LoadSprx(int processId, std::string libraryPath)`
Loads a SPRX library into a specified process.

Parameters:
- `processId`: The ID of the target process
- `libraryPath`: Path to the SPRX library to load

Returns:
- Library handle if successful
- Negative value if loading failed

#### `bool UnloadSprx(int processId, int libraryHandle)`
Unloads a previously loaded SPRX library.

Parameters:
- `processId`: The ID of the target process
- `libraryHandle`: Handle of the library to unload (obtained from LoadSprx)

Returns:
- `true` if unload was successful
- `false` if unload failed

## Backend Selection

The library automatically selects the appropriate backend:
1. If Fusion Driver is available, it will be used as the preferred backend
2. If Fusion Driver is not available, GoldHEN will be used as a fallback

## Error Handling

All functions provide boolean or integer return values to indicate success or failure. It's recommended to check these return values to ensure operations completed successfully.

## Thread Safety

The library maintains internal state for jailbreak backups. Care should be taken when using these functions across multiple threads.

## License

TODO: Add license information here.