"""
scanner_fa.py - Scanner with Integrated Finite Automata (FA)
Language: Cloud Infrastructure DSL
Laboratory 5 - Formal Languages and Compiler Design (FLCD)

This scanner implements the lexical analysis for the Cloud DSL by integrating
Finite Automata directly for:
  1. Identifiers:      M = ({q0, q1}, Sigma, delta, q0, {q1})
  2. Numeric Constants: M = ({q0, q1, q2, q3}, Sigma, delta, q0, {q1, q3})
  3. String Constants:  M = ({q0, q1, q2}, Sigma, delta, q0, {q2})

Symbol Table (ST) is implemented as a Hash Table of capacity m = 101 with
Linear Probing and ASCII summation modulo m + 1 (identical to Lab 3).
"""

import sys
import os

# ==============================================================================
# 1. Finite Automata Implementations
# ==============================================================================

class FiniteAutomata:
    @staticmethod
    def match_identifier(text: str) -> int:
        """
        Identifier FA:
          States: {q0, q1}
          Alphabet: [a-zA-Z0-9_]
          Start state: q0
          Final states: {q1}

          delta(q0, letter) = q1
          delta(q1, letter | digit | '_') = q1
        """
        state = 0
        pos = 0
        while pos < len(text):
            ch = text[pos]
            if state == 0:
                if ch.isalpha():
                    state = 1
                    pos += 1
                else:
                    break
            elif state == 1:
                if ch.isalnum() or ch == '_':
                    state = 1
                    pos += 1
                else:
                    break

        return pos if state == 1 else 0

    @staticmethod
    def match_number(text: str) -> int:
        """
        Numeric Constant FA (Integers):
          States: {q0, q1, q2, q3}
          Alphabet: {'0', '1'-'9', '-'}
          Start state: q0
          Final states: {q1, q3}

          delta(q0, '0')           = q1  (matches exactly "0")
          delta(q0, '-')           = q2  (negative sign)
          delta(q0, nonzero_digit) = q3  (positive integer)
          delta(q2, nonzero_digit) = q3  (negative integer)
          delta(q3, digit)         = q3  (further digits)
        """
        state = 0
        pos = 0
        while pos < len(text):
            ch = text[pos]
            if state == 0:
                if ch == '0':
                    state = 1
                    pos += 1
                elif ch == '-':
                    state = 2
                    pos += 1
                elif '1' <= ch <= '9':
                    state = 3
                    pos += 1
                else:
                    break
            elif state == 1:
                # "0" has no outgoing transitions (non-zero leading digits required for longer numbers)
                break
            elif state == 2:
                if '1' <= ch <= '9':
                    state = 3
                    pos += 1
                else:
                    break
            elif state == 3:
                if ch.isdigit():
                    state = 3
                    pos += 1
                else:
                    break

        return pos if state in (1, 3) else 0

    @staticmethod
    def match_string(text: str) -> int:
        """
        String Constant FA:
          States: {q0, q1, q2}
          Start state: q0
          Final states: {q2}

          delta(q0, '"')       = q1
          delta(q1, valid_chr) = q1
          delta(q1, '"')       = q2
        """
        valid_chars = set("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789 .:-_,!/@#?=&")
        state = 0
        pos = 0
        while pos < len(text):
            ch = text[pos]
            if state == 0:
                if ch == '"':
                    state = 1
                    pos += 1
                else:
                    break
            elif state == 1:
                if ch == '"':
                    state = 2
                    pos += 1
                    break
                elif ch in valid_chars:
                    state = 1
                    pos += 1
                else:
                    break
            elif state == 2:
                break

        return pos if state == 2 else 0


# ==============================================================================
# 2. Symbol Table (Hash Table m = 101, Linear Probing)
# ==============================================================================

ST_CAPACITY = 101

class STEntry:
    def __init__(self, symbol: str, category: str, initial_hash: int):
        self.occupied = True
        self.symbol = symbol
        self.category = category
        self.initial_hash = initial_hash

