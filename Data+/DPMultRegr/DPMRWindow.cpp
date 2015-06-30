/* ------------------------------------------------------------------ 

	Title: DPMRWindow

	Description:  The multiple regression display window

	Author :
		Stephen Wardlaw, M.D.
		Medical Research & Development
		Highrock
		Lyme, CT 06371

	Edit History:	
		16 March, 2001
			Created
		
----------------------------------------------------------------- */ 
#include "DPMRWindow.h"
#include "DPMultRegr.h"
#include "DPGlobals.h"
#include "DPData.h"
#include "WU_AppUtils.h"
#include "U_Conversions.h"
#include <AppKit.h>
#include <SupportKit.h>

const uint32 mesg_unmark	= 'Umrk';
const uint32 mesg_format	= 'Mfmt';
const uint32 mesg_psetup	= 'PrnS';
const uint32 mesg_print		= 'Prnt';
const uint32 data_unSuppress 	= 'Duns';
const uint32 data_unDelete		= 'Dudl';
const uint32 data_allowMark	= 'Damk';


// -------------------------------------------------------------------
//	* CONSTRUCTORS and DESTRUCTORS
// -------------------------------------------------------------------
DPMRWindow::DPMRWindow(BRect frame)
				: BWindow(frame, "Regression", B_TITLED_WINDOW_LOOK,B_NORMAL_WINDOW_FEEL,0)
{
	mDataChanged = false;
	mAllowMarking = true;
	mDocData = NULL;
	mPaperRect.Set(0,0,0,0);
	mPrintableRect = mPaperRect;
	// Add the main menu bar and menus
	BRect menuRect(0,0,1000,mb_height);
	BMenuBar* menuBar = new BMenuBar(menuRect,"MB");
	
	BMenu*	fileMenu = new BMenu("File");
	BMenuItem* aboutItem = new BMenuItem("About" B_UTF8_ELLIPSIS,new BMessage(B_ABOUT_REQUESTED));
	aboutItem->SetTarget(be_app);
	fileMenu->AddItem(aboutItem);
	fileMenu->AddSeparatorItem();
	fileMenu->AddItem(new BMenuItem("Open" B_UTF8_ELLIPSIS,new BMessage(B_MENU_OPEN),'O'));
	fileMenu->AddSeparatorItem();
	mSaveItem = new BMenuItem("Save",new BMessage(B_MENU_SAVE),'S');
	fileMenu->AddItem(mSaveItem);
	mSaveAsItem = new BMenuItem("Save As" B_UTF8_ELLIPSIS,new BMessage(B_MENU_SAVEAS));
	fileMenu->AddItem(mSaveAsItem);
	fileMenu->AddSeparatorItem();
	fileMenu->AddItem(new BMenuItem("Print Setup" B_UTF8_ELLIPSIS,new BMessage(mesg_psetup)));
	mPrintItem = new BMenuItem("Print" B_UTF8_ELLIPSIS,new BMessage(mesg_print));
	fileMenu->AddItem(mPrintItem);
	fileMenu->AddSeparatorItem();
	BMenuItem* theItem = new BMenuItem("Quit",new BMessage(B_QUIT_REQUESTED),'Q');
	fileMenu->AddItem(theItem);
	menuBar->AddItem(fileMenu);
	
	BMenu* dataMenu = new BMenu("Data");
	mAllowMarkItem = new BMenuItem("Allow Marking",new BMessage(data_allowMark));
	dataMenu->AddItem(mAllowMarkItem);
	mAllowMarkItem->SetMarked(true);
	dataMenu->AddSeparatorItem();
	BMenu* markMenu = new BMenu("Unselect");
	dataMenu->AddItem(markMenu);
	mUnSuppressItem = new BMenuItem("Unsuppress",new BMessage(data_unSuppress));
	markMenu->AddItem(mUnSuppressItem);
	mUnDeleteItem = new BMenuItem("Undelete",new BMessage(data_unDelete));
	markMenu->AddItem(mUnDeleteItem);
	menuBar->AddItem(dataMenu);
	AddChild(menuBar);
	

	// Get the path preference, if it exists..

	mFile = NULL;
	mOpenPanel = NULL;
	mSavePanel = NULL;
	::strcpy(mFileName,"Untitled");
	
	// Add the MR view to the window
	/*
	BRect grafRect = Bounds();
	grafRect.top += mb_height;
	grafRect.InsetBy(graf_margin,graf_margin);
	mPlot = new DPRegrPlot(grafRect);
	AddChild(mPlot);
	*/
	
}

