# FLCD - Laboratory 5: Integrating Finite Automata (FA) into Scanner (C Implementation)

## Overview
This laboratory assignment enhances the Lexical Analyzer (Scanner) for the domain-specific **Cloud Infrastructure Programming Language** by replacing pattern-based regex token recognition with explicit **Deterministic Finite Automata (DFA)** implemented in standard C for:
1. **Identifiers** ($M_{id}$)
2. **Numeric Constants** ($M_{num}$)
3. **String Constants** ($M_{str}$)

The implementation preserves the Symbol Table (ST) based on a custom Hash Table ($m = 101$, linear probing) and Program Internal Form (PIF) from Laboratory 3.

---

## Directory Organization

```
Laboratory_5/
├── src/                                  # C source & header files
│   ├── fa.h                              # FA declarations & mathematical specs
│   ├── fa.c                              # Explicit FA state-machine transition functions
│   ├── st.h                              # Symbol Table & PIF structures
│   ├── st.c                              # Hash Table (m=101) & PIF implementation
│   └── scanner.c                         # Hand-written scanner driving the FAs
├── input/                                # Source DSL programs
│   ├── program1.cloud.txt                # Test program 1 (valid cluster deployment)
│   ├── program2.cloud.txt                # Test program 2 (valid fleet maintenance)
│   └── program3_errors.cloud.txt         # Test program 3 (deliberate lexical errors)
├── output/                               # Generated PIF and Symbol Table outputs
│   ├── PIF_Program1.out                  # Generated PIF for program 1 (537 tokens)
│   ├── ST_Program1.out                   # Generated ST for program 1 (54 symbols)
│   ├── PIF_Program2.out                  # Generated PIF for program 2 (335 tokens)
│   ├── ST_Program2.out                   # Generated ST for program 2 (41 symbols)
│   ├── PIF_Program3_Errors.out           # Generated PIF for program 3
│   └── ST_Program3_Errors.out            # Generated ST for program 3
├── doc/                                  # Assignment documentation & specifications
│   ├── Lab5_2026.docx                    # Official laboratory assignment requirements
│   └── Laboratory5_Documentation.txt     # Comprehensive laboratory technical report
├── run_tests.bat                         # Automated build & test execution script
└── README.md                             # Project overview and instructions
```

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
  - $\delta(q_0, \text{'-'}) = q_2$ (negative prefix; strictly requires 1-9 next)
  - $\delta(q_0, \text{nonzero\_digit}) = q_3$ (positive integer)
  - $\delta(q_2, \text{nonzero\_digit}) = q_3$ (negative integer)
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

## How to Build & Run (GCC)

### Automated:
Double-click `run_tests.bat` or run:
```bat
.\run_tests.bat
```

### Manual Commands:
```bash
cd Laboratories/Laboratory_5

# Compile
gcc -Wall -Wextra -O2 -I src -o bin/scanner.exe src/scanner.c src/fa.c src/st.c

# Run tests
bin/scanner.exe input/program1.cloud.txt output/PIF_Program1.out output/ST_Program1.out
bin/scanner.exe input/program2.cloud.txt output/PIF_Program2.out output/ST_Program2.out
bin/scanner.exe input/program3_errors.cloud.txt output/PIF_Program3_Errors.out output/ST_Program3_Errors.out
```

---

## Test Verification

| Program | Status | Tokens (PIF) | Unique Symbols (ST) | Errors Detected |
|---|---|---|---|---|
| `program1.cloud.txt` | SUCCESS | 537 | 54 | 0 |
| `program2.cloud.txt` | SUCCESS | 335 | 41 | 0 |
| `program3_errors.cloud.txt` | FINISHED | 25 | 3 | 7 deliberate errors |
