# Laboratory 6 — Grammar Specification and Syntactical Analyzer Documentation

**Course:** Formal Languages and Compiler Design (FLCD)  
**Laboratory:** Laboratory 6 (Syntactical Analysis / Parser Implementation)  
**Language:** Cloud Infrastructure Domain-Specific Language (mini-DSL)  
**Tools:** Yacc/Bison (LALR(1) Parser Generator), Flex (Lexical Analyzer), GCC (C Compiler)

---

## 1. Overview and Architecture

This laboratory implements a complete syntactical analyzer (parser) for the **Cloud Infrastructure Programming Language** mini-DSL, building on top of the scanner and symbol table developed in Laboratory 3.

The implementation satisfies all requirements:
1. **Requirement 1 (Grammar Definition):** The abstract BNF from `CloudProgrammingLanguageBNF_v2.txt` has been converted into a strict, unambiguous, Yacc-compatible BNF specification where every production is numbered sequentially ($1 \dots 107$).
2. **Requirement 2 (Parsing from PIF):** When invoked with `--pif <pif_file>`, the parser reads token sequences directly from an existing Program Internal Form (PIF) file, performing syntactical analysis and outputting the exact sequence of production numbers.
3. **Requirement 3 (Lex + Yacc Integration):** When invoked with source code (`.cloud.txt`), Flex (`yylex()`) tokenizes the source, populates the Symbol Table (Hash Table, $m=101$, linear probing) and PIF table, and returns token codes to Bison (`yyparse()`). At the end of parsing, the generated PIF and ST files are written to disk.
4. **Zero Conflicts:** The LALR(1) grammar achieves **0 shift/reduce conflicts** and **0 reduce/reduce conflicts**, verified by Bison's state analysis in `parser.output`.
5. **Dual-Mode Equivalence:** Running `cloud_parser.exe` in Lex mode vs. PIF mode produces **100% byte-identical derivation sequences** on the test programs.

---

## 2. Terminal Symbols (Token Vocabulary)

Every terminal symbol corresponds strictly to the token vocabulary recognized by `scanner.lxi` and defined in `token1.in.txt`.

### 2.1 Keyword Tokens

| Bison `%token` | Source Lexeme | Category / Usage |
|---|---|---|
| `INT_T` | `int` | Integer scalar type |
| `STRING_T` | `string` | String scalar type |
| `BOOLEAN_T` | `boolean` | Boolean scalar type |
| `PROCESS_T` | `process` | Process declaration / type |
| `STATE_T` | `state` | State scalar type |
| `GROUP_T` | `group` | Service group declaration / type |
| `IP_ADDRESS_T` | `ip_address` | IP address scalar type |
| `DEPENDS_ON` | `depends_on` | Dependency specification |
| `IF` | `if` | Conditional branching |
| `ELSE` | `else` | Alternative branch |
| `SWITCH` | `switch` | Multi-way branching |
| `CASE` | `case` | Switch case label |
| `DEFAULT` | `default` | Switch default label |
| `WHILE` | `while` | While loop |
| `FOR` | `for` | For loop |
| `BREAK` | `break` | Loop / switch termination |
| `CONTINUE` | `continue` | Loop skip iteration |
| `TRY` | `try` | Exception handling block |
| `CATCH` | `catch` | Exception recovery block |
| `WAIT_SECONDS` | `wait_seconds` | Execution delay builtin |
| `PING_SERVER` | `ping_server` | Server reachability builtin |
| `SPIN_UP_INSTANCE` | `spin_up_instance` | Provisioning builtin |
| `KILL_INSTANCE` | `kill_instance` | Teardown builtin |
| `LOG_ALERT` | `log_alert` | Logging builtin |
| `START` | `start` | Process lifecycle: start |
| `STOP` | `stop` | Process lifecycle: stop |
| `RESTART` | `restart` | Process lifecycle: restart |
| `ENABLE` | `enable` | Service activation |
| `DISABLE` | `disable` | Service deactivation |
| `STATUS` | `status` | Process state query builtin |
| `AUTOSTART` | `autostart` | Autostart setting query builtin |
| `AND` | `and` | Logical conjunction |
| `OR` | `or` | Logical disjunction |
| `NOT` | `not` | Logical negation |
| `TRUE_T` | `true` | Boolean literal true |
| `FALSE_T` | `false` | Boolean literal false |
| `RUNNING_T` | `RUNNING` | State literal RUNNING |
| `STOPPED_T` | `STOPPED` | State literal STOPPED |
| `ERROR_T` | `ERROR` | State literal ERROR |