DPMRWindow::~DPMRWindow()
{
}
// -------------------------------------------------------------------
//	* PUBLIC METHODS
// -------------------------------------------------------------------
BMessage*
DPMRWindow::ArchiveAll()
{
	BMessage* fileArchive = new BMessage(DPData::fld_code);
	int32 setNum = 0;
	DPMultRegr* theApp = cast_as(be_app,DPMultRegr);
	const DPData* theData = theApp->DataSet(setNum++);
	while(theData != NULL) {
		BMessage* setArchive = new BMessage();
		theData->Archive(setArchive,true);
		fileArchive->AddMessage(data_set_str,setArchive);
		theData = theApp->DataSet(setNum++);
		delete setArchive;
	}
	return fileArchive;
}
// -------------------------------------------------------------------
void
DPMRWindow::MessageReceived(BMessage* theMessage)
{
	const char* labelStr;
	BRect frame;
	BMessenger* theMessenger;
	int32	colorVal;
	int16	markVal;
	
 	switch ( theMessage->what ){

		case B_SAVE_REQUESTED :	// User wants us to save
			SaveAsFile(theMessage);
			break;
			
		case B_REFS_RECEIVED :
			OpenFile(theMessage);
			break;
			
		case B_MENU_OPEN :
			if(CheckSave()) {
				CreateOpenPanel();
			}
			break;
			
		case B_MENU_SAVE :
			Save();
			break;
			
		case B_MENU_SAVEAS :
			CreateSavePanel();
			break;
			
		case FLDS_RESET :
			// ***************
			break;
			
		case mesg_psetup :
			SetUpPage();
			break;
			
		case mesg_print :
			Print();
			break;
			
		case data_allowMark :
			mAllowMarking = !mAllowMarkItem->IsMarked();
			mAllowMarkItem->SetMarked(mAllowMarking);
			break;
			
		case data_unSuppress :
			ClearStatus(data_suppress);
			break;
			
		case data_unDelete :
			ClearStatus(data_delete);
			break;
			
		default:
			BWindow::MessageReceived(theMessage);
	}
}
// -------------------------------------------------------------------
void
DPMRWindow::ReDraw()
{
	Lock();
	//mDispFrame->RefreshDisplays();
	//mPlot->Invalidate();
	Unlock();
}
// -------------------------------------------------------------------
//	* PROTECTED METHODS
// -------------------------------------------------------------------
bool
DPMRWindow::CheckSave()
{
	if(mDataChanged) {
		BAlert* theAlert = new BAlert("ChkSave","Save Changes To The Data?","Cancel","Don't Save","Save",B_WIDTH_FROM_LABEL);
		int32 choice = theAlert->Go();
		switch(choice) {
			case 0 :	// Cancel
				return false;
				break;
				
			case 1 :	// Don't save
				return true;
				break;
				
			case 2 :	//Save file
				mSavePanel->Show();
				return false;
				break;
			
			default :
				;
		}
	} 
	return true;
}
// -------------------------------------------------------------------
void
DPMRWindow::ClearStatus(EDataStatus status)
{
	/*
	DPRegression* theApp = cast_as(be_app,DPRegression);
	int32 indx = 0;
	DPData* theData = (DPData*)theApp->DataSet(indx++);
	while(theData != NULL) {
		theData->ClearStatus(status);
		theData = (DPData*)theApp->DataSet(indx++);
	}
	ReDraw();
	*/
}
// -------------------------------------------------------------------
void
DPMRWindow::CreateOpenPanel()
{
	if(mOpenPanel == NULL) {
		mOpenPanel = new BFilePanel(B_OPEN_PANEL);
		BMessenger* theMessenger = new BMessenger(this);
		mOpenPanel->SetTarget(*theMessenger);
		delete theMessenger;
		BDirectory theDirectory;
		if(theDirectory.SetTo(mAppPath.Path()) == B_OK) {
			mOpenPanel->SetPanelDirectory(&theDirectory);
		}
	}
	mOpenPanel->Show();
}		
// -------------------------------------------------------------------
void
DPMRWindow::CreateSavePanel()
{
	if(mSavePanel == NULL) {
		BMessenger* theMessenger = new BMessenger(this);
		mSavePanel = new BFilePanel(B_SAVE_PANEL);
		mSavePanel->SetTarget(*theMessenger);
		delete theMessenger;
		BDirectory theDirectory;
		if(theDirectory.SetTo(mAppPath.Path()) == B_OK) {
			mSavePanel->SetPanelDirectory(&theDirectory);
			mSavePanel->SetSaveText(mFileName);
		}
	}
	mSavePanel->Show();
}
// -------------------------------------------------------------------
void
DPMRWindow::FrameResized(float width, float height)
{
	/*
	if(mPaneSel->Value() > 0) {	// If opened up
		float width, height;
		BScrollBar* theScroll = mDispScroll->ScrollBar(B_VERTICAL);
		mDispFrame->GetPreferredSize(&width, &height);
		if(height <= mDispScroll->Bounds().Height()) {
			theScroll->SetRange(0,0);
		} else {
			theScroll->SetRange(0,height - mDispScroll->Bounds().Height());
			theScroll->SetProportion(mDispScroll->Bounds().Height()/height);
		}
	}
	*/
}
// -------------------------------------------------------------------
void
DPMRWindow::MenusBeginning()
{
	DPMultRegr* theApp = cast_as(be_app,DPMultRegr);
	mSaveAsItem->SetEnabled(theApp->NumSets() > 0);
	mSaveItem->SetEnabled(mDataChanged && mFile != NULL); 
	mPrintItem->SetEnabled(theApp->NumSets() > 0);
	bool haveSuppress = false;
	bool haveDelete = false;
	int32 setNum = 0;
	const DPData* theData = theApp->DataSet(setNum++);
	while(theData != NULL) {
		if(!haveSuppress)  haveSuppress = theData->HasSuppressedData();
		if(!haveDelete)  haveDelete = theData->HasDeletedData();
		theData = theApp->DataSet(setNum++);
	}
	mUnSuppressItem->SetEnabled(haveSuppress);
	mUnDeleteItem->SetEnabled(haveDelete);
}
// -------------------------------------------------------------------
void
DPMRWindow::OpenFile(BMessage* theMesg)
{
	delete mFile;	// Remove old file, if any
	char typeStr[64];
	mFile = NULL;
	entry_ref theRef;
	if(theMesg->FindRef("refs",&theRef) == B_OK) {
		BNode theNode(&theRef);
		if(theNode.InitCheck() == B_OK) {
			BNodeInfo theInfo(&theNode);
			if(theInfo.InitCheck() == B_OK && theInfo.GetType(typeStr) == B_OK) {
				// Check MIME type for translator, if required
				BMimeType mType(typeStr);
				if(mType.Type() != NULL && ::strcmp(mType.Type(),regrplot_mime_type) == 0) {
					// Open as a native file
					mFile = new BFile(&theRef,B_READ_WRITE);
					if(mFile->InitCheck() == B_OK) {
						BMessage* archive = new BMessage();
						if(archive->Unflatten(mFile) == B_OK) {
							// Get and add the data sets
							int32 sIndx = 0;
							DPData* theData;
							do {
								BMessage* setMesg = new BMessage();
								archive->FindMessage(data_set_str,sIndx++,setMesg);
								BArchivable* theObj = instantiate_object(setMesg);
								theData = cast_as(theObj,DPData);
								delete setMesg;
								/*
								if(theData != NULL) {
									DPRegression* theApp = cast_as(be_app,DPRegression);
									theApp->AddDisplaySet(theData);
								}
								*/
							} while (theData != NULL) ;
							//Lock();
							//mPlot->Rescale();
							//Unlock();
						}	// Of if(archive 
						delete archive;
					} else {	// Of if(mFile->InitCheck
						BAlert* theAlert = new BAlert("OFile","Can't Open This File","OK");
						theAlert->Go();
					}
				} else {	// Of if(type..
					BAlert* theAlert = new BAlert("TFile","Can't Open This File Type","OK");
					theAlert->Go();
				}
			}	// if(Type() != NULL
		}	// if(theNode.InitCheck
	}	// if(FindRef
	mDataChanged = false;
}		
// -------------------------------------------------------------------
void
DPMRWindow::Print()
{
	if(mDocData == NULL) {
		if(SetUpPage() != B_OK) {
			return;
		}
	}
	BAutolock wLock(this);
	// Set up for printing
	BPrintJob job("Regr Plot");
	job.SetSettings(mDocData);
	if(job.ConfigJob() == B_OK) {
		job.BeginJob();
		// Draw the pages
		BRect pRect = job.PrintableRect();
		pRect.InsetBy(50,50);
		// Make a square graph
		// BRect oldFrame = mPlot->Bounds();
		bool upRight = pRect.Width() < pRect.Height();
		float pWidth;
		BPoint pPt;
		if(upRight) {
			pWidth = pRect.Width();
		} else {
			pWidth = pRect.Height();
		}
		pPt.x = (pRect.Width() - pWidth)/2;
		pPt.y = (pRect.Height() - pWidth)/2;
		//mPlot->ResizeTo(pWidth,pWidth);
		//mPlot->SetFrame();
		//job.DrawView(mPlot,pRect,pPt);
		// Restore graph size
		//mPlot->ResizeTo(oldFrame.Width(),oldFrame.Height());
		//mPlot->ForceReDraw();
		job.SpoolPage();
		

		// Finish...
		job.CommitJob();
	}	// Of if(job.Config.. 
}
// -------------------------------------------------------------------
void
DPMRWindow::SaveAsFile(BMessage* theMesg)
{
	// Start a new file under a different name
	delete mFile;
	mFile = NULL;
	entry_ref theRef;
	BDirectory theDir;
	BPath thePath;
	const char* fString;
	bool completed = false;
	if(theMesg->FindRef("directory",&theRef) == B_OK) {
		fString = theMesg->FindString("name");
		if(fString[0] > 0) {
			::strcpy(mFileName,fString);
			theDir.SetTo(&theRef);
			if(theDir.InitCheck() == B_OK) {
				thePath.SetTo(&theDir,mFileName);
				if(thePath.InitCheck() == B_OK) {
					mFile = new BFile(thePath.Path(),B_READ_WRITE|B_CREATE_FILE);
					if(mFile->InitCheck() == B_OK) {
						Save();
						// Set the MIME type for this application
						BNodeInfo theInfo;
						theInfo.SetTo(mFile);
						theInfo.SetType(mr_mime_type);
						theInfo.SetPreferredApp(data_plus_multregr_sig);
						completed = true;
					}
				}
			}
		}
	}
	if(!completed) {
		char errStr[128];
		::strcpy(errStr,"The File Named ");
		::strcat(errStr,mFileName);
		::strcat(errStr," Could Not Be Saved.");
		BAlert* theAlert = new BAlert("FileError",errStr,"OK");
		theAlert->Go();
	}
//	mDataView->SetFileName(mFileName);
}
// -------------------------------------------------------------------
void
DPMRWindow::Save()
{
	if(mFile != NULL) {
	/*
		BMessage* fileArchive = ArchiveAll();
		mFile->Seek(0,SEEK_SET);
		fileArchive->Flatten(mFile);
		mDataChanged = false;
		delete fileArchive;
		*/
	}
}
// -------------------------------------------------------------------
status_t
DPMRWindow::SetUpPage()
{
	status_t status;
	BPrintJob theJob("Regr Plot");
	
	status = theJob.ConfigPage();
	if(status == B_OK) {
		mDocData = theJob.Settings();
		mPaperRect = theJob.PaperRect();
		mPrintableRect = theJob.PrintableRect();
	}
	return status;
}
// -------------------------------------------------------------------
bool 
DPMRWindow::QuitRequested()
{
	be_app->PostMessage(B_QUIT_REQUESTED);
	return(TRUE);
}
// -------------------------------------------------------------------
