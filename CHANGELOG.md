# Version 1.1
## Bugfixes
- Added missing include to stddef.h
## Tooling Changes
- Renamed the CMake project to 'rlh'
- Changed the project's build system so it is no longer monolithic
	- Removed vcpkg.json
	- Removed vcpkg toolchain file setting from CMakeLists.txt
	- Created an interface target the library called 'rlh'
- Added code linting using trunk.
# Version 1.0
- initial release
	

