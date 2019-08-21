/*
 * Copyright 1993, 1995, 2002, 2003 Paul Mattes.  All Rights Reserved.
 *   Permission to use, copy, modify, and distribute this software and its
 *   documentation for any purpose and without fee is hereby granted,
 *   provided that the above copyright notice appear in all copies and that
 *   both that copyright notice and this permission notice appear in
 *   supporting documentation.
 *
 * x026 -- A Keypunch Simluator
 */

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/Core.h>
#include <X11/Shell.h>
#include <X11/Xatom.h>
#include <X11/Xaw/Command.h>
#include <X11/Xaw/Dialog.h>
#include <X11/Xaw/Porthole.h>
#include <X11/xpm.h>

#include "collins.xpm"		/* card image */
#include "carnegie2.xpm"	/* card image */
#include "hole.xpm"		/* hole image */

#include "x026.bm"		/* icon */

/* Symbolic code for 'no such translation.' */
#define NS	0xffffffffU

/*
 * Character sets.
 * A character set is a 256-element mapping from ISO 8859-1 to punched
 * card code.  The card codes are defined per Douglas Jones's convention:
 *   numeric 9	00001
 *   numeric 8  00002
 *   numeric 7  00004
 *   numeric 6  00010
 *   numeric 5  00020
 *   numeric 4  00040
 *   numeric 3  00100
 *   numeric 2  00200
 *   numeric 1  00400
 *   zone 10    01000
 *   zone 11    02000
 *   zone 12    04000
 *
 * Some definitions are incomplete, for characters which are not represented
 * in ISO 8859-1.  When this program has an interactive keyboard, these will
 * probably be added.
 *
 * There is also no attempt to map control codes between 8859-1 and punched
 * card code.
 */
