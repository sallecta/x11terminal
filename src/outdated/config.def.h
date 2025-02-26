/* See LICENSE file for copyright and license details. */

/*
 * appearance
 *
 * font: see http://freedesktop.org/software/fontconfig/fontconfig-user.html
 */
static char font[] = "Liberation Mono:pixelsize=12:antialias=true:autohint=true";
static int borderpx = 2;

/*
 * What program is execed by st depends of these precedence rules:
 * 1: program passed with -e
 * 2: utmp option
 * 3: SHELL environment variable
 * 4: value of shell in /etc/passwd
 * 5: value of shell in config.h
 */
static char shell[] = "/bin/sh";
static char *utmp = NULL;
static char stty_args[] = "stty raw pass8 nl -echo -iexten -cstopb 38400";

/* identification sequence returned in DA and DECID */
static char vtiden[] = "\033[?6c";

/* Kerning / character bounding-box multipliers */
static float cwscale = 1.0;
static float chscale = 1.0;

/*
 * word delimiter string
 *
 * More advanced example: " `'\"()[]{}"
 */
static char worddelimiters[] = " ";

/* selection timeouts (in milliseconds) */
static unsigned int doubleclicktimeout = 300;
static unsigned int tripleclicktimeout = 600;

/* alt screens */
static int allowaltscreen = 1;

/* frames per second st should at maximum draw to the screen */
static unsigned int xfps = 120;
static unsigned int actionfps = 30;

/*
 * blinking timeout (set to 0 to disable blinking) for the terminal blinking
 * attribute.
 */
static unsigned int blinktimeout = 800;

/*
 * thickness of underline and bar cursors
 */
static unsigned int cursorthickness = 2;

/*
 * bell volume. It must be a value between -100 and 100. Use 0 for disabling
 * it
 */
static int bellvolume = 0;

/* default TERM value */
static char termname[] = "st-256color";

static unsigned int tabspaces = 8;

/* Terminal colors (16 first used in escape sequence) */
static const char *colorname[] = {
	/* 8 normal colors */
	"black",
	"red3",
	"green3",
	"yellow3",
	"blue2",
	"magenta3",
	"cyan3",
	"gray90",

	/* 8 bright colors */
	"gray50",
	"red",
	"green",
	"yellow",
	"#5c5cff",
	"magenta",
	"cyan",
	"white",

	[255] = 0,

	/* more colors can be added after 255 to use with DefaultXX */
	"#cccccc",
	"#555555",
};


/*
 * Default colors (colorname index)
 * foreground, background, cursor, reverse cursor
 */
static unsigned int defaultfg = 7;
static unsigned int defaultbg = 0;
static unsigned int defaultcs = 256;
static unsigned int defaultrcs = 257;

/*
 * Default shape of cursor
 * 2: Block ("█")
 * 4: Underline ("_")
 * 6: Bar ("|")
 * 7: Snowman ("☃")
 */
static unsigned int cursorshape = 2;

/*
 * Default colour and shape of the mouse cursor
 */
static unsigned int mouseshape = XC_xterm;
static unsigned int mousefg = 7;
static unsigned int mousebg = 0;

/*
 * Colors used, when the specific fg == defaultfg. So in reverse mode this
 * will reverse too. Another logic would only make the simple feature too
 * complex.
 */
static unsigned int defaultitalic = 11;
static unsigned int defaultunderline = 7;

/*
 * Internal mouse shortcuts.
 * Beware that overloading XELT_MOUSSE_LEFT will disable the selection.
 */
static xelt_MouseShortcut mshortcuts[] = {
	/* button               mask            string */
	{ Button4,              XELT_SIZE_XK_ANY_MOD,     "\031" },
	{ Button5,              XELT_SIZE_XK_ANY_MOD,     "\005" },
};

/* Internal keyboard shortcuts. */
#define MODKEY Mod1Mask

