#ifndef BINARY_H
#define BINARY_H
//////////////////////////////////////////////////////////////////////
// Macros for expressing a constant as binary
//////////////////////////////////////////////////////////////////////

#define _ashex(n) (0x ## n ## UL)
#define _movebit(from, to, n) ((n>>(from-to)) & (1<<to))

#define _binconst(n) (\
		_movebit(0, 0, n) | \
		_movebit(4, 1, n) | \
		_movebit(8, 2, n) | \
		_movebit(12, 3, n) | \
		_movebit(16, 4, n) | \
		_movebit(20, 5, n) | \
		_movebit(24, 6, n) | \
		_movebit(28, 7, n) )

#define binconst(n) _binconst(_ashex(n))

#define TILE(a, b, c, d, e, f, g, h) \
    _binconst(_ashex(a)), \
    _binconst(_ashex(b)), \
    _binconst(_ashex(c)), \
    _binconst(_ashex(d)), \
    _binconst(_ashex(e)), \
    _binconst(_ashex(f)), \
    _binconst(_ashex(g)), \
    _binconst(_ashex(h)), \
    _binconst(_ashex(a) >> 1), \
    _binconst(_ashex(b) >> 1), \
    _binconst(_ashex(c) >> 1), \
    _binconst(_ashex(d) >> 1), \
    _binconst(_ashex(e) >> 1), \
    _binconst(_ashex(f) >> 1), \
    _binconst(_ashex(g) >> 1), \
    _binconst(_ashex(h) >> 1)

#endif
