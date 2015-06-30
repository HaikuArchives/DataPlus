/* ------------------------------------------------------------------ 

	Title: DPDataView

	Description:  The master data manipulation view

	Author :
		Stephen Wardlaw, M.D.
		Medical Research & Development
		Highrock
		Lyme, CT 06371

	Edit History:	
		14 August, 1999
			Created
		
------------------------------------------------------------------ */ 

#ifndef _DPDataView
#define _DPDataView

#include "DPIconPanel.h"
#include "DPDataPanel.h"
#include <InterfaceKit.h>

class DPDataView : public BView{

	public:
		DPDataView(BRect frame,
					  	 const char* title	= "DPDataView",
					  	 uint32 mode			= B_FOLLOW_ALL_SIDES,
					  	 uint32 flags			= B_WILL_DRAW + 
												  	  B_FULL_UPDATE_ON_RESIZE + 
												  	  B_FRAME_EVENTS); 
	virtual
		~DPDataView();
		
	void
		AddDataSet(entry_ref* theRef);
		
	void
		AddDataSet(DPData* theData);
		
	void
		AddDataSet(BMessage* theMesg);

	void
		CheckFields();
		
	const DPDataPanel*
		DataPanel(int32 panelNum);
		
	const DPData*
		DataSet(int32 setNum);
		
	void
		RemoveAllData();
		
	void
		RemoveSelectedData();
		
	protected: 	// Methods
		
	virtual void
		Draw(BRect theRect);
		
	virtual void
		FrameResized(float width, float height);
		
	virtual void
		MessageReceived(BMessage* theMessage);
		
	void
		SizeViewAndWindow();
		
		
	protected:	// Data
	DPIconPanel*	mIconPanel;
		
	
};

#endif

