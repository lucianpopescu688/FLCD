# FLCD - Laboratory 6: Syntactical Analysis (Yacc/Bison Parser + Lex Integration + PIF Input)

## 1. Overview
This laboratory implements the **Syntactical Analyzer (Parser)** for the domain-specific **Cloud Infrastructure Programming Language (mini-DSL)** using **Yacc/Bison** (LALR(1)) and **Flex**, fully integrated with the Lexical Analyzer, Hash Table Symbol Table ($m = 101$, open addressing with linear probing), and Program Internal Form (PIF) from Laboratory 3.

The parser satisfies all course requirements:
1. **Grammar Specification (Requirement 1):** The abstract BNF from `CloudProgrammingLanguageBNF_v2.txt` has been converted into a strict, conflict-free LALR(1) BNF with **0 shift/reduce** and **0 reduce/reduce** conflicts. All 107 productions are sequentially numbered ($1 \dots 107$). Full specification in [`Lab6_Grammar_Documentation.md`](./Lab6_Grammar_Documentation.md).
2. **PIF Mode Parsing (Requirement 2):** When given an existing PIF file (`--pif <pif_file>`), the parser reads token sequences directly from the PIF table and produces the derivation string of productions without re-scanning source code.
3. **Lex + Yacc Integration (Requirement 3):** When given DSL source code (`.cloud.txt`), Flex (`yylex()`) tokenizes the input, populates the Symbol Table and PIF, and feeds token codes to `yyparse()`.
4. **Dual-Mode Deterministic Equivalence:** Parsing via source code (Lex+Yacc) versus parsing via generated PIF produces **100% byte-identical** production sequences.

---

## 2. Directory Structure

```
Laboratories/Laboratory_6/
├── parser.y                      # Bison LALR(1) grammar, semantic actions, dual-mode driver & main()
├── scanner.lxi                   # Flex lexical analyzer returning tokens & generating PIF/ST
├── st.h / st.c                   # Symbol Table (Hash Table, m=101) and PIF structures & operations
├── Lab6_Grammar_Documentation.md # Formal grammar specification with all 107 numbered productions
├── Lab6_2025.docx                # Official laboratory assignment requirements
├── run_tests.bat                 # Automated build, conflict verification, and test execution script
├── program1.cloud.txt            # Test program 1 (complex cluster deployment DSL script)
├── program2.cloud.txt            # Test program 2 (fleet health & maintenance DSL script)
├── program3_errors.cloud.txt     # Test program 3 (deliberate lexical and syntax errors)
├── parser.tab.c / parser.tab.h   # Bison-generated C parser and token header
├── parser.output                 # Bison LALR(1) state transition table & conflict report (0 conflicts)
├── lex.yy.c                      # Flex-generated lexical analyzer
├── PIF_Program1.out              # Generated PIF for Program 1 (537 tokens)
├── ST_Program1.out               # Generated Symbol Table for Program 1 (54 symbols)
├── productions_program1.out      # Sequence of production indices for Program 1 (Lex mode)
├── productions_program1_pif.out  # Sequence of production indices for Program 1 (PIF mode)
├── PIF_Program2.out              # Generated PIF for Program 2 (335 tokens)
├── ST_Program2.out               # Generated Symbol Table for Program 2 (41 symbols)
├── productions_program2.out      # Sequence of production indices for Program 2 (Lex mode)
├── productions_program2_pif.out  # Sequence of production indices for Program 2 (PIF mode)
├── PIF_Program3_Errors.out       # Partial PIF before syntax error in Program 3
└── productions_program3.out      # Partial production string before syntax failure in Program 3
```

---

## 3. How to Run

