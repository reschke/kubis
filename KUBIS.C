/*
	@(#)Kubis/kubis.c
	
	Julian F. Reschke, 20. Juli 1998
*/

#include <tos.h>
#include <vdi.h>
#include <aes.h>
#include <dragdrop.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <dosix\sys\mintbind.h>
#include <dosix\sys\os.h>
#include <dosix\sys\va.h>
#include <dosix\errno.h>

#include "kubrsc.rsh"
#include "kubrsc.h"

#include <flydial\clip.h>
#include <flydial\flydial.h>
#include <flydial\wndl.h>

#include "bubble.h"

#define PROGNAME "Kubis '99"

#define WM_BUTTOMED		33
#define WM_ICONIFY		34
#define WM_UNICONIFY	35
#define WM_ALLICONIFY	36

#define WF_ICONIFY		26
#define WF_UNICONIFY	27
#define WF_UNICONIFYXYWH	28

#define REDRAW_ALL	0
#define REDRAW_CHANGED 17

static int
has_iconify (void)
{
	static int hasit = -1;
	
	if (hasit < 0)
	{
		int dummy, val = 0;
	
		hasit = 0;
		
		appl_xgetinfo (11, &val, &dummy, &dummy, &dummy);
		if (val & 128) hasit = 1;
	}
	
	return hasit;
}

static int
has_bottom (void)
{
	static int hasit = -1;
	
	if (hasit < 0)
	{
		int dummy, val = 0;
	
		hasit = 0;
		
		appl_xgetinfo (11, &val, &dummy, &dummy, &dummy);
		if (val & 64) hasit = 1;
	}
	
	return hasit;
}

/* National Language Support */

typedef struct
{
	char *cant_open_window,
		*nobody,
		*cant_write_to_clipboard,
		*cant_open_score_file,
		*cant_write_to_score_file,
		*points_and_date,
		*game_over,
		*yes_or_no,
		*cancel, *ok, *cont, *copy,
		*left, *tleft, *tright, *right, *drop, *closew, *end,
		*new_score_title,
		*enter_name,
		*copyright,
		*bubble_score, *bubble_preview, *bubble_copyr;
} STRINGS;

STRINGS English =
{
	"Kubis: can't open window!",
	"nobody",
	"Kubis: can't write to clipboard!",
	"Kubis: can't open score file `%s' (%s)!",
	"Kubis: can't write to score file `%s' (%s)!",
	"%6ld points on %s",
	"Kubis: Game over - do you want to see the high score table?",
	"[Yes|[No",
	"[Cancel", "[OK", "[Continue", "[Copy",
	"left", "turn left", "turn right", "right", "drop",
	"close window", "end program",
	"New high score",
	"Please enter your name:",
	PROGNAME"|"
	"|Kubis is freeware. You may copy it, but commercial "
	"distribution (CD-ROMs etc.) requires the written "
	"permission of the author. If you wish to say 'thank you', "
	"send money to organizations like Amnesty International "
	"or GreenPeace!||"
	"Julian F. Reschke (jr@ms.maus.de)",
	"your game score", "preview of the next stone",
	"Kubis '99||Copyright (c) 1998 Julian F. Reschke",
};

STRINGS German =
{
	"Kubis: kann Fenster nicht îffnen!",
	"Niemand",
	"Kubis: kann Clipboard nicht beschreiben!",
	"Kubis: kann Scoredatei `%s' nicht îffnen (%s)!",
	"Kubis: kann Scoredatei `%s' nicht beschreiben (%s)!",
	"%6ld Punkte am %s",
	"Kubis: Spiel vorbei - wollen Sie die Highscore-Tabelle sehen?",
	"[Ja|[Nein",
	"[Abbruch", "[OK", "[Weiter", "[Kopieren",
	"nach links", "links drehen", "rechts drehen", "nach rechts",
	"fallen lassen", "Fenster schlieûen", "Beenden",
	"Neuer Highscore",
	"Bitte Namen eingeben:",
	PROGNAME"|"
	"|Kubis ist Freeware. Es darf frei weitergegeben werden, "
	"jedwede kommerzielle Benutzung (etwa auf CD-ROM) "
	"muû vom Autor genehmigt "
	"werden. Wer mag, Åberweise eine kleine Spende an eine "
	"gemeinnÅtzige Organisation wie Amnesty International "
	"oder Greenpeace!||"
	"Julian F. Reschke (jr@ms.maus.de)",
	"der Punktestand", "der nÑchste Stein",
	"Kubis '99||Copyright (c) 1998 Julian F. Reschke",
};

STRINGS *S = &English;

static void
set_language (void)
{
	int locale = DOGetLocale ("LC_MESSAGES");

	switch (locale)
	{
		case DO_LOCALE_GERMAN:	S = &German; break;
	}
}





/* ********************************** */
int StartLevel = 200;
int Level;
int TimerValue;
int iconified = FALSE;
int terminated = FALSE;


/* Sound-Kram */

#define LOAD(a,b)	(a),(b)
#define WAIT(c)		0xff,c
#define END			WAIT(0)

#define FREQ(a,b)	(a*2),(char)((125000L/(b))&0xff),(a*2+1),(char)((125000L/(b))/256)
#define LOOP(a,b,c,d)	0x80,(b),0x81,(a),(d),(c)

#if 0
static unsigned char belldata[] = 
{
	LOAD (0,0x34),
	LOAD (1,0),
	LOAD (2,0),
	LOAD (3,0),
	LOAD (4,0),
	LOAD (5,0),
	LOAD (6,0),
	LOAD (7,0xfe),
	LOAD (8,16),
	LOAD (9,0),
	LOAD (10,0),
	LOAD (11,0),
	LOAD (12,16),
	LOAD (13,9),
	END
};
#endif

static unsigned char pong[] = 
{
	LOAD (7, 0xff),
	FREQ (0, 131),
	LOAD (8, 16),
	LOAD (11, 0),
	LOAD (12, 2),
	LOAD (13, 9),
	LOAD (7, 0xff - 1),
	END
};

