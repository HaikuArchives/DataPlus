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

#ifndef _DPTextWindow
#define _DPTextWindow

#include <InterfaceKit.h>

class DPTextWindow : public BWindow {

	public:
		DPTextWindow(BRect 			frame,
					char*			name	= "Data+ Translation",
					window_type		type	= B_DOCUMENT_WINDOW,
					uint32 			flags	= 0);
					    
	void	// Adds wrapping text without line feeds
		Append(const char* thetext);
		
	void
		NewLine();
		
	void	// Shows the error code
		ShowError(int32 error);
		
	void	// Writes a single line with a CR
		SingleLine(const char* thetext);

	protected :	// Methods
	
	void
		CheckLength();
	
	bool
		QuitRequested();
	
	protected :	// Data
	
	void
		FrameResized(float width, float height);
			
	BTextView*	mTextView;
};
#endif

