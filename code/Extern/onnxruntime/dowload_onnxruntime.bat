@echo off

:: Uncomment and set the versions you want to download
set ONNXRUNTIME_VERSION=1.21.0
set ONNXRUNTIME_GENAI_VERSION=0.7.1

set TMP_PATH=.\tmp
if not exist "%TMP_PATH%" (
	mkdir "%TMP_PATH%"
	echo tmp folder created
)

if defined ONNXRUNTIME_VERSION (
	echo Downloading OnnxRuntime, version %ONNXRUNTIME_VERSION% ...
	curl -L https://github.com/microsoft/onnxruntime/releases/download/v%ONNXRUNTIME_VERSION%/onnxruntime-win-x64-%ONNXRUNTIME_VERSION%.zip -o %TMP_PATH%\onnxruntime-win-x64-%ONNXRUNTIME_VERSION%.zip
	tar -xvf %TMP_PATH%\onnxruntime-win-x64-%ONNXRUNTIME_VERSION%.zip
) else (
	echo Skipping OnnxRuntime download.
)

if defined ONNXRUNTIME_GENAI_VERSION (
	echo Downloading OnnxRuntime_GenAI, version %ONNXRUNTIME_GENAI_VERSION% ...
	curl -L https://github.com/microsoft/onnxruntime-genai/releases/download/v%ONNXRUNTIME_GENAI_VERSION%/onnxruntime-genai-%ONNXRUNTIME_GENAI_VERSION%-win-x64.zip -o %TMP_PATH%\onnxruntime-genai-%ONNXRUNTIME_GENAI_VERSION%-win-x64.zip
	tar -xvf %TMP_PATH%\onnxruntime-genai-%ONNXRUNTIME_GENAI_VERSION%-win-x64.zip
) else (
	echo Skipping OnnxRuntime_GenAI download.
)

pause