static unsigned char loesch[] = 
{
	LOAD (7, 0xff),
	FREQ (0, 262),
	FREQ (1, 330),
	FREQ (2, 392),
	LOAD (8, 16),
	LOAD (9, 16),
	LOAD (10, 16),
	LOAD (11, 0),
	LOAD (12, 16),
	LOAD (13, 9),
	LOAD (7, 0xff - 1),
	WAIT (5),
	LOAD (7, 0xff - 2),
	WAIT (5),
	LOAD (7, 0xff - 4 - 2 - 1),
	END
};

static unsigned char bang[] = 
{
	LOAD (7, 0xff),
	FREQ (0, 131),
	LOAD (8, 16),
	LOAD (11, 0),
	LOAD (12, 4),
	LOAD (13, 9),
	LOAD (7, 0xff - 1),
	END
};

/* play "down" sound */

static void
sound_down (void)
{
	int freq = 440 - Level;

	static unsigned char down[] = 
	{
		FREQ (0, 440),
		LOAD (6, 10),
		LOAD (7, 0xff - 1 - 4),
		LOAD (8, 4),
		WAIT (2),
		LOAD (8, 0),
		END
	};

	down[1] = (125000L/freq) & 255; down[3] = (125000L/freq) / 256;

	Dosound (down);
}


/* Variablen fÅr Icon-Redraw */

int fonts_loaded = 0;
int num_colors;

#define TIMEFAC		4

#define Random7         ((unsigned int)xbios(17))%7
#define Random4         ((unsigned int)xbios(17))&3
#define FALSE 0
#define NIL (&dummy)
#define MAX(a,b) ((a>b)?a:b)
#define MIN(a,b) ((a<b)?a:b)

char *
sccsid (void)
{
	return "@(#)"PROGNAME", Release B, Copyright (c) Julian F. Reschke, "__DATE__;
}

#define KILLER_BLOCK	7

extern unsigned char Blocks[8][4][4];

int workout[57];
int VDIHandle,window_handle,WW,WH,CWidth,CHeight,InitX,InitY,id;
int MId = -1;
int desk_x, desk_y, desk_w, desk_h;
int dummy,NoScores;
int MessageDepth;

char default_name[30];


/* Bubble GEM */

void
bubble_help (const char *string, int x, int y, int modal, int delay)
{
	int msg[8];
	int bubble_id;
	static int has_prot = -1;
	char *btext;
	size_t l = 5 + strlen (string);

	if (has_prot == -1)
	{
		has_prot = 0;
		if (DOGetCookie ('MiNT', NULL) || DOGetCookie ('MagX', NULL))
			has_prot = 1;
	}
	
	btext = Mxalloc (l, 0x20);
	if ((long)btext == EINVFN) btext = Malloc (l);
	
	if (!btext) return;

    /* Puffer fÅllen */
    strcpy (btext, string);

    bubble_id = appl_find ("BUBBLE  ");

    if (bubble_id >= 0)
    {
        msg[0] = BUBBLEGEM_SHOW;
        msg[1] = id;
        msg[2] = 0;
        msg[3] = x;
        msg[4] = y;
        *((char **)(&msg[5])) = btext;
        msg[7] = modal;

        if (appl_write (bubble_id, 16, msg) == 0)
        {
            Mfree (btext);
            return;
        }
        
        if (modal)
        {
        	int cx, cy, cm, dummy, first_cm;
        
        	graf_mkstate (&x, &y, &first_cm, &dummy);
        	evnt_timer (delay, 0);

			do
			{
				graf_mkstate (&cx, &cy, &cm, &dummy);
			} while (cx == x && cy == y && cm == first_cm);
	
        	msg[0] = BUBBLEGEM_HIDE;
        	appl_write (bubble_id, 16, msg);
        }
	}        
    
    Mfree (btext);
}


void
myhelp (OBJECT *tree, int ob, int x, int y)
{
	BGEM *bg;
	int actform;
	MFORM actmouse;
	int delay = 200;
	unsigned long bhlp;
	
	if (!DOGetCookie ('BGEM', (long *)&bg)) return;
	if (bg->magic != 'BGEM' || bg->release < 6) return;
	if (DOGetCookie ('BHLP', (long *)&bhlp)) delay = (int)(bhlp >> 16);

	if (tree == rs_trindex[HELP])
	{
		switch (ob)
		{
			case HLEFT:
				GrafGetForm (&actform, &actmouse);
				GrafMouse (USER_DEF, bg->mhelp);
				bubble_help ("links", x, y, 1, delay);
				GrafMouse (actform, &actmouse);
				break;
		
			default:
				break;
		}
	}
}


/* SIGTERM handler */

static void
term_handler (void)
{
	terminated = TRUE;
}

char ScoreLine[]="Score: 87654";
int ScoreIndex = 0;
char MyPath[256],FullPath[256];
unsigned long Score,Hz200,ActHz,OldHz;


#define	ESC		27
#define LEFT    0x4b00
#define RIGHT   0x4d00
#define DOWN	0x5000
#define	UP		0x4800

#define NUMSCORES       7


#define WIDTH 10


typedef struct
{
        unsigned int valid, x, y, w, h;
} WINDOW;

WINDOW game = { FALSE, 0, 0, 0, 0}, icon = {FALSE, 0, 0, 0, 0};

typedef struct
{
	char	Name[30];
	long	Score;
	int		Time, Date;
} ONESCORE;


ONESCORE TheScores[NUMSCORES];

typedef struct
{
        int     color,interior,style;
} COLORTYPE;

COLORTYPE MonoTable[]=
{
                0,1,0,
                1,2,2,
                1,2,3,
                1,2,4,
                1,2,5,
                1,2,6,
                1,2,7,
                1,2,1,
                1,2,8,
};


COLORTYPE FullTable[]=
{
                LWHITE,2,8,
                RED,2,8,
                GREEN,2,8,
                BLUE,2,8,
                CYAN,2,8,
                YELLOW,2,8,
                MAGENTA,2,8,
                LBLACK,2,8,
                BLACK,2,8
};

COLORTYPE *Colors;

