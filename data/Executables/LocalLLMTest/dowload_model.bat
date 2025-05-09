@echo off

:: Uncomment and set the model you want to download
::set MODEL_NAME=microsoft/Phi-3.5-mini-instruct-onnx
::set MODEL_DETAILS=cpu_and_mobile/cpu-int4-awq-block-128-acc-level-4

set MODELS_PATH=.\Models
if not exist "%MODELS_PATH%" (
	mkdir "%MODELS_PATH%"
	echo Models folder created
)

if defined MODEL_NAME if defined MODEL_DETAILS (
	echo Downloading model %MODEL_NAME% %MODEL_DETAILS% ...
	huggingface-cli download %MODEL_NAME% --include %MODEL_DETAILS%/* --local-dir %MODELS_PATH%
)

pause
