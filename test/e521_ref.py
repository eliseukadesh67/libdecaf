#!/usr/bin/env python3
"""Independent, from-scratch reference for the E-521 twisted Edwards curve.

Parameters are the canonical ones from https://neuromancer.sk/std/other/E-521 :
    p = 2**521 - 1
    a = 1,  d = -376014           (Edwards form  a*x^2 + y^2 = 1 + d*x^2*y^2)
    cofactor h = 4
    base point  G = (Gx, 12)
    prime order n (of the base point).

Only Python big integers are used, so this depends on nothing else and can serve
as a trustworthy oracle for the libdecaf E-521 point arithmetic and encoding.
"""

p  = 2**521 - 1
a  = 1
d  = (-376014) % p
Gx = 0x752cb45c48648b189df90cb2296b2878a3bfd9f42fc6c818ec8bf3c9c0c6203913f6ecc5ccc72434b1ae949d568fc99c6059d0fb13364838aa302a940a2f19ba6c
Gy = 12
n  = 2**519 - 337554763258501705789107630418782636071904961214051226618635150085779108655765

def inv(x):
    return pow(x % p, p - 2, p)

def on_curve(P):
    x, y = P
    return (a*x*x + y*y - 1 - d*x*x*y*y) % p == 0

def add(P, Q):
    """Complete twisted-Edwards addition (a=1)."""
    x1, y1 = P; x2, y2 = Q
    dxy = (d * x1 * x2 * y1 * y2) % p
    x3 = ((x1*y2 + y1*x2) * inv(1 + dxy)) % p
    y3 = ((y1*y2 - a*x1*x2) * inv(1 - dxy)) % p
    return (x3, y3)

def scalarmul(k, P):
    R = (0, 1)          # neutral element of a twisted Edwards curve
    k %= n
    while k:
        if k & 1:
            R = add(R, P)
        P = add(P, P)
        k >>= 1
    return R

def encode(P):
    """EdDSA-style point encoding: y little-endian in 66 bytes, x parity in bit 0x80 of the last byte."""
    x, y = P
    e = bytearray(y.to_bytes(66, 'little'))
    if x & 1:
        e[65] |= 0x80
    return bytes(e)

if __name__ == "__main__":
    G = (Gx, Gy)
    assert on_curve(G), "generator not on curve!"
    assert scalarmul(n, G) == (0, 1), "base point order != n!"
    for k in range(1, 7):
        print(f"k={k} enc=" + encode(scalarmul(k, G)).hex())