struct charset {
	char *name;
	unsigned char punch_type; /* for image save */
	unsigned charset[256];
} charsets[] = {
    /* 026 keypunch FORTRAN, the default. */
    { "bcd-h", 0x10,
      {    NS,    NS,    NS,    NS,    NS,    NS,    NS,    NS, /*          */
           NS,    NS,    NS,    NS,    NS,    NS,    NS,    NS, /*          */
           NS,    NS,    NS,    NS,    NS,    NS,    NS,    NS, /*          */
           NS,    NS,    NS,    NS,    NS,    NS,    NS,    NS, /*          */
        00000,    NS,    NS,    NS, 02102,    NS,    NS, 00042, /*  !"#$%&' */
        01042, 04042, 02042, 04000, 01102, 02000, 04102, 01400, /* ()*+,-./ */
        01000, 00400, 00200, 00100, 00040, 00020, 00010, 00004, /* 01234567 */
        00002, 00001,    NS,    NS,    NS, 00102,    NS,    NS, /* 89:;<=>? */
           NS, 04400, 04200, 04100, 04040, 04020, 04010, 04004, /* @ABCDEFG */
        04002, 04001, 02400, 02200, 02100, 02040, 02020, 02010, /* HIJKLMNO */
        02004, 02002, 02001, 01200, 01100, 01040, 01020, 01010, /* PQRSTUVW */
        01004, 01002, 01001,    NS,    NS,    NS,    NS,    NS, /* XYZ[\]^_ */
           NS,    NS,    NS,    NS,    NS,    NS,    NS,    NS, /* `abcdefg */
           NS,    NS,    NS,    NS,    NS,    NS,    NS,    NS, /* hijklmno */
           NS,    NS,    NS,    NS,    NS,    NS,    NS,    NS, /* pqrstuvw */
           NS,    NS,    NS,    NS,    NS,    NS,    NS,    NS, /* xyz{|}~  */
           NS,    NS,    NS,    NS,    NS,    NS,    NS,    NS, /*          */
           NS,    NS,    NS,    NS,    NS,    NS,    NS,    NS, /*          */
           NS,    NS,    NS,    NS,    NS,    NS,    NS,    NS, /*          */
           NS,    NS,    NS,    NS,    NS,    NS,    NS,    NS, /*          */
           NS,    NS,    NS,    NS,    NS,    NS,    NS,    NS, /*  ¡¢£¤¥¦§ */
           NS,    NS,    NS,    NS,    NS,    NS,    NS,    NS, /* ¨©ª«¬­®¯ */
           NS,    NS,    NS,    NS,    NS,    NS,    NS,    NS, /* °±²³´µ¶· */
           NS,    NS,    NS,    NS,    NS,    NS,    NS,    NS, /* ¸¹º»¼½¾¿ */
           NS,    NS,    NS,    NS,    NS,    NS,    NS,    NS, /* ÀÁÂÃÄÅÆÇ */
           NS,    NS,    NS,    NS,    NS,    NS,    NS,    NS, /* ÈÉÊËÌÍÎÏ */
           NS,    NS,    NS,    NS,    NS,    NS,    NS,    NS, /* ÐÑÒÓÔÕÖ× */
           NS,    NS,    NS,    NS,    NS,    NS,    NS,    NS, /* ØÙÚÛÜÝÞß */
           NS,    NS,    NS,    NS,    NS,    NS,    NS,    NS, /* àáâãäåæç */
           NS,    NS,    NS,    NS,    NS,    NS,    NS,    NS, /* èéêëìíîï */
           NS,    NS,    NS,    NS,    NS,    NS,    NS,    NS, /* ðñòóôõö÷ */
           NS,    NS,    NS,    NS,    NS,    NS,    NS,    NS  /* øùúûüýþÿ */
      }
    },
    /* Original S/360 EBCDIC. */
    { "ebcdic", 0x20,
      {    NS,    NS,    NS,    NS,    NS,    NS,    NS,    NS, /*          */
           NS,    NS,    NS,    NS,    NS,    NS,    NS,    NS, /*          */
           NS,    NS,    NS,    NS,    NS,    NS,    NS,    NS, /*          */
           NS,    NS,    NS,    NS,    NS,    NS,    NS,    NS, /*          */
        00000, 04006, 00006, 00102, 02102, 01042, 04000, 00022, /*  !"#$%&' */
        04022, 02022, 02042, 04012, 01102, 02000, 04102, 01400, /* ()*+,-./ */
        01000, 00400, 00200, 00100, 00040, 00020, 00010, 00004, /* 01234567 */
        00002, 00001, 00202, 02012, 04042, 00012, 01012, 01006, /* 89:;<=>? */
        00042, 04400, 04200, 04100, 04040, 04020, 04010, 04004, /* @ABCDEFG */
        04002, 04001, 02400, 02200, 02100, 02040, 02020, 02010, /* HIJKLMNO */
        02004, 02002, 02001, 01200, 01100, 01040, 01020, 01010, /* PQRSTUVW */
        01004, 01002, 01001,    NS,    NS,    NS,    NS, 01022, /* XYZ[\]^_ */
           NS, 05400, 05200, 05100, 05040, 05020, 05010, 05004, /* `abcdefg */
        05002, 05001, 06400, 06200, 06100, 06040, 06020, 06010, /* hijklmno */
        06004, 06002, 06001, 03200, 03100, 03040, 03020, 03010, /* pqrstuvw */
        03004, 03002, 03001,    NS, 04006,    NS,    NS,    NS, /* xyz{|}~  */
           NS,    NS,    NS,    NS,    NS,    NS,    NS,    NS, /*          */
           NS,    NS,    NS,    NS,    NS,    NS,    NS,    NS, /*          */
           NS,    NS,    NS,    NS,    NS,    NS,    NS,    NS, /*          */
           NS,    NS,    NS,    NS,    NS,    NS,    NS,    NS, /*          */
           NS,    NS, 04202,    NS,    NS,    NS,    NS,    NS, /*  ¡¢£¤¥¦§ */
           NS,    NS,    NS,    NS, 02006,    NS,    NS,    NS, /* ¨©ª«¬­®¯ */
           NS,    NS,    NS,    NS,    NS,    NS,    NS,    NS, /* °±²³´µ¶· */
           NS,    NS,    NS,    NS,    NS,    NS,    NS,    NS, /* ¸¹º»¼½¾¿ */
           NS,    NS,    NS,    NS,    NS,    NS,    NS,    NS, /* ÀÁÂÃÄÅÆÇ */
           NS,    NS,    NS,    NS,    NS,    NS,    NS,    NS, /* ÈÉÊËÌÍÎÏ */
           NS,    NS,    NS,    NS,    NS,    NS,    NS,    NS, /* ÐÑÒÓÔÕÖ× */
           NS,    NS,    NS,    NS,    NS,    NS,    NS,    NS, /* ØÙÚÛÜÝÞß */
           NS,    NS,    NS,    NS,    NS,    NS,    NS,    NS, /* àáâãäåæç */
           NS,    NS,    NS,    NS,    NS,    NS,    NS,    NS, /* èéêëìíîï */
           NS,    NS,    NS,    NS,    NS,    NS,    NS,    NS, /* ðñòóôõö÷ */
           NS,    NS,    NS,    NS,    NS,    NS,    NS,    NS  /* øùúûüýþÿ */
      }
    },
    /* 026 keypunch commercial. */
    { "bcd-a", 0x08,
      {    NS,    NS,    NS,    NS,    NS,    NS,    NS,    NS, /*          */
           NS,    NS,    NS,    NS,    NS,    NS,    NS,    NS, /*          */
           NS,    NS,    NS,    NS,    NS,    NS,    NS,    NS, /*          */
           NS,    NS,    NS,    NS,    NS,    NS,    NS,    NS, /*          */
        00000,    NS,    NS, 00102, 02102, 01042, 04000,    NS, /*  !"#$%&' */
           NS,    NS, 02042,    NS, 01102, 02000, 04102, 01400, /* ()*+,-./ */
        01000, 00400, 00200, 00100, 00040, 00020, 00010, 00004, /* 01234567 */
        00002, 00001,    NS,    NS,    NS,    NS,    NS,    NS, /* 89:;<=>? */
        00042, 04400, 04200, 04100, 04040, 04020, 04010, 04004, /* @ABCDEFG */
        04002, 04001, 02400, 02200, 02100, 02040, 02020, 02010, /* HIJKLMNO */
        02004, 02002, 02001, 01200, 01100, 01040, 01020, 01010, /* PQRSTUVW */
        01004, 01002, 01001,    NS,    NS,    NS,    NS,    NS, /* XYZ[\]^_ */
           NS,    NS,    NS,    NS,    NS,    NS,    NS,    NS, /* `abcdefg */
           NS,    NS,    NS,    NS,    NS,    NS,    NS,    NS, /* hijklmno */
           NS,    NS,    NS,    NS,    NS,    NS,    NS,    NS, /* pqrstuvw */
           NS,    NS,    NS,    NS,    NS,    NS,    NS,    NS, /* xyz{|}~  */
           NS,    NS,    NS,    NS,    NS,    NS,    NS,    NS, /*          */
           NS,    NS,    NS,    NS,    NS,    NS,    NS,    NS, /*          */
           NS,    NS,    NS,    NS,    NS,    NS,    NS,    NS, /*          */
           NS,    NS,    NS,    NS,    NS,    NS,    NS,    NS, /*          */
           NS,    NS,    NS,    NS, 04042,    NS,    NS,    NS, /*  ¡¢£¤¥¦§ */
           NS,    NS,    NS,    NS,    NS,    NS,    NS,    NS, /* ¨©ª«¬­®¯ */
           NS,    NS,    NS,    NS,    NS,    NS,    NS,    NS, /* °±²³´µ¶· */
           NS,    NS,    NS,    NS,    NS,    NS,    NS,    NS, /* ¸¹º»¼½¾¿ */
           NS,    NS,    NS,    NS,    NS,    NS,    NS,    NS, /* ÀÁÂÃÄÅÆÇ */
           NS,    NS,    NS,    NS,    NS,    NS,    NS,    NS, /* ÈÉÊËÌÍÎÏ */
           NS,    NS,    NS,    NS,    NS,    NS,    NS,    NS, /* ÐÑÒÓÔÕÖ× */
           NS,    NS,    NS,    NS,    NS,    NS,    NS,    NS, /* ØÙÚÛÜÝÞß */
           NS,    NS,    NS,    NS,    NS,    NS,    NS,    NS, /* àáâãäåæç */
           NS,    NS,    NS,    NS,    NS,    NS,    NS,    NS, /* èéêëìíîï */
           NS,    NS,    NS,    NS,    NS,    NS,    NS,    NS, /* ðñòóôõö÷ */
           NS,    NS,    NS,    NS,    NS,    NS,    NS,    NS  /* øùúûüýþÿ */
      }
    },
    /* IBM 1401. */
    { "1401", 0x0,
      {    NS,    NS,    NS,    NS,    NS,    NS,    NS,    NS, /*          */
           NS,    NS,    NS,    NS,    NS,    NS,    NS,    NS, /*          */
           NS,    NS,    NS,    NS,    NS,    NS,    NS,    NS, /*          */
           NS,    NS,    NS,    NS,    NS,    NS,    NS,    NS, /*          */
        00000, 03002, 01006, 00102, 02102, 01042, 04000, 01012, /*  !"#$%&' */
        04022, 02022, 02042,    NS, 01102, 02000, 04102, 01400, /* ()*+,-./ */
        01000, 00400, 00200, 00100, 00040, 00020, 00010, 00004, /* 01234567 */
        00002, 00001, 00022, 02012, 04012, 01022, 00012, 05000, /* 89:;<=>? */
        00042, 04400, 04200, 04100, 04040, 04020, 04010, 04004, /* @ABCDEFG */
        04002, 04001, 02400, 02200, 02100, 02040, 02020, 02010, /* HIJKLMNO */
        02004, 02002, 02001, 01200, 01100, 01040, 01020, 01010, /* PQRSTUVW */
        01004, 01002, 01001,    NS,    NS,    NS,    NS,    NS, /* XYZ[\]^_ */
           NS,    NS,    NS,    NS,    NS,    NS,    NS,    NS, /* `abcdefg */
           NS,    NS,    NS,    NS,    NS,    NS,    NS,    NS, /* hijklmno */
           NS,    NS,    NS,    NS,    NS,    NS,    NS,    NS, /* pqrstuvw */
           NS,    NS,    NS,    NS,    NS,    NS,    NS,    NS, /* xyz{|}~  */
           NS,    NS,    NS,    NS,    NS,    NS,    NS,    NS, /*          */
           NS,    NS,    NS,    NS,    NS,    NS,    NS,    NS, /*          */
           NS,    NS,    NS,    NS,    NS,    NS,    NS,    NS, /*          */
           NS,    NS,    NS,    NS,    NS,    NS,    NS,    NS, /*          */
           NS,    NS,    NS,    NS, 04042,    NS,    NS, 04006, /*  ¡¢£¤¥¦§ */
           NS,    NS,    NS,    NS,    NS,    NS,    NS,    NS, /* ¨©ª«¬­®¯ */
           NS, 01202,    NS,    NS,    NS,    NS,    NS,    NS, /* °±²³´µ¶· */
           NS,    NS,    NS,    NS,    NS,    NS,    NS,    NS, /* ¸¹º»¼½¾¿ */
           NS,    NS,    NS,    NS,    NS,    NS,    NS,    NS, /* ÀÁÂÃÄÅÆÇ */
           NS,    NS,    NS,    NS,    NS,    NS,    NS,    NS, /* ÈÉÊËÌÍÎÏ */
           NS,    NS,    NS,    NS,    NS,    NS,    NS,    NS, /* ÐÑÒÓÔÕÖ× */
           NS,    NS,    NS,    NS,    NS,    NS,    NS,    NS, /* ØÙÚÛÜÝÞß */
           NS,    NS,    NS,    NS,    NS,    NS,    NS,    NS, /* àáâãäåæç */
           NS,    NS,    NS,    NS,    NS,    NS,    NS,    NS, /* èéêëìíîï */
           NS,    NS,    NS,    NS,    NS,    NS,    NS,    NS, /* ðñòóôõö÷ */
           NS,    NS,    NS,    NS,    NS,    NS,    NS,    NS, /* øùúûüýþÿ */
      }
    },
    /* 029 keypunch. */
    { "029", 0x20,
      {    NS,    NS,    NS,    NS,    NS,    NS,    NS,    NS, /*          */
           NS,    NS,    NS,    NS,    NS,    NS,    NS,    NS, /*          */
           NS,    NS,    NS,    NS,    NS,    NS,    NS,    NS, /*          */
           NS,    NS,    NS,    NS,    NS,    NS,    NS,    NS, /*          */
        00000, 03002, 00006, 00102, 02102, 01042, 04000, 00022, /*  !"#$%&' */
        04022, 02022, 02042, 04012, 01102, 02000, 04102, 01400, /* ()*+,-./ */
        01000, 00400, 00200, 00100, 00040, 00020, 00010, 00004, /* 01234567 */
        00002, 00001, 00202, 02012, 04042, 00012, 01012, 01006, /* 89:;<=>? */
        00042, 04400, 04200, 04100, 04040, 04020, 04010, 04004, /* @ABCDEFG */
        04002, 04001, 02400, 02200, 02100, 02040, 02020, 02010, /* HIJKLMNO */
        02004, 02002, 02001, 01200, 01100, 01040, 01020, 01010, /* PQRSTUVW */
        01004, 01002, 01001,    NS,    NS,    NS,    NS, 01022, /* XYZ[\]^_ */
           NS,    NS,    NS,    NS,    NS,    NS,    NS,    NS, /* `abcdefg */
           NS,    NS,    NS,    NS,    NS,    NS,    NS,    NS, /* hijklmno */
           NS,    NS,    NS,    NS,    NS,    NS,    NS,    NS, /* pqrstuvw */
           NS,    NS,    NS,    NS, 04006,    NS,    NS,    NS, /* xyz{|}~  */
           NS,    NS,    NS,    NS,    NS,    NS,    NS,    NS, /*          */
           NS,    NS,    NS,    NS,    NS,    NS,    NS,    NS, /*          */
           NS,    NS,    NS,    NS,    NS,    NS,    NS,    NS, /*          */
           NS,    NS,    NS,    NS,    NS,    NS,    NS,    NS, /*          */
           NS,    NS, 05000,    NS,    NS,    NS,    NS,    NS, /*  ¡¢£¤¥¦§ */
           NS,    NS,    NS,    NS, 02006,    NS,    NS,    NS, /* ¨©ª«¬­®¯ */
           NS,    NS,    NS,    NS,    NS,    NS,    NS,    NS, /* °±²³´µ¶· */
           NS,    NS,    NS,    NS,    NS,    NS,    NS,    NS, /* ¸¹º»¼½¾¿ */
           NS,    NS,    NS,    NS,    NS,    NS,    NS,    NS, /* ÀÁÂÃÄÅÆÇ */
           NS,    NS,    NS,    NS,    NS,    NS,    NS,    NS, /* ÈÉÊËÌÍÎÏ */
           NS,    NS,    NS,    NS,    NS,    NS,    NS,    NS, /* ÐÑÒÓÔÕÖ× */
           NS,    NS,    NS,    NS,    NS,    NS,    NS,    NS, /* ØÙÚÛÜÝÞß */
           NS,    NS,    NS,    NS,    NS,    NS,    NS,    NS, /* àáâãäåæç */
           NS,    NS,    NS,    NS,    NS,    NS,    NS,    NS, /* èéêëìíîï */
           NS,    NS,    NS,    NS,    NS,    NS,    NS,    NS, /* ðñòóôõö÷ */
           NS,    NS,    NS,    NS,    NS,    NS,    NS,    NS /* øùúûüýþÿ */
      }
    },
    { NULL }
};

