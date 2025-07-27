/* GEM Resource C Source */

#include <portab.h>
#include <aes.h>
#include "KUBRSC.H"

#if !defined(WHITEBAK)
#define WHITEBAK    0x0040
#endif
#if !defined(DRAW3D)
#define DRAW3D      0x0080
#endif

#define FLAGS9  0x0200
#define FLAGS10 0x0400
#define FLAGS11 0x0800
#define FLAGS12 0x1000
#define FLAGS13 0x2000
#define FLAGS14 0x4000
#define FLAGS15 0x8000
#define STATE8  0x0100
#define STATE9  0x0200
#define STATE10 0x0400
#define STATE11 0x0800
#define STATE12 0x1000
#define STATE13 0x2000
#define STATE14 0x4000
#define STATE15 0x8000

TEDINFO rs_tedinfo[] =
{ "012345678901234567890123456789",
  "______________________________",
  "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXX",
  IBM  , 0, TE_CNTR , 0x1180, 0, -1, 31, 31,
  "KUBIS \04798  COPYRIGHT \275 98  JULIAN RESCHKE",
  "\0",
  "\0",
  SMALL, 0, TE_CNTR , 0x1180, 0, -1, 42, 1,
  "AM 12.13.24 UM 23:12 MIT 12345 PUNKTE1",
  "\0",
  "\0",
  SMALL, 0, TE_CNTR , 0x1180, 0, -1, 39, 1,
  "AM 12.13.24 UM 23:12 MIT 12345 PUNKTE2",
  "\0",
  "\0",
  SMALL, 0, TE_CNTR , 0x1180, 0, -1, 39, 1,
  "AM 12.13.24 UM 23:12 MIT 12345 PUNKTE3",
  "\0",
  "\0",
  SMALL, 0, TE_CNTR , 0x1180, 0, -1, 39, 1,
  "AM 12.13.24 UM 23:12 MIT 12345 PUNKTE4",
  "\0",
  "\0",
  SMALL, 0, TE_CNTR , 0x1180, 0, -1, 39, 1,
  "AM 12.13.24 UM 23:12 MIT 12345 PUNKTE5",
  "\0",
  "\0",
  SMALL, 0, TE_CNTR , 0x1180, 0, -1, 39, 1,
  "AM 12.13.24 UM 23:12 MIT 12345 PUNKTE6",
  "\0",
  "\0",
  SMALL, 0, TE_CNTR , 0x1180, 0, -1, 39, 1,
  "AM 12.13.24 UM 23:12 MIT 12345 PUNKTE7",
  "\0",
  "\0",
  SMALL, 0, TE_CNTR , 0x1180, 0, -1, 39, 1,
  "Copyright \275 1998  JULIAN F. RESCHKE",
  "\0",
  "\0",
  SMALL, 0, TE_CNTR , 0x1100, 0, 0, 36, 1,
  "^U",
  "\0",
  "\0",
  IBM  , 0, TE_CNTR , 0x1180, 0, -1, 3, 1,
  "^Q",
  "\0",
  "\0",
  IBM  , 0, TE_CNTR , 0x1180, 0, -1, 3, 1
};

