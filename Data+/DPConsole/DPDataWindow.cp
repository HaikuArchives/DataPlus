/* ------------------------------------------------------------------ 

	Title: DPDataWindow

	Description:  The window for managing Data+ data

	Author :
		Stephen Wardlaw, M.D.
		Medical Research & Development
		Highrock
		Lyme, CT 06371

	Edit History:	
		24 August, 1999
			Created
		
----------------------------------------------------------------- */ 
#include "DPDataWindow.h"
#include "DPGlobals.h"
#include "DPTextFile.h"
#include "DPFldWindow.h"
#include "DPDataPanel.h"
#include "DPDataFile.h"
#include "DataPlus.h"
#include "WU_AppUtils.h"
#include "WUPrefFile.h"
#include <AppKit.h>

const char 	pref_file[16] 		= "dataplus.prf";

const uint32 remove_sel			= 'Rsel';
const uint32 data_unMark		= 'Dumk';
const uint32 data_unSuppress 	= 'Duns';
const uint32 data_unDelete		= 'Dudl';
const uint32 data_allowMark		= 'Damk';
const uint32 file_export		= 'Fexp';

// Labels for save file panel menu
const char native_type_str[16]	= "DataPlus";
const char text_type_str[8]		= "Text";

// -------------------------------------------------------------------
//	* CONSTRUCTORS and DESTRUCTORS
// -------------------------------------------------------------------
DPDataWindow::DPDataWindow(BRect frame)
				: BWindow(frame, "Data+", B_TITLED_WINDOW_LOOK,B_NORMAL_WINDOW_FEEL, B_NOT_CLOSABLE|B_NOT_ZOOMABLE|B_NOT_RESIZABLE)
{
	mDataChanged = false;
	mAllowMarking = false;
	// Add the main menu bar and menus
	BRect menuRect(0,0,1000,mb_height);
	BMenuBar* menuBar = new BMenuBar(menuRect,"MB");
	
	BMenu*	fileMenu = new BMenu("File");
	BMenuItem* aboutItem = new BMenuItem("About Data+" B_UTF8_ELLIPSIS,new BMessage(B_ABOUT_REQUESTED));
	aboutItem->SetTarget(be_app);
	fileMenu->AddItem(aboutItem);
	fileMenu->AddSeparatorItem();
	fileMenu->AddItem(new BMenuItem("Open" B_UTF8_ELLIPSIS,new BMessage(B_MENU_OPEN),'O'));
	fileMenu->AddSeparatorItem();
	mSaveItem = new BMenuItem("Save",new BMessage(B_MENU_SAVE),'S');
	fileMenu->AddItem(mSaveItem);
	mSaveAsItem = new BMenuItem("Save As"B_UTF8_ELLIPSIS,new BMessage(B_MENU_SAVEAS));
	fileMenu->AddItem(mSaveAsItem);
	fileMenu->AddSeparatorItem();
	mExportItem = new BMenuItem("Export"B_UTF8_ELLIPSIS,new BMessage(file_export));
	fileMenu->AddItem(mExportItem);
	fileMenu->AddSeparatorItem();
	BMenuItem* theItem = new BMenuItem("Quit",new BMessage(B_QUIT_REQUESTED),'Q');
	fileMenu->AddItem(theItem);
	menuBar->AddItem(fileMenu);
	
	BMenu* dataMenu = new BMenu("Data");
	mClearItem = new BMenuItem("Clear All Data",new BMessage(clear_data));
	dataMenu->AddItem(mClearItem);
	mRemoveItem = new BMenuItem("Remove Selected Sets",new BMessage(remove_sel));
	dataMenu->AddItem(mRemoveItem);
	mAllowMarkItem = new BMenuItem("Allow Marking",new BMessage(data_allowMark));
	dataMenu->AddItem(mAllowMarkItem);
	mAllowMarkItem->SetMarked(mAllowMarking);
	dataMenu->AddSeparatorItem();
	BMenu* markMenu = new BMenu("Unselect");
	dataMenu->AddItem(markMenu);
	mUnMarkItem = new BMenuItem("Unmark",new BMessage(data_unMark));
	markMenu->AddItem(mUnMarkItem);
	mUnSuppressItem = new BMenuItem("Unsuppress",new BMessage(data_unSuppress));
	markMenu->AddItem(mUnSuppressItem);
	mUnDeleteItem = new BMenuItem("Undelete",new BMessage(data_unDelete));
	markMenu->AddItem(mUnDeleteItem);
	menuBar->AddItem(dataMenu);
	AddChild(menuBar);
	
	// Set up a default data path to /boot/home
	find_directory(B_USER_DIRECTORY,&mAppPath);

	// Open the preferences file and see if there is anything in it
	mPrefFile = new WUPrefFile(pref_dir,pref_file);
	BMessage* prefMesg = mPrefFile->GetPrefData();
	if(prefMesg != NULL) {
		BPoint thePt;
		if(prefMesg->FindPoint(name_posn,&thePt) == B_OK) {
			MoveTo(thePt);
		}
		const char* pName = prefMesg->FindString(name_dataDir);
		if(pName != NULL) {
			mAppPath.SetTo(pName);
		}
	}
	delete prefMesg;
	
	// File panels wll be created if needed
	::strcpy(mFileName,"Untitled");
	mOpenPanel = NULL;
	mSavePanel = NULL;
	mFileFilter = NULL;
	mFile = NULL;
	
	// Create a BEntry pointing to a temporary file
	BPath thePath;
	::GetAppDirectoryPath(&thePath);
	thePath.Append(dp_temp_file);
	mTempFileRef.SetTo(thePath.Path());
	
	// Add the data handler to the window
	BRect viewRect = Bounds();
	viewRect.top += mb_height + 2;
	mDataView= new DPDataView(viewRect);
	AddChild(mDataView);
	mDataView->AddDataSet((entry_ref*)NULL);	// Add a 'starting' data panel
	//SetSizeLimits((frame.right - frame.left)/2,9999,(frame.bottom - frame.top)/2,9999);
}