#define PC_COLOR_CREAM		(0 << 3)
#define PC_COLOR_WHITE		(1 << 3)
#define PC_COLOR_YELLOW		(2 << 3)
#define PC_COLOR_PINK		(3 << 3)
#define PC_COLOR_BLUE		(4 << 3)
#define PC_COLOR_GREEN		(5 << 3)
#define PC_COLOR_ORANGE		(6 << 3)
#define PC_COLOR_BROWN		(7 << 3)
#define PC_COLOR_YELLOW_STRIPE	(10 << 3)
#define PC_COLOR_PINK_STRIPE	(11 << 3)
#define PC_COLOR_BLUE_STRIPE	(12 << 3)
#define PC_COLOR_GREEN_STRIPE	(13 << 3)
#define PC_COLOR_ORANGE_STRIPE	(14 << 3)
#define PC_COLOR_BROWN_STRIPE	(15 << 3)

#define PC_CORNER_ROUND		(0 << 2)
#define PC_CORNER_SQUARE	(1 << 2)

#define PC_CUT_NEITHER		0
#define PC_CUT_RIGHT		1
#define PC_CUT_LEFT		2
#define PC_CUT_BOTH		3

struct card_type {
	char *name;
	char **pixmap_source;
	unsigned char card_type[3];
} cards[] = {
	{ "collins", collins,
		{ PC_COLOR_CREAM | PC_CORNER_ROUND | PC_CUT_LEFT,
		  0, 0 } },
	{ "cmu", carnegie2,
		{ PC_COLOR_YELLOW_STRIPE | PC_CORNER_ROUND | PC_CUT_RIGHT,
		  0, 0 } },
	{ NULL, NULL }
};

#define SLOW	75
#define FAST	25

#define CELL_X_NUM	693
#define CELL_X_DENOM	80
#define CELL_WIDTH	(CELL_X_NUM / CELL_X_DENOM)
#define CELL_X(col)	(((col) * CELL_X_NUM) / CELL_X_DENOM)
#define COL_FROM_X(x)	(((x) * CELL_X_DENOM) / CELL_X_NUM)

#define CELL_Y_NUM	296
#define CELL_Y_DENOM	12
#define CELL_HEIGHT	(CELL_Y_NUM / CELL_Y_DENOM)
#define CELL_Y(row)	(((row) * CELL_Y_NUM) / CELL_Y_DENOM)
#define ROW_FROM_Y(y)	(((y) * CELL_Y_DENOM) / CELL_Y_NUM)

#define HOLE_WIDTH	5	/* unused for now */
#define HOLE_HEIGHT	11	/* unused for now */

#define TOP_PAD		15
#define TEXT_PAD	8
#define HOLE_PAD	11
#define LEFT_PAD	31
#define RIGHT_PAD	15
#define BOTTOM_PAD	15
#define CARD_AIR	5

#define	BUTTON_GAP	5
#define BUTTON_BW	2
#define BUTTON_WIDTH	50
#define	BUTTON_HEIGHT	20

static char		*programname;
static Widget		toplevel;
static XtAppContext	appcontext;
static Display		*display;
static int		default_screen;
static int		root_window;
static int		card_window;
static int		depth;
static XFontStruct	*ifontinfo;
static Atom		a_delete_me;
static int		line_number = 100;
static Pixmap		hole_pixmap;
struct charset		*ccharset = NULL;
struct card_type	*ccard_type = NULL;

/* Internal actions. */
static void do_nothing(int);
static void do_data(int);
static void do_multipunch(int);
static void do_left(int);
static void do_right(int);
static void do_home(int);
static void do_pan_right(int);
static void do_pan_left(int);
static void do_pan_up(int);
static void do_slam(int);
static void do_newcard(int);

static void save(int ignored);

/* Application resources. */
typedef struct {
	Pixel	foreground;
	Pixel	background;
	Pixel	cabinet;
	Pixel	cardcolor;
	Pixel	errcolor;
	char	*ifontname;
	char	*charset;
	char	*card;
	Boolean	autonumber;
	Boolean	typeahead;
	Boolean	help;
} AppRes, *AppResptr;

