@echo off
REM ===================================================================
REM  Laboratory 6 - Build and Test Script
REM  Cloud Infrastructure DSL - Yacc Parser + Lex Integration + PIF
REM ===================================================================
setlocal enabledelayedexpansion

cd /d "%~dp0"

REM --- Locate win_flex / win_bison (bundled tools\ dir, or PATH) -----
set "BISON=bison"
set "FLEX=flex"
if exist "tools\win_bison.exe" (
    set "BISON=tools\win_bison.exe"
    set "FLEX=tools\win_flex.exe"
)

echo ===================================================================
echo  STEP 1: Compile parser with Bison (LALR(1))
echo ===================================================================
%BISON% -d -v parser.y
if %ERRORLEVEL% NEQ 0 (
    echo [FAIL] Bison failed to compile parser.y
    exit /b 1
)
echo [OK] Bison generated parser.tab.c and parser.tab.h

echo.
echo --- Conflict Verification (parser.output) ---
findstr /C:"shift/reduce" parser.output >nul 2>&1
if %ERRORLEVEL% EQU 0 (
    echo [WARNING] shift/reduce conflicts detected:
    findstr /C:"shift/reduce" parser.output
) else (
    echo [PASS] 0 shift/reduce conflicts
)
findstr /C:"reduce/reduce" parser.output >nul 2>&1
if %ERRORLEVEL% EQU 0 (
    echo [WARNING] reduce/reduce conflicts detected:
    findstr /C:"reduce/reduce" parser.output
) else (
    echo [PASS] 0 reduce/reduce conflicts
)

echo.
echo ===================================================================
echo  STEP 2: Compile scanner with Flex
echo ===================================================================
%FLEX% scanner.lxi
if %ERRORLEVEL% NEQ 0 (
    echo [FAIL] Flex failed to compile scanner.lxi
    exit /b 1
)
echo [OK] Flex generated lex.yy.c

echo.
echo ===================================================================
echo  STEP 3: Compile with GCC
echo ===================================================================
gcc -O2 -Wall -o cloud_parser.exe parser.tab.c lex.yy.c st.c 2>gcc_errors.tmp
if %ERRORLEVEL% NEQ 0 (
    echo [FAIL] GCC compilation failed:
    type gcc_errors.tmp
    del gcc_errors.tmp 2>nul
    exit /b 1
)
del gcc_errors.tmp 2>nul
echo [OK] Successfully built cloud_parser.exe

echo.
echo ===================================================================
echo  STEP 4: Run Test 1 - program1.cloud.txt
echo ===================================================================

echo [Test 1A - Lex+Yacc mode]
cloud_parser.exe program1.cloud.txt productions_program1.out PIF_Program1.out ST_Program1.out
if %ERRORLEVEL% EQU 0 (
    echo [PASS] program1.cloud.txt parsed successfully
) else (
    echo [FAIL] program1.cloud.txt failed
)

echo.
echo [Test 1B - PIF mode]
cloud_parser.exe --pif PIF_Program1.out productions_program1_pif.out
if %ERRORLEVEL% EQU 0 (
    echo [PASS] PIF_Program1.out parsed successfully in PIF mode
) else (
    echo [FAIL] PIF_Program1.out failed
)

echo.
echo [Test 1C - Equivalence Verification]
fc /b productions_program1.out productions_program1_pif.out >nul
if %ERRORLEVEL% EQU 0 (
    echo [PASS] Output productions match identically between Lex mode and PIF mode!
) else (
    echo [FAIL] Output productions differ between Lex and PIF modes.
)

echo.
echo ===================================================================
echo  STEP 5: Run Test 2 - program2.cloud.txt
echo ===================================================================

echo [Test 2A - Lex+Yacc mode]
cloud_parser.exe program2.cloud.txt productions_program2.out PIF_Program2.out ST_Program2.out
if %ERRORLEVEL% EQU 0 (
    echo [PASS] program2.cloud.txt parsed successfully
) else (
    echo [FAIL] program2.cloud.txt failed
)

echo.
echo [Test 2B - PIF mode]
cloud_parser.exe --pif PIF_Program2.out productions_program2_pif.out
if %ERRORLEVEL% EQU 0 (
    echo [PASS] PIF_Program2.out parsed successfully in PIF mode
) else (
    echo [FAIL] PIF_Program2.out failed
)

echo.
echo [Test 2C - Equivalence Verification]
fc /b productions_program2.out productions_program2_pif.out >nul
if %ERRORLEVEL% EQU 0 (
    echo [PASS] Output productions match identically between Lex mode and PIF mode!
) else (
    echo [FAIL] Output productions differ between Lex and PIF modes.
)

echo.
echo ===================================================================
echo  STEP 6: Run Test 3 - program3_errors.cloud.txt (Intentional Errors)
echo ===================================================================
cloud_parser.exe program3_errors.cloud.txt productions_program3.out PIF_Program3_Errors.out ST_Program3_Errors.out
if %ERRORLEVEL% NEQ 0 (
    echo [PASS - EXPECTED] program3_errors.cloud.txt was correctly rejected with syntax/lexical errors.
) else (
    echo [UNEXPECTED] program3_errors.cloud.txt parsed without error.
)

echo.
echo ===================================================================
echo  Summary of Generated Artifacts:
echo    - parser.tab.c, parser.tab.h, parser.output
echo    - lex.yy.c, cloud_parser.exe
echo    - productions_program1.out, productions_program1_pif.out
echo    - productions_program2.out, productions_program2_pif.out
echo    - productions_program3.out
echo    - PIF_Program1.out, ST_Program1.out
echo    - PIF_Program2.out, ST_Program2.out
echo ===================================================================
