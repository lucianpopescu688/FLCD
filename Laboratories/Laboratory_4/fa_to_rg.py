class FiniteAutomaton:
    def __init__(self):
        self.states = {"q0", "q1"}
        self.start_state = "q0"
        self.final_states = {"q1"}
        self.transitions = {}

    def add_transition(self, from_state, symbol_class, to_state):
        if from_state not in self.transitions:
            self.transitions[from_state] = []
        self.transitions[from_state].append((symbol_class, to_state))


def convert_fa_to_rg(fa: FiniteAutomaton):
    productions = []
    # Mapping FA states to Grammar Non-Terminals
    state_to_nt = {"q0": "S", "q1": "A", "q2": "B", "q3": "C"}

    for state, trans_list in fa.transitions.items():
        lhs = state_to_nt[state]
        for symbol_class, target_state in trans_list:
            rhs_nt = state_to_nt[target_state]

            # Check if target state has outgoing transitions
            target_has_transitions = target_state in fa.transitions and len(fa.transitions[target_state]) > 0

            # 1. Q_i -> symbol Q_j
            # (Only add this if the target state actually goes somewhere, avoiding dead Non-Terminals)
            if target_has_transitions or target_state not in fa.final_states:
                productions.append(f"{lhs} -> {symbol_class} {rhs_nt}")

            # 2. If Q_j is final, add terminating rule Q_i -> symbol
            if target_state in fa.final_states:
                productions.append(f"{lhs} -> {symbol_class}")

    return productions


if __name__ == "__main__":
    # Define the FA for Cloud Programming Language Identifiers
    fa = FiniteAutomaton()
    fa.add_transition("q0", "letter", "q1")
    fa.add_transition("q1", "letter", "q1")
    fa.add_transition("q1", "digit", "q1")
    fa.add_transition("q1", "underscore", "q1")

    print("=== FINITE AUTOMATON (FA) MODEL: IDENTIFIERS ===")
    print(f"States: {fa.states}")
    print(f"Start State: {fa.start_state}")
    print(f"Final States: {fa.final_states}")

    print("\n=== GENERATED REGULAR GRAMMAR (RG) PRODUCTIONS: IDENTIFIERS ===")
    rg_productions = convert_fa_to_rg(fa)

    unique_productions = list(dict.fromkeys(rg_productions))
    for prod in unique_productions:
        print(prod)

    # Define the FA for Cloud Programming Language Numeric Constants
    fa_const = FiniteAutomaton()
    fa_const.states = {"q0", "q1", "q2", "q3"}
    fa_const.final_states = {"q1", "q3"}

    fa_const.add_transition("q0", "0", "q1")
    fa_const.add_transition("q0", "-", "q2")
    fa_const.add_transition("q0", "nonzero_digit", "q3")
    fa_const.add_transition("q2", "nonzero_digit", "q3")
    fa_const.add_transition("q3", "digit", "q3")

    print("\n=== FINITE AUTOMATON (FA) MODEL: CONSTANTS ===")
    print(f"States: {fa_const.states}")
    print(f"Start State: {fa_const.start_state}")
    print(f"Final States: {fa_const.final_states}")

    print("\n=== GENERATED REGULAR GRAMMAR (RG) PRODUCTIONS: CONSTANTS ===")
    rg_const_productions = convert_fa_to_rg(fa_const)

    unique_const_productions = list(dict.fromkeys(rg_const_productions))
    for prod in unique_const_productions:
        print(prod)

    # Define the FA for Cloud Programming Language String Constants
    fa_string = FiniteAutomaton()
    fa_string.states = {"q0", "q1", "q2"}
    fa_string.final_states = {"q2"}

    fa_string.add_transition("q0", "quote", "q1")
    fa_string.add_transition("q1", "char", "q1")
    fa_string.add_transition("q1", "quote", "q2")

    print("\n=== FINITE AUTOMATON (FA) MODEL: STRINGS ===")
    print(f"States: {fa_string.states}")
    print(f"Start State: {fa_string.start_state}")
    print(f"Final States: {fa_string.final_states}")

    print("\n=== GENERATED REGULAR GRAMMAR (RG) PRODUCTIONS: STRINGS ===")
    rg_string_productions = convert_fa_to_rg(fa_string)

    unique_string_productions = list(dict.fromkeys(rg_string_productions))
    for prod in unique_string_productions:
        print(prod)