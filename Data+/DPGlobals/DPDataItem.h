/* ------------------------------------------------------------------ 

	Title: DPDataItem
	
	Description:  A class for showing and listing the data

	Author :
		Stephen Wardlaw, M.D.
		Medical Research & Development
		Highrock
		Lyme, CT 06371

	Edit History:	
		07 October, 1999
			Created
		
------------------------------------------------------------------ */ 
#ifndef _DPDataItem
#define _DPDataItem

#include "DPData.h"
#include <InterfaceKit.h>

class DPDataItem : public BListItem {

	public :
	
	enum {
		class_ID = 'DItm'
	};
	
	static BArchivable*
		Instantiate(BMessage* theArchive);
	
		DPDataItem(BMessage* theMesg);
		
		DPDataItem(DPData* theSet, int32 theRecord);
		
	virtual
		~DPDataItem();
		
	status_t
		Archive(BMessage* theArchive, bool deep = true) const;

	void
		DrawItem(BView* owner, BRect itemRect, bool drawEverything = false);
	
	record_key
		ItemKey() {return mKey;}
		
	virtual float
		Width();
		
	protected :	// Data
	
	enum {
		max_name_len = 15,
		max_num_len = 11
	};
	
	char			mRecName[max_name_len + 1];
	char			mXVal[max_num_len + 1];
	char			mYVal[max_num_len + 1];
	record_key	mKey;
	font_height	mFHeight;
};

#endif
