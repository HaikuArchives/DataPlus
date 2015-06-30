/* ------------------------------------------------------------------ 

	Title: DPTextWindow

	Description:  A window class for showing the data transactions

	Author :
		Stephen Wardlaw, M.D.
		Medical Research & Development
		Highrock
		Lyme, CT 06371

	Edit History:	
		19 August, 1999
			Adapted from WTextWindow
		18 February, 1998
			Created
		
------------------------------------------------------------------ */ 

#include "DPTextWindow.h"

#include <stdio.h>
#include <AppKit.h>

const int32 max_lines = 256;	// Max lines in window

// -------------------------------------------------------------------
//	* CONSTRUCTOR
// -------------------------------------------------------------------
DPTextWindow::DPTextWindow(BRect frame,
						 char*	name,
						 window_type type,
						 uint32 flags):BWindow(frame,name,type,flags)
{
	// Add the text view
	BRect textFrame = Bounds();
	textFrame.right -= B_V_SCROLL_BAR_WIDTH;
	BRect textRect = textFrame;
	textRect.OffsetTo(B_ORIGIN);
	BRect r = Bounds();
	r.InsetBy(3.0,3.0);
	mTextView = new BTextView(textFrame, "text_view", textRect,
				B_FOLLOW_ALL_SIDES, B_WILL_DRAW|B_PULSE_NEEDED);
	AddChild(new BScrollView("scroll_view", mTextView,
				B_FOLLOW_ALL_SIDES, 0, false, true, B_NO_BORDER));
	mTextView->MakeSelectable(false);
	mTextView->MakeEditable(false);
	Show();
}
// -------------------------------------------------------------------
//	* Public Methods
// -------------------------------------------------------------------
void
DPTextWindow::Append(const char* theText)
{
	Lock();
	mTextView->Select(mTextView->TextLength(),mTextView->TextLength());
	mTextView->Insert(theText);
	CheckLength();
	mTextView->ScrollToSelection();
	Unlock();
}
// -------------------------------------------------------------------
void
DPTextWindow::NewLine()
{
	Lock();
	// Add a new line
	char* crlf = {"\n"};
	mTextView->Insert(crlf);
	Unlock();
}
// -------------------------------------------------------------------
void
DPTextWindow::ShowError(int32 error)
{
	char errStr[64];
	
	sprintf(errStr,"Error Code = %ld",error);
	SingleLine(errStr);
	CheckLength();
}
// -------------------------------------------------------------------
void		// Writes a single line with a CR
DPTextWindow::SingleLine(const char* theText)
{
	Append(theText);
	NewLine();
}
// -------------------------------------------------------------------
//	* Protected Methods
// -------------------------------------------------------------------
void
DPTextWindow::CheckLength()
{
	if(mTextView->CountLines() > max_lines) {
		mTextView->Delete(0,mTextView->OffsetAt(max_lines/4));
		mTextView->Select(mTextView->TextLength(),mTextView->TextLength());
	}
}
// -------------------------------------------------------------------
void
DPTextWindow::FrameResized(float width, float height) 
{
	BRect textRect = mTextView->TextRect();
	
	textRect.right = textRect.left + (width - B_V_SCROLL_BAR_WIDTH - 3.0);
	mTextView->SetTextRect(textRect);
}
// -------------------------------------------------------------------
bool 
DPTextWindow::QuitRequested()
{
	return(TRUE);
}
// -------------------------------------------------------------------
