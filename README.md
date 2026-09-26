# Palmer

FF7 Field Background Editor

Initial source code release by Aali132.

## Tech Stack
If you're curious to know it, Palmer is made with:
- C++ code base
- Latest MSVC available on [Visual Studio 2026 Community Edition](https://visualstudio.microsoft.com/vs/features/cplusplus/)
- [vcpkg](https://vcpkg.io/) (dependency manager)
- [CMake](https://cmake.org/)
- [libpng](http://www.libpng.org/pub/png/libpng.html)
- [glew](http://glew.sourceforge.net/)
- [FreeGLUT](http://freeglut.sourceforge.net/)

## How to build

Available build profiles:

- x86-Release (default, the same used to release artifacts in this Github page)
- x86-RelWithDebInfo (used while developing to better debug some issues)
- x86-MinSizeRel
- x86-Debug (prefer it if you want to use a debugger attached to the game)

Once the project is built you can find the output in this path: `.build/bin`

### Preparation

> **Please note:**
>
> Palmer uses vcpkg as a package manager to resolve dependencies. Failing to follow these steps will result in build errors.

0. Clone this repository using the `--recursive` flag, eg. `git clone --recursive https://github.com/julianxhokaxhiu/Palmer.git`

### NuGet

> **Please note:**
>
> This step will speed up your compilation times by avoiding the vcpkg dependencies rebuild.

0. Make sure you have [NuGet CLI installed](https://learn.microsoft.com/en-us/nuget/install-nuget-client-tools?tabs=windows#install-nugetexe).
1. [Create a Personal Access token ( classic )](https://github.com/settings/tokens/new) with the `read:packages` permission.
  This setup is read-only and is only used to consume prebuilt packages.
2. Open a `cmd` window and run the following commands ( replace `YOUR_GITHUB_USERNAME` and `YOUR_GITHUB_PAT` accordingly ):
```pwsh
$ nuget sources add -Name github -Source "https://nuget.pkg.github.com/julianxhokaxhiu/index.json" -Username YOUR_GITHUB_USERNAME -Password YOUR_GITHUB_PAT -StorePasswordInClearText
$ nuget setApiKey YOUR_GITHUB_PAT -Source "https://nuget.pkg.github.com/julianxhokaxhiu/index.json"
$ [Environment]::SetEnvironmentVariable("VCPKG_BINARY_SOURCES", "clear;nuget,github,read;default,readwrite", "User")
```

### Visual Studio

> **Please note:**
>
> By default Visual Studio will pick the **x86-Release** build configuration, but you can choose any other profile available.
> Palmer uses vcpkg as a package manager to resolve dependencies. Failing to follow these steps will result in build errors.

0. Download the the latest [Visual Studio Community](https://visualstudio.microsoft.com/vs/community/) installer
1. Run the installer and import this [.vsconfig](.vsconfig) file in the installer to pick the components required to build this project
2. Make sure you select the English Language pack in the language list before clicking Install
3. Go inside the [`vcpkg`](./vcpkg) folder and double click `bootstrap-vcpkg.bat`
4. Open a `cmd` window in [`vcpkg`](./vcpkg) and run the following command: `vcpkg integrate install`
5. Once installed, open this repository **as a folder** in Visual Studio
6. Choose as preset in the status bar the one you desire
7. Click the `Build` button

### Visual Studio Code

0. **REQUIRED!** Follow up the steps to install Visual Studio, which will also install the MSVC toolchain
1. Download and install the latest [Visual Studio Code](https://code.visualstudio.com/) release
2. Install the following extensions:
   - https://marketplace.visualstudio.com/items?itemName=ms-vscode.cpptools
   - https://marketplace.visualstudio.com/items?itemName=ms-vscode.cmake-tools
3. Open this repository as a folder in Visual Studio Code
4. Choose as preset in the status bar the one you desire
5. Click the button on the status bar `Build`

## Auto-Formatting

### CMake Files

0. **REQUIRED!** Install [Python](https://www.python.org/)
1. Install [cmake-format](https://github.com/cheshirekow/cmake_format#installation) and make sure the binary is available in your PATH environment variable
2. **OPTIONAL!** Integrate it [in your own IDE](https://github.com/cheshirekow/cmake_format#integrations) ( eg. for Visual Studio Code use [the relative extension](https://marketplace.visualstudio.com/items?itemName=cheshirekow.cmake-format) )

## License

Palmer is released under GPLv3 license, and you can get a copy of the license here: [COPYING.txt](COPYING.txt)

If you paid for it, remember to ask for a refund to the person who sold you a copy. Make also sure you get a copy of the source code if you got it as a binary only.

If the person who gave you a copy will refuse to give you the source code, report it here: https://www.gnu.org/licenses/gpl-violation.html

All rights belong to their respective owners.
