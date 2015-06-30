/* ------------------------------------------------------------------ 

	Title: DRFileFilter

	Description:  A file filter for DataPlus console

	Author :
		Stephen Wardlaw, M.D.
		Medical Research & Development
		Highrock
		Lyme, CT 06371

	Edit History:	
		27 March, 2001
			Created
		
----------------------------------------------------------------- */ 
#include "DRFileFilter.h"

#include "DPGlobals.h"
#include <stdio.h>

bool
DRFileFilter::Filter(const entry_ref* theRef, BNode* node, struct stat* st, const char* fType)
{
	if(::strcmp(fType,regrplot_mime_type) == 0) return true;
	if S_ISDIR(st->st_mode) return true;
	return false;
	
}
		
