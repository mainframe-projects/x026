/*
 * Punched-card codes for those ASCII characters that have 94-character
 * EBCDIC graphic equivalents
 */
#define z12	0x001
#define z11	0x002
#define n0	0x004
#define n1	0x008
#define n2	0x010
#define	n3	0x020
#define n4	0x040
#define n5	0x080
#define n6	0x100
#define n7	0x200
#define n8	0x400
#define n9	0x800
#define NS	0xfff

/* BCD-H (026 FORTRAN) */
unsigned hollerith_bcd_h[256] = {
	NS, NS, NS, NS, NS, NS, NS, NS,		/* 0x00 .. 0x07 */
	NS, NS, NS, NS, NS, NS, NS, NS,		/* 0x08 .. 0x0f */
	NS, NS, NS, NS, NS, NS, NS, NS,		/* 0x10 .. 0x17 */
	NS, NS, NS, NS, NS, NS, NS, NS,		/* 0x18 .. 0x1f */
	0,		/* space */
	NS,		/* ! */
	NS,		/* " */
	NS,		/* # */
	z11 | n8 | n3,	/* $ */
	NS,		/* % */
	NS,		/* & */
	n8 | n4,	/* ' */
	n0 | n8 | n4,	/* ( */
	z12 | n8 | n4,	/* ) */
	z11 | n8 | n4,	/* * */
	z12,		/* + */
	n0 | n8 | n3,	/* , */
	z11,		/* - */
	z12 | n8 | n3,	/* . */
	n0 | n1,	/* / */
	n0,		/* 0 */
	n1,		/* 1 */
	n2,		/* 2 */
	n3,		/* 3 */
	n4,		/* 4 */
	n5,		/* 5 */
	n6,		/* 6 */
	n7,		/* 7 */
	n8,		/* 8 */
	n9,		/* 9 */
	NS,		/* : */
	NS,		/* ; */
	NS,		/* < */
	n8 | n3,	/* = */
	NS,		/* > */
	NS,		/* ? */
	NS,		/* @ */
	z12 | n1,	/* A */
	z12 | n2,	/* B */
	z12 | n3,	/* C */
	z12 | n4,	/* D */
	z12 | n5,	/* E */
	z12 | n6,	/* F */
	z12 | n7,	/* G */
	z12 | n8,	/* H */
	z12 | n9,	/* I */
	z11 | n1,	/* J */
	z11 | n2,	/* K */
	z11 | n3,	/* L */
	z11 | n4,	/* M */
	z11 | n5,	/* N */
	z11 | n6,	/* O */
	z11 | n7,	/* P */
	z11 | n8,	/* Q */
	z11 | n9,	/* R */
	n0 | n2,	/* S */
	n0 | n3,	/* T */
	n0 | n4,	/* U */
	n0 | n5,	/* V */
	n0 | n6,	/* W */
	n0 | n7,	/* X */
	n0 | n8,	/* Y */
	n0 | n9,	/* Z */
	NS, NS, NS, NS, NS, NS, NS, NS, NS, NS, NS, NS,
	NS, NS, NS, NS, NS, NS, NS, NS, NS, NS, NS, NS,
	NS, NS, NS, NS, NS, NS, NS, NS, NS, NS, NS, NS,
	NS,

	NS, NS, NS, NS, NS, NS, NS, NS,
	NS, NS, NS, NS, NS, NS, NS, NS,
	NS, NS, NS, NS, NS, NS, NS, NS,
	NS, NS, NS, NS, NS, NS, NS, NS,
	NS, NS, NS, NS, NS, NS, NS, NS,
	NS, NS, NS, NS, NS, NS, NS, NS,
	NS, NS, NS, NS, NS, NS, NS, NS,
	NS, NS, NS, NS, NS, NS, NS, NS,
	NS, NS, NS, NS, NS, NS, NS, NS,
	NS, NS, NS, NS, NS, NS, NS, NS,
	NS, NS, NS, NS, NS, NS, NS, NS,
	NS, NS, NS, NS, NS, NS, NS, NS,
	NS, NS, NS, NS, NS, NS, NS, NS,
	NS, NS, NS, NS, NS, NS, NS, NS,
	NS, NS, NS, NS, NS, NS, NS, NS,
	NS, NS, NS, NS, NS, NS, NS, NS
};

