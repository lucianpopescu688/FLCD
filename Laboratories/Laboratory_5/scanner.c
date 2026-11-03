/*
 * scanner.c - Hand-written Scanner with Integrated Finite Automata
 *
 * Laboratory 5: Integrating FA/RG into the Scanner
 * Language: Cloud Infrastructure DSL
 *
 * This scanner replaces the Flex-generated lexer from Lab 3 with a
 * hand-written scanner that delegates identifier and constant recognition
 * to explicit Finite Automata (defined in fa.c / fa.h).
 *
 * Instead of regex patterns, the scanner:
 *   1. Skips whitespace and comments
 *   2. Tries to match reserved words and operators (longest match)
 *   3. Delegates to fa_string()     for string constants
 *   4. Delegates to fa_number()     for numeric constants
 *   5. Delegates to fa_identifier() for identifiers
 *   6. Reports lexical errors for unrecognized tokens
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "st.h"
#include "fa.h"

/* ── Reserved Words ───────────────────────────────────────────────── */
static const char *RESERVED_WORDS[] = {
    /* Types */
    "int", "string", "boolean", "process", "state", "group", "ip_address",
    /* Control flow */
    "depends_on", "if", "else", "switch", "case", "default",
    "while", "for", "break", "continue", "try", "catch", "wait_seconds",
    /* IO / process commands */
    "ping_server", "spin_up_instance", "kill_instance", "log_alert",
    "start", "stop", "restart", "enable", "disable", "status", "autostart",
    /* Logical operators (word-form) */
    "and", "or", "not",
    NULL
};

/* ── Boolean and State Constants (also matched by FA identifier, but
 *    we check them explicitly as reserved constants) ─────────────── */
static const char *BOOL_STATE_CONSTANTS[] = {
    "true", "false", "RUNNING", "STOPPED", "ERROR",
    NULL
};

/* ── Multi-char Operators (order matters: longest first) ─────────── */
static const char *OPERATORS[] = {
    "==", "!=", "<=", ">=",
    "<", ">", "=",
    "+", "-", "*", "/", "%",
    NULL
};

/* ── Separators ──────────────────────────────────────────────────── */
static const char *SEPARATORS = ";:,()[]{}";

/* ── Helpers ─────────────────────────────────────────────────────── */

/* Check if `word` is in a NULL-terminated string array */
static int is_in_list(const char *word, const char **list) {
    for (int i = 0; list[i] != NULL; i++) {
        if (strcmp(word, list[i]) == 0) return 1;
    }
    return 0;
}

/* Try to match one of the multi-char operators at position `src`.
 * Returns the length of the matched operator, or 0 if none matched. */
static int match_operator(const char *src) {
    for (int i = 0; OPERATORS[i] != NULL; i++) {
        size_t len = strlen(OPERATORS[i]);
        if (strncmp(src, OPERATORS[i], len) == 0) {
            return (int)len;
        }
    }
    return 0;
}

/* ── Main Scanner ────────────────────────────────────────────────── */

