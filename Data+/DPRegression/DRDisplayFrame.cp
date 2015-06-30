/* ------------------------------------------------------------------ 

	Title: DRDisplayFrame

	Description:  A frame for showing the sets of regression data.
	
	Author :
		Stephen Wardlaw, M.D.
		Medical Research & Development
		Highrock
		Lyme, CT 06371

	Edit History:	
		23 November, 1999
			Created
		
------------------------------------------------------------------ */
#include "DRDisplayFrame.h"
#include <SupportKit.h>
#include "WColorControl.h"

// -------------------------------------------------------------------
//	* CONSTRUCTORS and DESTRUCTORS
// -------------------------------------------------------------------
DRDisplayFrame::DRDisplayFrame(BRect frame, 	
							const char* name,
							uint32 resizingMode,
							uint32 flags):BView(frame,name,resizingMode,flags)
{
	SetPenSize(2);
	SetHighColor(color_med_gray);
}
							
DRDisplayFrame::~DRDisplayFrame()
{
}
// -------------------------------------------------------------------
//	* PUBLIC METHODS
// ------------------------------------------------------------------ 
void
DRDisplayFrame::AddDisplay(DRRegrData* theData)
{
	BAutolock lock(Window());
	DRRegrView* theView = new DRRegrView(theData);
	mData.AddItem(theView);
	AddChild(theView);
	ResizeToPreferred();
}
// ------------------------------------------------------------------ 
const DRRegrView*
DRDisplayFrame::DisplayView(int32 viewNum) const
{
	return (const DRRegrView*)mData.ItemAt(viewNum);
}
// ------------------------------------------------------------------ 
void
DRDisplayFrame::Draw(BRect frame)
{
//	SetHighColor(color_red);
//	StrokeRect(Bounds());
}
// ------------------------------------------------------------------ 
void
DRDisplayFrame::RefreshDisplays()
{
	BAutolock lock(Window());
	int32 dispNum = 0;
	DRRegrView* theView = (DRRegrView*)DisplayView(dispNum++);
	while(theView != NULL) {
		theView->ResetDisplay();
		theView->Invalidate();
		theView = (DRRegrView*)DisplayView(dispNum++);
	}
}		
// ------------------------------------------------------------------ 
void
DRDisplayFrame::ResizeToPreferred()
{
	const float disp_margin = 5;
	float vPosn = disp_margin;
	float width, height;
	for(int32 dispNum = 0; dispNum < mData.CountItems(); dispNum++) {
		DRRegrView* theView = (DRRegrView*)mData.ItemAt(dispNum);
		theView->ResizeToPreferred();
		theView->GetPreferredSize(&width,&height);
		theView->MoveTo(disp_margin,vPosn);
		vPosn += height + disp_margin;
	}
	ResizeTo(width + disp_margin*2,vPosn);
}
// ------------------------------------------------------------------ 