/* BCD-A (026 commercial) */
unsigned hollerith_bcd_a[256] = {
	NS, NS, NS, NS, NS, NS, NS, NS,		/* 0x00 .. 0x07 */
	NS, NS, NS, NS, NS, NS, NS, NS,		/* 0x08 .. 0x0f */
	NS, NS, NS, NS, NS, NS, NS, NS,		/* 0x10 .. 0x17 */
	NS, NS, NS, NS, NS, NS, NS, NS,		/* 0x18 .. 0x1f */
	0,		/* space */
	NS,		/* ! */
	NS,		/* " */
	n8 | n3,	/* # */
	z11 | n8 | n3,	/* $ */
	n0 | n8 | n4,	/* % */
	z12,		/* & */
	NS,		/* ' */
	NS,		/* ( */
	NS,		/* ) */ /* z12 | n8 | n4 */
	z11 | n8 | n4,	/* * */
	NS,		/* + */
	n0 | n8 | n3,	/* , */
	z11,		/* - */
	z12 | n8 | n3,	/* . */
	n0 | n1,	/* / */
	n0,		/* 0 */
	n1,		/* 1 */
	n2,		/* 2 */
	n3,		/* 3 */
	n4,		/* 4 */
	n5,		/* 5 */
	n6,		/* 6 */
	n7,		/* 7 */
	n8,		/* 8 */
	n9,		/* 9 */
	NS,		/* : */
	NS,		/* ; */
	NS,		/* < */
	NS,		/* = */
	NS,		/* > */
	NS,		/* ? */
	n8 | n4,	/* @ */
	z12 | n1,	/* A */
	z12 | n2,	/* B */
	z12 | n3,	/* C */
	z12 | n4,	/* D */
	z12 | n5,	/* E */
	z12 | n6,	/* F */
	z12 | n7,	/* G */
	z12 | n8,	/* H */
	z12 | n9,	/* I */
	z11 | n1,	/* J */
	z11 | n2,	/* K */
	z11 | n3,	/* L */
	z11 | n4,	/* M */
	z11 | n5,	/* N */
	z11 | n6,	/* O */
	z11 | n7,	/* P */
	z11 | n8,	/* Q */
	z11 | n9,	/* R */
	n0 | n2,	/* S */
	n0 | n3,	/* T */
	n0 | n4,	/* U */
	n0 | n5,	/* V */
	n0 | n6,	/* W */
	n0 | n7,	/* X */
	n0 | n8,	/* Y */
	n0 | n9,	/* Z */
	NS, NS, NS, NS, NS, NS, NS, NS, NS, NS, NS, NS,
	NS, NS, NS, NS, NS, NS, NS, NS, NS, NS, NS, NS,
	NS, NS, NS, NS, NS, NS, NS, NS, NS, NS, NS, NS,
	NS,

	NS, NS, NS, NS, NS, NS, NS, NS,	/* 0x80 .. 0x87 */
	NS, NS, NS, NS, NS, NS, NS, NS,	/* 0x88 .. 0x8f */
	NS, NS, NS, NS, NS, NS, NS, NS,	/* 0x90 .. 0x97 */
	NS, NS, NS, NS, NS, NS, NS, NS,	/* 0x98 .. 0x9f */
	NS, NS, NS, NS,			/* 0xa0 .. 0xa3 */
	z12 | n8 | n4,	/* currency symbol */
	                    NS, NS, NS, /* 0xa5 .. 0xa7 */
	NS, NS, NS, NS, NS, NS, NS, NS,
	NS, NS, NS, NS, NS, NS, NS, NS,
	NS, NS, NS, NS, NS, NS, NS, NS,
	NS, NS, NS, NS, NS, NS, NS, NS,
	NS, NS, NS, NS, NS, NS, NS, NS,
	NS, NS, NS, NS, NS, NS, NS, NS,
	NS, NS, NS, NS, NS, NS, NS, NS,
	NS, NS, NS, NS, NS, NS, NS, NS,
	NS, NS, NS, NS, NS, NS, NS, NS,
	NS, NS, NS, NS, NS, NS, NS, NS,
	NS, NS, NS, NS, NS, NS, NS, NS
};
