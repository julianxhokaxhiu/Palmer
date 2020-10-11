# Palmer

FF7 Field Background Editor

Initial source code release by Aali132.

## Tech Stack
If you're curious to know it, Palmer is made with:
- C++ code base
- Latest MSVC available on [Visual Studio 2019 Community Edition](https://visualstudio.microsoft.com/vs/features/cplusplus/)
- [CMake](https://cmake.org/)
- [libpng](http://www.libpng.org/pub/png/libpng.html)
- [glew](http://glew.sourceforge.net/)
- [FreeGLUT](http://freeglut.sourceforge.net/)

## How to build

Tested build profiles:
- x86-Release ( default, the same used to release artifacts in this Github page )
- x86-RelWithDebInfo ( used while developing to better debug some issues )

Output folder: `.dist/build/[CHOSEN_PROFILE]/bin` ( eg. `.dist/build/x86-Release/bin` )

### Visual Studio

> **Please note:**
>
> By default Visual Studio will pick the **x86-Release** build configuration, but you can choose any other profile available.

0) Download the the latest [Visual Studio Community](https://visualstudio.microsoft.com/vs/community/) installer
1) Run the installer and import this [.vsconfig](.vsconfig) file in the installer to pick the required components to build this project
2) Once installed, open this repository **as a folder** in Visual Studio 2019 and click the build button.

### Visual Studio Code

0) **REQUIRED!** Follow up the steps to install Visual Studio, which will also install the MSVC toolchain
1) Download and install the latest [Visual Studio Code](https://code.visualstudio.com/) release
2) Install the following extensions:
   - https://marketplace.visualstudio.com/items?itemName=ms-vscode.cpptools
   - https://marketplace.visualstudio.com/items?itemName=ms-vscode.cmake-tools
3) Open this repository as a folder in Visual Studio code
4) Choose as build profile in the status bar `CMake: [Release]` ( or one of the aforementioned profiles )
5) Click the button on the status bar `Build`

## License

Palmer is released under GPLv3 license, and you can get a copy of the license here: [COPYING.txt](COPYING.txt)

If you paid for it, remember to ask for a refund to the person who sold you a copy. Make also sure you get a copy of the source code if you got it as a binary only.

If the person who gave you a copy will refuse to give you the source code, report it here: https://www.gnu.org/licenses/gpl-violation.html

All rights belong to their respective owners.