static void scan(const char *source, SymbolTable *st, PIF *pif,
                 int *error_count) {
    int line_num = 1;
    int pos = 0;
    int len = (int)strlen(source);
    char token_buf[MAX_SYMBOL_LEN];

    while (pos < len) {
        /* ── 1. Skip whitespace ──────────────────────────────────── */
        if (source[pos] == '\n') {
            line_num++;
            pos++;
            continue;
        }
        if (source[pos] == ' ' || source[pos] == '\t' ||
            source[pos] == '\r') {
            pos++;
            continue;
        }

        /* ── 2. Skip block comments  %%...%%  ────────────────────── */
        if (pos + 1 < len && source[pos] == '%' && source[pos + 1] == '%') {
            pos += 2;
            while (pos + 1 < len &&
                   !(source[pos] == '%' && source[pos + 1] == '%')) {
                if (source[pos] == '\n') line_num++;
                pos++;
            }
            if (pos + 1 < len) pos += 2; /* skip closing %% */
            continue;
        }

        /* ── 3. Skip single-line comments  //...  ────────────────── */
        if (pos + 1 < len && source[pos] == '/' && source[pos + 1] == '/') {
            while (pos < len && source[pos] != '\n') pos++;
            continue;
        }

        /* ── 4. Separators ───────────────────────────────────────── */
        if (strchr(SEPARATORS, source[pos])) {
            token_buf[0] = source[pos];
            token_buf[1] = '\0';
            genPIF(pif, token_buf, 0, token_buf, line_num);
            pos++;
            continue;
        }

        /* ── 5. Operators & Negative numbers ───────────────────── */
        {
            /* Check if '-' is the start of a malformed or valid negative number */
            if (source[pos] == '-') {
                /* Look backward to see if '-' could be a unary minus */
                int prev = pos - 1;
                while (prev >= 0 && (source[prev] == ' ' ||
                       source[prev] == '\t' || source[prev] == '\r'))
                    prev--;

                int is_unary = (prev < 0 ||
                                source[prev] == '=' || source[prev] == '(' ||
                                source[prev] == '[' || source[prev] == ',' ||
                                source[prev] == ';' || source[prev] == ':' ||
                                source[prev] == '{');

                if (is_unary && pos + 1 < len) {
                    /* Malformed negative zero: -0, -01, etc. */
                    if (source[pos + 1] == '0') {
                        int start = pos;
                        pos += 2;
                        while (pos < len && (isalnum((unsigned char)source[pos]) || source[pos] == '_'))
                            pos++;
                        int err_len = pos - start;
                        if (err_len >= MAX_SYMBOL_LEN) err_len = MAX_SYMBOL_LEN - 1;
                        strncpy(token_buf, &source[start], err_len);
                        token_buf[err_len] = '\0';
                        printf("Lexical error at line %d: malformed identifier or numeric constant '%s'\n",
                               line_num, token_buf);
                        (*error_count)++;
                        continue;
                    }

                    /* Try FA for negative number */
                    int num_len = fa_number(&source[pos]);
                    if (num_len > 0) {
                        /* Check for malformed negative number like -5abc */
                        int total = num_len;
                        int malformed = 0;
                        while (pos + total < len &&
                               (isalnum((unsigned char)source[pos + total]) ||
                                source[pos + total] == '_')) {
                            total++;
                            malformed = 1;
                        }
                        if (malformed) {
                            strncpy(token_buf, &source[pos], total);
                            token_buf[total] = '\0';
                            printf("Lexical error at line %d: malformed identifier or numeric constant '%s'\n",
                                   line_num, token_buf);
                            (*error_count)++;
                            pos += total;
                            continue;
                        }
                        strncpy(token_buf, &source[pos], num_len);
                        token_buf[num_len] = '\0';
                        int stp = st_pos(st, token_buf, "CONST");
                        genPIF(pif, "const", stp, token_buf, line_num);
                        pos += num_len;
                        continue;
                    }
                }
            }

            int op_len = match_operator(&source[pos]);
            if (op_len > 0) {
                strncpy(token_buf, &source[pos], op_len);
                token_buf[op_len] = '\0';
                genPIF(pif, token_buf, 0, token_buf, line_num);
                pos += op_len;
                continue;
            }
        }

        /* ── 6. String constants (via FA) ────────────────────────── */
        if (source[pos] == '"') {
            int str_len = fa_string(&source[pos]);
            if (str_len > 0) {
                strncpy(token_buf, &source[pos], str_len);
                token_buf[str_len] = '\0';
                int stp = st_pos(st, token_buf, "CONST");
                genPIF(pif, "const", stp, token_buf, line_num);
                pos += str_len;
                continue;
            } else {
                /* FA rejected: malformed or unclosed string */
                int start = pos;
                pos++; /* skip opening quote */
                while (pos < len && source[pos] != '"' && source[pos] != '\n')
                    pos++;
                if (pos < len && source[pos] == '"') pos++; /* skip closing quote */
                int err_len = pos - start;
                if (err_len >= MAX_SYMBOL_LEN) err_len = MAX_SYMBOL_LEN - 1;
                strncpy(token_buf, &source[start], err_len);
                token_buf[err_len] = '\0';
                printf("Lexical error at line %d: invalid or unclosed string constant '%s'\n",
                       line_num, token_buf);
                (*error_count)++;
                continue;
            }
        }

        /* ── 7. Identifiers, reserved words, bool/state constants (via FA) ── */
        if (isalpha((unsigned char)source[pos])) {
            int id_len = fa_identifier(&source[pos]);
            if (id_len > 0) {
                strncpy(token_buf, &source[pos], id_len);
                token_buf[id_len] = '\0';

                /* Check if it's a reserved word */
                if (is_in_list(token_buf, RESERVED_WORDS)) {
                    genPIF(pif, token_buf, 0, token_buf, line_num);
                }
                /* Check if it's a boolean/state constant */
                else if (is_in_list(token_buf, BOOL_STATE_CONSTANTS)) {
                    int stp = st_pos(st, token_buf, "CONST");
                    genPIF(pif, "const", stp, token_buf, line_num);
                }
                /* Otherwise it's a user-defined identifier */
                else {
                    int stp = st_pos(st, token_buf, "ID");
                    genPIF(pif, "id", stp, token_buf, line_num);
                }
                pos += id_len;
                continue;
            }
        }

        /* ── 8. Numeric constants (via FA) ───────────────────────── */
        if (isdigit((unsigned char)source[pos])) {
            int num_len = fa_number(&source[pos]);
            if (num_len > 0) {
                /* Check for malformed token: number followed immediately by
                 * letters or underscores (e.g. "123abc", "007x") */
                int total = num_len;
                int malformed = 0;
                while (pos + total < len &&
                       (isalnum((unsigned char)source[pos + total]) ||
                        source[pos + total] == '_')) {
                    total++;
                    malformed = 1;
                }
                if (malformed) {
                    strncpy(token_buf, &source[pos], total);
                    token_buf[total] = '\0';
                    printf("Lexical error at line %d: malformed identifier or numeric constant '%s'\n",
                           line_num, token_buf);
                    (*error_count)++;
                    pos += total;
                    continue;
                }
                strncpy(token_buf, &source[pos], num_len);
                token_buf[num_len] = '\0';
                int stp = st_pos(st, token_buf, "CONST");
                genPIF(pif, "const", stp, token_buf, line_num);
                pos += num_len;
                continue;
            } else {
                /* FA rejected but starts with digit: malformed (e.g. "007") */
                int start = pos;
                while (pos < len &&
                       (isalnum((unsigned char)source[pos]) ||
                        source[pos] == '_'))
                    pos++;
                int err_len = pos - start;
                if (err_len >= MAX_SYMBOL_LEN) err_len = MAX_SYMBOL_LEN - 1;
                strncpy(token_buf, &source[start], err_len);
                token_buf[err_len] = '\0';
                printf("Lexical error at line %d: malformed identifier or numeric constant '%s'\n",
                       line_num, token_buf);
                (*error_count)++;
                continue;
            }
        }

        /* ── 9. Unrecognized token ───────────────────────────────── */
        token_buf[0] = source[pos];
        token_buf[1] = '\0';
        printf("Lexical error at line %d: unrecognized token '%s'\n",
               line_num, token_buf);
        (*error_count)++;
        pos++;
    }
}