unsigned int Screen[12][25];
unsigned int Old[12][25];

int running;
int falling,game_paused,WhichOne,WhichTurn,AtX,AtY;

int next_stone = -1;
int next_phase;

static void treeinit (OBJECT *t)
{
	int i = 0;
			
	do
	{
		rsrc_obfix (t, i++);
	} while (!(t[i-1].ob_flags & LASTOB));
}



static void
set_fill_attributes (int c, int s, int i)
{
	static int org_c = -1;
	static int org_s = -1;
	static int org_i = -1;

	if (c != org_c) vsf_color (VDIHandle, c);
	if (s != org_s) vsf_style (VDIHandle, s);
	if (i != org_i) vsf_interior (VDIHandle, i);
	org_i = i; org_s = s; org_c = c;
}

void
DoIconRedraw (int x, int y, int w, int h)
{
	int dx, dy, dw, dh;
	int x1, y1, w1, h1;
	int hsize, vsize, margin;

	hsize = icon.w / 10;
	margin = (icon.w % hsize) >> 1;
	vsize = (int)(((long)hsize * workout[3]) / workout[4]);
	
	graf_mouse (M_OFF, NULL);
	wind_get (window_handle, WF_FIRSTXYWH, &dx, &dy, &dw, &dh);

	while (dw && dh)
	{	
		int xy[4];
		int i, j;

		if (RectInter (x, y, w, h, dx, dy, dw, dh, &x1, &y1, &w1,
			&h1))
		{
			RectAES2VDI (x1, y1, w1, h1, xy);
 			vs_clip (VDIHandle, 1, xy);
	
			set_fill_attributes (Colors[0].color, Colors[0].style,
				Colors[0].interior);
			v_bar (VDIHandle, xy);
		
			for (i = 21; i > (20 - (icon.h / vsize)); i--)
				for (j = 0; j < 10; j++)
				{
					int xy[4];
					int colnum;
					COLORTYPE *c;
					
					RectAES2VDI (icon.x + margin + j * hsize,
						icon.y + icon.h - (22 - i) * vsize,
						hsize,
						vsize, xy);
	
					colnum = Screen[j+1][i];
					c = &Colors[colnum];
				
					if (colnum)
					{	
						if (c->color != -1)
						{
							set_fill_attributes (c->color, c->style, c->interior);

							v_bar (VDIHandle, xy);
	
							RastDrawRect (VDIHandle,
								icon.x + margin + j * hsize,
								icon.y + icon.h - (22 - i) * vsize,
								hsize,
								vsize);
						}
					}
			}
		}
			
		wind_get (window_handle, WF_NEXTXYWH, &dx, &dy, &dw, &dh);
	}
	graf_mouse (M_ON, NULL);
}

int ClipDIF (const char *filetype)
{
	char fullname[128];
	FILE *fp;
	int i;

	ClipFindFile (filetype, fullname);
	fp = fopen (fullname,"w");

	if (!fp)
	{
		DialFatal (S->cant_write_to_clipboard);
		return FALSE;
	}

	for (i=0; i<NUMSCORES; i++)
		fprintf(fp,"'%s', %ld\n",TheScores[i].Name,TheScores[i].Score);

	fclose (fp);
	return (!ferror(fp));
}


void PerformClip (void)
{
	int Ok = TRUE;

	GrafMouse (HOURGLASS,0L);
	ClipClear (NULL);
	Ok = ClipDIF ("TXT");
	if (!Ok) ClipClear (NULL);
	GrafMouse (ARROW, NULL);
}

void CloseWindow (void)
{
	wind_close (window_handle);
	wind_delete (window_handle);
}

long GetHz200 (void)
{
	OldHz = Hz200;
	Hz200 = *(long *)0x4ba;
	return Hz200;
}

static long
get_hz200 (void)
{
	return *(long *)0x4ba;
}



void Terminate (int num)
{
	extern int _app;

	WindUpdate (END_UPDATE);

	switch (num)
	{
		case 2:
			wind_close (window_handle);
			wind_delete (window_handle);
		case 1:
			if (fonts_loaded) vst_unload_fonts (VDIHandle, 0);
			v_clsvwk (VDIHandle);
			DialFatal (S->cant_open_window);
	}
	
	if (!_app) evnt_keybd();

	appl_exit ();
	exit(0);
}


void InitScores (const char *filename, int err)
{
	int i;

	for (i = 0; i < NUMSCORES; i++)
	{
		TheScores[i].Name[0] =
		TheScores[i].Score =
		TheScores[i].Time =
		TheScores[i].Date = 0;
	}
	GrafMouse (ARROW, 0L);
	DialWarn (S->cant_open_score_file, filename, strerror (err));
}


void ReadScores (void)
{
	long h;
	long Res;

	GrafMouse (HOURGLASS, NULL);

	h = Fopen (FullPath, 0);

	if (h < 0)
	{
		InitScores (FullPath, (int) h);
	}
	else
	{
		Res = Fread ((int)h, (long)(NUMSCORES*sizeof(ONESCORE)), TheScores);
		if (Res != (long)(NUMSCORES*sizeof(ONESCORE)))
			InitScores (FullPath, EUNEXPECTEDEOF);
		Fclose ((int)h);
		strcpy (default_name, TheScores[0].Name);
	}
	
	GrafMouse (ARROW, NULL);
}



void WriteScores (void)
{
	long h;
	long Res;

	NoScores = FALSE;
	GrafMouse (HOURGLASS, NULL);
	h = Fcreate (FullPath, 0);

	if (h < 0) 
	{
		DialWarn (S->cant_write_to_score_file, FullPath, strerror
			((int) h));
		NoScores = TRUE;
		GrafMouse (ARROW, NULL);
		return;
	}

	Res = Fwrite ((int) h, NUMSCORES*sizeof(ONESCORE), TheScores);

	if (Res != NUMSCORES * sizeof(ONESCORE))
	{
		DialWarn (S->cant_write_to_score_file, FullPath, EUNEXPECTEDEOF);
		NoScores = TRUE;
	}

	Fclose ((int) h);
	GrafMouse (ARROW, NULL);
}


