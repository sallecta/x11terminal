#if   defined(__linux)
	#include <pty.h>
#elif defined(__OpenBSD__) || defined(__NetBSD__) || defined(__APPLE__)
	#include <util.h>
#elif defined(__FreeBSD__) || defined(__DragonFly__)
	#include <libutil.h>
#endif

#include "xelt_defines.h"
#include "xelt_enums.h"
#include "xelt_types.h"
#include "xelt_macroses.h"

char charbuf256[256];
 
 /* function definitions used in config.h */
static void clipcopy(const xelt_Arg *);
static void clippaste(const xelt_Arg *);
static void numlock(const xelt_Arg *);
static void selpaste(const xelt_Arg *);
static void xzoom(const xelt_Arg *);
static void xzoomabs(const xelt_Arg *);
static void xzoomreset(const xelt_Arg *);
static void printsel(const xelt_Arg *);
static void printscreen(const xelt_Arg *) ;
static void toggleprinter(const xelt_Arg *);
static void sendbreak(const xelt_Arg *);
static void togglefullscreen(const xelt_Arg *);
//
static void printAndExit(const char *, ...);
static void draw(void);
static void redraw(void);
static void drawregion(int, int, int, int);
static void execsh(void);
static void stty(void);
static void sigchld(int);
static void run(void);

static void csidump(void);
static void csihandle(void);
static void csiparse(void);
static void csireset(void);
static int eschandle(xelt_uchar);
static void strdump(void);
static void strhandle(void);
static void strparse(void);
static void strreset(void);

static void tprinter(char *, size_t);
static void tdumpsel(void);
static void tdumpline(int);
static void tdump(void);
static void tclearregion(int, int, int, int);
static void tcursor(int);
static void tdeletechar(int);
static void tdeleteline(int);
static void tinsertblank(int);
static void tinsertblankline(int);
static int tlinelen(int);
static void tmoveto(int, int);
static void tmoveato(int, int);
static void tnew(int, int);
static void tnewline(int);
static void tputtab(int);
static void tputc(xelt_CharCode);
static void treset(void);
static void tresize(int, int);
static void tscrollup(int, int);
static void tscrolldown(int, int);
static void tsetcolorattr(int *, int);
static void tsetchar(xelt_CharCode, xelt_Glyph *, int, int);
static void tsetscroll(int, int);
static void tswapscreen(void);
static void tsetdirt(int, int);
static void tsetmode(int, int, int *, int);
static void tfulldirt(void);
// static void techo(xelt_CharCode);
static void tcontrolcode(xelt_uchar );
static void tdectest(char );
static void tdeftran(char);
static inline int match(xelt_uint, xelt_uint);
static void ttynew(void);
static size_t ttyread(void);
static void ttyresize(void);
static void ttywrite1(const char *, size_t);
static void tstrsequence(xelt_uchar);

static inline xelt_ushort sixd_to_16bit(int);
static int xmakeglyphfontspecs(XftGlyphFontSpec *, const xelt_Glyph *, int, int, int);
static void xdrawglyphfontspecs(const XftGlyphFontSpec *, xelt_Glyph, int, int, int);
static void xdrawglyph(xelt_Glyph, int, int);
static void xhints(void);
static void xclear(int, int, int, int);
static void xdrawcursor(void);
static void xinit(void);
static void xloadcols(void);
static int xsetcolorname(int, const char *);
static int xgeommasktogravity(int);
static int xloadfont(xelt_Font *, FcPattern *);
static void xloadfonts(char *, double);
static void xsettitle(char *);
static void window_title_set(void);
static void xsetpointermotion(int);
static void xseturgency(int);
static void xsetsel(char *, Time);
static void xunloadfont(xelt_Font *);
static void xunloadfonts(void);
static void xresize(int, int);

static void evhandler_expose(XEvent *);
static void evhandler_visibility(XEvent *);
static void evhandler_unmap(XEvent *);
static char *kmap(KeySym, xelt_uint);
static void evhandler_keypress(XEvent *);
static void evhandler_clientmsg(XEvent *);
static void cresize(int, int);
static void evhandler_configure(XEvent *);
static void evhandler_focus(XEvent *);
static void evhandler_btnrelease(XEvent *);
static void evhandler_btnpress(XEvent *);
static void evhandler_motion(XEvent *);
static void evhandler_propnotify(XEvent *);
static void evhandler_selnotify(XEvent *);
static void evhandler_selclear(XEvent *);
static void evhandler_selrequest(XEvent *);

static void selinit(void);
static void selnormalize(void);
static inline int selected(int, int);
static char *getsel(void);
static void selcopy(Time);
static void selscroll(int, int);
static void selsnap(int *, int *, int);
static int x2col(int);
static int y2row(int);
static void getbuttoninfo(XEvent *);
static void mousereport(XEvent *);

static size_t utf8decode(char *, xelt_CharCode *, size_t);
static xelt_CharCode utf8decodebyte(char, size_t *);
static size_t utf8encode(xelt_CharCode, char *);
static char utf8encodebyte(xelt_CharCode, size_t);
static char *utf8strchr(char *s, xelt_CharCode u);
static size_t utf8validate(xelt_CharCode *, size_t);

static ssize_t xwrite(int, const char *, size_t);
static void *xmalloc(size_t);
static void *xrealloc(void *, size_t);
static char *xstrdup(char *);

struct timespec drawtimeout;
struct timespec  *tv = NULL;
struct timespec  now;
struct timespec last;
//struct timespec lastblink;