OBJECT rs_object[] =
{ 
  /******** Tree 0 NEWSCORE ****************************************************/
        -1,        1, NSNENTER        , G_BOX             |0x1800,   /* Object 0  */
  NONE, OUTLINED, (LONG)0x00021100L,
  0x0000, 0x0000, 0x0023, 0x0009,
  TNAME           ,       -1,       -1, G_IBOX            |0x1100,   /* Object 1  */
  TOUCHEXIT, CROSSED|OUTLINED, (LONG)0x00FF1101L,
  0x0021, 0x0000, 0x0002, 0x0001,
  ABBRUCH         ,       -1,       -1, G_FTEXT           ,   /* Object 2 TNAME */
  EXIT|EDITABLE, OUTLINED, (LONG)&rs_tedinfo[0],
  0x0002, 0x0005, 0x001E, 0x0001,
  OK              ,       -1,       -1, G_BUTTON          |0x1200,   /* Object 3 ABBRUCH */
  SELECTABLE|EXIT, NORMAL, (LONG)"[Abbruch",
  0x000D, 0x0007, 0x0009, 0x0001,
  NSTITLE         ,       -1,       -1, G_BUTTON          |0x1200,   /* Object 4 OK */
  SELECTABLE|DEFAULT|EXIT, NORMAL, (LONG)"[OK",
  0x0018, 0x0007, 0x0009, 0x0001,
  NSNENTER        ,       -1,       -1, G_STRING          |0x1300,   /* Object 5 NSTITLE */
  NONE, NORMAL, (LONG)"Neuer Highscore",
  0x0002, 0x0001, 0x000F, 0x0001,
         0,       -1,       -1, G_STRING          ,   /* Object 6 NSNENTER */
  LASTOB, NORMAL, (LONG)"Bitte Namen eingeben:",
  0x0002, 0x0003, 0x0015, 0x0001,
  
  /******** Tree 1 SCORES ****************************************************/
        -1,        1, SCOK            , G_BOX             |0x1800,   /* Object 0  */
  NONE, OUTLINED, (LONG)0x00021100L,
  0x0000, 0x0000, 0x0026, 0x0016,
         2,       -1,       -1, G_IBOX            |0x1100,   /* Object 1  */
  TOUCHEXIT, CROSSED|OUTLINED, (LONG)0x00FF1101L,
  0x0024, 0x0000, 0x0002, 0x0001,
         3,       -1,       -1, G_STRING          |0x1300,   /* Object 2  */
  NONE, NORMAL, (LONG)"Kubis - Top Seven",
  0x0002, 0x0001, 0x0011, 0x0001,
         4,       -1,       -1, G_TEXT            ,   /* Object 3  */
  NONE, NORMAL, (LONG)&rs_tedinfo[1],
  0x0000, 0x0003, 0x0026, 0x0001,
         7, SCLINE1         , SC1             , G_IBOX            ,   /* Object 4  */
  NONE, NORMAL, (LONG)0x00001100L,
  0x0001, 0x0004, 0x0024, 0x0002,
  SC1             ,       -1,       -1, G_STRING          ,   /* Object 5 SCLINE1 */
  NONE, NORMAL, (LONG)" 1. 0123456789012345678901234567891",
  0x0000, 0x0001, 0x0023, 0x0001,
         4,       -1,       -1, G_TEXT            ,   /* Object 6 SC1 */
  NONE, NORMAL, (LONG)&rs_tedinfo[2],
  0x0001, 0x0000, 0x0022, 0x0001,
        10, SCLINE2         , SC2             , G_IBOX            ,   /* Object 7  */
  NONE, NORMAL, (LONG)0x00001100L,
  0x0001, 0x0006, 0x0024, 0x0002,
  SC2             ,       -1,       -1, G_STRING          ,   /* Object 8 SCLINE2 */
  NONE, NORMAL, (LONG)" 2. 0123456789012345678901234567892",
  0x0000, 0x0001, 0x0023, 0x0001,
         7,       -1,       -1, G_TEXT            ,   /* Object 9 SC2 */
  NONE, NORMAL, (LONG)&rs_tedinfo[3],
  0x0001, 0x0000, 0x0022, 0x0001,
        13, SCLINE3         , SC3             , G_IBOX            ,   /* Object 10  */
  NONE, NORMAL, (LONG)0x00001100L,
  0x0001, 0x0008, 0x0024, 0x0002,
  SC3             ,       -1,       -1, G_STRING          ,   /* Object 11 SCLINE3 */
  NONE, NORMAL, (LONG)" 3. 0123456789012345678901234567893",
  0x0000, 0x0001, 0x0023, 0x0001,
        10,       -1,       -1, G_TEXT            ,   /* Object 12 SC3 */
  NONE, NORMAL, (LONG)&rs_tedinfo[4],
  0x0001, 0x0000, 0x0022, 0x0001,
        16, SCLINE4         , SC4             , G_IBOX            ,   /* Object 13  */
  NONE, NORMAL, (LONG)0x00001100L,
  0x0001, 0x000A, 0x0024, 0x0002,
  SC4             ,       -1,       -1, G_STRING          ,   /* Object 14 SCLINE4 */
  NONE, NORMAL, (LONG)" 4. 0123456789012345678901234567894",
  0x0000, 0x0001, 0x0023, 0x0001,
        13,       -1,       -1, G_TEXT            ,   /* Object 15 SC4 */
  NONE, NORMAL, (LONG)&rs_tedinfo[5],
  0x0001, 0x0000, 0x0022, 0x0001,
        19, SCLINE5         , SC5             , G_IBOX            ,   /* Object 16  */
  NONE, NORMAL, (LONG)0x00001100L,
  0x0001, 0x000C, 0x0024, 0x0002,
  SC5             ,       -1,       -1, G_STRING          ,   /* Object 17 SCLINE5 */
  NONE, NORMAL, (LONG)" 5. 0123456789012345678901234567895",
  0x0000, 0x0001, 0x0023, 0x0001,
        16,       -1,       -1, G_TEXT            ,   /* Object 18 SC5 */
  NONE, NORMAL, (LONG)&rs_tedinfo[6],
  0x0001, 0x0000, 0x0022, 0x0001,
        22, SCLINE6         , SC6             , G_IBOX            ,   /* Object 19  */
  NONE, NORMAL, (LONG)0x00001100L,
  0x0001, 0x000E, 0x0024, 0x0002,
  SC6             ,       -1,       -1, G_STRING          ,   /* Object 20 SCLINE6 */
  NONE, NORMAL, (LONG)" 6. 0123456789012345678901234567896",
  0x0000, 0x0001, 0x0023, 0x0001,
        19,       -1,       -1, G_TEXT            ,   /* Object 21 SC6 */
  NONE, NORMAL, (LONG)&rs_tedinfo[7],
  0x0001, 0x0000, 0x0022, 0x0001,
  SCCOPY          , SCLINE7         , SC7             , G_IBOX            ,   /* Object 22  */
  NONE, NORMAL, (LONG)0x00001100L,
  0x0001, 0x0010, 0x0024, 0x0002,
  SC7             ,       -1,       -1, G_STRING          ,   /* Object 23 SCLINE7 */
  NONE, NORMAL, (LONG)" 7. 0123456789012345678901234567897",
  0x0000, 0x0001, 0x0023, 0x0001,
        22,       -1,       -1, G_TEXT            ,   /* Object 24 SC7 */
  NONE, NORMAL, (LONG)&rs_tedinfo[8],
  0x0001, 0x0000, 0x0022, 0x0001,
  SCOK            ,       -1,       -1, G_BUTTON          |0x1200,   /* Object 25 SCCOPY */
  SELECTABLE|EXIT, NORMAL, (LONG)"[Kopieren",
  0x0002, 0x0014, 0x0009, 0x0001,
         0,       -1,       -1, G_BUTTON          |0x1200,   /* Object 26 SCOK */
  SELECTABLE|DEFAULT|EXIT|LASTOB, NORMAL, (LONG)"[OK",
  0x001D, 0x0014, 0x0007, 0x0001,
  
  /******** Tree 2 HELP ****************************************************/
        -1,        1, HEND            , G_BOX             |0x1800,   /* Object 0  */
  NONE, OUTLINED, (LONG)0x00021100L,
  0x0000, 0x0000, 0x0026, 0x0015,
         2,       -1,       -1, G_STRING          |0x1300,   /* Object 1  */
  NONE, NORMAL, (LONG)"Kubis \04799",
  0x0002, 0x0001, 0x0009, 0x0001,
         3,       -1,       -1, G_TEXT            ,   /* Object 2  */
  NONE, NORMAL, (LONG)&rs_tedinfo[9],
  0x0000, 0x0003, 0x0024, 0x0001,
         4,       -1,       -1, G_BOXCHAR         ,   /* Object 3  */
  NONE, OUTLINED|SHADOWED, (LONG) ((LONG)'7' << 24)|0x00FF1100L,
  0x0002, 0x0005, 0x0002, 0x0001,
         5,       -1,       -1, G_BOXCHAR         ,   /* Object 4  */
  NONE, OUTLINED|SHADOWED, (LONG) ((LONG)'8' << 24)|0x00FF1100L,
  0x0002, 0x0007, 0x0002, 0x0001,
         6,       -1,       -1, G_BOXCHAR         ,   /* Object 5  */
  NONE, OUTLINED|SHADOWED, (LONG) ((LONG)')' << 24)|0x00FF1100L,
  0x0002, 0x0009, 0x0002, 0x0001,
         7,       -1,       -1, G_BOXCHAR         ,   /* Object 6  */
  NONE, OUTLINED|SHADOWED, (LONG) ((LONG)'9' << 24)|0x00FF1100L,
  0x0002, 0x000B, 0x0002, 0x0001,
         8,       -1,       -1, G_BOXCHAR         ,   /* Object 7  */
  NONE, OUTLINED|SHADOWED, (LONG) ((LONG)'\0' << 24)|0x00FF1100L,
  0x0002, 0x000D, 0x0005, 0x0001,
         9,       -1,       -1, G_TEXT            ,   /* Object 8  */
  NONE, OUTLINED|SHADOWED, (LONG)&rs_tedinfo[10],
  0x0002, 0x0011, 0x0003, 0x0001,
        10,       -1,       -1, G_BOXCHAR         ,   /* Object 9  */
  NONE, OUTLINED|SHADOWED, (LONG) ((LONG)'P' << 24)|0x00FF1100L,
  0x0002, 0x000F, 0x0003, 0x0001,
        11,       -1,       -1, G_BOXCHAR         ,   /* Object 10  */
  NONE, OUTLINED|SHADOWED, (LONG) ((LONG)'\004' << 24)|0x00FF1100L,
  0x0005, 0x0005, 0x0002, 0x0001,
        12,       -1,       -1, G_BOXCHAR         ,   /* Object 11  */
  NONE, OUTLINED|SHADOWED, (LONG) ((LONG)'\002' << 24)|0x00FF1100L,
  0x0005, 0x0007, 0x0002, 0x0001,
        13,       -1,       -1, G_BOXCHAR         ,   /* Object 12  */
  NONE, OUTLINED|SHADOWED, (LONG) ((LONG)'\001' << 24)|0x00FF1100L,
  0x0005, 0x0009, 0x0002, 0x0001,
        14,       -1,       -1, G_BOXCHAR         ,   /* Object 13  */
  NONE, OUTLINED|SHADOWED, (LONG) ((LONG)'\003' << 24)|0x00FF1100L,
  0x0005, 0x000B, 0x0002, 0x0001,
        15,       -1,       -1, G_TEXT            ,   /* Object 14  */
  NONE, OUTLINED|SHADOWED, (LONG)&rs_tedinfo[11],
  0x0002, 0x0013, 0x0003, 0x0001,
        16,       -1,       -1, G_BUTTON          |0x1200,   /* Object 15  */
  SELECTABLE|DEFAULT|EXIT, NORMAL, (LONG)"[OK",
  0x001D, 0x0013, 0x0007, 0x0001,
        17,       -1,       -1, G_IBOX            |0x1100,   /* Object 16  */
  TOUCHEXIT, CROSSED|OUTLINED, (LONG)0x00FF1101L,
  0x0024, 0x0000, 0x0002, 0x0001,
  HLEFT           ,       -1,       -1, G_BUTTON          |0x1200,   /* Object 17  */
  SELECTABLE|EXIT, NORMAL, (LONG)"[Info",
  0x001D, 0x0011, 0x0007, 0x0001,
  HTLEFT          ,       -1,       -1, G_STRING          ,   /* Object 18 HLEFT */
  NONE, NORMAL, (LONG)"nach links",
  0x0009, 0x0005, 0x000A, 0x0001,
  HTRIGHT         ,       -1,       -1, G_STRING          ,   /* Object 19 HTLEFT */
  NONE, NORMAL, (LONG)"nach links drehen",
  0x0009, 0x0007, 0x0011, 0x0001,
  HRIGHT          ,       -1,       -1, G_STRING          ,   /* Object 20 HTRIGHT */
  NONE, NORMAL, (LONG)"nach rechts drehen",
  0x0009, 0x0009, 0x0012, 0x0001,
  HDROP           ,       -1,       -1, G_STRING          ,   /* Object 21 HRIGHT */
  NONE, NORMAL, (LONG)"nach rechts",
  0x0009, 0x000B, 0x000B, 0x0001,
  HPAUSE          ,       -1,       -1, G_STRING          ,   /* Object 22 HDROP */
  NONE, NORMAL, (LONG)"fallen lassen",
  0x0009, 0x000D, 0x000D, 0x0001,
  HCLOSEW         ,       -1,       -1, G_STRING          ,   /* Object 23 HPAUSE */
  NONE, NORMAL, (LONG)"Pause",
  0x0009, 0x000F, 0x0005, 0x0001,
  HEND            ,       -1,       -1, G_STRING          ,   /* Object 24 HCLOSEW */
  NONE, NORMAL, (LONG)"Fenster schlie\236en",
  0x0009, 0x0011, 0x0011, 0x0001,
         0,       -1,       -1, G_STRING          ,   /* Object 25 HEND */
  LASTOB, NORMAL, (LONG)"Beenden",
  0x0009, 0x0013, 0x0007, 0x0001
};

OBJECT *rs_trindex[] =
{ &rs_object[0],   /* Tree  0 NEWSCORE         */
  &rs_object[7],   /* Tree  1 SCORES           */
  &rs_object[34]    /* Tree  2 HELP             */
};
