/* ------------------------------------------------------------------ 

	Title: DPFldWindow

	Description:  Allows the manual selection of data fields.
	
	Author :
		Stephen Wardlaw, M.D.
		Medical Research & Development
		Highrock
		Lyme, CT 06371

	Edit History:	
		23 August, 1999
			Created
		
------------------------------------------------------------------ */ 

#ifndef _DPFldWindow
#define _DPFldWindow

#include "DPData.h"
#include <InterfaceKit.h>

class DPFldWindow : public BWindow {

	public:
		DPFldWindow(DPData* theData,
						BRect		frame,
						char*		name	= "Select Fields",
						window_type		type	= B_DOCUMENT_WINDOW,
						uint32 			flags	= 0);

	virtual
		~DPFldWindow();
		
	virtual void
		MessageReceived(BMessage* theMesg);
		
	protected : // Methods
	
	void
		CreateWindow(BRect frame);
	
	void
		SetFields();
	
	void
		TestSelections();
	
	protected : // Data
	
	DPData*			mData;
	BView*			mView;
	int16				mNumFlds;
	BMenuField**	mSelector;
};
	

#endif