class SymbolTable:
    def __init__(self, capacity: int = ST_CAPACITY):
        self.capacity = capacity
        # 1-indexed: positions 1..capacity (index 0 is unused)
        self.table = [None] * (self.capacity + 1)
        self.count = 0

    def hash(self, symbol: str) -> int:
        ascii_sum = sum(ord(c) for c in symbol)
        return (ascii_sum % self.capacity) + 1

    def search(self, symbol: str) -> int:
        h0 = self.hash(symbol)
        for i in range(self.capacity):
            pos = ((h0 - 1 + i) % self.capacity) + 1
            entry = self.table[pos]
            if entry is None or not entry.occupied:
                return -1
            if entry.symbol == symbol:
                return pos
        return -1

    def insert(self, symbol: str, category: str) -> int:
        if self.count >= self.capacity:
            raise OverflowError("Symbol Table capacity exceeded")
        h0 = self.hash(symbol)
        for i in range(self.capacity):
            pos = ((h0 - 1 + i) % self.capacity) + 1
            entry = self.table[pos]
            if entry is None or not entry.occupied:
                self.table[pos] = STEntry(symbol, category, h0)
                self.count += 1
                return pos
        return -1

    def get_pos(self, symbol: str, category: str) -> int:
        idx = self.search(symbol)
        if idx != -1:
            return idx
        return self.insert(symbol, category)

    def write_file(self, filename: str):
        with open(filename, "w", encoding="utf-8") as f:
            f.write("========================================================================\n")
            f.write(f"  SYMBOL TABLE (Hash Table, m = {self.capacity}, Open Addressing / Linear Probing)\n")
            f.write(f"  Hash Function: h(k) = (sum_ASCII(k) mod {self.capacity}) + 1\n")
            f.write(f"  Total Unique Symbols: {self.count}\n")
            f.write("========================================================================\n")
            f.write(f"{'ST Index (Bucket)':<18} | {'Initial h(k)':<12} | {'Category':<10} | {'Symbolic Name (Lexeme)'}\n")
            f.write("-------------------+--------------+------------+--------------------------------\n")
            for pos in range(1, self.capacity + 1):
                entry = self.table[pos]
                if entry is not None and entry.occupied:
                    f.write(f"{pos:<18} | {entry.initial_hash:<12} | {entry.category:<10} | {entry.symbol}\n")


# ==============================================================================
# 3. Program Internal Form (PIF)
# ==============================================================================

class PIFEntry:
    def __init__(self, token: str, st_pos: int, lexeme: str, line: int):
        self.token = token
        self.st_pos = st_pos
        self.lexeme = lexeme
        self.line = line

class PIF:
    def __init__(self):
        self.entries = []

    def add(self, token: str, st_pos: int, lexeme: str, line: int):
        self.entries.append(PIFEntry(token, st_pos, lexeme, line))

    def write_file(self, filename: str):
        with open(filename, "w", encoding="utf-8") as f:
            f.write("========================================================================\n")
            f.write(f"  PROGRAM INTERNAL FORM (PIF) - Total Tokens: {len(self.entries)}\n")
            f.write("========================================================================\n")
            f.write(f"{'Position (Code)':<15} | {'Line':<6} | {'Token':<20} | {'ST Index (Bucket)':<18} | {'Matched Lexeme'}\n")
            f.write("----------------+--------+----------------------+--------------------+----------------------------\n")
            for i, e in enumerate(self.entries):
                f.write(f"{i:<15} | {e.line:<6} | {e.token:<20} | {e.st_pos:<18} | {e.lexeme}\n")


# ==============================================================================
# 4. Scanner Implementation
# ==============================================================================

RESERVED_WORDS = {
    "int", "string", "boolean", "process", "state", "group", "ip_address",
    "depends_on", "if", "else", "switch", "case", "default",
    "while", "for", "break", "continue", "try", "catch", "wait_seconds",
    "ping_server", "spin_up_instance", "kill_instance", "log_alert",
    "start", "stop", "restart", "enable", "disable", "status", "autostart",
    "and", "or", "not"
}

BOOL_STATE_CONSTANTS = {"true", "false", "RUNNING", "STOPPED", "ERROR"}

MULTI_OPERATORS = ["==", "!=", "<=", ">="]
SINGLE_OPERATORS = ["<", ">", "=", "+", "-", "*", "/", "%"]
SEPARATORS = set(";:,()[]{}")