static AppRes appres;

/* Command-line options. */
static XrmOptionDescRec options[]= {
	{ "-ifont",	".ifont",	XrmoptionSepArg,	NULL },
	{ "-nonumber",	".autoNumber",  XrmoptionNoArg,		"False" },
	{ "-typeahead",	".typeahead",	XrmoptionNoArg,		"True" },
	{ "-mono",	".cabinet",	XrmoptionNoArg,		"black" },
	{ "-charset",	".charset",	XrmoptionSepArg,	NULL },
	{ "-card",	".card",	XrmoptionSepArg,	NULL },

	/* Options compatible with Douglas Jones's tools */
	{ "-026ftn",	".charset",	XrmoptionNoArg,		"bcd-h" },
	{ "-026comm",	".charset",	XrmoptionNoArg,		"bcd-a" },
	{ "-029",	".charset",	XrmoptionNoArg,		"029" },
	{ "-EBCDIC",	".charset",	XrmoptionNoArg,		"ebcdic" },
	
	{ "-help",	".help",	XrmoptionNoArg,		"True" },
};

/* Resource list. */
#define offset(field) XtOffset(AppResptr, field)
static XtResource resources[] = {
	{ XtNforeground, XtCForeground, XtRPixel, sizeof(Pixel),
	offset(foreground), XtRString, "XtDefaultForeground" },
	{ XtNbackground, XtCBackground, XtRPixel, sizeof(Pixel),
	offset(background), XtRString, "XtDefaultBackground" },
	{ "cabinet", "Cabinet", XtRPixel, sizeof(Pixel),
	offset(cabinet), XtRString, "grey92" },
	{ "cardColor", "CardColor", XtRPixel, sizeof(Pixel),
	offset(cardcolor), XtRString, "ivory" },
	{ "errColor", "ErrColor", XtRPixel, sizeof(Pixel),
	offset(errcolor), XtRString, "firebrick" },
	{ "ifont", "IFont", XtRString, sizeof(String),
	offset(ifontname), XtRString, 0 },
	{ "autoNumber", "AutoNumber", XtRBoolean, sizeof(Boolean),
	offset(autonumber), XtRString, "True" },
	{ "typeahead", "Typeahead", XtRBoolean, sizeof(Boolean),
	offset(typeahead), XtRString, "False" },
	{ "charset", "Charset", XtRString, sizeof(String),
	offset(charset), XtRString, "bcd-h" },
	{ "card", "Card", XtRString, sizeof(String),
	offset(card), XtRString, "collins" },
	{ "help", "Help", XtRBoolean, sizeof(Boolean),
	offset(help), XtRString, "False" },
};
#undef offset

/* Fallback resources. */
static String fallbacks[] = {
	"*ifont:	7x13",
	"*pos.font:	6x13bold",
	"*dialog*value*font: fixed",
	"*font:		variable",
	"*cabinet:	grey92",
	"*cardColor:	ivory",
	NULL
};

/* Xt actions. */
static void data(Widget, XEvent *, String *, Cardinal *);
static void multi_punch_data(Widget, XEvent *, String *, Cardinal *);
static void delete_window(Widget, XEvent *, String *, Cardinal *);
static void home(Widget, XEvent *, String *, Cardinal *);
static void left(Widget, XEvent *, String *, Cardinal *);
static void next(Widget, XEvent *, String *, Cardinal *);
static void redraw(Widget, XEvent *, String *, Cardinal *);
static void right(Widget, XEvent *, String *, Cardinal *);
static void tab(Widget, XEvent *, String *, Cardinal *);
static void insert_selection(Widget, XEvent *, String *, Cardinal *);
static void confirm(Widget, XEvent *, String *, Cardinal *);

/* Xt callbacks. */
static void discard(Widget, XtPointer, XtPointer);

/* Actions. */
XtActionsRec actions[] = {
	{ "Data",	data },
	{ "MultiPunchData", multi_punch_data },
	{ "DeleteWindow", delete_window },
	{ "Home",	home },
	{ "Left",	left },
	{ "Next",	next },
	{ "Redraw",	redraw },
	{ "Right",	right },
	{ "Tab",	tab },
	{ "insert-selection", insert_selection },
	{ "confirm",	confirm }
};
int actioncount = XtNumber(actions);

/* Forward references. */
static void define_widgets(void);
static void new_card(void);
static void save_popup(void);

/* Syntax. */
void
usage(void)
{
	fprintf(stderr, "Usage: %s [x026-options] [Xt-options]\n",
			programname);
	fprintf(stderr, "x026-options:\n\
  -ifont <font>    Interpreter (card edge) font, defaults to 7x13\n\
  -nonumber        Do not automatically number cards in cols 73..80\n\
  -typeahead       Allow typeahead (very un-026-like)\n\
  -charset <name>  Keypunch character set:\n\
     bcd-h    026 FORTRAN (default)\n\
     bcd-a    026 commercial\n\
     029      029 standard\n\
     1401     IBM 1401\n\
     ebcdic   S/360 EBCDIC\n\
  -card <name>     Card image:\n\
     collins  Collins Radio Corporation\n\
     cmu      Carnegie Mellon University\n\
  -026ftn          Alias for '-charset bcd-h'\n\
  -026comm         Alias for '-charset bcd-a'\n\
  -029             Alias for '-charset 029'\n\
  -EBCDIC          Alias for '-charset ebcdic'\n\
  -help            Display this text\n");
	exit(1);
}

int
main(int argc, char *argv[])
{
	XtTranslations table;
	Pixmap icon;
	int i;

	/* Figure out who we are */
	programname = strrchr(argv[0], '/');
	if (programname)
		++programname;
	else
		programname = argv[0];

	/* Initialze Xt and fetch our resources. */
	toplevel = XtVaAppInitialize(
	    &appcontext,
	    "X026",
	    options, XtNumber(options),
	    &argc, argv,
	    fallbacks,
	    XtNinput, True,
	    XtNallowShellResize, False,
	    NULL);
	if (argc > 1)
		usage();
	XtGetApplicationResources(toplevel, &appres, resources,
	    XtNumber(resources), 0, 0);
	if (appres.help)
		usage();

	/* Set up some globals. */
	display = XtDisplay(toplevel);
	default_screen = DefaultScreen(display);
	root_window = RootWindow(display, default_screen);
	depth = DefaultDepthOfScreen(XtScreen(toplevel));
	a_delete_me = XInternAtom(display, "WM_DELETE_WINDOW", False);

	/* Set up actions. */
	XtAppAddActions(appcontext, actions, actioncount);

	/* Load fonts. */
	ifontinfo = XLoadQueryFont(display, appres.ifontname);
	if (ifontinfo == NULL)
		XtError("Can't load interpreter font");

	/* Pick out the character set. */
	for (i = 0; charsets[i].name; i++) {
		if (!strcasecmp(appres.charset, charsets[i].name)) {
			ccharset = &charsets[i];
			break;
		}
	}
	if (!charsets[i].name) {
		fprintf(stderr, "No such charset: '%s', "
				"defaulting to '%s'\n"
		                "Use '-help' to list the available "
				"character sets\n",
				appres.charset, charsets[0].name);
		ccharset = &charsets[0];
	}

#if defined(DUMP_TABLE) /*[*/
	for (i = 0; i < 257; i++) {
		if (i && !(i % 8)) {
			int j;

			printf(", /* ");
			for (j = i - 8; j < i; j++) {
				if ((j & 0x7f) > ' ' && j != 0x7f)
					putchar(j);
				else
					putchar(' ');
			}
			printf(" */\n");
		}
		if (i == 257)
			break;
		if (i % 8)
			printf(", ");
		if (ccharset->charset[i] == NS)
			printf("   NS");
		else
			printf("0%04o", ccharset->charset[i]);
	}
#endif /*]*/

	/* Define the widgets. */
	define_widgets();

	/* Set up a cute (?) icon. */
	icon = XCreateBitmapFromData(display, XtWindow(toplevel),
	    (char *)x026_bits, x026_width, x026_height);
	XtVaSetValues(toplevel, XtNiconPixmap, icon, XtNiconMask, icon, NULL);

	/* Allow us to die gracefully. */
	XSetWMProtocols(display, XtWindow(toplevel), &a_delete_me, 1);
	table = XtParseTranslationTable(
	    "<Message>WM_PROTOCOLS: DeleteWindow()");
	XtOverrideTranslations(toplevel, table);

#if defined(SOUND) /*[*/
	/* Set up clicks. */
	audio_init();
#endif /*]*/

	new_card();

	/* Process X events forever. */
	XtAppMainLoop(appcontext);

	return 0;
}


