# FLCD - Laboratory 5: Integrating Finite Automata (FA) into Scanner

## Overview
This laboratory assignment enhances the Lexical Analyzer (Scanner) for the domain-specific **Cloud Infrastructure Programming Language** by replacing pattern-based regex token recognition with explicit **Deterministic Finite Automata (DFA)** for:
1. **Identifiers** ($M_{id}$)
2. **Numeric Constants** ($M_{num}$)
3. **String Constants** ($M_{str}$)

The implementation preserves the Symbol Table (ST) based on a custom Hash Table ($m = 101$, linear probing) and Program Internal Form (PIF) from Laboratory 3.

---

## Formal Definitions of the Finite Automata

### 1. Identifier FA ($M_{id}$)
- **States:** $Q = \{q_0, q_1\}$
- **Alphabet:** $\Sigma = \{a-z, A-Z, 0-9, \_ \}$
- **Initial State:** $q_0$
- **Final States:** $F = \{q_1\}$
- **Transitions ($\delta$):**
  - $\delta(q_0, \text{letter}) = q_1$
  - $\delta(q_1, \text{letter} \mid \text{digit} \mid \text{underscore}) = q_1$

### 2. Numeric Constant FA ($M_{num}$)
- **States:** $Q = \{q_0, q_1, q_2, q_3\}$
- **Alphabet:** $\Sigma = \{0, 1-9, -\}$
- **Initial State:** $q_0$
- **Final States:** $F = \{q_1, q_3\}$
- **Transitions ($\delta$):**
  - $\delta(q_0, \text{'0'}) = q_1$ (matches exactly "0")
  - $\delta(q_0, \text{'-'}) = q_2$ (negative prefix)
  - $\delta(q_0, \text{nonzero\_digit}) = q_3$ (positive number)
  - $\delta(q_2, \text{nonzero\_digit}) = q_3$ (negative number)
  - $\delta(q_3, \text{digit}) = q_3$ (subsequent digits)

### 3. String Constant FA ($M_{str}$)
- **States:** $Q = \{q_0, q_1, q_2\}$
- **Alphabet:** $\Sigma = \{\text{'"'}, \text{valid\_chars}\}$
- **Initial State:** $q_0$
- **Final States:** $F = \{q_2\}$
- **Transitions ($\delta$):**
  - $\delta(q_0, \text{'"'}) = q_1$
  - $\delta(q_1, \text{valid\_char}) = q_1$
  - $\delta(q_1, \text{'"'}) = q_2$

---

## File Structure

```
Laboratory_5/
├── fa.h                         # C header: FA declarations and formal specs
├── fa.c                         # C implementation: state machine transition functions
├── st.h                         # C header: Symbol Table & PIF structures
├── st.c                         # C implementation: Hash Table (m=101) & PIF
├── scanner.c                    # C scanner integrating FA functions
├── scanner_fa.py                # Python scanner with integrated FA
├── run_tests.bat                # Automated test runner batch script
├── program1.cloud.txt           # Test program 1 (valid cluster deployment)
├── program2.cloud.txt           # Test program 2 (valid fleet maintenance)
├── program3_errors.cloud.txt    # Test program 3 (deliberate lexical errors)
├── PIF_Program1.out             # Generated PIF for program 1 (537 tokens)
├── ST_Program1.out              # Generated ST for program 1 (54 symbols)
├── PIF_Program2.out             # Generated PIF for program 2 (335 tokens)
├── ST_Program2.out              # Generated ST for program 2 (41 symbols)
├── PIF_Program3_Errors.out      # Generated PIF for program 3
├── ST_Program3_Errors.out       # Generated ST for program 3
└── Laboratory5_Documentation.txt# Comprehensive laboratory report
```

---

## How to Run

### Python Version:
```bash
py scanner_fa.py program1.cloud.txt PIF_Program1.out ST_Program1.out
py scanner_fa.py program2.cloud.txt PIF_Program2.out ST_Program2.out
py scanner_fa.py program3_errors.cloud.txt PIF_Program3_Errors.out ST_Program3_Errors.out
```
Or simply double-click `run_tests.bat`.

### C Version (GCC):
```bash
gcc -Wall -Wextra -o scanner.exe scanner.c fa.c st.c
./scanner.exe program1.cloud.txt PIF_Program1.out ST_Program1.out
./scanner.exe program2.cloud.txt PIF_Program2.out ST_Program2.out
./scanner.exe program3_errors.cloud.txt PIF_Program3_Errors.out ST_Program3_Errors.out
```

---

## Test Verification

| Program | Status | Tokens (PIF) | Unique Symbols (ST) | Errors Detected |
|---|---|---|---|---|
| `program1.cloud.txt` | SUCCESS | 537 | 54 | 0 |
| `program2.cloud.txt` | SUCCESS | 335 | 41 | 0 |
| `program3_errors.cloud.txt` | FINISHED | 25 | 3 | 7 deliberate errors |
