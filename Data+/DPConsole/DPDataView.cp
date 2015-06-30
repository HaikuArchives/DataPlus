/* ------------------------------------------------------------------ 

	Title: DPDataView

	Description:  The master data manipulation view


	Author :
		Stephen Wardlaw, M.D.
		Medical Research & Development
		Highrock
		Lyme, CT 06371

	Edit History:	
		27 March, 2001
			Changed file structure
		20 September, 1999
			Much rearranging..
		14 August, 1999
			Created
		
------------------------------------------------------------------ */ 
#include "DPDataView.h"

#include "WColorControl.h"
#include "DPDataPanel.h"
#include "DPGlobals.h"
#include "DataPlus.h"
#include "DPDataFile.h"

#include <SupportKit.h>

const float ctrl_ht	= 30;	// Room for control icons
const float ctrl_wd = 144;
// -------------------------------------------------------------------
// *  Constructors & Destructor             
// -------------------------------------------------------------------
DPDataView::DPDataView(BRect frame,
			   const char* title,
			   uint32 mode, 
			   uint32 flags):BView(frame,title,mode,flags)
{
	BRect iRect(0,0,ctrl_wd,ctrl_ht);
	mIconPanel = new DPIconPanel(iRect);
	AddChild(mIconPanel);
	SetViewColor(color_gray18);
	CheckFields();
	

	// The view is sized upon addition of a panel
}

DPDataView::~DPDataView()
{
}

// -------------------------------------------------------------------
// *  Public Methods             
// -------------------------------------------------------------------
void
DPDataView::AddDataSet(entry_ref* theRef)
{
	BAutolock lock(Window());
	// Delete the last data panel, if any, since it's empty
	int32 numKids = CountChildren();
	BView* lastPanel = NULL;
	char typeStr[B_MIME_TYPE_LENGTH];
	// There is always the launch panel, which we can't remove
	if(numKids > 1) {
		lastPanel = ChildAt(numKids - 1);
		RemoveChild(lastPanel);
	}

	// The panel sizes itself
	DPDataPanel* thePanel = NULL;
	BRect frame(0,0,0,0);
	// If a null reference, add a 'blank' panel
	if(theRef == NULL) {
		thePanel = new DPDataPanel(theRef,frame);
		AddChild(thePanel);
		
	} else {  // See what kind of file we have..
		BNode theNode(theRef);
		if(theNode.InitCheck() == B_OK) {
			BNodeInfo theInfo(&theNode);
			if(theInfo.InitCheck() == B_OK && theInfo.GetType(typeStr) == B_OK) {
				// Check MIME type for translator, if required
				BMimeType mType(typeStr);
				if(mType.Type() != NULL) {
					if(::strcmp(mType.Type(),text_mime_type) == 0) {
						thePanel = new DPDataPanel(theRef,frame);
						AddChild(thePanel);
						
					} else if(::strcmp(mType.Type(),native_mime_type) == 0) {
						// Open as a native file
						DPDataFile theFile(theRef);
						status_t status = theFile.InitCheck();
						if(status == B_OK) {
							// Get and add the data sets
							for(int16 setNum = 0; setNum < theFile.NumSets(); setNum++) {
								DPData* theData = theFile.GetSet(setNum);
								if(theData != NULL) {
									thePanel = new DPDataPanel(theData,frame);
									AddChild(thePanel);
								}
							} 	// Of for(
						} else {	// If a file error
							theFile.ShowFileAlert(status);
						}
					}
				}	// if(Type() != NULL
			}	// if(theNode.InitCheck
		} 
		
		if(thePanel == NULL) {
			BAlert* theAlert = new BAlert("OFile","Can't Open This File Type","OK");
			theAlert->Go();
		}
	}
	
	CheckFields();
	
	// Reattach the blank data panel to the end
	if(lastPanel != NULL) {	// If removed, re-establish blank panel
		AddChild(lastPanel);
	}

	// Set up the window
	SizeViewAndWindow();
}
// -------------------------------------------------------------------
void
DPDataView::AddDataSet(DPData* theData)
{
	BAutolock lock(Window());
	// Delete the last data panel, if any, since it's empty
	int32 numKids = CountChildren();
	BView* lastPanel = NULL;
	// There is always the launch panel, which we can't remove
	if(numKids > 1) {
		lastPanel = ChildAt(numKids - 1);
		RemoveChild(lastPanel);
	}

	// The panel sizes itself
	BRect frame(0,0,0,0);
	DPDataPanel* thePanel = new DPDataPanel(theData,frame);
	AddChild(thePanel);
	CheckFields();
	
	// Reattach the blank data panel to the end
	if(lastPanel != NULL) {	// If removed, re-establish blank panel
		AddChild(lastPanel);
	}

	// Set up the window
	SizeViewAndWindow();
}
// -------------------------------------------------------------------
void
DPDataView::AddDataSet(BMessage* theData)
{
	BAutolock lock(Window());
	// Delete the last data panel, if any, since it's empty
	int32 numKids = CountChildren();
	BView* lastPanel = NULL;
	// There is always the launch panel, which we can't remove
	if(numKids > 1) {
		lastPanel = ChildAt(numKids - 1);
		RemoveChild(lastPanel);
	}

	// The panel sizes itself
	BRect frame(0,0,0,0);
	DPDataPanel* thePanel = new DPDataPanel(theData,frame);
	delete theData;
	AddChild(thePanel);
	CheckFields();
	
	// Reattach the blank data panel to the end
	if(lastPanel != NULL) {	// If removed, re-establish blank panel
		AddChild(lastPanel);
	}

	// Set up the window
	SizeViewAndWindow();
}
// -------------------------------------------------------------------
// Sees how many fields are available & sends data to launch panel
void
DPDataView::CheckFields()
{
	BAutolock lock(Window());
		
	// Go through data list and see how many fields are present
	int maxFlds = -1;
	int32 indx = 0;
	const DPData* theSet = DataSet(indx++);
	while(theSet != NULL) {
		int flds = 0;
		if(theSet->XField() >= 0) {
			flds++;
			if(theSet->YField() >= 0) {
				flds++;
				if(theSet->ZField() >= 0) {
					flds++;
				}
			}
		}
		if(flds > maxFlds) maxFlds = flds ;
		theSet = DataSet(indx++);
	}
	mIconPanel->SetButtonsFor(--indx,maxFlds);
}
// -------------------------------------------------------------------
const DPDataPanel*
DPDataView::DataPanel(int32 panelNum)
{
	BAutolock lock(Window());
	int32 indx = 0;
	BView* child = ChildAt(indx++);
	while(child != NULL) {
		if(::strcmp(child->Name(),panel_name) == 0 && panelNum-- == 0) {
			return cast_as(child,DPDataPanel);
		}
		child = ChildAt(indx++);
	}
	return NULL;
}		
// -------------------------------------------------------------------
const DPData*
DPDataView::DataSet(int32 setNum)
{
	const DPDataPanel* thePanel = DataPanel(setNum);
	if(thePanel != NULL) {
		return thePanel->Data();
	} 
	
	return NULL;
}
// -------------------------------------------------------------------
void
DPDataView::RemoveAllData()
{
	BAutolock lock(Window());
	const DPDataPanel* thePanel = DataPanel(0);
	while(thePanel != NULL) {
		RemoveChild((DPDataPanel*)thePanel);
		delete thePanel;
		thePanel = DataPanel(0);
	}
	AddDataSet((entry_ref*)NULL);
}
// -------------------------------------------------------------------
void
DPDataView::RemoveSelectedData()
{
	BAutolock lock(Window());
	int32 indx = 0;
	const DPDataPanel* thePanel = DataPanel(indx);
	while(thePanel != NULL) {
		if(thePanel->Selected()) {
			RemoveChild((DPDataPanel*)thePanel);
			delete thePanel;
			indx--;
		}
		indx++;
		thePanel = DataPanel(indx);
	}
	CheckFields();
	SizeViewAndWindow();
}
// -------------------------------------------------------------------
// *  Protected Methods             
// -------------------------------------------------------------------
void
DPDataView::Draw(BRect theRect)
{
}
// -------------------------------------------------------------------
		
