/*
 * fa.c - Finite Automata for Identifiers and Constants
 *
 * Laboratory 5: Integrating FA/RG into the Scanner
 *
 * Each function implements an explicit FA transition table.
 * The FA walks through the input character-by-character, following
 * transitions. When no transition is possible, the FA stops and
 * checks if the current state is a final (accepting) state.
 */

#include "fa.h"
#include <ctype.h>

/*
 * FA for Identifiers
 * M = ({q0, q1}, {letter, digit, _}, delta, q0, {q1})
 *
 * delta(q0, letter)     = q1
 * delta(q1, letter)     = q1
 * delta(q1, digit)      = q1
 * delta(q1, underscore) = q1
 */
int fa_identifier(const char *input) {
    /* State encoding: 0 = q0 (start), 1 = q1 (final) */
    int state = 0;
    int pos = 0;

    while (input[pos] != '\0') {
        char ch = input[pos];

        switch (state) {
            case 0: /* q0: start state */
                if (isalpha((unsigned char)ch)) {
                    state = 1; /* delta(q0, letter) = q1 */
                    pos++;
                } else {
                    goto done; /* No transition -> reject */
                }
                break;

            case 1: /* q1: accepting state */
                if (isalpha((unsigned char)ch) || isdigit((unsigned char)ch) || ch == '_') {
                    state = 1; /* delta(q1, letter|digit|_) = q1 */
                    pos++;
                } else {
                    goto done; /* No transition -> stop */
                }
                break;
        }
    }

done:
    /* Accept only if we ended in a final state (q1) */
    if (state == 1) {
        return pos;
    }
    return 0;
}

/*
 * FA for Numeric Constants (Integers)
 * M = ({q0, q1, q2, q3}, {0, 1-9, -}, delta, q0, {q1, q3})
 *
 * delta(q0, '0')           = q1   (matches exactly "0")
 * delta(q0, '-')           = q2   (matches the negative sign)
 * delta(q0, nonzero_digit) = q3   (starts a positive number)
 * delta(q2, nonzero_digit) = q3   (starts a negative number)
 * delta(q3, digit)         = q3   (continues the number)
 */
int fa_number(const char *input) {
    /* State encoding: 0 = q0, 1 = q1 (final), 2 = q2, 3 = q3 (final) */
    int state = 0;
    int pos = 0;

    while (input[pos] != '\0') {
        char ch = input[pos];

        switch (state) {
            case 0: /* q0: start state */
                if (ch == '0') {
                    state = 1; /* delta(q0, '0') = q1 */
                    pos++;
                } else if (ch == '-') {
                    state = 2; /* delta(q0, '-') = q2 */
                    pos++;
                } else if (ch >= '1' && ch <= '9') {
                    state = 3; /* delta(q0, nonzero_digit) = q3 */
                    pos++;
                } else {
                    goto done;
                }
                break;

            case 1: /* q1: final state (matched "0"), no outgoing transitions */
                goto done;

            case 2: /* q2: after '-', expecting nonzero digit */
                if (ch >= '1' && ch <= '9') {
                    state = 3; /* delta(q2, nonzero_digit) = q3 */
                    pos++;
                } else {
                    goto done; /* '-' alone or '-0' -> not accepted from here */
                }
                break;

            case 3: /* q3: final state, continue with more digits */
                if (isdigit((unsigned char)ch)) {
                    state = 3; /* delta(q3, digit) = q3 */
                    pos++;
                } else {
                    goto done;
                }
                break;
        }
    }

done:
    /* Accept if in final state q1 or q3 */
    if (state == 1 || state == 3) {
        return pos;
    }
    return 0;
}

/*
 * FA for String Constants
 * M = ({q0, q1, q2}, {", valid_chars}, delta, q0, {q2})
 *
 * delta(q0, '"')        = q1
 * delta(q1, valid_char) = q1
 * delta(q1, '"')        = q2
 *
 * Valid string characters: a-zA-Z0-9 and the set: space . : - _ , ! / @ # ? = &
 */
static int is_string_char(char ch) {
    if (isalnum((unsigned char)ch)) return 1;
    switch (ch) {
        case ' ': case '.': case ':': case '-': case '_':
        case ',': case '!': case '/': case '@': case '#':
        case '?': case '=': case '&':
            return 1;
        default:
            return 0;
    }
}

int fa_string(const char *input) {
    /* State encoding: 0 = q0 (start), 1 = q1 (inside string), 2 = q2 (final) */
    int state = 0;
    int pos = 0;

    while (input[pos] != '\0') {
        char ch = input[pos];

        switch (state) {
            case 0: /* q0: expecting opening quote */
                if (ch == '"') {
                    state = 1; /* delta(q0, '"') = q1 */
                    pos++;
                } else {
                    goto done;
                }
                break;

            case 1: /* q1: inside string */
                if (ch == '"') {
                    state = 2; /* delta(q1, '"') = q2 */
                    pos++;
                    goto done; /* q2 has no outgoing transitions */
                } else if (is_string_char(ch)) {
                    state = 1; /* delta(q1, valid_char) = q1 */
                    pos++;
                } else {
                    goto done; /* Invalid character inside string */
                }
                break;

            case 2: /* q2: final state, no outgoing transitions */
                goto done;
        }
    }

done:
    /* Accept only if we ended in the final state q2 */
    if (state == 2) {
        return pos;
    }
    return 0;
}
