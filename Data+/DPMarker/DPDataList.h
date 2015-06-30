/* ------------------------------------------------------------------ 

	Title: DPDataList
	
	Description:  A class for listing the data

	Author :
		Stephen Wardlaw, M.D.
		Medical Research & Development
		Highrock
		Lyme, CT 06371

	Edit History:	
		07 October, 1999
			Created
		
------------------------------------------------------------------ */ 
#ifndef _DPDataList
#define _DPDataList

#include <InterfaceKit.h>

class DPDataList : public BListView {

	public :
	
		DPDataList(BMessage* theMesg,
					  BRect frame				= BRect(5,5,50,50),
					  const char* name		= "datalist",
					  list_view_type type 	= B_SINGLE_SELECTION_LIST,
					  uint32 resizingMode 	= B_FOLLOW_LEFT | B_FOLLOW_TOP,
					  uint32 flags			= B_WILL_DRAW | B_NAVIGABLE | B_FRAME_EVENTS);
						
	virtual void
		GetPreferredSize(float* width, float* height);
	
	virtual void
		ResizeToPreferred();
};

#endif