/* Definitions for buttons along the bottom left. */

struct button {
	char *label;
	void (*callback)(int);
} button[] = {
	{ "Off", (void (*)())exit },
	{ "Save", save },
	{ NULL, NULL },
};

/* Callback function for buttons. */
static void
button_callback(Widget w, XtPointer client_data, XtPointer call_data)
{
	struct button *b = (struct button *)client_data;

	(*b->callback)(0);
}

/* Card-image data structures. */

#define N_COLS	80	/* number of columns (80, of course) */
#define N_ROWS	12	/* number of rows */
#define N_OV	10	/* number of overpunches */

typedef struct card {
	unsigned short holes[N_COLS];
	int n_ov[N_COLS];
	unsigned char coltxt[N_COLS][N_OV];
	int seq;
	struct card *prev;
	struct card *next;
} card;
static card *ccard;

static int col = 0;
static GC gc, invgc, holegc, corner_gc;

static Widget container, porth, scrollw, cardw, posw;
static int scrollw_column;
#define SCROLLW_X()	(-(LEFT_PAD+CELL_X(scrollw_column)))

static Dimension card_width, card_height;
static Dimension hole_width, hole_height;

/* Set up the keypunch. */
static void
define_widgets(void)
{
	Dimension w, h;
	Widget ww;
	XtTranslations t;
	XGCValues xgcv;
	int i;
	Pixmap pixmap, shapemask;
	Pixmap hole_shapemask;
	XpmAttributes attributes;
	static char translations[] = "\
		<Key>Left:	Left()\n\
		<Key>BackSpace:	Left()\n\
		<Key>Right:	Right()\n\
		<Key>space:	Right()\n\
		<Key>Home:	Home()\n\
		<Key>Return:	Next()\n\
		<Key>KP_Enter:	Home()\n\
		<Key>Down:	Next()\n\
		<Key>Tab:	Tab()\n\
		<Btn2Down>:	insert-selection(PRIMARY)\n\
		Alt<Key>:	MultiPunchData()\n\
		Meta<Key>:	MultiPunchData()\n\
		<Key>:		Data()\n";

	/* Create a container for the whole thing. */
	container = XtVaCreateManagedWidget(
	    "container", compositeWidgetClass, toplevel,
	    XtNwidth, 10,
	    XtNheight, 10,
	    XtNbackground, appres.cabinet,
	    NULL);
	XtRealizeWidget(toplevel);

	/* Figure out the pixmap. */
	for (i = 0; cards[i].name != NULL; i++) {
		if (!strcmp(appres.card, cards[i].name)) {
			ccard_type = &cards[i];
			break;
		}
	}
	if (cards[i].name == NULL) {
		fprintf(stderr, "No such card '%s', defaulting to '%s'\n"
				"Use '-help' to list the types\n",
				appres.card, cards[0].name);
		ccard_type = &cards[i];
	}
	attributes.valuemask = XpmSize;
	if (XpmCreatePixmapFromData(display, XtWindow(container),
			ccard_type->pixmap_source, &pixmap, &shapemask,
			&attributes) != XpmSuccess) {
		XtError("XpmCreatePixmapFromData failed");
	}
	card_width = attributes.width;
	card_height = attributes.height;
	attributes.valuemask = XpmSize;
	if (XpmCreatePixmapFromData(display, XtWindow(container), hole,
			&hole_pixmap, &hole_shapemask, &attributes) != XpmSuccess) {
		XtError("XpmCreatePixmapFromData failed");
	}
	hole_width = attributes.width;
	hole_height = attributes.height;
	w = card_width + 2*CARD_AIR;
	h = card_height + 2*CARD_AIR + 2*BUTTON_GAP + 2*BUTTON_BW + BUTTON_HEIGHT;
	XtVaSetValues(container,
	    XtNwidth, w,
	    XtNheight, h,
	    NULL);

	/* Create the porthole within the container. */
	porth = XtVaCreateManagedWidget(
	    "porthole", portholeWidgetClass, container,
	    XtNwidth, card_width,
	    XtNheight, card_height,
	    XtNx, CARD_AIR,
	    XtNy, CARD_AIR,
	    XtNborderWidth, 0,
	    NULL);

	/* Create scrollable region within the porthole. */
	scrollw_column = 40;
	scrollw = XtVaCreateManagedWidget(
	    "scroll", compositeWidgetClass, porth,
	    XtNwidth, card_width * 3,
	    XtNheight, card_height * 3,
	    XtNx, SCROLLW_X(),
	    XtNy, -(2*card_height + TOP_PAD),
	    XtNbackground, appres.cabinet,
	    XtNborderWidth, 0,
	    NULL);

	/* Create the card itself. */
	cardw = XtVaCreateManagedWidget(
	    "card", compositeWidgetClass, scrollw,
	    XtNwidth, card_width,
	    XtNheight, card_height,
	    XtNx, card_width,
	    XtNy, card_height,
	    XtNborderWidth, 0,
	    XtNbackground, appres.cardcolor,
	    XtNbackgroundPixmap, pixmap,
	    NULL);

	/* Create the buttons in the lower left. */
	for (i = 0; button[i].label; i++) {
		ww = XtVaCreateManagedWidget(
		    button[i].label, commandWidgetClass, container,
		    XtNwidth, BUTTON_WIDTH,
		    XtNheight, BUTTON_HEIGHT,
		    XtNx, BUTTON_GAP + i*(2*BUTTON_BW + BUTTON_WIDTH + BUTTON_GAP),
		    XtNy, card_height + 2*CARD_AIR + BUTTON_GAP,
		    XtNborderWidth, BUTTON_BW,
		    XtNborderColor, appres.background,
		    NULL
		);
		XtAddCallback(ww, XtNcallback, button_callback,
		    (XtPointer)&button[i]);
	}

	/* Add the position counter in the lower right. */
	posw = XtVaCreateManagedWidget(
	    "pos", labelWidgetClass, container,
	    XtNlabel, "0",
	    XtNwidth, BUTTON_WIDTH,
	    XtNx, w - BUTTON_GAP - BUTTON_WIDTH - 2*BUTTON_BW,
	    XtNy, card_height + 2*CARD_AIR + BUTTON_GAP,
	    XtNheight, BUTTON_HEIGHT,
	    XtNborderWidth, BUTTON_BW,
	    XtNborderColor, appres.background,
	    XtNresize, False,
	    NULL);

	/* Add the discard button. */
	ww = XtVaCreateManagedWidget(
	    "discard", commandWidgetClass, container,
	    XtNlabel, "Discard",
	    XtNwidth, BUTTON_WIDTH,
	    XtNx, w - 2* (BUTTON_GAP + BUTTON_WIDTH + 2*BUTTON_BW),
	    XtNy, card_height + 2*CARD_AIR + BUTTON_GAP,
	    XtNheight, BUTTON_HEIGHT,
	    XtNborderWidth, BUTTON_BW,
	    XtNborderColor, appres.background,
	    NULL);
	XtAddCallback(ww, XtNcallback, discard, NULL);

	/* Create graphics contexts for drawing. */
	xgcv.foreground = appres.foreground;
	xgcv.background = appres.cardcolor;
	xgcv.font = ifontinfo->fid;
	gc = XtGetGC(toplevel, GCForeground|GCBackground|GCFont, &xgcv);
	xgcv.foreground = appres.cabinet;
	corner_gc = XtGetGC(toplevel, GCForeground|GCBackground, &xgcv);
	xgcv.foreground = appres.cardcolor;
	xgcv.background = appres.foreground;
	xgcv.font = ifontinfo->fid;
	invgc = XtGetGC(toplevel, GCForeground|GCBackground|GCFont, &xgcv);
	xgcv.tile = hole_pixmap;
	xgcv.fill_style = FillTiled;
	holegc = XtGetGC(toplevel, GCTile|GCFillStyle, &xgcv);

	/* Fix the size of the toplevel window. */
	XtVaSetValues(toplevel,
	    XtNwidth, w,
	    XtNheight, h,
	    XtNbaseWidth, w,
	    XtNbaseHeight, h,
	    XtNminWidth, w,
	    XtNminHeight, h,
	    XtNmaxWidth, w,
	    XtNmaxHeight, h,
	    NULL);

	/* Define event translations. */
	t = XtParseTranslationTable(translations);
	XtOverrideTranslations(container, t);
	t = XtParseTranslationTable("<Expose>: Redraw()");
	XtOverrideTranslations(cardw, t);

	/* Inflate it all. */
	XtRealizeWidget(toplevel);
	card_window = XtWindow(cardw);

	/* Add the cursor. */
#define C2H (BUTTON_HEIGHT + 2*BUTTON_GAP + 2*BUTTON_BW + 10)
	ww = XtVaCreateManagedWidget(
	    "cursor2", compositeWidgetClass, container,
	    XtNwidth, CELL_WIDTH,
	    XtNheight, C2H,
	    XtNx, CARD_AIR + LEFT_PAD + CELL_X(((N_COLS/2)+1)) + 1,
	    XtNy, h-C2H-2,
	    NULL);
	XtRealizeWidget(ww);
}

