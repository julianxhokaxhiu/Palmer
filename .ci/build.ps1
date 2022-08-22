#*****************************************************************************#
#    Copyright (C) 2014 Aali132                                               #
#    Copyright (C) 2022 Julian Xhokaxhiu                                      #
#                                                                             #
#    This file is part of Palmer                                              #
#                                                                             #
#    Palmer is free software: you can redistribute it and\or modify           #
#    it under the terms of the GNU General Public License as published by     #
#    the Free Software Foundation, either version 3 of the License            #
#                                                                             #
#    Palmer is distributed in the hope that it will be useful,                #
#    but WITHOUT ANY WARRANTY; without even the implied warranty of           #
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            #
#    GNU General Public License for more details.                             #
#*****************************************************************************#

$env:_RELEASE_PATH = ".build"
if ($env:_BUILD_BRANCH -eq "refs/heads/master" -Or $env:_BUILD_BRANCH -eq "refs/tags/canary")
{
  $env:_IS_BUILD_CANARY = "true"
}
elseif ($env:_BUILD_BRANCH -like "refs/tags/*")
{
  $env:_BUILD_VERSION = $env:_BUILD_VERSION.Substring(0,$env:_BUILD_VERSION.LastIndexOf('.')) + ".0"
}
$env:_RELEASE_VERSION = "v${env:_BUILD_VERSION}"

$vcpkgRoot = "C:\vcpkg"
$vcpkgBaseline = [string](jq --arg baseline "builtin-baseline" -r '.[$baseline]' vcpkg.json)
$vcpkgOriginUrl = &"git" -C $vcpkgRoot remote get-url origin
$vcpkgBranchName = &"git" -C $vcpkgRoot branch --show-current

Write-Output "--------------------------------------------------"
Write-Output "BUILD CONFIGURATION: $env:_RELEASE_CONFIGURATION"
Write-Output "RELEASE VERSION: $env:_RELEASE_VERSION"
Write-Output "VCPKG ORIGIN: $vcpkgOriginUrl"
Write-Output "VCPKG BRANCH: $vcpkgBranchName"
Write-Output "VCPKG BASELINE: $vcpkgBaseline"
Write-Output "--------------------------------------------------"

Write-Host "##vso[task.setvariable variable=_BUILD_VERSION;]${env:_BUILD_VERSION}"
Write-Host "##vso[task.setvariable variable=_RELEASE_VERSION;]${env:_RELEASE_VERSION}"
Write-Host "##vso[task.setvariable variable=_IS_BUILD_CANARY;]${env:_IS_BUILD_CANARY}"

# Load vcvarsall environment for x86
$vcvarspath = &"${env:ProgramFiles(x86)}\Microsoft Visual Studio\Installer\vswhere.exe" -prerelease -latest -property InstallationPath
cmd.exe /c "call `"$vcvarspath\VC\Auxiliary\Build\vcvarsall.bat`" x86 && set > %temp%\vcvars.txt"
Get-Content "$env:temp\vcvars.txt" | Foreach-Object {
  if ($_ -match "^(.*?)=(.*)$") {
    Set-Content "env:\$($matches[1])" $matches[2]
  }
}

git -C $vcpkgRoot pull --all
git -C $vcpkgRoot checkout $vcpkgBaseline
git -C $vcpkgRoot clean -fxd

cmd.exe /c "call $vcpkgRoot\bootstrap-vcpkg.bat"

mkdir "C:\vcpkg\downloads\tools\yasm\1.3.0.6" | Out-Null
Invoke-WebRequest -Uri "http://www.tortall.net/projects/yasm/snapshots/v1.3.0.6.g1962/yasm-1.3.0.6.g1962.exe" -SkipCertificateCheck -OutFile "C:\vcpkg\downloads\tools\yasm\1.3.0.6\yasm.exe"

vcpkg integrate install

mkdir $env:_RELEASE_PATH | Out-Null
cmake -G "Visual Studio 17 2022" -T host=x86 -A Win32 -D_EXE_VERSION="$env:_BUILD_VERSION" -DCMAKE_BUILD_TYPE="$env:_RELEASE_CONFIGURATION" -DCMAKE_TOOLCHAIN_FILE="$vcpkgRoot\scripts\buildsystems\vcpkg.cmake" -S . -B $env:_RELEASE_PATH
cmake --build $env:_RELEASE_PATH --config $env:_RELEASE_CONFIGURATION

mkdir .dist\pkg\Palmer | Out-Null
Copy-Item -R "$env:_RELEASE_PATH\bin\*" .dist\pkg\Palmer

7z a ".\.dist\${env:_RELEASE_NAME}-${env:_RELEASE_VERSION}.zip" ".\.dist\pkg\Palmer\*"
