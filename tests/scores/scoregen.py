import re
from typing import ByteString
from typing import List
from typing import Sequence
from typing import Tuple
from typing import TypeAlias
from typing import Union

Event: TypeAlias = Union[int, Tuple[int, int]]
EventList: TypeAlias = List[Event]
EventSeq: TypeAlias = Sequence[Event]

CLOCK = 14.31818e6

SLOT_SUBADDR = [
     0,  1,  2,  3,  4,  5,  8,  9,
    10, 11, 12, 13, 16, 17, 18, 19,
    20, 21, 32, 33, 34, 35, 36, 37,
    40, 41, 42, 43, 44, 45, 48, 49,

    50, 51, 52, 53, 24, 25, 26, 27,
    28, 29, 56, 57, 58, 59, 60, 61,
     6,  7, 14, 15, 22, 23, 30, 31,
    38, 39, 46, 47, 54, 55, 62, 63
]

CH2X_SUBADDR = [
     0,  1,  2,  3,  4,  5,  6,  7,  8,
    16, 17, 18, 19, 20, 21, 22,

    23, 24,  9, 10, 11, 12, 13, 14, 15,
    25, 26, 27, 28, 29, 30, 31
]

SPN_REGEX = re.compile(
    r'^\s*(?P<tone>[A-G])'
    r'\s*(?P<variant>b+|#+|)'
    r'\s*(?P<octave>[+\-]\s*?[0-9]*)'
    r'\s*$'
)


def midi_tet_to_hz(note: float) -> float:
    hz = 440. * 2.**((note - 69.) / 12.)
    return hz


def spn_tet_to_hz(code: str) -> float:
    m = SPN_REGEX.match(code)
    g = m.groupdict()
    tone = ord(g['tone']) - ord('A')
    variant = (g['variant'].count('#') - g['variant'].count('b')) / 2
    octave = int(g['octave']) or 4
    note = 12*octave + tone + variant
    return midi_tet_to_hz(note)


def hz_to_fnum(
    freq: float,
    block: int = 0,
    clock: float = CLOCK,
) -> int:

    if freq < 0:
        raise ValueError('negative freq')

    block = block.__index__()
    if not 0 <= block <= 7:
        raise ValueError('invalid block')

    if not 10e6 <= clock <= 16e6:
        raise ValueError('invalid clock')

    fk = 2 ** 19
    fs = clock / 288
    bdiv = 2 ** (block - 1)
    fnum = freq * fk / fs / bdiv
    inum = round(fnum)

    if not 0 <= inum <= 0x3FF:
        raise ValueError('fnum overflow')

    return inum.__index__()


def ms_to_tick(
    ms: float,
    clock: float = CLOCK,
) -> int:

    if ms < 0:
        raise ValueError('negative time')

    if not 10e6 <= clock <= 16e6:
        raise ValueError('invalid clock')

    s = ms / 1000
    ts = 288 / clock
    r = s / ts
    tick = round(r)
    return tick.__index__()


def events_to_dro(events: EventSeq) -> ByteString:

    total_ms = 0
    total_size = 0
    prev_addrhi = 0
    buffer = bytearray()
    append = buffer.append
    extend = buffer.extend

    extend(b'DBRAWOPL')  # magic
    extend(b'\x00\x01')  # version 1.0

    extend(b'\x00\x00\x00\x00')  # milliseconds (to be patched)
    extend(b'\x00\x00\x00\x00')  # size (to be patched)

    extend(b'\x01\x01\x00\x00')  # OPL3 + legacy 1 ms delay

    for event in events:
        try:
            address, value = event

        except TypeError:
            tick = ms_to_tick(event)
            total_ms += tick

            while tick:
                if tick <= 0x100:
                    append(0x00)  # delay, UINT8
                    append(tick - 1)
                    total_size += 2
                    tick = 0
                else:
                    word = (tick - 1) & 0xFFFF
                    append(0x01)  # delay, UINT16LE
                    append(word & 0xFF)
                    append(word >> 8)
                    total_size += 3
                    tick -= word
        else:
            address = address.__index__()
            if not 0 <= address <= 0x1FF:
                raise ValueError('invalid address')

            value = value.__index__()
            if not 0 <= value <= 0xFF:
                raise ValueError('invalid value')

            addrhi = address >> 8
            if addrhi < prev_addrhi:
                append(0x02)  # OPL low
                total_size += 1
            elif addrhi > prev_addrhi:
                append(0x03)  # OPL high
                total_size += 1
            prev_addrhi = addrhi

            addrlo = address & 0xFF
            if addrlo <= 0x04:
                append(0x04)  # escape
                total_size += 1

            append(addrlo)  # register
            append(value)  # value
            total_size += 2

    buffer[10:14] = total_ms.to_bytes(4, 'little')
    buffer[14:18] = total_size.to_bytes(4, 'little')
