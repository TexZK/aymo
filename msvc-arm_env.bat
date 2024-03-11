rem Run this script to setup environment for the MSVC ARM compiler
"D:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsamd64_arm.bat"
cd "D:\Documenti\GitHub\aymo"
meson setup vs --backend vs --cross-file msvc-arm.txt