void DoHelp (void)
{
	static OBJECT *o = NULL;
	DIALINFO D;
	int exob;

	if (!o) {
		o = rs_trindex[HELP];
		treeinit (o);
		
		o[HLEFT].ob_spec.free_string = S->left;
		o[HTLEFT].ob_spec.free_string = S->tleft;
		o[HTRIGHT].ob_spec.free_string = S->tright;
		o[HRIGHT].ob_spec.free_string = S->right;
		o[HDROP].ob_spec.free_string = S->drop;
		o[HCLOSEW].ob_spec.free_string = S->closew;
		o[HEND].ob_spec.free_string = S->end;

		ObjcTreeInit (o);
	}
	
	DialCenter(o);
	DialStart(o,&D);
	DialDraw(&D);
	FormSetHelpFun (myhelp, 0);
	exob = 0x7fff & DialDo (&D, 0);
	o[exob].ob_state &= ~SELECTED;
	DialEnd(&D);
	
	if (!(o[exob].ob_flags & DEFAULT))
	{
		DialSay (S->copyright);
		DoHelp ();
	}
}





void
ShowScores (void)
{
	static int date_lines[] = {SC1, SC2, SC3, SC4, SC5, SC6, SC7};
	int bt;
	ONESCORE *t=TheScores;
	int i;
	char *c;
	static OBJECT *o = NULL;
	DIALINFO D;

	if (!o) {
		o = rs_trindex[SCORES];
		treeinit (o);

		o[SCCOPY].ob_spec.free_string = S->copy;
		o[SCCOPY].ob_width = HandXSize * (int) (1 + strlen (S->copy));

		ObjcTreeInit (o);
	}
	
	DialCenter (o);

/* TEST TEST TEST */
#ifdef TEST
	{
		WNDL_INFO *W = WndlStart (o);
		
		if (W)
		{
			WndlOpen (W);		
			WndlDo (W);
			WndlEnd (W);
		}
	}
#endif

	DialStart (o, &D);

	for (i = 0; i < NUMSCORES; i++)
	{
		char timestring[20];
		time_t ti = DOTostime2Unix (t[i].Date, t[i].Time);
		struct tm timeptr = * (localtime (&ti));
		
		strftime (timestring, (int) sizeof (timestring),
			"%a, %x, %R", &timeptr);

		c = o[SCLINE1 + i*3].ob_spec.free_string;
		strncpy (&c[4], t[i].Name, 30);
		c[34] = 0;

		sprintf (o[date_lines[i]].ob_spec.tedinfo->te_ptext,
			S->points_and_date, t[i].Score, timestring);
	}

	DialDraw(&D);
	
	do
	{
		o[SCCOPY].ob_state &= (~DISABLED);
	
		ObjcChange(o,SCCOPY,0,o->ob_x,o->ob_y,o->ob_width,
			o->ob_height,o[SCCOPY].ob_state,1);

		bt = 0x7f & DialDo(&D,0);
		ObjcDsel(o,bt);
		if (bt == SCCOPY) PerformClip();
	} while (bt != SCOK);
	
	DialEnd(&D);
}




void
InsertScore (void)
{
	int i,j;
	ONESCORE *s;
	int bt;
	DIALINFO DNewScore;
	static OBJECT *TNewScore = NULL;
	
	if (!TNewScore)
	{
		TNewScore = rs_trindex[NEWSCORE];
		treeinit (TNewScore);

		TNewScore[NSTITLE].ob_spec.free_string = S->new_score_title;
		TNewScore[NSTITLE].ob_width = HandXSize * (int)
			strlen (S->new_score_title);
		TNewScore[NSNENTER].ob_spec.free_string = S->enter_name;

		ObjcTreeInit (TNewScore);
	}
	
	DialCenter (TNewScore);	
	DialStart (TNewScore, &DNewScore);

	TNewScore[OK].ob_state = TNewScore[ABBRUCH].ob_state = NORMAL;  
	strcpy(TNewScore[TNAME].ob_spec.tedinfo->te_ptext, default_name);
	DialDraw(&DNewScore);
	bt = 0x7f & DialDo(&DNewScore, NULL);
	ObjcDsel(TNewScore,bt);
	DialEnd(&DNewScore);

	if (bt == OK)
	{
		i = 0; s = TheScores;
		while((s[i].Score) > Score) i++;
	
		j = NUMSCORES-2;
		while(j >= i)
		{
			s[j + 1] = s[j];
			j --;       
		}

		s[i].Score = Score;
		s[i].Date = Tgetdate ();
		s[i].Time = Tgettime ();
		memcpy(s[i].Name,TNewScore[TNAME].ob_spec.tedinfo->te_ptext,30);
		WriteScores();
		ShowScores();
	}
}




void DoScores (void)
{
	if (Score > TheScores[NUMSCORES-1].Score)
		InsertScore();
	else
	{
		if (0 == DialAsk (S->yes_or_no, 0, S->game_over))
			ShowScores();
	}
}	

/* sende eine Redraw-Message an mich selbst */

void
send_self_redraw (int x, int y, int w, int h, int mode)
{
	int mbuf[8];
	int *m = mbuf;

	*m++ = WM_REDRAW;
	*m++ = mode;
	*m++ = 0;
	*m++ = window_handle;
	*m++ = x;
	*m++ = y;
	*m++ = w;
	*m   = h;

	appl_write (id, 16, mbuf);
}

void
full_redraw (void)
{
	int x, y, w, h;
	
	wind_get (window_handle, WF_WORKXYWH, &x, &y, &w, &h);
	send_self_redraw (x, y, w, h, REDRAW_ALL);
}

void
send_redraw (void)
{
	if (MessageDepth >= 3)
	{
		full_redraw ();
		MessageDepth = 0;
	}
	else
	{
		int x, y, dummy;
		
		MessageDepth++;

		wind_get (window_handle, WF_WORKXYWH, &x, &y, &dummy, &dummy);

		send_self_redraw (x + (AtX - 4) * CWidth,
			y + (AtY - 4) * CHeight, 12 * CWidth, 12 * CHeight,
			REDRAW_CHANGED);
	}
}
	

