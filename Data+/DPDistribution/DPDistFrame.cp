/* ------------------------------------------------------------------ 

	Title: DPDistFrame

	Description:  A frame for showing the sets of regression data.
	
	Author :
		Stephen Wardlaw, M.D.
		Medical Research & Development
		Highrock
		Lyme, CT 06371

	Edit History:	
		05 June, 2000
			Created
		
------------------------------------------------------------------ */
#include "DPDistFrame.h"
#include <SupportKit.h>
#include "WColorControl.h"
#include "UGraph_Util.h"

// -------------------------------------------------------------------
//	* CONSTRUCTORS and DESTRUCTORS
// -------------------------------------------------------------------
DPDistFrame::DPDistFrame(BRect frame, 	
							const char* name,
							uint32 resizingMode,
							uint32 flags):BView(frame,name,resizingMode,flags)
{
	SetPenSize(2);
	SetHighColor(color_med_gray);
}
							
DPDistFrame::~DPDistFrame()
{
}
// -------------------------------------------------------------------
//	* PUBLIC METHODS
// ------------------------------------------------------------------ 
void
DPDistFrame::AddDisplay(DPDistData* theData)
{
	BAutolock lock(Window());
	DPDistView* theView = new DPDistView(theData);
	mData.AddItem(theView);
	AddChild(theView);
	ResizeToPreferred();
}
// ------------------------------------------------------------------ 
const DPDistView*
DPDistFrame::DisplayView(int32 viewNum) const
{
	return (const DPDistView*)mData.ItemAt(viewNum);
}
// ------------------------------------------------------------------ 
void
DPDistFrame::Draw(BRect frame)
{
//	SetHighColor(color_red);
//	StrokeRect(Bounds());
}
// ------------------------------------------------------------------ 
void
DPDistFrame::RefreshDisplays()
{
	BAutolock lock(Window());
	int32 dispNum = 0;
	DPDistView* theView = (DPDistView*)DisplayView(dispNum++);
	while(theView != NULL) {
	//	theView->ResetDisplay();	//*********?
		theView->ResizeToPreferred();
		theView->Invalidate();
		theView = (DPDistView*)DisplayView(dispNum++);
	}
}		
// ------------------------------------------------------------------ 
void
DPDistFrame::ResizeToPreferred()
{
	const float disp_margin = 5;
	float vPosn = disp_margin;
	float width, height;
	for(int32 dispNum = 0; dispNum < mData.CountItems(); dispNum++) {
		DPDistView* theView = (DPDistView*)mData.ItemAt(dispNum);
		theView->ResizeToPreferred();
		theView->GetPreferredSize(&width,&height);
		theView->MoveTo(disp_margin,vPosn);
		vPosn += height + disp_margin;
	}
	ResizeTo(width + disp_margin*2,vPosn);
}
// ------------------------------------------------------------------ 