static xelt_Shortcut shortcuts[] = {
	/* mask                 keysym          function        argument */
	{ XELT_SIZE_XK_ANY_MOD,           XK_Break,       sendbreak,      {.i =  0} },
	{ ControlMask,          XK_Print,       toggleprinter,  {.i =  0} },
	{ ShiftMask,            XK_Print,       printscreen,    {.i =  0} },
	{ XELT_SIZE_XK_ANY_MOD,           XK_Print,       printsel,       {.i =  0} },
	{ MODKEY|ShiftMask,     XK_Prior,       xzoom,          {.f = +1} },
	{ MODKEY|ShiftMask,     XK_Next,        xzoom,          {.f = -1} },
	{ MODKEY|ShiftMask,     XK_Home,        xzoomreset,     {.f =  0} },
	{ ShiftMask,            XK_Insert,      selpaste,       {.i =  0} },
	{ MODKEY|ShiftMask,     XK_Insert,      clippaste,      {.i =  0} },
	{ MODKEY|ShiftMask,     XK_C,           clipcopy,       {.i =  0} },
	{ MODKEY|ShiftMask,     XK_V,           clippaste,      {.i =  0} },
	{ MODKEY,               XK_Num_Lock,    numlock,        {.i =  0} },
    { XELT_SIZE_XK_NO_MOD,            XK_F11,         togglefullscreen,   {.i =  0} },
};

/*
 * Special keys (change & recompile st.info accordingly)
 *
 * Mask value:
 * * Use XELT_SIZE_XK_ANY_MOD to match the key no matter modifiers state
 * * Use XELT_SIZE_XK_NO_MOD to match the key alone (no modifiers)
 * appkey value:
 * * 0: no value
 * * > 0: keypad application mode enabled
 * *   = 2: terminal.numlock = 1
 * * < 0: keypad application mode disabled
 * appcursor value:
 * * 0: no value
 * * > 0: cursor application mode enabled
 * * < 0: cursor application mode disabled
 * crlf value
 * * 0: no value
 * * > 0: crlf mode is enabled
 * * < 0: crlf mode is disabled
 *
 * Be careful with the order of the definitions because st searches in
 * this table sequentially, so any XELT_SIZE_XK_ANY_MOD must be in the last
 * position for a key.
 */

/*
 * If you want keys other than the X11 function keys (0xFD00 - 0xFFFF)
 * to be mapped below, add them to this array.
 */
static KeySym mappedkeys[] = { -1 };

/*
 * State bits to ignore when matching key or button events.  By default,
 * numlock (Mod2Mask) and keyboard layout (XELT_SIZE_XK_SWITCH_MOD) are ignored.
 */
static uint ignoremod = Mod2Mask|XELT_SIZE_XK_SWITCH_MOD;

/*
 * Override mouse-select while mask is active (when XELT_TERMINAL_MOUSE is set).
 * Note that if you want to use ShiftMask with selmasks, set this to an other
 * modifier, set to 0 to not use it.
 */
static uint forceselmod = ShiftMask;

/*
 * This is the huge key array which defines all compatibility to the Linux
 * world. Please decide about changes wisely.
 */
