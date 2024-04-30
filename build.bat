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

set "should_time=0"
set "opt_options=/O2 /Oi /fp:fast /Zo /Z7"
if "%3"=="debug" (
	set "opt_options=/Zo /Z7 /Od /Oi /fp:fast"
) else if "%3"=="time" (
  set "should_time=1"
) else if "%3" neq "" (
	echo Third argument must be either 'debug', 'time' or omitted
	goto end
)

if "%4" neq "" (
	echo Invalid number of arguments
	goto end
)

set chosen_compiler=cl
if %day_num% == 12 set chosen_compiler=clang
if %day_num% geq 17 set chosen_compiler=clang
if %day_num% geq 19 set chosen_compiler=cl

if %chosen_compiler% == "cl" (
  cl /nologo %opt_options% /W3 day%day_num%.c /link /subsystem:console /opt:icf /opt:ref /incremental:no /pdb:day%day_num%.pdb /out:day%day_num%.exe
) else (
  set "warnings=-Wall -Wextra -Wshadow -Wconversion -Wnull-dereference -Wdouble-promotion -Wformat=2"

  set "ignored_warnings=-Wno-unused-parameter"

  REM set "debug= -Og -fuse-ld=lld.exe -g -gcodeview -Wl,/debug,/pdb:day%day_num%.pdb"

  REM clang -target x86_64-pc-windows-msvc %debug% %warnings% %ignored_warnings% -std=gnu11 -o day%day_num%.exe day%day_num%.c
  
  REM -lclang_rt.builtins-x86_64.lib
  clang-cl %opt_options% %warnings% %ignored_warnings% day%day_num%.c /link /subsystem:console /opt:icf /opt:ref /incremental:no /pdb:day%day_num%.pdb /out:day%day_num%.exe
)

if %errorlevel% neq 0 goto end

if %should_time%==1 (
  rtime day%day_num%.exe %input_file%
) else (
  day%day_num%.exe %input_file%
)

:end
endlocal



:end
endlocal
