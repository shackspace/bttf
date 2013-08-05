#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <avr/pgmspace.h>

#define a (1 << 8)
#define b (1 << 9)
#define c (1 << 11)
#define d (1 << 14)
#define e (1 << 15)
#define f (1 << 0)
#define g (1 << 1)
#define h (1 << 5)
#define k (1 << 6)
#define m (1 << 7)
#define n (1 << 10)
#define p (1 << 12)
#define s (1 << 3)
#define r (1 << 13)
#define t (1 << 2)
#define u (1 << 4)

const PROGMEM uint16_t font16['Z' - '0' + 1] = {
	/* 0 */ a | b | c | d | e | f | g | h | n | t,
	/* 1 */ c | d | n,
	/* 2 */ a | b | c | p | u | g | f | e,
	/* 3 */ a | b | c | p | u | d | f | e,
	/* 4 */ h | u | p | c | d,
	/* 5 */ b | a | h | u | p | d | e | f,
	/* 6 */ b | a | h | g | f | e | d | u | p,
	/* 7 */ a | b | c | d,
	/* 8 */ a | b | c | d | e | f | g | h | u | p,
	/* 9 */ a | b | c | d | e | f | p | u | h,
	/* some special chars - WARNING: these might not be what they seem ;) */
	/* : */ f,
	/* ; */ m | t,
	/* < */ n | r,
	/* = */ 0,
	/* > */ b | c | d | e,
	/* ? */ a | b | n | r | e | f,
	/* @ */ 0,
	/* A */ g | h | a | b | c | d | p | u,
	/* B */ a | b | c | d | e | f | m | s | p,
	/* C */ a | b | g | h | e | f,
	/* D */ a | b | c | d | e | f | m | s,
	/* E */ a | b | g | h | e | f | u,
	/* F */ a | b | g | h | u,
	/* G */ a | b | g | h | e | f | d | p,
	/* H */ c | d | g | h | p | u,
	/* I */ a | b | m | s | e | f,
	/* J */ c | d | e | f | g,
	/* K */ g | h | u | n | r,
	/* L */ g | h | e | f,
	/* M */ g | h | k | n | c | d,
	/* N */ g | h | k | r | c | d,
	/* O */ a | b | c | d | e | f | g | h,
	/* P */ a | b | c | p | u | g | h,
	/* Q */ a | b | c | d | e | f | g | h | r,
	/* R */ a | b | c | p | u | r | g | h,
	/* S */ a | b | h | u | r | e | f,
	/* T */ a | b | m | s,
	/* U */ c | d | e | f | g | h,
	/* V */ g | h | n | t,
	/* W */ c | d | e | f | g | h | s,
	/* X */ k | n | r | t,
	/* Y */ c | p | u | h | s,
	/* Z */ a | b | n | t | e | f
};

#undef a
#undef b
#undef c
#undef d
#undef e
#undef f
#undef g
#undef h
#undef k
#undef m
#undef n
#undef p
#undef r
#undef s
#undef t
#undef u

/* 7-segment, first half */

#define a (1 << 7)
#define b (1 << 6)
#define c (1 << 13)
#define d (1 << 4)
#define e (1 << 5)
#define f (1 << 11)
#define g (1 << 12)
#define FONTNAME font7a
#include "font7.h"
#undef FONTNAME
#undef a
#undef b
#undef c
#undef d
#undef e
#undef f
#undef g

/* 7-segment, second half */

#define a (1 << 9)
#define b (1 << 10)
#define c (1 << 1)
#define d (1 << 15)
#define e (1 << 3)
#define f (1 << 8)
#define g (1 << 2)
#define FONTNAME font7b
#include "font7.h"
#undef FONTNAME
#undef a
#undef b
#undef c
#undef d
#undef e
#undef f
#undef g