/* Punch a character into a particular column of the current card. */
static Boolean
punch_char(int cn, unsigned char c)
{
	int j;

	if (ccharset->charset[c] == NS) {
		/* Map lowercase, to be polite. */
		if (islower(c) && ccharset->charset[toupper(c)] != NS)
			c = toupper(c);
		else
			return False;
	}

	/* Space?  Do nothing. */
	if (!ccharset->charset[c])
		return True;

	ccard->holes[cn] |= ccharset->charset[c];

	/* Redundant? */
	for (j = 0; j < ccard->n_ov[cn]; j++)
		if (ccard->coltxt[cn][j] == c)
			return True;

	if (ccard->n_ov[cn] < N_OV) {
		ccard->coltxt[cn][ccard->n_ov[cn]] = c;
		++ccard->n_ov[cn];
	}

	return True;
}

/* Render the image of a card column onto the X display. */
static void
draw_col(int cn)
{
	int i;
	int j;
	int x = LEFT_PAD + CELL_X(cn);

#if defined(DEBUG) /*[*/
	printf(" draw_col(col %d)\n", cn);
#endif /*]*/

	/* Draw the text at the top, possibly overstruck. */
	for (j = 0; j < ccard->n_ov[cn]; j++) {
		if (ccard->coltxt[cn][j] < ' ')
			continue;
		XDrawString(display, card_window, gc,
		    x, TOP_PAD + TEXT_PAD, (char *)&ccard->coltxt[cn][j], 1);
	}

	/* Draw the holes, top to bottom. */
	for (i = 0; i < N_ROWS; i++) {
		if (ccard->holes[cn] & (0x800>>i)) {
			XGCValues xgcv;

			xgcv.ts_x_origin = x;
			xgcv.ts_y_origin = TOP_PAD + HOLE_PAD + (CELL_Y(i));
			XChangeGC(display, holegc,
					GCTileStipXOrigin|GCTileStipYOrigin,
					&xgcv);

			XFillRectangle(display, card_window, holegc,
			    x,
			    TOP_PAD + HOLE_PAD + (CELL_Y(i)),
			    hole_width, hole_height);
		}
	}
}

/* Update the column indicator in the lower right. */
static void
set_posw(int c)
{
	static char bb[3];

	col = c;
	(void) sprintf(bb, "%02d", col+1);
	XtVaSetValues(posw, XtNlabel, bb, NULL);
}

/* Go to the next card. */
static void
do_newcard(int replace)
{
	int i;

	if (!ccard || !replace) {
		card *c;

		c = (card *)XtMalloc(sizeof(card));
		c->prev = ccard;
		c->next = NULL;
		if (ccard)
			ccard->next = c;
		ccard = c;
		c->seq = line_number;
		line_number += 10;
	}
	(void) memset(ccard->coltxt, ' ', sizeof(ccard->coltxt));
	(void) memset(ccard->holes, 0, sizeof(ccard->holes));
	(void) memset(ccard->n_ov, 0, sizeof(ccard->n_ov));
	set_posw(0);
	if (appres.autonumber) {
		char ln_buf[9];

		(void) sprintf(ln_buf, "%08d", ccard->seq);
		for (i = 0; i < 8; i++)
			punch_char(72+i, ln_buf[i]);
	}
}

/* Redraw the entire card image. */
static void
redraw(Widget wid, XEvent  *event, String  *params, Cardinal *num_params)
{
	int i;
	Dimension x, y, w, h;

	if (event && event->type == Expose) {
		x = event->xexpose.x;
		y = event->xexpose.y;
		w = event->xexpose.width;
		h = event->xexpose.height;
#if defined(DEBUG) /*[*/
		printf("redraw x=%d y=%d w=%d h=%d\n",
				x, y, w, h);
#endif /*]*/
	} else {
		x = y = 0;
		w = card_width;
		h = card_height;
#if defined(DEBUG) /*[*/
		printf("redraw without Expose event\n");
#endif /*]*/
	}

	/* Slice off the padding. */
	if (x < LEFT_PAD) {			/* Left. */
		if (w <= LEFT_PAD - x) {
#if defined(DEBUG) /*[*/
			printf("ignoring left\n");
#endif /*]*/
			return;
		}
		w -= LEFT_PAD - x;
		x = 0;
	} else
		x -= LEFT_PAD;
	if (y < TOP_PAD) {			/* Top. */
		if (h <= TOP_PAD - y) {
#if defined(DEBUG) /*[*/
			printf("ignoring top\n");
#endif /*]*/
			return;
		}
		h -= TOP_PAD - y;
		y = 0;
	} else
		y -= TOP_PAD;
	if (x >= (CELL_X(N_COLS))) {		/* Right. */
#if defined(DEBUG) /*[*/
		printf("ignoring right\n");
#endif /*]*/
		return;
	}
	if (x + w > (CELL_X(N_COLS)))
		w = (CELL_X(N_COLS)) - x;
	if (y >= (CELL_Y(N_ROWS))) {		/* Bottom. */
#if defined(DEBUG) /*[*/
		printf("ignoring left\n");
#endif /*]*/
		return;
	}
	if (y + h > (CELL_Y(N_ROWS)))
		h = (CELL_Y(N_ROWS)) - y;
	
	for (i = COL_FROM_X(x);
	     i < COL_FROM_X(x + w + CELL_WIDTH) && i < N_COLS;
	     i++) {
		draw_col(i);
	}
}

/* Exit. */
static void
delete_window(Widget wid, XEvent *event, String *params, Cardinal *num_params)
{
	exit(0);
}

/* Find the first card in the deck. */
static card *
first_card(void)
{
	card *c;

	for (c = ccard; c && c->prev; c = c->prev)
		;
	return c;
}

static void
save(int ignored)
{
	save_popup();
}

/*
 * Internals of functions that are enqueued with a delay.
 */

static void
do_nothing(int ignored)
{
}

static void
do_data(int c)
{
	extern void do_right(int);

	if (col < N_COLS && punch_char(col, c)) {
		draw_col(col);
#if defined(SOUND) /*[*/
		loud_click();
#endif /*]*/
		do_right(0);
	}
}

