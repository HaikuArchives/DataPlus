/* ------------------------------------------------------------------ 

	Title: DPMultRegr

	Description:  Multiple-regression analysis for Data+

	Author :
		Stephen Wardlaw, M.D.
		Medical Research & Development
		Highrock
		Lyme, CT 06371

	Edit History:	
		16 March, 2001
			Created
		
------------------------------------------------------------------ */
#include "DPMultRegr.h"
#include "DPGlobals.h"
#include "DPData.h"
#include "WU_AppUtils.h"
#include "UStringFmt_Utils.h"

#include <AppKit.h>
#include <StorageKit.h>

// -------------------------------------------------------------------
//	* MAIN Program
// -------------------------------------------------------------------
int
main()
{
	DPMultRegr theApp;

	theApp.Run();

	return(0);
}
// -------------------------------------------------------------------
//	* CONSTRUCTORS and DESTRUCTORS
// -------------------------------------------------------------------
DPMultRegr::DPMultRegr() : BApplication(data_plus_multregr_sig)
{

	// We should get preference data for this
	
	mData = new BList();
	
	// Show the regression graph
	BRect	dataRect;
	dataRect.Set(100, 100, 450, 450);
	mMRWindow = new DPMRWindow(dataRect);
	mMRWindow->Show();
}

DPMultRegr::~DPMultRegr()
{
//	for(int32 items = 0; items < mData->CountItems(); items++) {
//		DPRegrData* theRData = (DPRegrData*)mData->ItemAt(items);
//		delete theRData;
//	}
	delete mData;
}
// -------------------------------------------------------------------
//	* PUBLIC METHODS
// ------------------------------------------------------------------ 
void
DPMultRegr::AboutRequested()
{
	BAlert* theAlert = new BAlert("","Data+ Multiple-Regression Tool V0.5 \n By: Stephen Wardlaw, M.D. swardlaw@connix.com","OK");
	theAlert->Go();
}
// -------------------------------------------------------------------
const DPData*
DPMultRegr::DataSet(int16 setNum)
{
	return (const DPData*)mData->ItemAt(setNum);
}
// -------------------------------------------------------------------
void
DPMultRegr::MessageReceived(BMessage* theMesg)
{
	DPData* theData;
	DPData* firstSet = NULL;
	BMessage* theSet;
	BArchivable* theObj;
	int32	index = 0;
	status_t status = B_OK;
	BString theString;
	bool setSep = false;
	
	switch (theMesg->what) {
	
		case DPData::fld_code :
		/*
			while(status == B_OK) {
				theSet = new BMessage();
				status = theMesg->FindMessage(data_set_str,index++,theSet);
				if(status == B_OK) {
					theObj = instantiate_object(theSet);
					if(theObj != NULL) {
						theData = cast_as(theObj,DPData);
						if(theData != NULL) {
							if(firstSet == NULL) {
								firstSet = theData;
							}
							if(setSep) {
								theString.Append("+");
							}
							theString.Append(theData->SetName());
							setSep = true;
							AddDisplaySet(theData);
						}
					}
				}	// if(status == B_OK)
				delete theSet;
			}	// Of while(status == B_OK)
			if(firstSet != NULL) {
				//BString theString;
				//firstSet->MakeTitleString(&theString,true);
				mMRWindow->SetTitle(theString.String());
			}
			mMRWindow->PostMessage(FLDS_RESET);
			*/
			break;
			
		case mesg_mark :	// A series of data to be marked
			DetachCurrentMessage();
			MarkData(theMesg);
			break;
			
		default :
			BApplication::MessageReceived(theMesg);
	}
}
// -------------------------------------------------------------------
//	* Protected Methods
// ------------------------------------------------------------------ 
void
DPMultRegr::MarkData(BMessage* theMesg)
{	/***
	if(mMRWindow->AllowMarking()) {
		int32 setNum = 0;
		DPRegrData* theRData = (DPRegrData*)DisplayData(setNum++);
		while (theRData != NULL) {
			DPData* theData = (DPData*)theRData->Data();
			theData->MarkData(theMesg);
			theRData->Recalculate();
			theRData = (DPRegrData*)DisplayData(setNum++);
		}
		mMRWindow->ReDraw();
	}
	delete theMesg;
	***/
}
// -------------------------------------------------------------------
