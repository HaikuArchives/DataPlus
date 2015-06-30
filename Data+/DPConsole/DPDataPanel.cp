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
#include "DPDataPanel.h"

#include "DPGlobals.h"
#include "DPData.h"
#include "DPTextFile.h"
#include "DataPlus.h"
#include "DPFldWindow.h"

#include <AppKit.h>

const uint32 name_mesg = 'Cnam';
// -------------------------------------------------------------------
// *  Constructors & Destructor             
// -------------------------------------------------------------------
DPDataPanel::DPDataPanel(entry_ref* theRef,
						BRect frame,
			   			const char* title,
			   			uint32 mode, 
			   			uint32 flags):BView(frame,title,mode,flags)
{
	// See if a data set can be generated
	mData = NULL;
	if(theRef != NULL) {
		DPTextFile* theXlator = new DPTextFile(theRef);
		mData = theXlator->ParseFile();
		delete theXlator;
		if(mData != NULL) {
			mData->SetSetName(theRef->name);
		}
	}
	CreatePanel();
}

DPDataPanel::DPDataPanel(BMessage* theMesg,
						BRect frame,
			   			const char* title,
			   			uint32 mode, 
			   			uint32 flags):BView(frame,title,mode,flags)
{
	// See if a data set can be generated
	mData = NULL;
	if(theMesg != NULL) {
		DPTextFile* theXlator = new DPTextFile(theMesg);
		mData = theXlator->ParseFile();
		delete theXlator;
	}
	CreatePanel();
}

DPDataPanel::DPDataPanel(DPData* theData,
						BRect frame,
			   			const char* title,
			   			uint32 mode, 
			   			uint32 flags):BView(frame,title,mode,flags)
{
	// See if a data set can be generated
	mData = theData;
	CreatePanel();
}

DPDataPanel::~DPDataPanel()
{
	delete mData;
}

// -------------------------------------------------------------------
// *  Protected Methods             
// -------------------------------------------------------------------
void
DPDataPanel::CreatePanel()
{
	const float	str_ht 		= 17;
	const float str_wd		= 214;
	const float view_marg	= 5;
	
	char fString[64];
	// Make a frame for the string view
	BRect fRect(0,0,0,0);
	// Make selector & find size -  use any-old frame for now
	mSelectButton = new BButton(fRect,"select","Select Fields",new BMessage(select_data));
	// Set the correct button size
	float ht; 
	float wd;
	mSelectButton->GetPreferredSize(&wd,&ht);
	mSelectButton->ResizeTo(wd,ht);
	// Set size of this view
	float viewWd = str_wd + view_marg*2;
	float viewHt = str_ht + view_marg*3 + ht;
	if(mData != NULL) {	// We need another panel
		viewHt += str_ht + view_marg;
	}	
	ResizeTo(viewWd,viewHt);
	AddChild(mSelectButton);
	// Place the button
	mSelectButton->MoveTo(viewWd - (wd + view_marg),viewHt - (ht + view_marg));

	// Make the string view
	fRect.Set(view_marg,view_marg,view_marg + str_wd,view_marg + str_ht);
	if(mData == NULL) {
		::strcpy(fString,"Drop Or Open File");
		BStringView* nameView = new BStringView(fRect,"fName",fString,B_FOLLOW_NONE,B_WILL_DRAW);
		AddChild(nameView);
	} else {
		const char* set_title = "Set Name:";
		::strcpy(fString,mData->SetName());
		mSetName = new BTextControl(fRect,"fName",set_title,fString,new BMessage(name_mesg));
		mSetName->SetDivider(StringWidth(set_title) + 5);
		mSetName->SetTarget(this);
		AddChild(mSetName);
	}
	if(mData != NULL) {
		// Make the record & field string view
		sprintf(fString,"%ld Records Containing %ld Data Fields",mData->NumRecords(), mData->NumFields());
		BRect sRect(view_marg,fRect.bottom + view_marg,fRect.right,fRect.bottom + view_marg + str_ht);
		BStringView* theView = new BStringView(sRect,"recsNum",fString,B_FOLLOW_TOP | B_FOLLOW_LEFT);
		AddChild(theView);
	}
	// Make the check box
	const float box_wd = 60;
	float center = mSelectButton->Frame().top + mSelectButton->Frame().Height()/2 - str_ht/2;
	fRect.Set(view_marg,center,view_marg + box_wd, center + str_ht);
	mCombineBox = new BCheckBox(fRect,"combine","Combine Set",NULL);
	AddChild(mCombineBox);
	
	if(mData == NULL) {
		mSelectButton->SetEnabled(false);
		mCombineBox->SetEnabled(false);
	}
	mSelected = false;
}
// -------------------------------------------------------------------
void
DPDataPanel::Draw(BRect theRect)
{
	BRect fRect = Bounds();
	fRect.InsetBy(2,2);
	if(mSelected) {
		SetPenSize(3);
	}
	StrokeRect(fRect);
}
// -------------------------------------------------------------------
void
DPDataPanel::GetData(BMessage* theMessage)
{
   status_t status = B_OK;
   int32 refIndx = 0;
   entry_ref theRef;
   DPDataView* theView;
   
	while(status == B_OK) {
		status = theMessage->FindRef("refs",refIndx++,&theRef);
		if(status == B_OK) {
			theView = cast_as(Parent(),DPDataView);
			theView->AddDataSet(&theRef);
		}
	} 
	delete theMessage;
}
// -------------------------------------------------------------------		
void
DPDataPanel::FrameResized(float width, float height)
{
}
// -------------------------------------------------------------------
void
DPDataPanel::MessageReceived(BMessage* theMessage)
{
    DPDataView* theView;
  
 	switch ( theMessage->what ){
 	
 		case select_data :
 			mData->SetSetName(mSetName->Text());
  			SelectFields();
 			break;
 			
 		case name_mesg :
 			mData->SetSetName(mSetName->Text());
 			break;
 			
 		case B_MIME_TYPE :	// Dropped data
			theView = cast_as(Parent(),DPDataView);
 			Window()->DetachCurrentMessage();
			theView->AddDataSet(theMessage);
 			break;
 			
		case 'DATA' :
			if(mData == NULL) {	// Do only if the blank panel
				Window()->DetachCurrentMessage();
				GetData(theMessage);
			}
    		break;
    		
		default :
			this->BView::MessageReceived(theMessage);
	}
}
// -------------------------------------------------------------------
void
DPDataPanel::MouseDown(BPoint thePt)
{
	if(mData != NULL) {	// Only mark a panel with data
		mSelected = !mSelected;
		Window()->Lock();
		Invalidate();
		Window()->Unlock();
	}
}
// -------------------------------------------------------------------
void
DPDataPanel::SelectFields()
{
	const float frame_wd = 200;
	const float frame_ht = 100 + 25*mData->NumFields();
	// Put selection window alongside this one
	BRect frame = Window()->Frame();
	BScreen* theScreen = new BScreen();
	BRect sRect = theScreen->Frame();
	delete theScreen;
	
	// Preferably put on right side
	if((frame.right + frame_wd) < sRect.right) {
		frame.left = frame.right;
		frame.right += frame_wd;
	} else {
		frame.right = frame.left;
		frame.left -= frame_wd;
	}
	frame.bottom = frame.top + frame_ht;
	
	new DPFldWindow(mData,frame);
}
// -------------------------------------------------------------------

		
