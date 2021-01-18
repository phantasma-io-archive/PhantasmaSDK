from enum import IntEnum


class Opcode(IntEnum):
    NOP = 0
    # Register
    MOVE = 1
    COPY = 2
    PUSH = 3
    POP = 4
    SWAP = 5
    # Flow
    CALL = 6
    EXTCALL = 7
    JMP = 8
    JMPIF = 9
    JMPNOT = 10
    RET = 11
    THROW = 12
    # Data
    LOAD = 13
    CAST = 14
    CAT = 15
    SUBSTR = 16
    LEFT = 17
    RIGHT = 18
    SIZE = 19
    COUNT = 20
    # Logical
    NOT = 21
    AND = 22
    OR = 23
    XOR = 24
    EQUAL = 25
    LT = 26
    GT = 27
    LTE = 28
    GTE = 29
    # Numeric
    INC = 30
    DEC = 31
    SIGN = 32
    NEGATE = 33
    ABS = 34
    ADD = 35
    SUB = 36
    MUL = 37
    DIV = 38
    MOD = 39
    SHL = 40
    SHR = 41
    MIN = 42
    MAX = 43
    # Context
    THIS = 44
    CTX = 45
    SWITCH = 46
    # Array
    PUT = 47
    GET = 48
