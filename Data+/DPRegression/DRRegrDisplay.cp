/* ------------------------------------------------------------------ 

	Title: DRRegrDisplay

	Description:  The window which shows tab views of the regression
		data and which allows setting of the plot marks and colors

	Author :
		Stephen Wardlaw, M.D.
		Medical Research & Development
		Highrock
		Lyme, CT 06371

	Edit History:	
		11 November, 1999
			Created
		
------------------------------------------------------------------ */ 

#include "DRRegrDisplay.h"

#include "DRRegrView.h"
#include "WColorControl.h"

// -------------------------------------------------------------------
//	* Constructors and Destructor
// -------------------------------------------------------------------
DRRegrDisplay::DRRegrDisplay(BRect frame,
								const char* title,
								window_look look,
								window_feel feel,
								uint32 flags):BWindow(frame,title,look,feel,flags)
{
	mFirstDisp = true;
	frame = Bounds();
	BView* theView = new BView(frame,"",B_FOLLOW_ALL,B_WILL_DRAW);
	AddChild(theView);
	theView->SetViewColor(color_std_gray);
	frame.InsetBy(5,5);
	mTab = new BTabView(frame,"disptabs",B_WIDTH_FROM_WIDEST);
	mTab->SetResizingMode(B_FOLLOW_NONE);
	theView->AddChild(mTab);
	// Will be resized when first data panel is added
}
		
DRRegrDisplay::~DRRegrDisplay()
{
}
// -------------------------------------------------------------------
//	* Public Methods
// -------------------------------------------------------------------
void
DRRegrDisplay::AddDisplay(DPRegrData* theRData)
{
	BAutolock lock(this);
	BString theTitle;
	const DPData* theData = theRData->Data();
	DPRegrView* theView = new DPRegrView(theRData,BRect(0,0,5,5),theData->SetName());
	BTab* theTab = new BTab();
	mTab->AddTab(theView,theTab);
	theView->ResizeToPreferred();
	
	// If first panel added, resize everything
	const float ctrl_margin = 5;
	if(mFirstDisp) {
		BRect frame = theView->Bounds();
		frame.bottom += mTab->TabHeight();
		frame.InsetBy(-ctrl_margin,-ctrl_margin);
		mTab->ResizeTo(frame.Width(),frame.Height());	// Tab frame
		ResizeTo(frame.Width() + ctrl_margin*2,frame.Height() + ctrl_margin*2);
		mTab->MoveTo(ctrl_margin,ctrl_margin);
		theView->MoveTo(ctrl_margin,ctrl_margin);
		Show();
		mFirstDisp = false;
	} else {
		theView->MoveTo(ctrl_margin,ctrl_margin);
	}
	mTab->Select(0);
	theView->Invalidate();
}
// -------------------------------------------------------------------
BView*
DRRegrDisplay::GetView(int32 tabNum)
{
	BTab* theTab = mTab->TabAt(tabNum);
	if(theTab != NULL) {
		DPRegrView* theView = (DPRegrView*)theTab->View();
		mTab->Select(tabNum);	// Connect to window
		return theView->DisplayView();
	}
	return NULL;
}
// -------------------------------------------------------------------
void
DRRegrDisplay::MessageReceived(BMessage* theMesg)
{
	switch (theMesg->what) {
	
		default :
			BWindow::MessageReceived(theMesg);
	}
}
// -------------------------------------------------------------------
void
DRRegrDisplay::RefreshDisplays()
{
	BAutolock lock(this);
	int32 index = 0;
	BTab* theTab = mTab->TabAt(index++);
	while (theTab != NULL) {
		DPRegrView* theView = (DPRegrView*)theTab->View();
		if(theView != NULL) {
			theView->ResetDisplay();
			theView->Invalidate();
		}
		theTab = mTab->TabAt(index++);
	}
	mTab->Invalidate();
}
// -------------------------------------------------------------------
//	* Protected Methods
// -------------------------------------------------------------------
