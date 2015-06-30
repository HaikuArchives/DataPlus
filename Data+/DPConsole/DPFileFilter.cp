/* ------------------------------------------------------------------ 

	Title: DPFileFilter

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
#include "DPFileFilter.h"

#include "DPGlobals.h"
#include <stdio.h>

bool
DPFileFilter::Filter(const entry_ref* theRef, BNode* node, struct stat* st, const char* fType)
{
	if(::strcmp(fType,native_mime_type) == 0) return true;
	if(::strcmp(fType,text_mime_type) == 0) return true;
	if S_ISDIR(st->st_mode) return true;
	return false;
	
}
		
