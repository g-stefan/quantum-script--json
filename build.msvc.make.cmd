@echo off
rem Public domain
rem http://unlicense.org/
rem Created by Grigore Stefan <g_stefan@yahoo.com>

set ACTION=%1
if "%1" == "" set ACTION=make

echo -^> %ACTION% quantum-script-extension-json

goto StepX
:cmdX
%*
if errorlevel 1 goto cmdXError
goto :eof
:cmdXError
echo "Error: %ACTION%"
exit 1
:StepX

call :cmdX file-to-cs --touch=source/quantum-script-extension-json.cpp --file-in=source/quantum-script-extension-json.js --file-out=source/quantum-script-extension-json.src --is-string --name=extensionJSONSource
call :cmdX xyo-cc --mode=%ACTION% @util/quantum-script-extension-json.static.compile.info
call :cmdX xyo-cc --mode=%ACTION% @util/quantum-script-extension-json.dynamic.compile.info