### 2.2 Operator and Separator Tokens

| Bison `%token` / Literal | Source Lexeme | Description |
|---|---|---|
| `EQ` | `==` | Relational equality |
| `NE` | `!=` | Relational inequality |
| `LE` | `<=` | Less-than or equal |
| `GE` | `>=` | Greater-than or equal |
| `'<'` | `<` | Strictly less-than |
| `'>'` | `>` | Strictly greater-than |
| `'='` | `=` | Assignment operator |
| `'+'` | `+` | Addition operator |
| `'-'` | `-` | Subtraction operator |
| `'*'` | `*` | Multiplication operator |
| `'/'` | `/` | Division operator |
| `'%'` | `%` | Modulo operator |
| `';'` | `;` | Statement terminator |
| `':'` | `:` | Colon separator |
| `','` | `,` | List delimiter |
| `'('`, `')'` | `(`, `)` | Parentheses |
| `'['`, `']'` | `[`, `]` | Array bracket delimiters |
| `'{'`, `'}'` | `{`, `}` | Block delimiters |

### 2.3 Value and Identifier Tokens

| Bison `%token` | Scanner Match Pattern | PIF Representation |
|---|---|---|
| `ID` | `[a-zA-Z][a-zA-Z0-9_]*` | `id` (ST position $1 \dots 101$) |
| `NUMBER` | `"0" \| ("-"?[1-9][0-9]*)` | `const` (ST position $1 \dots 101$) |
| `STRING_CONST` | `"..."` | `const` (ST position $1 \dots 101$) |

---

## 3. Formal Grammar Productions (Numbered 1 .. 107)

Every production is assigned a strictly sequential index from $1$ to $107$. The parser emits each index when the corresponding production rule is applied during LALR(1) bottom-up reduction.