void
DPDataView::FrameResized(float width, float height)
{
}
// -------------------------------------------------------------------
void
DPDataView::MessageReceived(BMessage* theMessage)
{
   
 	switch ( theMessage->what ){
 	
    	
		default :
			this->BView::MessageReceived(theMessage);
	}
}
// -------------------------------------------------------------------
void
DPDataView::SizeViewAndWindow()
{
	BAutolock lock(Window());
	const float	panel_margin = 5;
	float height = panel_margin;
	int32 indx = 0;
	DPDataPanel* thePanel = (DPDataPanel*)DataPanel(indx++);
	float width = thePanel->Bounds().Width();
	DPDataPanel* nextPanel = (DPDataPanel*)DataPanel(indx);
	// The 'lastItem' is the last item that can be combined
	while(thePanel != NULL) {
		bool combine = (nextPanel != NULL && nextPanel->Data() != NULL);
		thePanel->AllowCombine(combine);
		thePanel->MoveTo(0,height);
		height += thePanel->Bounds().Height();
		thePanel = (DPDataPanel*)DataPanel(indx++);
		nextPanel = (DPDataPanel*)DataPanel(indx);
	}
	height += ctrl_ht + mb_height + panel_margin*2;
	Window()->ResizeTo(width,height);	// Resizes this view too
	mIconPanel->MoveTo(panel_margin,Bounds().Height() - (ctrl_ht + panel_margin));
	
	// Since all data changes to the data sets pass through here,
	// we use this opportunity to make the temporary data file.
//	DPDataWindow* dWind = cast_as(Window(),DPDataWindow);
//	dWind->ArchiveToTemp();
}
// -------------------------------------------------------------------

		
