#include "st.h"

void st_init(SymbolTable *st) {
    memset(st, 0, sizeof(SymbolTable));
}

/*
 * Toy hash function from Course 1, Slide 29:
 * h(k) = (val(k) mod m) + 1, where val(k) is the sum of ASCII codes of chars.
 */
int st_hash(const char *symbol) {
    unsigned long ascii_sum = 0;
    for (size_t i = 0; symbol[i] != '\0'; i++) {
        ascii_sum += (unsigned char)symbol[i];
    }
    return (int)(ascii_sum % ST_CAPACITY) + 1;
}

/*
 * Searches for a symbol in the Hash Table using linear probing.
 * Returns the 1-based position (1..ST_CAPACITY) if found, or -1 if not found.
 */
int st_search(const SymbolTable *st, const char *symbol) {
    int h0 = st_hash(symbol);
    for (int i = 0; i < ST_CAPACITY; i++) {
        int probe_pos = ((h0 - 1 + i) % ST_CAPACITY) + 1;
        if (!st->entries[probe_pos].occupied) {
            return -1; /* Empty slot reached -> symbol not in table */
        }
        if (strcmp(st->entries[probe_pos].symbol, symbol) == 0) {
            return probe_pos;
        }
    }
    return -1;
}

/*
 * Inserts a new symbol into the Hash Table using linear probing.
 * Returns the 1-based position (1..ST_CAPACITY), or -1 if the table is full.
 */
int st_insert(SymbolTable *st, const char *symbol, const char *category) {
    if (st->count >= ST_CAPACITY) {
        fprintf(stderr, "Error: Symbol Table overflow (capacity %d exceeded)\n", ST_CAPACITY);
        return -1;
    }
    int h0 = st_hash(symbol);
    for (int i = 0; i < ST_CAPACITY; i++) {
        int probe_pos = ((h0 - 1 + i) % ST_CAPACITY) + 1;
        if (!st->entries[probe_pos].occupied) {
            st->entries[probe_pos].occupied = 1;
            strncpy(st->entries[probe_pos].symbol, symbol, MAX_SYMBOL_LEN - 1);
            st->entries[probe_pos].symbol[MAX_SYMBOL_LEN - 1] = '\0';
            strncpy(st->entries[probe_pos].category, category, 15);
            st->entries[probe_pos].category[15] = '\0';
            st->entries[probe_pos].initial_hash = h0;
            st->count++;
            return probe_pos;
        }
    }
    return -1;
}

/*
 * Pos(token, ST) from Course 1, Slide 25:
 * Searches token in ST; if found returns position; if not found inserts and returns position.
 */
int st_pos(SymbolTable *st, const char *symbol, const char *category) {
    int existing_pos = st_search(st, symbol);
    if (existing_pos != -1) {
        return existing_pos;
    }
    return st_insert(st, symbol, category);
}

void st_write_file(const SymbolTable *st, const char *filename) {
    FILE *fp = fopen(filename, "w");
    if (!fp) {
        perror("Failed to open ST output file");
        return;
    }
    fprintf(fp, "========================================================================\n");
    fprintf(fp, "  SYMBOL TABLE (Hash Table, m = %d, Open Addressing / Linear Probing)\n", ST_CAPACITY);
    fprintf(fp, "  Hash Function: h(k) = (sum_ASCII(k) mod %d) + 1\n", ST_CAPACITY);
    fprintf(fp, "  Total Unique Symbols: %d\n", st->count);
    fprintf(fp, "========================================================================\n");
    fprintf(fp, "%-10s | %-12s | %-10s | %s\n", "Position", "Initial h(k)", "Category", "Symbolic Name (Lexeme)");
    fprintf(fp, "-----------+--------------+------------+--------------------------------\n");
    for (int pos = 1; pos <= ST_CAPACITY; pos++) {
        if (st->entries[pos].occupied) {
            fprintf(fp, "%-10d | %-12d | %-10s | %s\n",
                    pos,
                    st->entries[pos].initial_hash,
                    st->entries[pos].category,
                    st->entries[pos].symbol);
        }
    }
    fclose(fp);
}

void pif_init(PIF *pif) {
    pif->count = 0;
}

void genPIF(PIF *pif, const char *token, int st_pos, const char *lexeme, int line) {
    if (pif->count >= MAX_PIF_CAPACITY) {
        fprintf(stderr, "Error: PIF capacity exceeded\n");
        return;
    }
    PIFEntry *entry = &pif->entries[pif->count++];
    strncpy(entry->token, token, MAX_SYMBOL_LEN - 1);
    entry->token[MAX_SYMBOL_LEN - 1] = '\0';
    entry->st_pos = st_pos;
    strncpy(entry->lexeme, lexeme, MAX_SYMBOL_LEN - 1);
    entry->lexeme[MAX_SYMBOL_LEN - 1] = '\0';
    entry->line = line;
}

void pif_write_file(const PIF *pif, const char *filename) {
    FILE *fp = fopen(filename, "w");
    if (!fp) {
        perror("Failed to open PIF output file");
        return;
    }
    fprintf(fp, "========================================================================\n");
    fprintf(fp, "  PROGRAM INTERNAL FORM (PIF) - Total Tokens: %d\n", pif->count);
    fprintf(fp, "========================================================================\n");
    fprintf(fp, "%-6s | %-20s | %-10s | %s\n", "Line", "Token", "ST_Pos", "Matched Lexeme");
    fprintf(fp, "-------+----------------------+------------+----------------------------\n");
    for (int i = 0; i < pif->count; i++) {
        fprintf(fp, "%-6d | %-20s | %-10d | %s\n",
                pif->entries[i].line,
                pif->entries[i].token,
                pif->entries[i].st_pos,
                pif->entries[i].lexeme);
    }
    fclose(fp);
}