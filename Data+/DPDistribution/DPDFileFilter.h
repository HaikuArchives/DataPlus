/* ------------------------------------------------------------------ 

	Title: DPDFileFilter

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
#ifndef _DPDFileFilter
#define _DPDFileFilter

#include <StorageKit.h>

class DPDFileFilter : public BRefFilter {

	public:
	
	virtual bool
		Filter(const entry_ref* theRef, BNode* node, struct stat* st, const char* fType);
		
};
	
#endif
