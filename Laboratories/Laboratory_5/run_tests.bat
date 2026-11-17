@echo off
echo =======================================================================
echo   FLCD Laboratory 5 - Testing Scanner with Integrated Finite Automata (C)
echo =======================================================================

if not exist output mkdir output
if not exist bin mkdir bin

echo.
echo Compiling C Scanner with GCC...
gcc -Wall -Wextra -O2 -I src -o bin\scanner.exe src\scanner.c src\fa.c src\st.c
if errorlevel 1 (
    echo Compilation failed!
    pause
    exit /b 1
)
echo Compilation successful: bin\scanner.exe generated.

echo.
echo [1/3] Running lexical analysis on program1.cloud.txt...
bin\scanner.exe input\program1.cloud.txt output\PIF_Program1.out output\ST_Program1.out

echo.
echo [2/3] Running lexical analysis on program2.cloud.txt...
bin\scanner.exe input\program2.cloud.txt output\PIF_Program2.out output\ST_Program2.out

echo.
echo [3/3] Running lexical analysis on program3_errors.cloud.txt (Error diagnostics)...
bin\scanner.exe input\program3_errors.cloud.txt output\PIF_Program3_Errors.out output\ST_Program3_Errors.out

echo.
echo =======================================================================
echo   Tests completed! Outputs saved to output\ folder
echo =======================================================================
pause