static xelt_Key key[] = {
	/* keysym           mask            string      appkey appcursor crlf */
	{ XK_KP_Home,       ShiftMask,      "\033[2J",       0,   -1,    0},
	{ XK_KP_Home,       ShiftMask,      "\033[1;2H",     0,   +1,    0},
	{ XK_KP_Home,       XELT_SIZE_XK_ANY_MOD,     "\033[H",        0,   -1,    0},
	{ XK_KP_Home,       XELT_SIZE_XK_ANY_MOD,     "\033[1~",       0,   +1,    0},
	{ XK_KP_Up,         XELT_SIZE_XK_ANY_MOD,     "\033Ox",       +1,    0,    0},
	{ XK_KP_Up,         XELT_SIZE_XK_ANY_MOD,     "\033[A",        0,   -1,    0},
	{ XK_KP_Up,         XELT_SIZE_XK_ANY_MOD,     "\033OA",        0,   +1,    0},
	{ XK_KP_Down,       XELT_SIZE_XK_ANY_MOD,     "\033Or",       +1,    0,    0},
	{ XK_KP_Down,       XELT_SIZE_XK_ANY_MOD,     "\033[B",        0,   -1,    0},
	{ XK_KP_Down,       XELT_SIZE_XK_ANY_MOD,     "\033OB",        0,   +1,    0},
	{ XK_KP_Left,       XELT_SIZE_XK_ANY_MOD,     "\033Ot",       +1,    0,    0},
	{ XK_KP_Left,       XELT_SIZE_XK_ANY_MOD,     "\033[D",        0,   -1,    0},
	{ XK_KP_Left,       XELT_SIZE_XK_ANY_MOD,     "\033OD",        0,   +1,    0},
	{ XK_KP_Right,      XELT_SIZE_XK_ANY_MOD,     "\033Ov",       +1,    0,    0},
	{ XK_KP_Right,      XELT_SIZE_XK_ANY_MOD,     "\033[C",        0,   -1,    0},
	{ XK_KP_Right,      XELT_SIZE_XK_ANY_MOD,     "\033OC",        0,   +1,    0},
	{ XK_KP_Prior,      ShiftMask,      "\033[5;2~",     0,    0,    0},
	{ XK_KP_Prior,      XELT_SIZE_XK_ANY_MOD,     "\033[5~",       0,    0,    0},
	{ XK_KP_Begin,      XELT_SIZE_XK_ANY_MOD,     "\033[E",        0,    0,    0},
	{ XK_KP_End,        ControlMask,    "\033[J",       -1,    0,    0},
	{ XK_KP_End,        ControlMask,    "\033[1;5F",    +1,    0,    0},
	{ XK_KP_End,        ShiftMask,      "\033[K",       -1,    0,    0},
	{ XK_KP_End,        ShiftMask,      "\033[1;2F",    +1,    0,    0},
	{ XK_KP_End,        XELT_SIZE_XK_ANY_MOD,     "\033[4~",       0,    0,    0},
	{ XK_KP_Next,       ShiftMask,      "\033[6;2~",     0,    0,    0},
	{ XK_KP_Next,       XELT_SIZE_XK_ANY_MOD,     "\033[6~",       0,    0,    0},
	{ XK_KP_Insert,     ShiftMask,      "\033[2;2~",    +1,    0,    0},
	{ XK_KP_Insert,     ShiftMask,      "\033[4l",      -1,    0,    0},
	{ XK_KP_Insert,     ControlMask,    "\033[L",       -1,    0,    0},
	{ XK_KP_Insert,     ControlMask,    "\033[2;5~",    +1,    0,    0},
	{ XK_KP_Insert,     XELT_SIZE_XK_ANY_MOD,     "\033[4h",      -1,    0,    0},
	{ XK_KP_Insert,     XELT_SIZE_XK_ANY_MOD,     "\033[2~",      +1,    0,    0},
	{ XK_KP_Delete,     ControlMask,    "\033[M",       -1,    0,    0},
	{ XK_KP_Delete,     ControlMask,    "\033[3;5~",    +1,    0,    0},
	{ XK_KP_Delete,     ShiftMask,      "\033[2K",      -1,    0,    0},
	{ XK_KP_Delete,     ShiftMask,      "\033[3;2~",    +1,    0,    0},
	{ XK_KP_Delete,     XELT_SIZE_XK_ANY_MOD,     "\033[P",       -1,    0,    0},
	{ XK_KP_Delete,     XELT_SIZE_XK_ANY_MOD,     "\033[3~",      +1,    0,    0},
	{ XK_KP_Multiply,   XELT_SIZE_XK_ANY_MOD,     "\033Oj",       +2,    0,    0},
	{ XK_KP_Add,        XELT_SIZE_XK_ANY_MOD,     "\033Ok",       +2,    0,    0},
	{ XK_KP_Enter,      XELT_SIZE_XK_ANY_MOD,     "\033OM",       +2,    0,    0},
	{ XK_KP_Enter,      XELT_SIZE_XK_ANY_MOD,     "\r",           -1,    0,   -1},
	{ XK_KP_Enter,      XELT_SIZE_XK_ANY_MOD,     "\r\n",         -1,    0,   +1},
	{ XK_KP_Subtract,   XELT_SIZE_XK_ANY_MOD,     "\033Om",       +2,    0,    0},
	{ XK_KP_Decimal,    XELT_SIZE_XK_ANY_MOD,     "\033On",       +2,    0,    0},
	{ XK_KP_Divide,     XELT_SIZE_XK_ANY_MOD,     "\033Oo",       +2,    0,    0},
	{ XK_KP_0,          XELT_SIZE_XK_ANY_MOD,     "\033Op",       +2,    0,    0},
	{ XK_KP_1,          XELT_SIZE_XK_ANY_MOD,     "\033Oq",       +2,    0,    0},
	{ XK_KP_2,          XELT_SIZE_XK_ANY_MOD,     "\033Or",       +2,    0,    0},
	{ XK_KP_3,          XELT_SIZE_XK_ANY_MOD,     "\033Os",       +2,    0,    0},
	{ XK_KP_4,          XELT_SIZE_XK_ANY_MOD,     "\033Ot",       +2,    0,    0},
	{ XK_KP_5,          XELT_SIZE_XK_ANY_MOD,     "\033Ou",       +2,    0,    0},
	{ XK_KP_6,          XELT_SIZE_XK_ANY_MOD,     "\033Ov",       +2,    0,    0},
	{ XK_KP_7,          XELT_SIZE_XK_ANY_MOD,     "\033Ow",       +2,    0,    0},
	{ XK_KP_8,          XELT_SIZE_XK_ANY_MOD,     "\033Ox",       +2,    0,    0},
	{ XK_KP_9,          XELT_SIZE_XK_ANY_MOD,     "\033Oy",       +2,    0,    0},
	{ XK_Up,            ShiftMask,      "\033[1;2A",     0,    0,    0},
	{ XK_Up,            ControlMask,    "\033[1;5A",     0,    0,    0},
	{ XK_Up,            Mod1Mask,       "\033[1;3A",     0,    0,    0},
	{ XK_Up,            XELT_SIZE_XK_ANY_MOD,     "\033[A",        0,   -1,    0},
	{ XK_Up,            XELT_SIZE_XK_ANY_MOD,     "\033OA",        0,   +1,    0},
	{ XK_Down,          ShiftMask,      "\033[1;2B",     0,    0,    0},
	{ XK_Down,          ControlMask,    "\033[1;5B",     0,    0,    0},
	{ XK_Down,          Mod1Mask,       "\033[1;3B",     0,    0,    0},
	{ XK_Down,          XELT_SIZE_XK_ANY_MOD,     "\033[B",        0,   -1,    0},
	{ XK_Down,          XELT_SIZE_XK_ANY_MOD,     "\033OB",        0,   +1,    0},
	{ XK_Left,          ShiftMask,      "\033[1;2D",     0,    0,    0},
	{ XK_Left,          ControlMask,    "\033[1;5D",     0,    0,    0},
	{ XK_Left,          Mod1Mask,       "\033[1;3D",     0,    0,    0},
	{ XK_Left,          XELT_SIZE_XK_ANY_MOD,     "\033[D",        0,   -1,    0},
	{ XK_Left,          XELT_SIZE_XK_ANY_MOD,     "\033OD",        0,   +1,    0},
	{ XK_Right,         ShiftMask,      "\033[1;2C",     0,    0,    0},
	{ XK_Right,         ControlMask,    "\033[1;5C",     0,    0,    0},
	{ XK_Right,         Mod1Mask,       "\033[1;3C",     0,    0,    0},
	{ XK_Right,         XELT_SIZE_XK_ANY_MOD,     "\033[C",        0,   -1,    0},
	{ XK_Right,         XELT_SIZE_XK_ANY_MOD,     "\033OC",        0,   +1,    0},
	{ XK_ISO_Left_Tab,  ShiftMask,      "\033[Z",        0,    0,    0},
	{ XK_Return,        Mod1Mask,       "\033\r",        0,    0,   -1},
	{ XK_Return,        Mod1Mask,       "\033\r\n",      0,    0,   +1},
	{ XK_Return,        XELT_SIZE_XK_ANY_MOD,     "\r",            0,    0,   -1},
	{ XK_Return,        XELT_SIZE_XK_ANY_MOD,     "\r\n",          0,    0,   +1},
	{ XK_Insert,        ShiftMask,      "\033[4l",      -1,    0,    0},
	{ XK_Insert,        ShiftMask,      "\033[2;2~",    +1,    0,    0},
	{ XK_Insert,        ControlMask,    "\033[L",       -1,    0,    0},
	{ XK_Insert,        ControlMask,    "\033[2;5~",    +1,    0,    0},
	{ XK_Insert,        XELT_SIZE_XK_ANY_MOD,     "\033[4h",      -1,    0,    0},
	{ XK_Insert,        XELT_SIZE_XK_ANY_MOD,     "\033[2~",      +1,    0,    0},
	{ XK_Delete,        ControlMask,    "\033[M",       -1,    0,    0},
	{ XK_Delete,        ControlMask,    "\033[3;5~",    +1,    0,    0},
	{ XK_Delete,        ShiftMask,      "\033[2K",      -1,    0,    0},
	{ XK_Delete,        ShiftMask,      "\033[3;2~",    +1,    0,    0},
	{ XK_Delete,        XELT_SIZE_XK_ANY_MOD,     "\033[P",       -1,    0,    0},
	{ XK_Delete,        XELT_SIZE_XK_ANY_MOD,     "\033[3~",      +1,    0,    0},
	{ XK_BackSpace,     XELT_SIZE_XK_NO_MOD,      "\177",          0,    0,    0},
	{ XK_Home,          ShiftMask,      "\033[2J",       0,   -1,    0},
	{ XK_Home,          ShiftMask,      "\033[1;2H",     0,   +1,    0},
	{ XK_Home,          XELT_SIZE_XK_ANY_MOD,     "\033[H",        0,   -1,    0},
	{ XK_Home,          XELT_SIZE_XK_ANY_MOD,     "\033[1~",       0,   +1,    0},
	{ XK_End,           ControlMask,    "\033[J",       -1,    0,    0},
	{ XK_End,           ControlMask,    "\033[1;5F",    +1,    0,    0},
	{ XK_End,           ShiftMask,      "\033[K",       -1,    0,    0},
	{ XK_End,           ShiftMask,      "\033[1;2F",    +1,    0,    0},
	{ XK_End,           XELT_SIZE_XK_ANY_MOD,     "\033[4~",       0,    0,    0},
	{ XK_Prior,         ControlMask,    "\033[5;5~",     0,    0,    0},
	{ XK_Prior,         ShiftMask,      "\033[5;2~",     0,    0,    0},
	{ XK_Prior,         XELT_SIZE_XK_ANY_MOD,     "\033[5~",       0,    0,    0},
	{ XK_Next,          ControlMask,    "\033[6;5~",     0,    0,    0},
	{ XK_Next,          ShiftMask,      "\033[6;2~",     0,    0,    0},
	{ XK_Next,          XELT_SIZE_XK_ANY_MOD,     "\033[6~",       0,    0,    0},
	{ XK_F1,            XELT_SIZE_XK_NO_MOD,      "\033OP" ,       0,    0,    0},
	{ XK_F1, /* F13 */  ShiftMask,      "\033[1;2P",     0,    0,    0},
	{ XK_F1, /* F25 */  ControlMask,    "\033[1;5P",     0,    0,    0},
	{ XK_F1, /* F37 */  Mod4Mask,       "\033[1;6P",     0,    0,    0},
	{ XK_F1, /* F49 */  Mod1Mask,       "\033[1;3P",     0,    0,    0},
	{ XK_F1, /* F61 */  Mod3Mask,       "\033[1;4P",     0,    0,    0},
	{ XK_F2,            XELT_SIZE_XK_NO_MOD,      "\033OQ" ,       0,    0,    0},
	{ XK_F2, /* F14 */  ShiftMask,      "\033[1;2Q",     0,    0,    0},
	{ XK_F2, /* F26 */  ControlMask,    "\033[1;5Q",     0,    0,    0},
	{ XK_F2, /* F38 */  Mod4Mask,       "\033[1;6Q",     0,    0,    0},
	{ XK_F2, /* F50 */  Mod1Mask,       "\033[1;3Q",     0,    0,    0},
	{ XK_F2, /* F62 */  Mod3Mask,       "\033[1;4Q",     0,    0,    0},
	{ XK_F3,            XELT_SIZE_XK_NO_MOD,      "\033OR" ,       0,    0,    0},
	{ XK_F3, /* F15 */  ShiftMask,      "\033[1;2R",     0,    0,    0},
	{ XK_F3, /* F27 */  ControlMask,    "\033[1;5R",     0,    0,    0},
	{ XK_F3, /* F39 */  Mod4Mask,       "\033[1;6R",     0,    0,    0},
	{ XK_F3, /* F51 */  Mod1Mask,       "\033[1;3R",     0,    0,    0},
	{ XK_F3, /* F63 */  Mod3Mask,       "\033[1;4R",     0,    0,    0},
	{ XK_F4,            XELT_SIZE_XK_NO_MOD,      "\033OS" ,       0,    0,    0},
	{ XK_F4, /* F16 */  ShiftMask,      "\033[1;2S",     0,    0,    0},
	{ XK_F4, /* F28 */  ControlMask,    "\033[1;5S",     0,    0,    0},
	{ XK_F4, /* F40 */  Mod4Mask,       "\033[1;6S",     0,    0,    0},
	{ XK_F4, /* F52 */  Mod1Mask,       "\033[1;3S",     0,    0,    0},
	{ XK_F5,            XELT_SIZE_XK_NO_MOD,      "\033[15~",      0,    0,    0},
	{ XK_F5, /* F17 */  ShiftMask,      "\033[15;2~",    0,    0,    0},
	{ XK_F5, /* F29 */  ControlMask,    "\033[15;5~",    0,    0,    0},
	{ XK_F5, /* F41 */  Mod4Mask,       "\033[15;6~",    0,    0,    0},
	{ XK_F5, /* F53 */  Mod1Mask,       "\033[15;3~",    0,    0,    0},
	{ XK_F6,            XELT_SIZE_XK_NO_MOD,      "\033[17~",      0,    0,    0},
	{ XK_F6, /* F18 */  ShiftMask,      "\033[17;2~",    0,    0,    0},
	{ XK_F6, /* F30 */  ControlMask,    "\033[17;5~",    0,    0,    0},
	{ XK_F6, /* F42 */  Mod4Mask,       "\033[17;6~",    0,    0,    0},
	{ XK_F6, /* F54 */  Mod1Mask,       "\033[17;3~",    0,    0,    0},
	{ XK_F7,            XELT_SIZE_XK_NO_MOD,      "\033[18~",      0,    0,    0},
	{ XK_F7, /* F19 */  ShiftMask,      "\033[18;2~",    0,    0,    0},
	{ XK_F7, /* F31 */  ControlMask,    "\033[18;5~",    0,    0,    0},
	{ XK_F7, /* F43 */  Mod4Mask,       "\033[18;6~",    0,    0,    0},
	{ XK_F7, /* F55 */  Mod1Mask,       "\033[18;3~",    0,    0,    0},
	{ XK_F8,            XELT_SIZE_XK_NO_MOD,      "\033[19~",      0,    0,    0},
	{ XK_F8, /* F20 */  ShiftMask,      "\033[19;2~",    0,    0,    0},
	{ XK_F8, /* F32 */  ControlMask,    "\033[19;5~",    0,    0,    0},
	{ XK_F8, /* F44 */  Mod4Mask,       "\033[19;6~",    0,    0,    0},
	{ XK_F8, /* F56 */  Mod1Mask,       "\033[19;3~",    0,    0,    0},
	{ XK_F9,            XELT_SIZE_XK_NO_MOD,      "\033[20~",      0,    0,    0},
	{ XK_F9, /* F21 */  ShiftMask,      "\033[20;2~",    0,    0,    0},
	{ XK_F9, /* F33 */  ControlMask,    "\033[20;5~",    0,    0,    0},
	{ XK_F9, /* F45 */  Mod4Mask,       "\033[20;6~",    0,    0,    0},
	{ XK_F9, /* F57 */  Mod1Mask,       "\033[20;3~",    0,    0,    0},
	{ XK_F10,           XELT_SIZE_XK_NO_MOD,      "\033[21~",      0,    0,    0},
	{ XK_F10, /* F22 */ ShiftMask,      "\033[21;2~",    0,    0,    0},
	{ XK_F10, /* F34 */ ControlMask,    "\033[21;5~",    0,    0,    0},
	{ XK_F10, /* F46 */ Mod4Mask,       "\033[21;6~",    0,    0,    0},
	{ XK_F10, /* F58 */ Mod1Mask,       "\033[21;3~",    0,    0,    0},
	{ XK_F11,           XELT_SIZE_XK_NO_MOD,      "\033[23~",      0,    0,    0},
	{ XK_F11, /* F23 */ ShiftMask,      "\033[23;2~",    0,    0,    0},
	{ XK_F11, /* F35 */ ControlMask,    "\033[23;5~",    0,    0,    0},
	{ XK_F11, /* F47 */ Mod4Mask,       "\033[23;6~",    0,    0,    0},
	{ XK_F11, /* F59 */ Mod1Mask,       "\033[23;3~",    0,    0,    0},
	{ XK_F12,           XELT_SIZE_XK_NO_MOD,      "\033[24~",      0,    0,    0},
	{ XK_F12, /* F24 */ ShiftMask,      "\033[24;2~",    0,    0,    0},
	{ XK_F12, /* F36 */ ControlMask,    "\033[24;5~",    0,    0,    0},
	{ XK_F12, /* F48 */ Mod4Mask,       "\033[24;6~",    0,    0,    0},
	{ XK_F12, /* F60 */ Mod1Mask,       "\033[24;3~",    0,    0,    0},
	{ XK_F13,           XELT_SIZE_XK_NO_MOD,      "\033[1;2P",     0,    0,    0},
	{ XK_F14,           XELT_SIZE_XK_NO_MOD,      "\033[1;2Q",     0,    0,    0},
	{ XK_F15,           XELT_SIZE_XK_NO_MOD,      "\033[1;2R",     0,    0,    0},
	{ XK_F16,           XELT_SIZE_XK_NO_MOD,      "\033[1;2S",     0,    0,    0},
	{ XK_F17,           XELT_SIZE_XK_NO_MOD,      "\033[15;2~",    0,    0,    0},
	{ XK_F18,           XELT_SIZE_XK_NO_MOD,      "\033[17;2~",    0,    0,    0},
	{ XK_F19,           XELT_SIZE_XK_NO_MOD,      "\033[18;2~",    0,    0,    0},
	{ XK_F20,           XELT_SIZE_XK_NO_MOD,      "\033[19;2~",    0,    0,    0},
	{ XK_F21,           XELT_SIZE_XK_NO_MOD,      "\033[20;2~",    0,    0,    0},
	{ XK_F22,           XELT_SIZE_XK_NO_MOD,      "\033[21;2~",    0,    0,    0},
	{ XK_F23,           XELT_SIZE_XK_NO_MOD,      "\033[23;2~",    0,    0,    0},
	{ XK_F24,           XELT_SIZE_XK_NO_MOD,      "\033[24;2~",    0,    0,    0},
	{ XK_F25,           XELT_SIZE_XK_NO_MOD,      "\033[1;5P",     0,    0,    0},
	{ XK_F26,           XELT_SIZE_XK_NO_MOD,      "\033[1;5Q",     0,    0,    0},
	{ XK_F27,           XELT_SIZE_XK_NO_MOD,      "\033[1;5R",     0,    0,    0},
	{ XK_F28,           XELT_SIZE_XK_NO_MOD,      "\033[1;5S",     0,    0,    0},
	{ XK_F29,           XELT_SIZE_XK_NO_MOD,      "\033[15;5~",    0,    0,    0},
	{ XK_F30,           XELT_SIZE_XK_NO_MOD,      "\033[17;5~",    0,    0,    0},
	{ XK_F31,           XELT_SIZE_XK_NO_MOD,      "\033[18;5~",    0,    0,    0},
	{ XK_F32,           XELT_SIZE_XK_NO_MOD,      "\033[19;5~",    0,    0,    0},
	{ XK_F33,           XELT_SIZE_XK_NO_MOD,      "\033[20;5~",    0,    0,    0},
	{ XK_F34,           XELT_SIZE_XK_NO_MOD,      "\033[21;5~",    0,    0,    0},
	{ XK_F35,           XELT_SIZE_XK_NO_MOD,      "\033[23;5~",    0,    0,    0},
};

/*
 * xelt_Selection types' masks.
 * Use the same masks as usual.
 * Button1Mask is always unset, to make masks match between ButtonPress.
 * ButtonRelease and MotionNotify.
 * If no match is found, regular selection is used.
 */
static uint selmasks[] = {
	[XELT_SEL_RECTANGULAR] = Mod1Mask,
};

/*
 * Printable characters in ASCII, used to estimate the advance width
 * of single wide characters.
 */
static char ascii_printable[] =
	" !\"#$%&'()*+,-./0123456789:;<=>?"
	"@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_"
	"`abcdefghijklmnopqrstuvwxyz{|}~";