```
 1. program                   -> statements_list

 2. statements_list           -> statement
 3. statements_list           -> statements_list statement

 4. statement                 -> declaration_statement
 5. statement                 -> assign_statement
 6. statement                 -> io_statement
 7. statement                 -> process_statement
 8. statement                 -> if_statement
 9. statement                 -> switch_statement
10. statement                 -> while_statement
11. statement                 -> for_statement
12. statement                 -> try_statement
13. statement                 -> break_statement
14. statement                 -> continue_statement
15. statement                 -> wait_statement

16. declaration_statement     -> type ID ';'
17. declaration_statement     -> type ID '=' expression ';'
18. declaration_statement     -> PROCESS_T ID ';'
19. declaration_statement     -> PROCESS_T ID '{' DEPENDS_ON ':' identifier_list '}' ';'
20. declaration_statement     -> GROUP_T ID '=' '[' identifier_list ']' ';'
21. declaration_statement     -> PROCESS_T '[' NUMBER ']' ID ';'
22. declaration_statement     -> PROCESS_T '[' NUMBER ']' ID '=' expression ';'
23. declaration_statement     -> GROUP_T '[' NUMBER ']' ID ';'
24. declaration_statement     -> GROUP_T '[' NUMBER ']' ID '=' expression ';'

25. identifier_list           -> ID
26. identifier_list           -> identifier_list ',' ID

27. assign_statement          -> ID '=' expression ';'
28. assign_statement          -> ID '[' expression ']' '=' expression ';'

29. assign_statement_nosemi   -> ID '=' expression
30. assign_statement_nosemi   -> ID '[' expression ']' '=' expression

31. io_statement              -> io_reserved_word '(' expression ')' ';'
32. process_statement         -> process_reserved_word '(' expression ')' ';'
33. wait_statement            -> WAIT_SECONDS '(' expression ')' ';'

34. if_statement              -> IF '(' condition ')' '{' statements_list '}' else_branch

35. else_branch               -> ELSE '{' statements_list '}'
36. else_branch               -> /* epsilon */

37. switch_statement          -> SWITCH '(' expression ')' '{' case_list default_case '}'

38. case_list                 -> case_statement
39. case_list                 -> case_list case_statement

40. case_statement            -> CASE constant ':' statements_list BREAK ';'

41. default_case              -> DEFAULT ':' statements_list
42. default_case              -> /* epsilon */

43. while_statement           -> WHILE '(' condition ')' '{' statements_list '}'

44. for_statement             -> FOR '(' for_init condition ';' assign_statement_nosemi ')' '{' statements_list '}'

45. for_init                  -> declaration_statement
46. for_init                  -> assign_statement

47. break_statement           -> BREAK ';'
48. continue_statement        -> CONTINUE ';'

49. try_statement             -> TRY '{' statements_list '}' CATCH '{' statements_list '}'

50. condition                 -> simple_condition
51. condition                 -> condition logical_operator simple_condition

52. simple_condition          -> expression relation expression
53. simple_condition          -> NOT '(' condition ')'
54. simple_condition          -> '(' condition ')'

55. expression                -> multiplicative_expr
56. expression                -> expression add_op multiplicative_expr

57. multiplicative_expr       -> term
58. multiplicative_expr       -> multiplicative_expr mul_op term

59. term                      -> ID
60. term                      -> ID '[' expression ']'
61. term                      -> NUMBER
62. term                      -> STRING_CONST
63. term                      -> boolean_constant
64. term                      -> state_constant
65. term                      -> builtin_call
66. term                      -> '(' expression ')'

67. builtin_call              -> builtin_reserved_word '(' expression ')'

68. type                      -> simple_type
69. type                      -> simple_type '[' NUMBER ']'

70. simple_type               -> INT_T
71. simple_type               -> STRING_T
72. simple_type               -> BOOLEAN_T
73. simple_type               -> STATE_T
74. simple_type               -> IP_ADDRESS_T

75. constant                  -> NUMBER
76. constant                  -> STRING_CONST
77. constant                  -> boolean_constant
78. constant                  -> state_constant

79. boolean_constant          -> TRUE_T
80. boolean_constant          -> FALSE_T

81. state_constant            -> RUNNING_T
82. state_constant            -> STOPPED_T
83. state_constant            -> ERROR_T

84. io_reserved_word          -> PING_SERVER
85. io_reserved_word          -> SPIN_UP_INSTANCE
86. io_reserved_word          -> KILL_INSTANCE
87. io_reserved_word          -> LOG_ALERT

88. process_reserved_word     -> START
89. process_reserved_word     -> STOP
90. process_reserved_word     -> RESTART
91. process_reserved_word     -> ENABLE
92. process_reserved_word     -> DISABLE

93. builtin_reserved_word     -> STATUS
94. builtin_reserved_word     -> AUTOSTART

95. add_op                    -> '+'
96. add_op                    -> '-'

97. mul_op                    -> '*'
98. mul_op                    -> '/'
99. mul_op                    -> '%'

100. relation                 -> EQ
101. relation                 -> NE
102. relation                 -> '<'
103. relation                 -> LE
104. relation                 -> '>'
105. relation                 -> GE

106. logical_operator         -> AND
107. logical_operator         -> OR
```

---

## 4. Conflict Resolution and LALR(1) Determinism

Converting the natural-language BNF into strict LALR(1) required resolving two structural ambiguities:

### 4.1 Process and Group Keyword Disambiguation
- **Problem:** In the raw BNF, `process` and `group` appeared both as `simple_type` (allowing `type ID ;` -> `process x ;`) and as dedicated declaration forms (`process x ;`, `process x { depends_on: ... } ;`, `group g = [...] ;`). This caused **shift/reduce conflicts** because the parser could not determine whether to reduce `process` to `simple_type` or shift to match the special process syntax.
- **Solution:** `PROCESS_T` and `GROUP_T` are segregated into explicit productions within `declaration_statement` (rules 18–24), covering single-process declarations, dependency blocks, group assignments, and array declarations (`process[3] services;`). `simple_type` (rules 70–74) retains purely scalar types (`int`, `string`, `boolean`, `state`, `ip_address`).