int IsOK (int num, int turn, int x, int y)
{
	int i, j;
	unsigned char tmp;

	for (i = 0; i < 4; i++)
	{
		if (i + y > 0)
		{
			tmp = Blocks[num][turn][i];
			for (j = 0; j < 4; j++)
			{
				if ((tmp & 1) && (Screen[x+j+1][y+i])) return FALSE;
				tmp >>= 1;
			}
		}
	}

	return TRUE;
}

static void
set_score (unsigned long n)
{
	static long oldscore;
	
	if (n == 0) oldscore = 0;

	sprintf (ScoreLine, "Score: %05ld", n);

	/* Die gibt es nur noch bis zur hîchsten Geschwindigkeit */

	if ((oldscore / 1000) != (n / 1000))
		if (Level > 1)
			next_stone = KILLER_BLOCK;
		
	oldscore = n;
}


void Draw (int x, int y, int w, int h, int mode)
{
	int cx,cy;
	int xy[4],StartI,EndI,StartJ,EndJ;
	int i,j;
	int wx, wy, ww, wh;
	COLORTYPE *c;

	if (mode == REDRAW_CHANGED && MessageDepth) MessageDepth--;

	set_fill_attributes (1, 0, 1);
	vswr_mode (VDIHandle, MD_REPLACE);
	wind_get (window_handle, WF_WORKXYWH, &wx, &wy, &ww, &wh);
	RectAES2VDI (wx, wy, ww, HandYSize+1, xy);
	v_bar (VDIHandle, xy);
		
	/* Next stone */
	
	if (next_stone >= 0)
	{
		int i, j;
		int size = HandYSize / 5;
		int offset = ((HandYSize - (size * 4)) + 1) / 2;
	
		set_fill_attributes (0, 2, 8);
		
		for (i = 0; i < 4; i++)
		{
			for (j = 0; j < 4; j++)
			{
				if (Blocks[next_stone][next_phase][i] & (1 << j))
				{
					int xy[4];
					COLORTYPE *c;

					c = &Colors[next_stone + 1];
					if (c->color == BLACK) c = &Colors[0];
					
					set_fill_attributes (c->color, c->style, c->interior);

					RectAES2VDI (wx + ww + (j - 5) * size,
						wy + offset + i * size, size, size, xy);
					v_bar (VDIHandle, xy);
				}
			}
		}		
	}	
	
	vswr_mode (VDIHandle, MD_XOR);
	v_gtext (VDIHandle, wx, wy, &ScoreLine[ScoreIndex]);
	wy += 1 + HandYSize;
	
	vswr_mode(VDIHandle,MD_REPLACE);
	StartJ = (y-wy)/CHeight;
	if(StartJ<0) StartJ = 0;
	EndJ = ((h+y)-wy)/CHeight;
	if(EndJ>19) EndJ=19;
	StartI = (x - wx) / CWidth;
	if(StartI<0) StartI = 0;
	EndI = (w + x - wx) / CWidth;
	if(EndI>9) EndI = 9;


	for (j = StartJ; j <= EndJ; j++)
	{
		cy = wy + (j*CHeight);
		for (i = StartI; i <= EndI; i++)
		{
			cx = wx + i * CWidth;

			if ((Old[i+1][j+2] != Screen[i+1][j+2]) || mode != REDRAW_CHANGED)
			{
				int colnum;

				RectAES2VDI (cx, cy, CWidth, CHeight, xy);
				colnum = Screen[i+1][j+2];
				c = &Colors[colnum];
				set_fill_attributes (c->color, c->style, c->interior);

#if 0
				/* In Farbe: Kachelmuster */
				if (colnum == 0 && c->color == LWHITE)
				{
					if ((i + j) & 1)
						set_fill_attributes (WHITE, 8, 2);
					else
						set_fill_attributes (LWHITE, 8, 2);
	
				}
#endif

				v_bar (VDIHandle, xy);

				/* RÑnder malen */

				if (colnum && c->color != BLACK)
					RastDrawRect (VDIHandle, cx, cy, CWidth, CHeight);

			}
			Old[i+1][j+2] = Screen[i+1][j+2];
		}
	}
}



static void
redraw (int x, int y, int w, int h, int mode)
{
	int xy[4], dx, dy, dw, dh, x1, y1, w1, h1;

/* printf ("Redraw for %d %d %d %d\n", x, y, w, h); */

	GrafMouse (M_OFF, NULL);

	RectInter (x, y, w, h, desk_x, desk_y, desk_w, desk_h,
		&x, &y, &w, &h);
	wind_get (window_handle, WF_FIRSTXYWH, &dx, &dy, &dw, &dh);

	while (dw && dh)
	{
		if (RectInter (x, y, w, h, dx, dy, dw, dh, &x1, &y1, &w1, &h1))
		{
			RectAES2VDI (x1, y1, w1, h1, xy);
			vs_clip (VDIHandle, TRUE, xy);
			Draw (x1, y1, w1, h1, mode);
		}
		wind_get (window_handle, WF_NEXTXYWH, &dx, &dy, &dw, &dh);
	}

	GrafMouse (M_ON, NULL);
}	


/* Redraw, das nicht Åber die Message-Pipe geht */

static void
full_direct_redraw (void)
{
	int x, y, w, h;

	wind_get (window_handle, WF_WORKXYWH, &x, &y, &w, &h);
	redraw (x, y, w, h, REDRAW_CHANGED);
}

void
SetBlock (int num, int turn, int posx, int posy)
{
	int i,j;
	unsigned char tmp;

	for (i = 0; i < 4; i++)
	{
		if (i + posy > 1)
		{
			tmp = Blocks[num][turn][i];

			for (j = 0; j < 4; j++)
			{
				if (tmp & 1)
					Screen[j+posx+1][i+posy] ^= num+1;
				tmp >>= 1;
			}
		}
	}
}


void InitFlags (void)
{
	game_paused = FALSE;
	falling = FALSE;
	running = TRUE;
}


