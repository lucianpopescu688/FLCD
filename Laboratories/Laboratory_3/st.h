#ifndef ST_H
#define ST_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Prime number for Hash Table capacity (Course 1, Slide 29: |A| = m, m prime) */
#define ST_CAPACITY 101
#define MAX_SYMBOL_LEN 256
#define MAX_PIF_CAPACITY 4096

typedef struct {
    int occupied;
    char symbol[MAX_SYMBOL_LEN];
    char category[16]; /* "ID" or "CONST" */
    int initial_hash;  /* h(k) = (ascii_sum % m) + 1 */
} STEntry;

typedef struct {
    STEntry entries[ST_CAPACITY + 1]; /* 1-indexed: positions 1..ST_CAPACITY */
    int count;
} SymbolTable;

typedef struct {
    char token[MAX_SYMBOL_LEN];  /* Token code: reserved word/op/sep lexeme, or "id" / "const" */
    int st_pos;                  /* 0 for keyword/op/sep, 1..m for id/const */
    char lexeme[MAX_SYMBOL_LEN]; /* Original lexeme from source code */
    int line;                    /* Source line number */
} PIFEntry;

typedef struct {
    PIFEntry entries[MAX_PIF_CAPACITY];
    int count;
} PIF;

/* Symbol Table Operations */
void st_init(SymbolTable *st);
int st_hash(const char *symbol);
int st_search(const SymbolTable *st, const char *symbol);
int st_insert(SymbolTable *st, const char *symbol, const char *category);
int st_pos(SymbolTable *st, const char *symbol, const char *category);
void st_write_file(const SymbolTable *st, const char *filename);

/* PIF Operations */
void pif_init(PIF *pif);
void genPIF(PIF *pif, const char *token, int st_pos, const char *lexeme, int line);
void pif_write_file(const PIF *pif, const char *filename);

#endif /* ST_H */