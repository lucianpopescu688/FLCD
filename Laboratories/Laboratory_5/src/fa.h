#ifndef FA_H
#define FA_H

/*
 * fa.h - Finite Automata for Identifiers and Constants
 *
 * Laboratory 5: Integrating FA/RG into the Scanner
 * 
 * Each FA is defined as M = (Q, Sigma, delta, q0, F) and implemented
 * via an explicit transition function that returns:
 *   - the number of characters consumed (accepted) if the input is accepted
 *   - 0 if the input is rejected (no valid token found)
 *
 * FA Definitions (from Lab 4):
 *
 * 1. IDENTIFIER FA:
 *    Q = {q0, q1}
 *    Sigma = {a-z, A-Z, 0-9, _}
 *    q0 = q0,  F = {q1}
 *    delta(q0, letter)     = q1
 *    delta(q1, letter)     = q1
 *    delta(q1, digit)      = q1
 *    delta(q1, underscore) = q1
 *
 * 2. NUMERIC CONSTANT FA:
 *    Q = {q0, q1, q2, q3}
 *    Sigma = {0, 1-9, -}
 *    q0 = q0,  F = {q1, q3}
 *    delta(q0, '0')           = q1
 *    delta(q0, '-')           = q2
 *    delta(q0, nonzero_digit) = q3
 *    delta(q2, nonzero_digit) = q3
 *    delta(q3, digit)         = q3
 *
 * 3. STRING CONSTANT FA:
 *    Q = {q0, q1, q2}
 *    Sigma = {", a-zA-Z0-9 .:-_,!/@#?=&}
 *    q0 = q0,  F = {q2}
 *    delta(q0, '"')  = q1
 *    delta(q1, char) = q1  (valid string characters)
 *    delta(q1, '"')  = q2
 */

/* Returns the length of the longest accepted identifier starting at `input`,
 * or 0 if no valid identifier is found.
 * Implements the Identifier FA from Lab 4. */
int fa_identifier(const char *input);

/* Returns the length of the longest accepted numeric constant starting at `input`,
 * or 0 if no valid numeric constant is found.
 * Implements the Numeric Constant FA from Lab 4. */
int fa_number(const char *input);

/* Returns the length of the longest accepted string constant starting at `input`,
 * or 0 if no valid string constant is found.
 * Implements the String Constant FA from Lab 4. */
int fa_string(const char *input);

#endif /* FA_H */