/* ── Entry Point ─────────────────────────────────────────────────── */

int main(int argc, char **argv) {
    const char *input_file = (argc > 1) ? argv[1] : "program1.cloud.txt";
    const char *pif_file   = (argc > 2) ? argv[2] : "PIF.out";
    const char *st_file    = (argc > 3) ? argv[3] : "ST.out";

    /* Read entire source file into memory */
    FILE *fp = fopen(input_file, "r");
    if (!fp) {
        perror("Cannot open source program file");
        return 1;
    }
    fseek(fp, 0, SEEK_END);
    long file_size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    char *source = (char *)malloc(file_size + 1);
    if (!source) {
        fprintf(stderr, "Error: failed to allocate memory\n");
        fclose(fp);
        return 1;
    }
    size_t bytes_read = fread(source, 1, file_size, fp);
    source[bytes_read] = '\0';
    fclose(fp);

    /* Initialize ST and PIF (static to avoid stack overflow on Windows 1MB stack) */
    static SymbolTable sym_table;
    static PIF pif_table;
    st_init(&sym_table);
    pif_init(&pif_table);

    /* Run the scanner */
    int error_count = 0;
    scan(source, &sym_table, &pif_table, &error_count);

    /* Write output files */
    pif_write_file(&pif_table, pif_file);
    st_write_file(&sym_table, st_file);

    if (error_count == 0) {
        printf("Lexical analysis completed successfully for '%s' (0 lexical errors).\n",
               input_file);
        printf("Generated '%s' (%d tokens) and '%s' (%d unique symbols).\n",
               pif_file, pif_table.count, st_file, sym_table.count);
    } else {
        printf("Lexical analysis finished for '%s' with %d lexical error(s).\n",
               input_file, error_count);
    }

    free(source);
    return 0;
}
