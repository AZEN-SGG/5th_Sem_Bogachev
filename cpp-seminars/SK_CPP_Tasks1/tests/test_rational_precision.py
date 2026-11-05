from fractions import Fraction
from pathlib import Path
import random
import struct
import subprocess
import sys


LIMIT = 1 << 63


def float_value(bits):
    return struct.unpack('<f', struct.pack('<I', bits))[0]


def rounded_bits(value):
    sign = 0x80000000 if value < 0 else 0
    value = abs(value)
    first, last = 0, 0x7f7fffff
    while first < last:
        middle = (first + last + 1) // 2
        if Fraction(float_value(middle)) <= value:
            first = middle
        else:
            last = middle - 1
    lower = Fraction(float_value(first))
    upper = Fraction(float_value(first + 1))
    if upper - value < value - lower:
        first += 1
    elif upper - value == value - lower and first & 1:
        first += 1
    return sign | first


def main():
    generator = random.Random(20261016)
    mantissas = (0, 1, 2, 3, (1 << 22) - 1, 1 << 22,
                 (1 << 23) - 2, (1 << 23) - 1)
    inputs = {sign | exponent << 23 | mantissa
              for sign in (0, 0x80000000)
              for exponent in range(256)
              for mantissa in mantissas}
    inputs.update(generator.getrandbits(32) for _ in range(10000))
    commands = []
    expected = []
    for bits in sorted(inputs):
        commands.append(f'f {bits:08x}\n')
        if bits & 0x7f800000 == 0x7f800000:
            expected.append('nan' if bits & 0x7fffff else
                            ('negative infinity' if bits >> 31 else 'infinity'))
        else:
            value = Fraction(float_value(bits))
            if abs(value) >= LIMIT:
                value = Fraction(-LIMIT if value < 0 else LIMIT)
            else:
                value = value.limit_denominator(LIMIT)
            expected.append(value)

    pairs = [(0, 1), (-LIMIT, 1), (-LIMIT, -1), (1, -LIMIT),
             (-1, -LIMIT), (-LIMIT, -LIMIT)]
    pairs.extend((generator.randrange(-LIMIT, LIMIT),
                  generator.randrange(1, LIMIT)) for _ in range(1000))
    for bits in (0x3f000000, 0x3f800000, 0x3f800001, 0x40000000,
                 0x3dcccccc, 0x447a0000):
        midpoint = (Fraction(float_value(bits))
                    + Fraction(float_value(bits + 1))) / 2
        denominator = min(LIMIT - 1, (LIMIT - 2) // midpoint)
        for offset in range(8):
            divisor = denominator - offset
            numerator = midpoint.numerator * divisor // midpoint.denominator
            pairs.extend((sign * num, divisor)
                         for sign in (-1, 1)
                         for num in (numerator, numerator + 1))
    for num, denom in pairs:
        commands.append(f'r {num} {denom}\n')
        expected.append(Fraction(num, denom))

    executable = Path(sys.argv[1]).resolve()
    result = subprocess.run([str(executable)], input=''.join(commands),
                            capture_output=True, text=True, check=True)
    if result.stderr:
        raise AssertionError(result.stderr)
    lines = result.stdout.splitlines()
    if len(lines) != len(expected):
        raise AssertionError('Wrong number of results')
    for command, line, reference in zip(commands, lines, expected):
        raw_num, raw_denom, raw_bits = line.split()
        num, denom, bits = int(raw_num), int(raw_denom), int(raw_bits, 16)
        if not -LIMIT <= num < LIMIT or not -LIMIT <= denom < LIMIT:
            raise AssertionError((command, 'Integer range', line))
        if isinstance(reference, str):
            if reference == 'nan':
                valid = denom == 0 and num == 0
                valid &= bits & 0x7f800000 == 0x7f800000 and bits & 0x7fffff != 0
            else:
                negative = reference == 'negative infinity'
                valid = denom == 0 and num == (-1 if negative else 1)
                valid &= bits == (0xff800000 if negative else 0x7f800000)
            if not valid:
                raise AssertionError((command, reference, line))
            continue
        if not denom or Fraction(num, denom) != reference:
            raise AssertionError((command, str(reference), line))
        if bits != rounded_bits(reference):
            raise AssertionError((command, 'Float rounding', line))
    print(f'Exact rational and float rounding checks: {len(expected)}, errors: 0')


if __name__ == '__main__':
    main()