### Method A: Automated Execution (Windows Batch Script)
Run the automated test runner directly from the terminal or by double-clicking:
```cmd
cd Laboratories\Laboratory_6
run_tests.bat
```
This script automatically:
1. Compiles `parser.y` with `bison -d -v`.
2. Inspects `parser.output` and asserts **0 shift/reduce** and **0 reduce/reduce** conflicts.
3. Compiles `scanner.lxi` with `flex`.
4. Compiles with `gcc` to produce `cloud_parser.exe`.
5. Executes Test 1 (`program1.cloud.txt`) in Lex mode, generates `PIF_Program1.out` and `ST_Program1.out`, and outputs `productions_program1.out`.
6. Executes Test 1 in PIF mode (`--pif PIF_Program1.out`) and outputs `productions_program1_pif.out`.
7. Performs binary comparison (`fc /b`) proving Lex mode and PIF mode outputs are **identical**.
8. Repeats the same verification for Test 2 (`program2.cloud.txt`).
9. Executes Test 3 (`program3_errors.cloud.txt`), confirming proper rejection and syntax error reporting.

---

### Method B: Manual Step-by-Step Commands

#### 1. Compile with Bison, Flex, and GCC:
```bash
cd Laboratories/Laboratory_6

# Generate parser C code and header (plus verbose LALR state output)
bison -d -v parser.y

# Generate scanner C code
flex scanner.lxi

# Compile executable
gcc -O2 -Wall -o cloud_parser.exe parser.tab.c lex.yy.c st.c
```
*(On Linux/macOS, use `-o cloud_parser` instead of `.exe`)*.

#### 2. Run in Lex + Yacc Mode (Source Code Input):
```bash
# Syntax: cloud_parser <source_file> <productions_out> <pif_out> <st_out>
./cloud_parser.exe program1.cloud.txt productions_program1.out PIF_Program1.out ST_Program1.out
./cloud_parser.exe program2.cloud.txt productions_program2.out PIF_Program2.out ST_Program2.out
```

#### 3. Run in PIF Mode (PIF Table Input):
```bash
# Syntax: cloud_parser --pif <pif_file> <productions_out>
./cloud_parser.exe --pif PIF_Program1.out productions_program1_pif.out
./cloud_parser.exe --pif PIF_Program2.out productions_program2_pif.out
```

#### 4. Test Syntax Error Detection:
```bash
./cloud_parser.exe program3_errors.cloud.txt productions_program3.out
# Expected output: Lexical error / syntax error reported on line 2 with nonzero exit code
```

---

## 4. Conflict Resolution in LALR(1) Grammar

The grammar achieves **0 conflicts** through two design solutions:
1. **Disambiguating `process` and `group`:**
   In the raw BNF, `process` and `group` were listed both as general types (`<simple_type>`) and as special declaration keywords (`process x { depends_on: ... };`, `group g = [...];`). This caused shift/reduce conflicts. By giving `process` and `group` explicit productions within `declaration_statement` (rules 18–24) and keeping `simple_type` strictly scalar (`int`, `string`, `boolean`, `state`, `ip_address`), declarations are parsed deterministically.
2. **Disambiguating Conditions and Expressions:**
   Both `condition` and `term` support parentheses (`( condition )` vs `( expression )`). If `boolean_constant` (`true`/`false`) were both a standalone condition and a term, a lookahead `')'` caused reduce/reduce conflicts. Restricting `condition` to relational comparisons, `not (...)`, and compound logic (`and`/`or`), while treating boolean constants as expressions (which can be compared: `cond == true`, or assigned: `flag = true`), resolves all reduce/reduce conflicts.

---

## 5. Summary of Test Results

| Test Program | Constructs Tested | Lex Mode Result | PIF Mode Result | Equivalence (`fc /b`) |
|---|---|---|---|---|
| `program1.cloud.txt` | `process`, `depends_on`, `group`, arrays, `try-catch`, `while`, `if-else`, `switch-case-default`, `for`, `not`, built-in calls | **PASS** (0 syntax errors) | **PASS** (0 syntax errors) | **Identical** (no differences) |
| `program2.cloud.txt` | Arithmetic expressions (`*`, `/`, `-`, `%`), nested `if-else`, compound `or`/`and`, array indexing, `for`, `while` | **PASS** (0 syntax errors) | **PASS** (0 syntax errors) | **Identical** (no differences) |
| `program3_errors.cloud.txt` | Malformed identifier `123invalid_id`, illegal operator `!` | **REJECTED** (syntax error at line 2) | N/A | Correctly caught |
