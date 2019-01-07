@echo off
:start
elara %1 %2 %3 %4 %5 %6 %7 %8 %9
if errorlevel 1 goto start
if errorlevel 0 goto end
goto start
:end