/* Sucht nach gefÅllten Zeilen */

static int
find_lines (void)
{
	int j;

	for (j = 21; j > 0; j--)
	{
		int i, full = TRUE;

		for (i = 1; i < 11; i++)
			if (!Screen[i][j])
				full = FALSE;
		
		if (full) return j;
	}

	return -1;
}


/* Lîscht gefÅllte Zeilen und sorgt fÅr Redraw */

static void
kill_lines (void)
{
	int which, i, j;

	which = find_lines ();

	if (which > 0)
	{
		long start_time = Supexec (get_hz200);
		long elapsed;
		
		if (Level > 1) Level--;

		Score += (200/ ( (int)((22-which)) ) );

		Dosound (loesch);

		for (i = 1;
			i < 30 && Supexec (get_hz200) - start_time < 50;
			i++)
		{	
			Screen[1 + (int)(Random () % 10)][which] = 0;
			full_direct_redraw ();
		}
		
		elapsed = (Supexec (get_hz200) - start_time) * 5;
		
		if (elapsed < 150)
			evnt_timer ((int) (150 - elapsed), 0);

		for (j = which; j > 0; j--)
			for (i = 1; i < 11; i++)
				Screen[i][j] = Screen[i][j-1];

		full_direct_redraw ();
		kill_lines ();
	}
}

void BringOn (void)
{
	falling = TRUE;

	kill_lines ();

	if (next_stone < 0)
	{
		next_stone = Random7;
		next_phase = Random4;
	}
		
	WhichOne = next_stone;
	WhichTurn = next_phase;

	next_stone = Random7;
	next_phase = Random4;

	AtX = 3;
	AtY = 0;

	if (IsOK (WhichOne, WhichTurn, AtX, AtY))
	{
		SetBlock (WhichOne, WhichTurn, AtX, AtY);
		send_redraw ();
	}
	else
	{
		running = FALSE;
		if (!NoScores) DoScores();
	}
}	


/* Splat-Effekt */

static void
splat (int x, int y, int *stones)
{
	int sign = -1;

	if (Random4 & 2) sign = 1;
	
	Screen[x][y] = KILLER_BLOCK + 1;
	full_direct_redraw ();

	if (! *stones) return;
	*stones -= 1;

	if (Screen[x][y + 1] == 0 && y < 21)
		splat (x, y + 1, stones);

	if (! *stones) return;

	if (Screen[x - sign][y] == 0)
		splat (x - sign, y, stones);

	if (! *stones) return;

	if (Screen[x + sign][y] == 0)
		splat (x + sign, y, stones);

	if (! *stones) return;

	if (Screen[x][y - 1] == 0 && y > 1)
		splat (x, y - 1, stones);
}



static void
deeper (void)
{
	sound_down ();

	SetBlock (WhichOne, WhichTurn, AtX, AtY);

	if (IsOK (WhichOne, WhichTurn, AtX, AtY+1))
	{
		SetBlock (WhichOne, WhichTurn, AtX, ++AtY);
		send_redraw ();
	}
	else
	{
/*		int maxdepth = 4; */
	
		SetBlock (WhichOne, WhichTurn, AtX, AtY);
		
/*		if (WhichOne == KILLER_BLOCK)
			splat (AtX + 2, AtY + 1, &maxdepth); */
		
		Dosound (bang);

		falling = FALSE;
		set_score (Score += 5);
		full_direct_redraw ();
	}
}

static void
drop_it (void)
{
	int maxdepth = 4;

	SetBlock (WhichOne, WhichTurn, AtX, AtY);

	while (IsOK (WhichOne, WhichTurn, AtX, ++AtY))
		Score++;

	SetBlock (WhichOne, WhichTurn, AtX, --AtY);

	Dosound (bang);
	
	if (WhichOne == KILLER_BLOCK)
		splat (AtX + 2, AtY + 1, &maxdepth);
	
	falling = FALSE;
	set_score (Score += 5);
	full_direct_redraw ();
}


static void
init_screen (void)
{
	int i, j;

	set_score (Score = 0);
	TimerValue = Level = StartLevel;
	ActHz = TimerValue / TIMEFAC;

	for(i = 0; i < 12; i++)
		for (j = 0; j < 25; j++)
			Old[i][j] = Screen[i][j] = ((j>=22)||(i==0)||(i==11));
}



void ClearScreen (void)
{
	running = FALSE;
	init_screen ();
}

void NewScreen (void)
{
	init_screen ();
	full_redraw ();
}


