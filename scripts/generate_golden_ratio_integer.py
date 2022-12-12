#!/usr/bin/env python3
"""Generate integers using golden ration."""

import mpmath


def generate_golden_ratio_integer():
    mpmath.mp.prec = 100

    golden_ratio = (mpmath.mpf(1) + mpmath.sqrt(mpmath.mpf(5))) / mpmath.mpf(2)
    print(f"Golden ratio: {golden_ratio}")

    integer32 = int(mpmath.power(mpmath.mpf(2), 32) / golden_ratio)
    print(f"32-bit integer: {integer32:X}")

    integer64 = int(mpmath.power(mpmath.mpf(2), 64) / golden_ratio)
    print(f"64-bit integer: {integer64:X}")


if __name__ == "__main__":
    generate_golden_ratio_integer()