DPDataWindow::~DPDataWindow()
{
	// Save the current window position and data directory
	BMessage* prefMesg = new BMessage();
	BPoint upLeft = Frame().LeftTop();
	prefMesg->AddPoint(name_posn,upLeft);
	prefMesg->AddString(name_dataDir,mAppPath.Path());
	mPrefFile->SetPrefData(prefMesg);
	delete prefMesg;
	
	delete mFile;
	delete mOpenPanel;
	delete mSavePanel;
	delete mPrefFile;
	delete mFileFilter;
	
	// Remove the temporary data file
	if(mTempFileRef.Exists()) {
		mTempFileRef.Remove();
	}
}
// -------------------------------------------------------------------
//	* PUBLIC METHODS
// -------------------------------------------------------------------
BMessage*
DPDataWindow::ArchiveAll(bool deep)
{
	BMessage* fileArchive = new BMessage(DPData::fld_code);
	int32 setNum = 0;
	const DPDataPanel* thePanel = mDataView->DataPanel(setNum);
	const DPData* theSet = mDataView->DataSet(setNum++);
	const DPData* nextSet = NULL;
	bool combine;
	while(theSet != NULL) {
		BMessage* setArchive = new BMessage();
		theSet->Archive(setArchive,deep);
		combine = thePanel->CanCombine();
		if(combine) {
			thePanel = mDataView->DataPanel(setNum);
			nextSet = mDataView->DataSet(setNum++);
			while(nextSet != NULL && combine) {
				if(!deep) {
					// Archive and re-instantiate 'nextSet' to get proper depth
					BMessage* deepArch = new BMessage();
					nextSet->Archive(deepArch,false);
					BArchivable* theObj = instantiate_object(deepArch);
					delete deepArch;
					nextSet = cast_as(theObj,DPData);
					if(nextSet == NULL) {
						throw("Error In Resurrecting Record Set");
					}
				}
				// Add fields from nextSet to theSet
				for(int32 n = 0; n < nextSet->NumRecords(); n++) {
					// Add fields as archived messages
					DPDataRec* dRec = nextSet->DataRecord(n);
					BMessage* recArch = new BMessage();
					dRec->Archive(recArch,deep);
					setArchive->AddMessage(data_rec_str,recArch);
					delete recArch;
				}
				combine = thePanel->CanCombine();
				if(!deep)	{	// Remove duplicate
					delete nextSet;
				}
				if(combine) {
					thePanel = mDataView->DataPanel(setNum);
					nextSet = mDataView->DataSet(setNum++);
				}
			}
		}
		fileArchive->AddMessage(data_set_str,setArchive);
		theSet = mDataView->DataSet(setNum++);
	}
	return fileArchive;
}
// ------------------------------------------------------------------ 
void
DPDataWindow::ArchiveToTemp(bool deep)
{ 
	DPDataFile* theFile = new DPDataFile(&mTempFileRef);
	theFile->Clear();
	if(theFile->InitCheck() == B_OK) {
		Save(theFile,deep);
		// Set the MIME type 
		BNodeInfo theInfo;
		theInfo.SetTo(theFile);
		theInfo.SetType(dptempfile_type);
	}
	delete theFile;
}
// ------------------------------------------------------------------ 
void
DPDataWindow::MarkData(BMessage* theMesg)
{
	if(mAllowMarking) {
		int32 setNum = 0;
		DPData* theData = (DPData*)mDataView->DataSet(setNum++);
		while(theData != NULL) {
			theData->MarkData(theMesg);
			theData = (DPData*)mDataView->DataSet(setNum++);		
		}
	}
	delete theMesg;
}
// -------------------------------------------------------------------
void
DPDataWindow::MessageReceived(BMessage* theMessage)
{
	
 	switch ( theMessage->what ){
 	
		case FLDS_RESET :
 			mDataView->CheckFields();
 			break;

		case B_SAVE_REQUESTED :	// User wants us to save
			SaveAsFile(theMessage);
			break;
			
		case B_REFS_RECEIVED :
			//ClearData();	// Clear old data first
			OpenFile(theMessage);
			break;
			
		case B_MENU_OPEN :
			if(CheckSave()) {
				if(mOpenPanel == NULL) {
					ConstructOpenPanel();
				}
				mOpenPanel->Show();
			}
			break;
			
		case B_MENU_SAVE :
			Save(mFile);
			break;
			
		case B_MENU_SAVEAS :
			if(mSavePanel == NULL) {
				ConstructSavePanel();
			}
			mSavePanel->Show();
			break;
			
		case file_export :
			;
			break;
			
		case clear_data :
			if(CheckSave()) {
				mDataView->RemoveAllData();
				delete mFile;
				mFile = NULL;
			}
			break;
		
		case remove_sel :
			if(CheckSave()) {
				mDataView->RemoveSelectedData();
				delete mFile;
				mFile = NULL;
			}
			break;
				
		case data_unMark :
			ClearStatus(data_marked);
			break;
			
		case data_unSuppress :
			ClearStatus(data_suppress);
			break;
			
		case data_unDelete :
			ClearStatus(data_delete);
			break;
			
		case data_allowMark :
			mAllowMarking = !mAllowMarkItem->IsMarked();
			mAllowMarkItem->SetMarked(mAllowMarking);
			break;
			
		default:
			BWindow::MessageReceived(theMessage);
	}
}
// -------------------------------------------------------------------
//	* PROTECTED METHODS
// -------------------------------------------------------------------
bool
DPDataWindow::CheckSave()
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
DPDataWindow::ClearStatus(EDataStatus status)
{
	int32 indx = 0;
	DPData* theData = (DPData*)mDataView->DataSet(indx++);
	while(theData != NULL) {
		theData->ClearStatus(status);
		theData = (DPData*)mDataView->DataSet(indx++);
	}
}
// -------------------------------------------------------------------
void
DPDataWindow::ConstructOpenPanel()
{
	mOpenPanel = new BFilePanel(B_OPEN_PANEL);
	BMessenger* theMessenger = new BMessenger(this);
	mOpenPanel->SetTarget(*theMessenger);
	mFileFilter = new DPFileFilter(); // Construct the file filter
	mOpenPanel->SetRefFilter(mFileFilter);
	delete theMessenger;
	BDirectory theDirectory;
	if(theDirectory.SetTo(mAppPath.Path()) == B_OK) {
		mOpenPanel->SetPanelDirectory(&theDirectory);
	}
}	
// -------------------------------------------------------------------
void
DPDataWindow::ConstructSavePanel()
{
	BMessenger* theMessenger = new BMessenger(this);
	mSavePanel = new BFilePanel(B_SAVE_PANEL);
	mSavePanel->SetTarget(*theMessenger);
	delete theMessenger;
	
	// Add the saving options menu
	mSavePanel->Window()->Lock();
	BView* panelView = mSavePanel->Window()->ChildAt(0);	// Bkgnd panel
	BView* fNameView = mSavePanel->Window()->FindView("text view");	
	BView* cBView = mSavePanel->Window()->FindView("cancel button");
	mSaveType = new BPopUpMenu(native_type_str);
	mSaveType->AddItem(new BMenuItem(native_type_str,NULL));
	mSaveType->AddItem(new BMenuItem(text_type_str,NULL));
	BMenuField* theFld = new BMenuField(BRect(0,0,0,0),NULL,NULL,mSaveType);
	panelView->AddChild(theFld);
	theFld->ResizeToPreferred();
	float height, width;
	theFld->GetPreferredSize(&width, &height);
	width = theFld->StringWidth(native_type_str);
	float top = cBView->Frame().top + (cBView->Frame().Height() - height)/2;
	float left = fNameView->Frame().right + (cBView->Frame().left - fNameView->Frame().right - width)/2;
	theFld->MoveTo(left,top);

	// Set the panel's directory
	BDirectory theDirectory;
	if(theDirectory.SetTo(mAppPath.Path()) == B_OK) {
		mSavePanel->SetPanelDirectory(&theDirectory);
		mSavePanel->SetSaveText(mFileName);
	}
	mSavePanel->Window()->Unlock();
}
// -------------------------------------------------------------------
void
DPDataWindow::MenusBeginning()
{
	mSaveItem->SetEnabled(mDataChanged && mFile != NULL);
	bool anySets = false;
	bool anySelected = false;
	int32 indx = 0;
	const DPDataPanel* thePanel = mDataView->DataPanel(indx++);
	bool haveMarks = false;
	bool haveSuppress = false;
	bool haveDelete = false;
	while(thePanel != NULL) {
		if(!anySets) anySets = (thePanel->Data() != NULL);
		if(!anySelected) anySelected = thePanel->Selected();
		const DPData* theData = thePanel->Data();
		if(theData != NULL) {
			if(!haveMarks)  haveMarks = theData->HasMarkedData();
			if(!haveSuppress)  haveSuppress = theData->HasSuppressedData();
			if(!haveDelete)  haveDelete = theData->HasDeletedData();
		}
		thePanel = mDataView->DataPanel(indx++);
	}
	mUnMarkItem->SetEnabled(haveMarks);
	mUnSuppressItem->SetEnabled(haveSuppress);
	mUnDeleteItem->SetEnabled(haveDelete);
	mSaveAsItem->SetEnabled(anySets);
	mSaveItem->SetEnabled(anySets && mFile != NULL);
	mClearItem->SetEnabled(anySets);
	mRemoveItem->SetEnabled(anySelected);
	mExportItem->SetEnabled(anySets);
}
// -------------------------------------------------------------------
void
DPDataWindow::OpenFile(BMessage* theMesg)
{
	delete mFile;	// Remove old file, if any
	mFile = NULL;
	entry_ref theRef;
	if(theMesg->FindRef("refs",&theRef) == B_OK) {
		BNode theNode(&theRef);
		if(theNode.InitCheck() == B_OK) {
			AddData(&theRef);
			// Set directory references
			BDirectory wDir;
			BEntry theEntry;
			status_t status = theEntry.SetTo(&theRef);
			status = theEntry.GetParent(&wDir);
			status = mAppPath.SetTo(&wDir,NULL);
			if(mSavePanel == NULL) {
				ConstructSavePanel();
			}
			mSavePanel->SetPanelDirectory(&wDir);
			::strcpy(mFileName,theRef.name);
			mSavePanel->SetSaveText(mFileName);
		}	// if(theNode.InitCheck
	}	// If FindRef
	mDataChanged = false;
	//mDataView->SetFileName(mFileName);
}		
// -------------------------------------------------------------------
void
DPDataWindow::SaveAsFile(BMessage* theMesg)
{
	// See what kind of file we want
	BMenuItem* theItem = mSaveType->FindMarked();
	bool saveAsText = (theItem != NULL && ::strcmp(theItem->Label(),text_type_str) == 0);

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
				// Set default path
				mAppPath.SetTo(&theDir,NULL);
				// Build file path
				thePath.SetTo(&theDir,mFileName);
				if(thePath.InitCheck() == B_OK) {
					if(saveAsText) {
						BFile textFile(thePath.Path(),B_READ_WRITE|B_CREATE_FILE);
						if(textFile.InitCheck() == B_OK) {
							SaveAsTextFile(&textFile);
							completed = true;
						} 
					} else {	// If saving as a native file
						mFile = new DPDataFile(thePath.Path());
						Save(mFile);
	
						// Set the MIME type and this application as preferred
						BNodeInfo theInfo;
						theInfo.SetTo(mFile);
						theInfo.SetType(native_mime_type);
						theInfo.SetPreferredApp(data_plus_sig);
						BMimeType theMime(native_mime_type);
						if(!theMime.IsInstalled()) {
							theMime.Install();
						}	
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
	//mDataView->SetFileName(mFileName);
}
// -------------------------------------------------------------------
// NOTE - The "Combine" option is not used with this file type
void
DPDataWindow::SaveAsTextFile(BFile* theFile)
{
	const char tab_char = '\t';
	const char ret_char = '\n';
	const char recIDName[16] = "Record_ID";
	
	int32 setNum = 0;
	const DPData* theSet = mDataView->DataSet(setNum++);
	while(theSet != NULL) {
		// Begin by saving the field titles
		// First field is the record name
		theFile->Write(recIDName,::strlen(recIDName));
		theFile->Write(&tab_char,1);
		for(int32 n = 0; n < theSet->NumFields(); n++) {
			const char* fldName = theSet->FieldType(n)->FieldName();
			theFile->Write(fldName,::strlen(fldName));
			if(n < (theSet->NumFields() - 1)) {
				theFile->Write(&tab_char,1);
			}
		}
		theFile->Write(&ret_char,1);
		
		// Now, save the non-deleted fields
		for(int32 n = 0; n < theSet->NumRecords(); n++) {
			DPDataRec* dRec = theSet->DataRecord(n);
			if(dRec->DataStatus() < data_delete) {
				const char* recName = dRec->RecordName();
				if(recName != NULL) {
					theFile->Write(recName,::strlen(recName));
				}
				theFile->Write(&tab_char,1);
				for(int32 fld = 0; fld < dRec->NumFields(); fld++) {
					const char* valStr = theSet->FieldType(fld)->DataString(dRec,fld);
					theFile->Write(valStr,::strlen(valStr));
					if(fld < (dRec->NumFields() - 1)) {
						theFile->Write(&tab_char,1);
					}
				}
				theFile->Write(&ret_char,1);
			} 
		}
		theFile->Write(&ret_char,1);
		theSet = mDataView->DataSet(setNum++);
	}
	// Set the MIME type and preferred application
	BNodeInfo theInfo;
	theInfo.SetTo(mFile);
	theInfo.SetType(text_mime_type);
	theInfo.SetPreferredApp(style_edit_app);
}
// -------------------------------------------------------------------
void
DPDataWindow::Save(DPDataFile* theFile, bool deep)
{
	if(theFile != NULL) {
		theFile->Clear();
		int32 setNum = 0;
		const DPDataPanel* thePanel = mDataView->DataPanel(setNum);
		const DPData* theSet = mDataView->DataSet(setNum++);
		const DPData* nextSet = NULL;
		bool combine;
		while(theSet != NULL) {
			combine = thePanel->CanCombine();
			// If we combine, we need to create a temporary set which is the
			// combination, and then archive it.
			if(combine) {
				DPData* tempRec = new DPData(*theSet);
				thePanel = mDataView->DataPanel(setNum);
				nextSet = mDataView->DataSet(setNum++);
				while(nextSet != NULL && combine) {
					// Add fields from nextSet to theSet
					for(int32 n = 0; n < nextSet->NumRecords(); n++) {
						// Add fields as archived messages
						const DPDataRec* dRec = nextSet->DataRecord(n);
						DPDataRec* newRec;
						if(deep) {
							newRec = new DPDataRec(*dRec);
						} else {
							newRec = new DPDataRec(*dRec,nextSet->XField(),nextSet->YField(),nextSet->ZField());
						}
						tempRec->AddRecord(newRec);
					}
					combine = thePanel->CanCombine();
					if(combine) {
						thePanel = mDataView->DataPanel(setNum);
						nextSet = mDataView->DataSet(setNum++);
					}
				}
				theFile->AddData(tempRec,deep);
				delete tempRec;
				
			} else {	// if(combine)
				theFile->AddData(theSet,deep);
			}
			
			theSet = mDataView->DataSet(setNum++);
		}
	}
}
// -------------------------------------------------------------------
bool DPDataWindow::QuitRequested()
{
	be_app->PostMessage(B_QUIT_REQUESTED);
	return(TRUE);
}