def scan_source(source: str, st: SymbolTable, pif: PIF) -> int:
    line_num = 1
    pos = 0
    length = len(source)
    error_count = 0

    while pos < length:
        ch = source[pos]

        # 1. Whitespace
        if ch == '\n':
            line_num += 1
            pos += 1
            continue
        if ch in (' ', '\t', '\r'):
            pos += 1
            continue

        # 2. Block comments %% ... %%
        if source[pos:pos+2] == '%%':
            pos += 2
            while pos < length and source[pos:pos+2] != '%%':
                if source[pos] == '\n':
                    line_num += 1
                pos += 1
            if pos < length:
                pos += 2
            continue

        # 3. Single-line comments // ...
        if source[pos:pos+2] == '//':
            while pos < length and source[pos] != '\n':
                pos += 1
            continue

        # 4. Separators
        if ch in SEPARATORS:
            pif.add(ch, 0, ch, line_num)
            pos += 1
            continue

        # 5. Operators & Negative Numbers
        if ch == '-':
            # Check for unary minus context
            prev = pos - 1
            while prev >= 0 and source[prev] in (' ', '\t', '\r'):
                prev -= 1
            is_unary = (prev < 0 or source[prev] in "=( [,;:{\n")

            if is_unary and pos + 1 < length:
                # Malformed negative zero
                if source[pos + 1] == '0':
                    start = pos
                    pos += 2
                    while pos < length and (source[pos].isalnum() or source[pos] == '_'):
                        pos += 1
                    err_lexeme = source[start:pos]
                    print(f"Lexical error at line {line_num}: malformed identifier or numeric constant '{err_lexeme}'")
                    error_count += 1
                    continue

                # FA for numeric constant
                num_len = FiniteAutomata.match_number(source[pos:])
                if num_len > 0:
                    # Check for malformed trailing identifier characters
                    total = num_len
                    malformed = False
                    while pos + total < length and (source[pos + total].isalnum() or source[pos + total] == '_'):
                        total += 1
                        malformed = True
                    if malformed:
                        err_lexeme = source[pos:pos + total]
                        print(f"Lexical error at line {line_num}: malformed identifier or numeric constant '{err_lexeme}'")
                        error_count += 1
                        pos += total
                        continue

                    num_str = source[pos:pos + num_len]
                    stp = st.get_pos(num_str, "CONST")
                    pif.add("const", stp, num_str, line_num)
                    pos += num_len
                    continue

        # Multi-char operators
        matched_op = None
        for op in MULTI_OPERATORS:
            if source[pos:pos+len(op)] == op:
                matched_op = op
                break
        if not matched_op:
            for op in SINGLE_OPERATORS:
                if source[pos:pos+len(op)] == op:
                    matched_op = op
                    break

        if matched_op:
            pif.add(matched_op, 0, matched_op, line_num)
            pos += len(matched_op)
            continue

        # 6. String constants (via FA)
        if ch == '"':
            str_len = FiniteAutomata.match_string(source[pos:])
            if str_len > 0:
                str_val = source[pos:pos + str_len]
                stp = st.get_pos(str_val, "CONST")
                pif.add("const", stp, str_val, line_num)
                pos += str_len
                continue
            else:
                start = pos
                pos += 1
                while pos < length and source[pos] not in ('"', '\n'):
                    pos += 1
                if pos < length and source[pos] == '"':
                    pos += 1
                err_str = source[start:pos]
                print(f"Lexical error at line {line_num}: invalid or unclosed string constant '{err_str}'")
                error_count += 1
                continue

        # 7. Identifiers / Reserved Words / Bool & State constants (via FA)
        if ch.isalpha():
            id_len = FiniteAutomata.match_identifier(source[pos:])
            if id_len > 0:
                word = source[pos:pos + id_len]
                if word in RESERVED_WORDS:
                    pif.add(word, 0, word, line_num)
                elif word in BOOL_STATE_CONSTANTS:
                    stp = st.get_pos(word, "CONST")
                    pif.add("const", stp, word, line_num)
                else:
                    stp = st.get_pos(word, "ID")
                    pif.add("id", stp, word, line_num)
                pos += id_len
                continue

        # 8. Numeric constants (via FA)
        if ch.isdigit():
            num_len = FiniteAutomata.match_number(source[pos:])
            if num_len > 0:
                total = num_len
                malformed = False
                while pos + total < length and (source[pos + total].isalnum() or source[pos + total] == '_'):
                    total += 1
                    malformed = True
                if malformed:
                    err_lexeme = source[pos:pos + total]
                    print(f"Lexical error at line {line_num}: malformed identifier or numeric constant '{err_lexeme}'")
                    error_count += 1
                    pos += total
                    continue

                num_val = source[pos:pos + num_len]
                stp = st.get_pos(num_val, "CONST")
                pif.add("const", stp, num_val, line_num)
                pos += num_len
                continue
            else:
                start = pos
                while pos < length and (source[pos].isalnum() or source[pos] == '_'):
                    pos += 1
                err_lexeme = source[start:pos]
                print(f"Lexical error at line {line_num}: malformed identifier or numeric constant '{err_lexeme}'")
                error_count += 1
                continue

        # 9. Unrecognized token
        print(f"Lexical error at line {line_num}: unrecognized token '{ch}'")
        error_count += 1
        pos += 1

    return error_count


def main():
    input_file = sys.argv[1] if len(sys.argv) > 1 else "program1.cloud.txt"
    pif_file   = sys.argv[2] if len(sys.argv) > 2 else "PIF.out"
    st_file    = sys.argv[3] if len(sys.argv) > 3 else "ST.out"

    if not os.path.exists(input_file):
        print(f"Cannot open source program file: {input_file}")
        sys.exit(1)

    with open(input_file, "r", encoding="utf-8") as f:
        source = f.read()

    st = SymbolTable()
    pif = PIF()

    error_count = scan_source(source, st, pif)

    pif.write_file(pif_file)
    st.write_file(st_file)

    if error_count == 0:
        print(f"Lexical analysis completed successfully for '{input_file}' (0 lexical errors).")
        print(f"Generated '{pif_file}' ({len(pif.entries)} tokens) and '{st_file}' ({st.count} unique symbols).")
    else:
        print(f"Lexical analysis finished for '{input_file}' with {error_count} lexical error(s).")

if __name__ == "__main__":
    main()
