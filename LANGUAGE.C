/*
	@(#)Kubis/language.c
	@(#)Julian F. Reschke, 5. Februar 1992
	
	Language support
*/

#include <tos.h>

#include "kubrsc.h"

static int
lang[] =
{
	ENGLISH, GERMAN, FRENCH, ENGLISH,
	ENGLISH, ENGLISH, ENGLISH, FRENCH,
	GERMAN, ENGLISH, ENGLISH, ENGLISH,
	ENGLISH, ENGLISH, ENGLISH
};

int
get_country (void)
{
	int country;
	char *lng = getenv ("LANG");
	SYSHDR *Sys;
	long oldstack;
	
	oldstack = Super (0L);
	Sys = *((SYSHDR **)(0x4f2L));
	Sys = Sys->os_base;
	Super ((void *)oldstack);
	
	country = Sys->os_palmode >> 1;

	if (lng)
	{
		if (!strcmp (lng, "german")) return GERMAN;
		if (!strcmp (lng, "english")) return ENGLISH;
/*		if (!strcmp (lng, "french")) return FRENCH;
*/	}

	if (country > 14) country = 0;

	return lang[country];
}
