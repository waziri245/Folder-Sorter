@echo off
set OUTDIR=..\shared

if not exist %OUTDIR% (
    mkdir %OUTDIR%
)

gcc -shared -o %OUTDIR%\folder_sorter.dll folder_sorter.c
echo Done! DLL created in %OUTDIR%
