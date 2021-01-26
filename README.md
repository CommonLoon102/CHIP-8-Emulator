# Summary
This is a CHIP-8 emulator written in C++ with a graphical interface (using wxWidgets). It supports sound and various features such as the ability to change the emulation speed, the colour scheme, and the sound effect. It also comes with some games/applications.

# What is CHIP-8?
CHIP-8 is an interpreted programming language, developed by Joseph Weisbecker. It was initially used on the COSMAC VIP and Telmac 1800 8-bit microcomputers in the mid-1970s. CHIP-8 programs are run on a CHIP-8 virtual machine. It was made to allow video games to be more easily programmed for these computers.

# Compiling and running
The emulator uses wxWidgets, SDL2 and SDL_mixer. I only supply visual studio solution files. The project settings were as follows:

C/C++ -- Additional Include Directories:
F:\SDKs\wxWidgets-3.1.3\include\msvc; F:\SDKs\wxWidgets-3.1.3\include; F:\SDKs\SDL2_mixer-2.0.4\include; F:\SDKs\SDL2-2.0.12\include

Linker -- Additional Library Directories:
F:\SDKs\wxWidgets-3.1.3\lib\vc_x64_lib; F:\SDKs\SDL2-2.0.12\lib\x64; F:\SDKs\SDL2_mixer-2.0.4\lib\x64

Linker -- Input -- Additional Dependencies:
SDL2.lib; SDL2main.lib; SDL2_mixer.lib
