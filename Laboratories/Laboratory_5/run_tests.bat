@echo off
echo =======================================================================
echo   FLCD Laboratory 5 - Testing Scanner with Integrated Finite Automata
echo =======================================================================

echo.
echo [1/3] Running lexical analysis on program1.cloud.txt...
py scanner_fa.py program1.cloud.txt PIF_Program1.out ST_Program1.out

echo.
echo [2/3] Running lexical analysis on program2.cloud.txt...
py scanner_fa.py program2.cloud.txt PIF_Program2.out ST_Program2.out

echo.
echo [3/3] Running lexical analysis on program3_errors.cloud.txt (Error diagnostics)...
py scanner_fa.py program3_errors.cloud.txt PIF_Program3_Errors.out ST_Program3_Errors.out

echo.
echo =======================================================================
echo   Tests completed! Outputs saved to PIF_*.out and ST_*.out
echo =======================================================================
pause
