/* ------------------------------------------------------------------ 

	Title: DPRegression

	Description:  Provides regression analysis & graphing for Data+

	Author :
		Stephen Wardlaw, M.D.
		Medical Research & Development
		Highrock
		Lyme, CT 06371

	Edit History:	
		29 November, 1999
			Modified for single-window
		12 September, 1999
			Created
		
------------------------------------------------------------------ */
#include "DPRegression.h"
#include "DPGlobals.h"
#include "DPData.h"
#include "DRRegrData.h"
#include "DRRegrDisplay.h"
#include "DRGlobals.h"
#include "U_Conversions.h"
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
	DPRegression theApp;

	theApp.Run();

	return(0);
}
// -------------------------------------------------------------------
//	* CONSTRUCTORS and DESTRUCTORS
// -------------------------------------------------------------------
DPRegression::DPRegression() : BApplication(data_plus_regr_sig)
{

	// Install this app and data file type if necessary
	BMimeType mime(data_plus_regr_sig);
	if (mime.InitCheck() == B_OK && !mime.IsInstalled()) {
		mime.Install();
	}
	BMimeType dataMime(regrplot_mime_type);
	if (dataMime.InitCheck() == B_OK && !dataMime.IsInstalled()) {
		dataMime.Install();
		// Set the document icons
		status_t status;
		BBitmap* lgIcon = new BBitmap(BRect(0,0,31,31),B_CMAP8);
		lgIcon->SetBits(largeIcon_bits,sizeof(largeIcon_bits),0,B_CMAP8);
		status = dataMime.SetIcon(lgIcon,B_LARGE_ICON);
		delete lgIcon;

		BBitmap* miniIcon = new BBitmap(BRect(0,0,15,15),B_CMAP8);
		miniIcon->SetBits(smallIcon_bits,sizeof(smallIcon_bits),0,B_CMAP8);
		status = dataMime.SetIcon(miniIcon,B_MINI_ICON);
		delete miniIcon;
	}
	// Make sure data marker is installed
	BMimeType markMime(data_plus_marker_sig);
	if (markMime.InitCheck() == B_OK && !markMime.IsInstalled()) {
		markMime.Install();
	}

	// We should get preference data for this
	
	mData = new BList();
	
	// Show the regression graph
	BRect	dataRect;
	dataRect.Set(100, 100, 450, 450);
	mRegrWindow = new DRRegrWindow(dataRect);
	mRegrWindow->Show();
}

DPRegression::~DPRegression()
{
	for(int32 items = 0; items < mData->CountItems(); items++) {
		DRRegrData* theRData = (DRRegrData*)mData->ItemAt(items);
		delete theRData;
	}
	delete mData;
}
// -------------------------------------------------------------------
//	* PUBLIC METHODS
// ------------------------------------------------------------------ 
void
DPRegression::AboutRequested()
{
	BAlert* theAlert = new BAlert("","Data+ Regression Tool V1.2.2 Final\n By: Stephen Wardlaw, M.D. swardlaw@connix.com","OK");
	theAlert->Go();
}
// -------------------------------------------------------------------
void
DPRegression::AddDisplaySet(DPData* theData)
{
	DRRegrData* theRData = new DRRegrData(theData);
	mData->AddItem(theRData);
	mRegrWindow->AddDisplay(theRData);
}
// -------------------------------------------------------------------
const DPData*
DPRegression::DataSet(int16 setNum)
{
	DRRegrData* theData = (DRRegrData*)mData->ItemAt(setNum);
	if(theData != NULL) {
		return theData->Data();
	}
	return NULL;
}
// -------------------------------------------------------------------
void
DPRegression::ClearDataSets()
{
/*
	for(int32 set = 0; set < mData->CountItems(); set++) {
		DPRegrDisplay* theDisp = DisplaySet(set);
		delete theDisp;
	}
	mData->MakeEmpty();
*/
}
// -------------------------------------------------------------------
void
DPRegression::MessageReceived(BMessage* theMesg)
{
	entry_ref theRef;
	status_t status = B_OK;
	
	switch (theMesg->what) {
	
		case launch_mesg :
			status = theMesg->FindRef("refs",&theRef);;
			if(status == B_OK) {
				mRegrWindow->OpenFile(&theRef,false);
			}
			mRegrWindow->PostMessage(FLDS_RESET);
			break;
			
		case mesg_mark :	// A series of data to be marked
			DetachCurrentMessage();
			MarkData(theMesg);
			break;
			
		case set_display :	// Data to be marked
			SetDisplays(theMesg);
			break;
			
		case refresh_disp :
//			mRegrDisplay->RefreshDisplays();
			break;
				
		default :
			BApplication::MessageReceived(theMesg);
	}
}
// -------------------------------------------------------------------
void
DPRegression::SetDisplays(BMessage* theMesg)
{
	record_key theKey;
	theMesg->FindInt64(disp_set_name,&theKey);
	for(int32 setNum = 0; setNum < NumSets(); setNum++) {
		DRRegrData* theRData = (DRRegrData*)DisplayData(setNum);
		if(theRData != NULL) {
			const DPData* theData = theRData->Data();
			if(theKey == theData->Key()) {
				int16 val = theMesg->FindInt16(disp_mark_name);
				int16 lVal = theMesg->FindInt16(disp_line_name);
				theRData->SetPlotMark((EPlotMark)val);
				theRData->SetDataColor(::ValueToColor(theMesg->FindInt32(mark_col_name)));
				theRData->SetLineType((ERegrLineType)lVal);
				mRegrWindow->ReDraw();
				return;	// *** EXIT HERE if found
			}
		}
	}
}	
// -------------------------------------------------------------------
//	* Protected Methods
// ------------------------------------------------------------------ 
void
DPRegression::MarkData(BMessage* theMesg)
{
	if(mRegrWindow->AllowMarking()) {
		int32 setNum = 0;
		DRRegrData* theRData = (DRRegrData*)DisplayData(setNum++);
		while (theRData != NULL) {
			DPData* theData = (DPData*)theRData->Data();
			theData->MarkData(theMesg);
			theRData->Recalculate();
			theRData = (DRRegrData*)DisplayData(setNum++);
		}
		mRegrWindow->ReDraw();
	}
	delete theMesg;
}
// -------------------------------------------------------------------
void
DPRegression::RefsReceived(BMessage* theMesg)
{
	entry_ref theRef;
	status_t status = theMesg->FindRef("refs",&theRef);;
	if(status == B_OK) {
		mRegrWindow->OpenFile(&theRef,true);
	}
}
// -------------------------------------------------------------------
