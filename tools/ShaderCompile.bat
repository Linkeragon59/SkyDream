@echo off

set CODE_PATH=%~dp0..\code
set GLSLC_EXE=%CODE_PATH%\Extern\VulkanSDK\1.3.275.0\bin\win64\glslc.exe

set DATA_PATH=%~dp0..\data
set SHADERS_PATH=%DATA_PATH%\%1

echo Compiling Shaders in path : %SHADERS_PATH%

echo ----------------------
echo --- Vertex Shaders ---
echo ----------------------
for %%f in (%SHADERS_PATH%\*.vert) do (
	echo %%~nf
	%GLSLC_EXE% %SHADERS_PATH%\%%~nf.vert -o %SHADERS_PATH%\%%~nf_vert.spv
)

echo ------------------------
echo --- Fragment Shaders ---
echo ------------------------
for %%f in (%SHADERS_PATH%\*.frag) do (
	echo %%~nf
	%GLSLC_EXE% %SHADERS_PATH%\%%~nf.frag -o %SHADERS_PATH%\%%~nf_frag.spv
)

pause