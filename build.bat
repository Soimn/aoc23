@echo off

setlocal
cd %~dp0

if "%1"=="" (
	echo Missing day number
	goto end
)
set day_num=%1

if not exist %day_num% (
	echo day %1 folder missing
	goto end
)
cd %day_num%

if "%2"=="" (
	echo Missing input file
	goto end
) else (
	set "input_file=%~n2.txt"
)

set "opt_options=/O2 /Oi /fp:fast /Zo /Z7"
if "%3"=="debug" (
	set "opt_options=/Zo /Z7 /Od /Oi /fp:fast"
) else if "%3" neq "" (
	echo Third argument must be either 'debug' or omitted
	goto end
)

if "%4" neq "" (
	echo Invalid number of arguments
	goto end
)

cl /nologo %opt_options% /W3 day%day_num%.c /link /subsystem:console /opt:icf /opt:ref /incremental:no /pdb:day%day_num%.pdb /out:day%day_num%.exe
if %errorlevel% neq 0 goto end

day%day_num%.exe %input_file%
if %errorlevel% neq 0 echo process returned %errorlevel%

:end
endlocal