void DoEvents(void)
{
	int actwd, which;
	int mb[8];
	int done = FALSE;
	MEVENT E;

	GrafMouse (ARROW, NULL);

	wind_get (window_handle, WF_CURRXYWH, &game.x, &game.y,
		&game.w, &game.h);

	while (!done && !terminated)
	{       
		wind_get (0, WF_TOP, &actwd, NIL, NIL, NIL);
		E.e_flags = MU_MESAG|MU_TIMER|MU_KEYBD;
		E.e_time = 2000L;
		
		if (actwd == window_handle && window_handle >= 0)
			E.e_time = TimerValue;

		E.e_mepbuf = mb;
		which = evnt_event (&E);
	
		if (which & MU_KEYBD)
		{
			if ((E.e_ks & 12) == 12 && (E.e_kr & 0xff00) == 0x3900
				&& has_iconify ())
			{
				which &= ~MU_KEYBD; which |= MU_MESAG;
				mb[0] = WM_ICONIFY;
				mb[3] = window_handle;
			}
		}
		
		if (which & MU_MESAG)
		{
			switch (mb[0])
			{
				case WM_BUTTOMED:
					wind_set (mb[3], WF_BOTTOM);
					break;

				case WM_MOVED:
					if (mb[3] == window_handle)
					{
						if (!iconified)
						{
							game.x = mb[4];
							game.y = mb[5];
						}
						else
						{
							icon.x = mb[4];
							icon.y = mb[5];
							icon.w = mb[6];
							icon.h = mb[7];
						}
						wind_set (window_handle, WF_CURRXYWH,
							mb[4], mb[5], mb[6], mb[7]);
					}     
					break;

				case WM_CLOSED:
					done = TRUE;
					CloseWindow();
					break;

				case WM_TOPPED:
				case WM_NEWTOP:
					if (mb[3] == window_handle)
						wind_set (mb[3], WF_TOP);
					break;

				case AC_OPEN:
				case 0x4711:
					if (window_handle >= 0)
						wind_set (window_handle, WF_TOP);
					break;
		
				case AC_CLOSE:
					if(mb[3]==MId)
					{
						done = TRUE;
						window_handle = -1;
					}
					break;

				case WM_REDRAW:
					if (mb[3] == window_handle)
					{
						if (!iconified)
						{
							WindUpdate (BEG_UPDATE);
							redraw (mb[4], mb[5], mb[6], mb[7], mb[1]);
							WindUpdate (END_UPDATE);
						}
						else
						{
							WindUpdate (BEG_UPDATE);
							DoIconRedraw (mb[4], mb[5], mb[6], mb[7]);
							WindUpdate (END_UPDATE);
						}
					}
					break;

				case AP_TERM:
					terminated = TRUE;
#if 0
{
	int mb[8];
	
	mb[0] = AP_TFAIL;
	shel_write (10, 0, 0, mb, 0);
}
#endif
					break;
					
				case AP_DRAGDROP:
					{
						static char pipename[] = "U:\\PIPE\\DRAGDROP.AA";
						long fd;
						
						pipename[18] = mb[7] & 0x00ff;
						pipename[17] = (mb[7] & 0xff00) >> 8;
						
						fd = Fopen (pipename, 2);
						if (fd >= 0)
						{
							char c = DD_NAK;
							
							Fwrite ((int) fd, 1, &c);
							Fclose ((int) fd);
						}
					}
					break;

				case WM_ICONIFY:
				case WM_ALLICONIFY:
				case WM_UNICONIFY:
					if (mb[3] == window_handle)
					{
						if (!iconified)
						{
							iconified = TRUE;
 							wind_close (mb[3]);
							wind_set (mb[3], WF_ICONIFY, mb[4], mb[5],
								mb[6], mb[7]);
 							wind_open (mb[3], -1, -1, -1, -1);
							wind_get (mb[3], WF_WORKXYWH,
								&icon.x, &icon.y, &icon.w, &icon.h);
/*							if (has_bottom ())
								wind_set (mb[3], WF_BOTTOM);
*/						}
						else
						{
							iconified = FALSE;
							wind_set (mb[3], WF_UNICONIFY,
								game.x, game.y, game.w, game.h);
							wind_set (mb[3], WF_TOP);
						}
					}
					break;

				case BUBBLEGEM_REQUEST:
					if (mb[3] == window_handle)
					{
						int x, y, w, dummy;
						int rel_x, rel_y;
						
						wind_get (window_handle, WF_WORKXYWH, &x, &y, &w, &dummy);

						rel_x = mb[4] - x; rel_y = mb[5] - y;
					
						if (iconified)
							bubble_help (S->bubble_copyr, mb[4], mb[5], 0, 0);
						else
							if (rel_y >= 0 && rel_y < HandYSize)
							{
								if (rel_x >= 0 && rel_x < 12 * HandXSize)
									bubble_help (S->bubble_score, mb[4], mb[5], 0, 0);
								else if (rel_x < w && rel_x > w - 2 * HandXSize)
									bubble_help (S->bubble_preview, mb[4], mb[5], 0, 0);
							}
					}
					break;


				default:
/*					fprintf (stderr, "%02x\n", mb[0]); */
					break;
		}		
	}

	if (which & MU_KEYBD)
	{
		int c = E.e_kr;
		
		if (c & 0xff) c &= 0xff;

		sound_down ();
	
		switch (c)
		{
			case 17:	 /* CTRL-Q */
				done = TRUE;
				CloseWindow ();
				ClearScreen ();
				break;

			case 21:
				done = TRUE;
				CloseWindow ();
				break;

			case ' ':
			case '*':
			case '-':
			case '~':
			case '0':
				if (!running)
				{
					InitFlags();
					NewScreen();
				}
				else
					if (falling) drop_it ();
				break;

		case '7':
		case LEFT:
				if(falling)
				{
					SetBlock(WhichOne,WhichTurn,AtX,AtY);
					if(IsOK(WhichOne,WhichTurn,AtX-1,AtY))
					{
						AtX--;
						SetBlock (WhichOne,WhichTurn,AtX,AtY);
					}
					else
					{
						SetBlock (WhichOne,WhichTurn,AtX,AtY);
						Dosound (pong);
					}
					send_redraw();
				}
				break;

		case '8':
		case DOWN:
				if(falling)
		    {
					SetBlock(WhichOne,WhichTurn,AtX,AtY);
					if(IsOK(WhichOne,(WhichTurn-1)&3,AtX,AtY))
					{
						WhichTurn -= 1;
						WhichTurn &= 3;
						SetBlock(WhichOne,WhichTurn,AtX,AtY);
						if(Score) Score--;
					}
					else
					{
						SetBlock(WhichOne,WhichTurn,AtX,AtY);
						Dosound (pong);
					}
					send_redraw();
				}
				break;

			case ')':
			case UP:
				if(falling)
				{
					SetBlock(WhichOne,WhichTurn,AtX,AtY);
					if(IsOK(WhichOne,(WhichTurn+1)&3,AtX,AtY))
					{
						WhichTurn += 1;
						WhichTurn &= 3;
						SetBlock(WhichOne,WhichTurn,AtX,AtY);
					}
					else
						SetBlock(WhichOne,WhichTurn,AtX,AtY);
					send_redraw();
				}
				break;

			case '9':
			case RIGHT:
				if(falling)
				{
					SetBlock(WhichOne,WhichTurn,AtX,AtY);
					if(IsOK(WhichOne,WhichTurn,AtX+1,AtY))
					{
						AtX++;
						SetBlock(WhichOne,WhichTurn,AtX,AtY);
					}
					else
					{
						SetBlock(WhichOne,WhichTurn,AtX,AtY);
						Dosound (pong);
					}
					send_redraw();
				}
				break;

			case ESC:
			case 13:
				if (running && !iconified)
				{
					if (!falling)
					{
						if (!game_paused)
	       					BringOn();
	       			}
	       			else
					{
						if(AtY & 1) Score++;
							deeper ();
					}
				}
				break;			
			
			case 0x6200:
				DoHelp ();
				break;

			case 'p':
			case 'P':
				game_paused ^= TRUE;
				break;

			default:
				va_key (E.e_ks, E.e_kr);
				break; 
			}
		}

		if ((which & MU_TIMER) && running && !iconified)
		{
			Supexec (GetHz200);

			if ((Hz200 - OldHz) > ActHz)
				ActHz++;
			else
				ActHz--;

			if (ActHz * TIMEFAC > Level && TimerValue > 0)
				TimerValue--;
			else
				if (ActHz * TIMEFAC < Level)
					TimerValue++;

			if (!falling)
			{
				if (!game_paused)
					BringOn();
			}
			else
				deeper ();			
		}
	}
}