static void
do_multipunch(int c)
{
	extern void do_right(int);

	if (col < N_COLS && punch_char(col, c)) {
		draw_col(col);
#if defined(SOUND) /*[*/
		loud_click();
#endif /*]*/
	}
}

static void
do_left(int c)
{
	extern void do_pan_left(int);

	if (col) {
		do_pan_left(0);
		set_posw(col - 1);
	}
}

static void
do_right(int do_click)
{
	extern void do_pan_right(int);

	if (col < N_COLS-1) {
		do_pan_right(do_click);
		set_posw(col + 1);
	}
}

static void
do_pan_left(int ignored)
{
	scrollw_column--;
	XtVaSetValues(scrollw, XtNx, SCROLLW_X(), NULL);
#if defined(SOUND) /*[*/
	soft_click();
#endif /*]*/
}

static void
do_pan_right(int do_click)
{
	scrollw_column++;
	XtVaSetValues(scrollw, XtNx, SCROLLW_X(), NULL);
#if defined(SOUND) /*[*/
	if (do_click)
		soft_click();
#endif /*]*/
}

static void
do_pan_up(int ignored)
{
	Dimension y;

	XtVaGetValues(scrollw, XtNy, &y, NULL);
	y += CELL_HEIGHT;
	XtVaSetValues(scrollw, XtNy, y, NULL);
}

static void
do_home(int ignored)
{
	do_pan_left(0);
	set_posw(col - 1);
}

static void
do_slam(int ignored)
{
	scrollw_column = 40;
	XtVaSetValues(scrollw,
	    XtNx, SCROLLW_X(),
	    XtNy, -(2*card_height) + TOP_PAD,
	    NULL);
}

/*
 * Event queueing: Inserting artificial delays in processing certain events.
 */
#define N_EVENTS	500
enum evtype { DUMMY, DATA, MULTI, LEFT, RIGHT, HOME,
	      PAN_RIGHT, PAN_LEFT, PAN_UP, SLAM, NEWCARD };
void (*eq_fn[])(int) = {
	do_nothing,
	do_data,
	do_multipunch,
	do_left,
	do_right,
	do_home,
	do_pan_right,
	do_pan_left,
	do_pan_up,
	do_slam,
	do_newcard
};
struct {
	enum evtype evtype;
	unsigned char param;	/* optional */
	int delay;
} eventq[N_EVENTS];
int eq_fillp, eq_emptyp, eq_count;

static void
deq_event(XtPointer data, XtIntervalId *id)
{
	int delay;

	if (!eq_count)
		return;
	(*eq_fn[eventq[eq_emptyp].evtype])(eventq[eq_emptyp].param);
	delay = eventq[eq_emptyp].delay;
	eq_emptyp = (eq_emptyp + 1) % N_EVENTS;
	if (--eq_count)
		(void) XtAppAddTimeOut(appcontext, delay, deq_event, NULL);
}

static Boolean
enq_event(enum evtype evtype, unsigned char param, Boolean restricted, int delay)
{
	if (restricted && eq_count) {
		XBell(display, 0);
		return False;
	}
	if (eq_count >= N_EVENTS) {
		XBell(display, 0);
		return False;
	}
	if (evtype == DUMMY)
		return True;
	eventq[eq_fillp].evtype = evtype;
	eventq[eq_fillp].param = param;
	eventq[eq_fillp].delay = delay;
	eq_fillp = (eq_fillp + 1) % N_EVENTS;
	if (!eq_count++)
		(void) XtAppAddTimeOut(appcontext, delay, deq_event, NULL);
	return True;
}

/*
 * Externals of delayed functions, called by the toolkit.
 */

static void
data(Widget wid, XEvent *event, String *params, Cardinal *num_params)
{
	XKeyEvent *kevent = (XKeyEvent *)event;
	char buf[10];
	KeySym ks;
	int ll;

	ll = XLookupString(kevent, buf, 10, &ks, (XComposeStatus *)NULL);
	if (ll == 1) {
		enq_event(DATA, buf[0] & 0xff, !appres.typeahead, SLOW);
	}
}

static void
multi_punch_data(Widget wid, XEvent *event, String *params, Cardinal *num_params)
{
	XKeyEvent *kevent = (XKeyEvent *)event;
	char buf[10];
	KeySym ks;
	int ll;

	ll = XLookupString(kevent, buf, 10, &ks, (XComposeStatus *)NULL);
	if (ll == 1) {
		enq_event(MULTI, buf[0], !appres.typeahead, SLOW);
	}
}

static void
left(Widget wid, XEvent *event, String *params, Cardinal *num_params)
{
	enq_event(LEFT, 0, !appres.typeahead, SLOW);
}

static void
right(Widget wid, XEvent *event, String *params, Cardinal *num_params)
{
	enq_event(RIGHT, 1, !appres.typeahead, SLOW);
}

static void
home(Widget wid, XEvent *event, String *params, Cardinal *num_params)
{
	int i;

	if (!enq_event(DUMMY, 0, True, SLOW))
		return;

	for (i = 0; i < col; i++)
		enq_event(HOME, 0, False, FAST);
}

static void
next(Widget wid, XEvent *event, String *params, Cardinal *num_params)
{
	if (ccard->next) {
		ccard = ccard->next;
		set_posw(0);
	} else
		new_card();
}

static void
tab(Widget wid, XEvent *event, String *params, Cardinal *num_params)
{
	int i;

	if (!enq_event(DUMMY, 0, True, SLOW))
		return;

	for (i = col; i < 6; i++)
		enq_event(RIGHT, 1, False, SLOW);
}

/* Throw away this card. */
static void
discard(Widget w, XtPointer client_data, XtPointer call_data)
{
	int i;

	if (!enq_event(DUMMY, 0, True, SLOW))
		return;

	/* Scroll the previous card away. */
	if (ccard) {
		for (i = 0; i <= col; i++)
			enq_event(LEFT, 0, False, FAST);
		for (i = 0; i < N_COLS/2; i++)
			enq_event(PAN_LEFT, 0, False, FAST);
	}

	enq_event(NEWCARD, True, False, FAST);

	/* Scroll the new card down. */
	enq_event(SLAM, 0, False, SLOW);
	for (i = 0; i <= N_ROWS; i++)
		enq_event(PAN_UP, 0, False, SLOW);

}

/* Generate a fresh card. */
static void
new_card(void)
{
	int i;

	/* Scroll the previous card away. */
	if (ccard) {
		for (i = col; i < N_COLS; i++)
			enq_event(RIGHT, 0, False, FAST);
		for (i = 0; i < N_COLS/2 + 2; i++)
			enq_event(PAN_RIGHT, 0, False, FAST);
	}

	enq_event(NEWCARD, False, False, FAST);

	/* Scroll the new card down. */
	enq_event(SLAM, 0, False, SLOW);
	for (i = 0; i <= N_ROWS; i++)
		enq_event(PAN_UP, 0, False, SLOW);
}

#define NP	5
Atom paste_atom[NP];
int n_pasting = 0;
int pix = 0;
Time paste_time;

/* Pasting support. */
static void
paste_callback(Widget w, XtPointer client_data, Atom *selection, Atom *type,
    XtPointer value, unsigned long *length, int *format)
{
	char *s;
	unsigned long len;

	if ((value == NULL) || (*length == 0)) {
		XtFree(value);

		/* Try the next one. */
		if (n_pasting > pix)
			XtGetSelectionValue(w, paste_atom[pix++], XA_STRING,
			    paste_callback, NULL, paste_time);
		return;
	}

	s = (char *)value;
	len = *length;
	while (len--) {
		unsigned char c = *s++;

		if (c == '\n')
			break;
		if (c < ' ') {
			continue;
		}
		if (!enq_event(DATA, c, False, SLOW))
			break;
	}
	n_pasting = 0;

	XtFree(value);
}

