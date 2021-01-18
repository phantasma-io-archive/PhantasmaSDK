from enum import IntEnum


class VMType(IntEnum):
    NONE = 0
    STRUCT = 1
    BYTES = 2
    NUMBER = 3
    STRING = 4
    TIMESTAMP = 5
    BOOL = 6
    ENUM = 7
    OBJECT = 8
