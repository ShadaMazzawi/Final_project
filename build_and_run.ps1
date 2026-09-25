param(
    [ValidateSet("Release", "Debug", "RelWithDebInfo")]
    [string]$Configuration = "Release"
)

$ErrorActionPreference = "Stop"
$buildDir = Join-Path $PSScriptRoot "build\$Configuration"

$vsPath = & "${env:ProgramFiles(x86)}\Microsoft Visual Studio\Installer\vswhere.exe" `
    -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath
if (-not $vsPath) {
    Write-Error "Could not find a Visual Studio installation with the C++ toolset."
    exit 1
}
$vcvars = Join-Path $vsPath "VC\Auxiliary\Build\vcvarsall.bat"

Write-Host "Configuring ($Configuration)..."
cmd /c "`"$vcvars`" x64 && cmake -G Ninja -DCMAKE_BUILD_TYPE=$Configuration -B `"$buildDir`" -S `"$PSScriptRoot`""
if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }

Write-Host "Building..."
cmd /c "`"$vcvars`" x64 && ninja -C `"$buildDir`""
if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }

& "$buildDir\perlin_noise.exe"