static void
insert_selection(Widget w, XEvent *event, String *params, Cardinal *num_params)
{
	int	i;
	Atom	a;
	XButtonEvent *bevent = (XButtonEvent *)event;

	n_pasting = 0;
	for (i = 0; i < *num_params; i++) {
		a = XInternAtom(display, params[i], True);
		if (a == None) {
			XtWarning("no atom for selection");
			continue;
		}
		if (n_pasting < NP)
			paste_atom[n_pasting++] = a;
	}
	pix = 0;
	if (n_pasting > pix) {
		paste_time = bevent->time;
		XtGetSelectionValue(w, paste_atom[pix++], XA_STRING,
		    paste_callback, NULL, paste_time);
	}
}

/* 'Save' pop-up. */

static Boolean save_popup_created = False;
static Widget save_shell, save_dialog;

static void
center_it(Widget w, XtPointer client_data, XtPointer call_data)
{
	Dimension width, height;
	Position toplevel_x, toplevel_y;
	Dimension toplevel_width, toplevel_height;

	/* Get the popup's dimensions */
	XtVaGetValues(w,
	    XtNwidth, &width,
	    XtNheight, &height,
	NULL);

	/* Get the toplevel position and dimensions. */
	XtVaGetValues(toplevel,
	    XtNwidth, &toplevel_width,
	    XtNheight, &toplevel_height,
	    XtNx, &toplevel_x,
	    XtNy, &toplevel_y,
	    NULL);

	/* Compute our position relatve to those. */
	XtVaSetValues(w,
	    XtNx, toplevel_x + (toplevel_width - width) / 2,
	    XtNy, toplevel_y + (toplevel_height - height) / 2,
	    NULL);
}

static void
save_file_ascii(void)
{
	char *sfn;
	FILE *f;
	card *c;
	int i, j, h;

	XtVaGetValues(save_dialog, XtNvalue, &sfn, NULL);
	f = fopen(sfn, "w");
	if (f == NULL) {
	    XBell(display, 100);
	    XtVaSetValues(save_dialog,
		XtNlabel, strerror(errno),
		NULL);
	    XtVaSetValues(XtNameToWidget(save_dialog, XtNlabel),
		XtNforeground, appres.errcolor,
		NULL);
	    return;
	}
	XtPopdown(save_shell);

	for (c = first_card(); c; c = c->next) {
		for (i = 0; i < N_COLS; i++) {
			if (!c->n_ov[i]) {
				fputc(' ', f);
				continue;
			}
			/* Try to find a single character. */
			for (h = 0; h < 256; h++) {
				if (h == '\n' || h == '\b')
					continue;
				if (ccharset->charset[h] != NS &&
				    ccharset->charset[h] == c->holes[i]) {
					fputc(h, f);
					break;
				}
			}
			if (h < 256)
				continue;
			/* Write it with backspaces. */
			for (j = 0; j < c->n_ov[i]; j++) {
				if (j)
					fputc('\b', f);
				fputc(c->coltxt[i][j], f);
			}
		}
		fputc('\n', f);
	}
	fclose(f);
}

static void
save_file_image(void)
{
	char *sfn;
	FILE *f;
	card *c;

	XtVaGetValues(save_dialog, XtNvalue, &sfn, NULL);
	f = fopen(sfn, "w");
	if (f == NULL) {
	    XBell(display, 100);
	    XtVaSetValues(save_dialog,
		XtNlabel, strerror(errno),
		NULL);
	    XtVaSetValues(XtNameToWidget(save_dialog, XtNlabel),
		XtNforeground, appres.errcolor,
		NULL);
	    return;
	}
	XtPopdown(save_shell);

	/* Write the header. */
	fprintf(f, "H80");

	/* Write the cards. */
	for (c = first_card(); c; c = c->next) {
		int i;
		unsigned char b3[3];

		fprintf(f, "%c%c%c",
		    0x80 | ccard_type->card_type[0],
		    0x80 | ccard_type->card_type[1] | ccharset->punch_type,
		    0x80 | ccard_type->card_type[2]);
		for (i = 0; i < N_COLS; i++) {
			if (i % 2) {
				b3[1] |= (c->holes[i] >> 8) & 0xf;
				b3[2] = c->holes[i] & 0xff;
				if (fwrite(b3, 1, 3, f) < 3)
					break;
			} else {
				b3[0] = c->holes[i] >> 4;
				b3[1] = (c->holes[i] & 0xf) << 4;
			}
		}
	}
	fclose(f);
}

static void
save_ascii_callback(Widget w, XtPointer client_data, XtPointer call_data)
{
	save_file_ascii();
}

static void
save_image_callback(Widget w, XtPointer client_data, XtPointer call_data)
{
	save_file_image();
}

static void
cancel_callback(Widget w, XtPointer client_data, XtPointer call_data)
{
	XtPopdown(save_shell);
}

static void
confirm(Widget w, XEvent *event, String *params, Cardinal *num_params)
{
	save_file_ascii();
}

static void
save_popup(void)
{
	Widget w;

	if (!save_popup_created) {
		Dimension width, height;

		/* Create the shell. */
		save_shell = XtVaCreatePopupShell("save",
		    transientShellWidgetClass, toplevel,
		    NULL);
		XtAddCallback(save_shell, XtNpopupCallback, center_it, NULL);

		/* Create the dialog in the popup. */
		save_dialog = XtVaCreateManagedWidget(
		    "dialog", dialogWidgetClass,
		    save_shell,
		    XtNvalue, "",
		    XtNbackground, appres.cabinet,
		    NULL);
		w = XtNameToWidget(save_dialog, XtNvalue);
		XtVaSetValues(w,
		    XtNwidth, 200,
		    NULL);
		XtOverrideTranslations(w,
		    XtParseTranslationTable("<Key>Return: confirm()"));
		XtVaSetValues(XtNameToWidget(save_dialog, XtNlabel),
		    XtNwidth, 200,
		    XtNbackground, appres.cabinet,
		    NULL);
		if (appres.cabinet == XBlackPixel(display, default_screen))
			XtVaSetValues(XtNameToWidget(save_dialog, XtNlabel),
			    XtNforeground, XWhitePixel(display, default_screen),
			    NULL);

		/* Add 'Save' and 'Cancel' buttons to the dialog. */
		w = XtVaCreateManagedWidget("Save Ascii", commandWidgetClass,
		    save_dialog,
		    NULL);
		XtAddCallback(w, XtNcallback, save_ascii_callback, NULL);
		w = XtVaCreateManagedWidget("Save Images", commandWidgetClass,
		    save_dialog,
		    NULL);
		XtAddCallback(w, XtNcallback, save_image_callback, NULL);
		w = XtVaCreateManagedWidget("Cancel", commandWidgetClass,
		    save_dialog,
		    NULL);
		XtAddCallback(w, XtNcallback, cancel_callback, NULL);

		/* Set the width and height. */
		XtRealizeWidget(save_shell);
		XtVaGetValues(save_shell,
		    XtNwidth, &width,
		    XtNheight, &height,
		NULL);
		XtVaSetValues(save_shell,
		    XtNheight, height,
		    XtNwidth, width,
		    XtNbaseHeight, height,
		    XtNbaseWidth, width,
		    XtNminHeight, height,
		    XtNminWidth, width,
		    XtNmaxHeight, height,
		    XtNmaxWidth, width,
		    NULL);

		save_popup_created = True;
	}
	XtVaSetValues(save_dialog,
	    XtNlabel, "Save File Name",
	    NULL);
	if (appres.cabinet == XBlackPixel(display, default_screen))
		XtVaSetValues(XtNameToWidget(save_dialog, XtNlabel),
		    XtNforeground, XWhitePixel(display, default_screen),
		    NULL);
	else
		XtVaSetValues(XtNameToWidget(save_dialog, XtNlabel),
		    XtNforeground, appres.foreground,
		    NULL);
	XtPopup(save_shell, XtGrabExclusive);
	/*XSetWMProtocols(display, XtWindow(save_shell), &delete_me, 1);*/
}
