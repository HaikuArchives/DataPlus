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

#include "DPDataList.h"
#include "DPDataItem.h"

#include <AppKit.h>

const int16 items_shown = 12;

// -------------------------------------------------------------------
// *  Constructors & Destructor             
// -------------------------------------------------------------------
DPDataList::DPDataList(BMessage* theMesg,
							BRect frame,
							const char* name,
							list_view_type type,
							uint32 resizingMode,
							uint32 flags):BListView(frame,name,type,resizingMode,flags)
{
	uint32 index = 0;
	BMessage* tempMesg = new BMessage();
	DPDataItem* theItem;
	status_t status = theMesg->FindMessage("DPDataItem",index++,tempMesg);
	while(status == B_OK) {
		BArchivable* theObj = instantiate_object(tempMesg);
		if(theObj != NULL) {
			 theItem = cast_as(theObj,DPDataItem);
			 if(theItem != NULL) {
			 	AddItem(theItem);
			 }
		}
		tempMesg->MakeEmpty();
		status = theMesg->FindMessage("DPDataItem",index++,tempMesg);
	}
	delete tempMesg;
}
// -------------------------------------------------------------------
// *  Protected Methods             
// -------------------------------------------------------------------
void
DPDataList::GetPreferredSize(float* width, float* height)
{
	DPDataItem* theItem = (DPDataItem*)FirstItem();
	if(theItem != NULL) {
		*width = theItem->Width();
		*height = theItem->Height()*items_shown;
	}
}
// -------------------------------------------------------------------
void
DPDataList::ResizeToPreferred()
{
	DPDataItem* theItem = (DPDataItem*)FirstItem();
	if(theItem != NULL) {
		ResizeTo(theItem->Width(),theItem->Height()*items_shown);
	}
}
// -------------------------------------------------------------------
