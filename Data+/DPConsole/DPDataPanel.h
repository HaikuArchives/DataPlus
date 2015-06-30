/* ------------------------------------------------------------------ 

	Title: DPDataPanel

	Description:  Shows and allows selection of a single data set

	Author :
		Stephen Wardlaw, M.D.
		Medical Research & Development
		Highrock
		Lyme, CT 06371

	Edit History:	
		24 August, 1999
			Created
		
------------------------------------------------------------------ */ 

#ifndef _DPDataPanel
#define _DPDataPanel

#include "DPData.h"
#include "DPFldWindow.h"
#include <InterfaceKit.h>

const char panel_name[8]	= "dpanel";

class DPDataPanel : public BView{

	public:
		DPDataPanel(entry_ref* theRef,
					BRect frame,
					const char* title	= panel_name,
					uint32 mode			= B_FOLLOW_NONE,
					uint32 flags		= B_WILL_DRAW + 
										  B_FULL_UPDATE_ON_RESIZE + 
										  B_FRAME_EVENTS); 
		DPDataPanel(DPData* theData,
					BRect frame,
					const char* title	= panel_name,
					uint32 mode			= B_FOLLOW_NONE,
					uint32 flags		= B_WILL_DRAW + 
										  B_FULL_UPDATE_ON_RESIZE + 
										  B_FRAME_EVENTS); 

		DPDataPanel(BMessage* theMesg,
					BRect frame,
					const char* title	= panel_name,
					uint32 mode			= B_FOLLOW_NONE,
					uint32 flags		= B_WILL_DRAW + 
										  B_FULL_UPDATE_ON_RESIZE + 
										  B_FRAME_EVENTS); 
	virtual
		~DPDataPanel();
		
	void
		AllowCombine(bool combine) const {mCombineBox->SetEnabled(combine);}
		
	bool
		CanCombine() const {return mCombineBox->Value() > 0;}
		
	const DPData*
		Data() const {return (const DPData*)mData;}
		
	virtual void
		MouseDown(BPoint thePt);
		
	void
		ProcessDropMesg(BMessage* theMesg);
					
	void
		SetCombine() {mCombineBox->SetEnabled(false);}
		
	void
		SetSelect(bool select) {mSelected = select;}
		
	bool
		Selected() const {return mSelected;}
		
	protected: 	// Methods
	
	void
		AttachedToWindow(){mSelectButton->SetTarget(this);}
	
	void
		CreatePanel();
		
	virtual void
		Draw(BRect theRect);
		
	void
		GetData(BMessage* theMesg);
		
	virtual void
		FrameResized(float width, float height);
		
	virtual void
		MessageReceived(BMessage* theMessage);
		
	void
		SelectFields();
		
		
	protected:	// Data
	DPData*			mData;	
	BTextControl*	mSetName;
	BButton*		mSelectButton;
	BCheckBox*		mCombineBox;
	bool			mSelected;
};

#endif