int OpenWindow(void)
{
	extern int _app;
	GRECT g;
	unsigned int MHeight;
	int dummy;
	static char title[]=" "PROGNAME" ";

	ReadScores();

	game_paused = FALSE;
	MessageDepth = 0;
	HandScreenSize (&desk_x, &desk_y, &desk_w, &desk_h);
	MHeight = desk_h;

	num_colors = workout[13];
	CHeight = MHeight/24;
	if((CHeight*workout[4])> 6000L) CHeight = 6000/workout[4];
	CWidth = (CHeight*workout[4])/workout[3];
	wind_calc(WC_BORDER,NAME|CLOSER|MOVER,0,0,CWidth*10,CHeight*20+HandYSize+1,
				NIL,NIL,&WW,&WH);
	if((CWidth*10)<(HandXSize*14)) ScoreIndex = 7;

	g.g_w = WW;
	g.g_h = WH;
	WindCenter (&g);
	InitX = g.g_x;
	InitY = g.g_y;

	if(game.valid)
	{
		InitX = game.x;
		InitY = game.y;
	}
    else
	{
		game.x = InitX;
		game.y = InitY;
		game.valid = TRUE;
	}

	window_handle = wind_create (CLOSER|MOVER|NAME|0x4000, InitX, InitY,
	 WW, WH);

	if(window_handle < 0)
	{
		if(!_app)
			return(1);
		else
			Terminate(1);
	}
	wind_set (window_handle, WF_NAME, title);

	wind_open(window_handle,InitX,InitY,WW,WH);


#if 0
{
	char name[500];
	
	name[0] = '\0';
	
	wind_get (window_handle, WF_NAME, name);
	DialSay (name);
}
#endif


	vst_alignment(VDIHandle,0,5,NIL,NIL);

	Colors = MonoTable;
	if (num_colors >= 16) Colors = FullTable;
	
	return 2;
}

int main (void)
{
	extern int _app;
	int mbuf[8], event, dum;
	static int workin[]={1,1,1,1,1,1,1,1,1,1,2};
	
	set_language ();
	strcpy (default_name, S->nobody);

	id = appl_init();
	
	if (!_app || (_GemParBlk.global[0] >= 0x400))
		MId = menu_register (id, "  "PROGNAME);
	
	/* AP_TERM nur dann, wenn nicht ACC */
	if (0 != appl_xgetinfo (10, &event, &dum, &dum, &dum))
		if ((event & 0xff) >= 9 && _app)
			shel_write (9, 1, 0, NULL, NULL);
		
	WindUpdate (BEG_UPDATE);
	DialInit (Malloc, Mfree);

	DialSetButtons (S->cont, S->cancel, S->ok);

	VDIHandle = HandAES;
	v_opnvwk (workin, &VDIHandle, workout);
	vsf_perimeter (VDIHandle, 1);

	if (HandSFId != vst_font (VDIHandle, HandSFId))
	{
		if (vq_gdos ())
		{
			fonts_loaded = 1;
			vst_load_fonts (VDIHandle, 0);
		}
		
		vst_font (VDIHandle, HandSFId);
	}
	
	vst_height (VDIHandle, HandSFHeight, mbuf, mbuf, mbuf, mbuf);

	TimerValue = Level = StartLevel;

	{
		/* Score-File finden */

		char scfile1[]="\\usr\\games\\kubis\\kubis.inf";
		char scfile2[]="kubis\\kubis.inf";
		char scfile3[]="kubis.inf";
	
		strcpy (FullPath, scfile1);
		if (!shel_find (FullPath))
		{
			strcpy (FullPath, scfile2);
			if (!shel_find (FullPath))
			{
				strcpy (FullPath, scfile3);
				shel_find (FullPath);
			}
		}
	}

	WindUpdate (END_UPDATE);

	Psignal (15, term_handler);
	
	ClearScreen();
	if (_app)
	{

#if 1
{

WORD msg[8];
WORD bubble_id;

    bubble_id = appl_find("BUBBLE  ");

    if (bubble_id >= 0)
    {
        msg[0] = 0xBABE;
        msg[1] = id;
        msg[2] = 0;
        msg[3] = 97;
        msg[4] = 8;
        msg[5] = 
        msg[6] = 
        msg[7] = 0;
        if (appl_write(bubble_id, 16, msg) == 0)
        {
            /* Fehler */
        }
    }
}
#endif

va_open ("http://www.muenster.de/~reschke/index.html");

		OpenWindow ();
		DoEvents ();
		DialExit ();
		appl_exit ();
		if (fonts_loaded) vst_unload_fonts (VDIHandle, 0);
		v_clsvwk (VDIHandle);
	}
	else
	{
		while (!terminated)
		{
			evnt_mesag(mbuf);
			if (mbuf[0] == AC_OPEN || mbuf[0] == 0x4711)
			{
				if(OpenWindow()!=2)
					DialFatal (S->cant_open_window);
				else
					DoEvents();
			}
		}
	}
	return 0;
}