### 4.2 Condition vs. Expression Disjointness
- **Problem:** When `boolean_constant` appeared both in `simple_condition` and in `term`, and both allowed parenthesized sub-clauses (`( condition )` in simple_condition vs. `( expression )` in term), a 1-token lookahead `')'` caused **reduce/reduce conflicts**: upon seeing `IF ( true )`, the parser could not determine whether `true` should reduce to `boolean_constant -> simple_condition` (condition) or `boolean_constant -> term` (expression).
- **Solution:** `condition` is strictly formulated as relational comparisons (`expr rel expr`), logical negations (`not ( cond )`), or parenthesized conditions (`( cond )`). Boolean constants (`true`, `false`) are terms within expressions (rule 63), allowing relational checks (`if (maintenance_mode == true)`), variable assignments (`scaling_needed = false;`), and switch cases (`case true:`). This completely removes the reduce/reduce conflict, achieving **0 conflicts**.

---

## 5. Verification Results

Building and running `run_tests.bat` validates the complete pipeline:

```
===================================================================
 STEP 1: Compile parser with Bison (LALR(1))
===================================================================
[OK] Bison generated parser.tab.c and parser.tab.h

--- Conflict Verification (parser.output) ---
[PASS] 0 shift/reduce conflicts
[PASS] 0 reduce/reduce conflicts

===================================================================
 STEP 2: Compile scanner with Flex
===================================================================
[OK] Flex generated lex.yy.c

===================================================================
 STEP 3: Compile with GCC
===================================================================
[OK] Successfully built cloud_parser.exe

===================================================================
 STEP 4: Run Test 1 - program1.cloud.txt
===================================================================
[Test 1A - Lex+Yacc mode]
Parsing source program: 'program1.cloud.txt' (Lex + Yacc) ...
Parsing completed successfully (0 syntax errors).
Production sequence saved to 'productions_program1.out'.
Generated PIF: 'PIF_Program1.out' (537 tokens), ST: 'ST_Program1.out' (54 symbols).
[PASS] program1.cloud.txt parsed successfully

[Test 1B - PIF mode]
Parsing from PIF input: 'PIF_Program1.out' ...
Parsing completed successfully (0 syntax errors).
Production sequence saved to 'productions_program1_pif.out'.
[PASS] PIF_Program1.out parsed successfully in PIF mode

[Test 1C - Equivalence Verification]
[PASS] Output productions match identically between Lex mode and PIF mode!

===================================================================
 STEP 5: Run Test 2 - program2.cloud.txt
===================================================================
[Test 2A - Lex+Yacc mode]
Parsing source program: 'program2.cloud.txt' (Lex + Yacc) ...
Parsing completed successfully (0 syntax errors).
Production sequence saved to 'productions_program2.out'.
Generated PIF: 'PIF_Program2.out' (335 tokens), ST: 'ST_Program2.out' (41 symbols).
[PASS] program2.cloud.txt parsed successfully

[Test 2B - PIF mode]
Parsing from PIF input: 'PIF_Program2.out' ...
Parsing completed successfully (0 syntax errors).
Production sequence saved to 'productions_program2_pif.out'.
[PASS] PIF_Program2.out parsed successfully in PIF mode

[Test 2C - Equivalence Verification]
[PASS] Output productions match identically between Lex mode and PIF mode!

===================================================================
 STEP 6: Run Test 3 - program3_errors.cloud.txt (Intentional Errors)
===================================================================
Parse error at line 2: syntax error
Parsing source program: 'program3_errors.cloud.txt' (Lex + Yacc) ...
70 68 61 57 55 17 4 2 70 Lexical error at line 2: malformed identifier or numeric constant '123invalid_id'
68 
Parsing finished with syntax errors.
[PASS - EXPECTED] program3_errors.cloud.txt was correctly rejected with syntax/lexical errors.
```
