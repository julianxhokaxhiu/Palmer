#*****************************************************************************#
#    Copyright (C) 2014 Aali132                                               #
#    Copyright (C) 2020 Julian Xhokaxhiu                                      #
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

$env:_RELEASE_PATH = ".dist\build\x86-${env:_RELEASE_CONFIGURATION}"
if ($env:_BUILD_BRANCH -eq "refs/heads/master" -Or $env:_BUILD_BRANCH -eq "refs/tags/canary")
{
  $env:_IS_BUILD_CANARY = "true"
}
elseif ($env:_BUILD_BRANCH -like "refs/tags/*")
{
  $env:_BUILD_VERSION = $env:_BUILD_VERSION.Substring(0,$env:_BUILD_VERSION.LastIndexOf('.')) + ".0"
}
$env:_RELEASE_VERSION = "v${env:_BUILD_VERSION}"

Write-Output "--------------------------------------------------"
Write-Output "BUILD CONFIGURATION: $env:_RELEASE_CONFIGURATION"
Write-Output "RELEASE VERSION: $env:_RELEASE_VERSION"
Write-Output "--------------------------------------------------"

Write-Host "##vso[task.setvariable variable=_BUILD_VERSION;]${env:_BUILD_VERSION}"
Write-Host "##vso[task.setvariable variable=_RELEASE_VERSION;]${env:_RELEASE_VERSION}"
Write-Host "##vso[task.setvariable variable=_IS_BUILD_CANARY;]${env:_IS_BUILD_CANARY}"

mkdir $env:_RELEASE_PATH | Out-Null
cmake -G "Visual Studio 16 2019" -A Win32 -D_EXE_VERSION="$env:_BUILD_VERSION" -DCMAKE_BINARY_DIR="$env:_RELEASE_PATH" -S . -B $env:_RELEASE_PATH
cmake --build $env:_RELEASE_PATH --config $env:_RELEASE_CONFIGURATION

mkdir .dist\pkg\Palmer | Out-Null
Copy-Item -R "$env:_RELEASE_PATH\bin\*" .dist\pkg\Palmer

7z a ".\.dist\${env:_RELEASE_NAME}-${env:_RELEASE_VERSION}.zip" ".\.dist\pkg\Palmer\*"
