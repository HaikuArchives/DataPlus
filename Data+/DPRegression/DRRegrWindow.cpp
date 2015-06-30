/* ------------------------------------------------------------------ 

	Title: DRRegrWindow

	Description:  The graph window

	Author :
		Stephen Wardlaw, M.D.
		Medical Research & Development
		Highrock
		Lyme, CT 06371

	Edit History:	
		08 Dec, 2000
			Added preference file
		29 November, 1999
			Modified for single-window
		14 August, 1999
			Created
		
----------------------------------------------------------------- */ 
#include "DRRegrWindow.h"
#include "DPRegression.h"
#include "DRRegrDisplay.h"
#include "DPGlobals.h"
#include "DRGlobals.h"
#include "DPData.h"
#include "WGAxisEditWind.h"
#include "WGAxisEdit.h"
#include "WU_AppUtils.h"
#include "U_Conversions.h"
#include <AppKit.h>
#include <SupportKit.h>

const char 	pref_file[16] 	= "regrplot.prf";

const uint32 docIcon_mini	= 'DRiS';
const uint32 docIcon_large	= 'DRiL';

const uint32 mesg_unmark	= 'Umrk';
const uint32 mesg_format	= 'Mfmt';
const uint32 mesg_swap		= 'Swap';
const uint32 mesg_setAxes	= 'Aset';
const uint32 mesg_psetup	= 'PrnS';
const uint32 mesg_print		= 'Prnt';
const uint32 mesg_panel		= 'Panl';
const uint32 data_unMark		= 'Dumk';
const uint32 data_unSuppress 	= 'Duns';
const uint32 data_unDelete		= 'Dudl';
const uint32 data_allowMark	= 'Damk';

// Positioning constants
const float graf_margin	= 3;	// Plot margin
const float min_ht		= 350;
const float min_wd		= 350;

// -------------------------------------------------------------------
//	* CONSTRUCTORS and DESTRUCTORS
// -------------------------------------------------------------------
DRRegrWindow::DRRegrWindow(BRect frame)
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
	//mSaveItem = new BMenuItem("Save",new BMessage(B_MENU_SAVE),'S');
	//fileMenu->AddItem(mSaveItem);
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
	mUnMarkItem = new BMenuItem("Unmark",new BMessage(data_unMark));
	markMenu->AddItem(mUnMarkItem);
	mUnSuppressItem = new BMenuItem("Unsuppress",new BMessage(data_unSuppress));
	markMenu->AddItem(mUnSuppressItem);
	mUnDeleteItem = new BMenuItem("Undelete",new BMessage(data_unDelete));
	markMenu->AddItem(mUnDeleteItem);
	dataMenu->AddSeparatorItem();
	dataMenu->AddItem(new BMenuItem("Reset Axes",new BMessage(FLDS_RESET)));
	dataMenu->AddItem(new BMenuItem("Swap Axes",new BMessage(mesg_swap)));
	dataMenu->AddSeparatorItem();
	dataMenu->AddItem(new BMenuItem("Set Axes"B_UTF8_ELLIPSIS,new BMessage(mesg_setAxes)));
	menuBar->AddItem(dataMenu);
	AddChild(menuBar);
	
	// Resource IDs for archived pictures
	const uint32 pctrl_open		= 'Popn';
	const uint32 pctrl_closed	= 'Pclo';
	const uint32 pctrl_dis		= 'Pcds';
	// Add the view select control to the menu bar
	BPicture* picOff = ::GetAppPicture(pctrl_open);
	BPicture* picOn = ::GetAppPicture(pctrl_closed);
	BPicture* picDis = ::GetAppPicture(pctrl_dis);
	BRect bFrame(0,0,13,13);
	bFrame.OffsetBy(menuBar->Bounds().right - 16,3);
	mPaneSel = new BPictureButton(bFrame,"",picOff,picOn,new BMessage(mesg_panel),B_TWO_STATE_BUTTON,B_FOLLOW_TOP|B_FOLLOW_RIGHT);
	mPaneSel->SetDisabledOff(picDis);
	mPaneSel->SetDisabledOn(picDis);
	delete picOff;
	delete picOn;
	delete picDis;
	menuBar->AddChild(mPaneSel);
	mPaneSel->SetEnabled(false);

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
	
	mFile = NULL;
	mOpenPanel = NULL;
	mFileFilter = NULL;
	mSavePanel = NULL;
	::strcpy(mFileName,"Untitled");
	
	// Add the graph to the window
	BRect grafRect = Bounds();
	grafRect.top += mb_height;
	grafRect.InsetBy(graf_margin,graf_margin);
	mPlot = new DRRegrPlot(grafRect);
	AddChild(mPlot);
	
	// Add the odometer
	mOdometer = new WGOdometer(mPlot,BRect(0,0,0,0),false);
	mOdometer->SetResizingMode(B_FOLLOW_RIGHT | B_FOLLOW_TOP);
	menuBar->AddChild(mOdometer);
	mOdometer->SetViewColor(menuBar->ViewColor());
	mOdometer->SetLowColor(menuBar->ViewColor());
	PositionOdometer();
	
	// Add the view for showing the regression data, but do not size it
	BRect gFrame = mPlot->Frame();
	gFrame.OffsetBy(gFrame.Width(),0);
	gFrame.right = gFrame.left + 200;
	gFrame.bottom -= mb_height;
	mDispFrame = new DRDisplayFrame(gFrame);
	mDispScroll = new BScrollView("dispscroll",mDispFrame,B_FOLLOW_TOP_BOTTOM,0,false,true,B_NO_BORDER);
	AddChild(mDispScroll);
	ShowHideDisplay(false);
}

DRRegrWindow::~DRRegrWindow()
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

}
// -------------------------------------------------------------------
//	* PUBLIC METHODS
// -------------------------------------------------------------------
void
DRRegrWindow::AddDisplay(DRRegrData* theData)
{
	Lock();
	mDispFrame->AddDisplay(theData);
	ShowHideDisplay(true);
	mPaneSel->SetEnabled(true);
	mPaneSel->SetValue(1);
	Unlock();
}		
// -------------------------------------------------------------------
void
DRRegrWindow::MessageReceived(BMessage* theMessage)
{
	const char* labelStr;
	WGAxisEditWind* editWind;
	BRect frame;
	BMessenger* theMessenger;
	BMessage	axisMesg;
	int32	colorVal;
	int16	markVal;
	
 	switch ( theMessage->what ){

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
					CreateOpenPanel();
				}
				mOpenPanel->Show();
			}
			break;
			
		case B_MENU_SAVE :
			Save();
			break;
			
		case B_MENU_SAVEAS :
			if(mSavePanel == NULL) {
				CreateSavePanel();
			}
			mSavePanel->Show();
			break;
			
		case FLDS_RESET :
			Lock();
			mPlot->Rescale();
			LoadRegrData();
			Unlock();
			break;
			
		case mesg_format :
			break; 
			
		case mesg_swap :
			SwapAxes();
			break; 
			
		case mesg_psetup :
			SetUpPage();
			break;
			
		case mesg_print :
			Print();
			break;
			
		case mesg_panel :
			ShowHideDisplay(mPaneSel->Value() > 0);
			break;
			
		case mesg_setAxes :	// Opens window to set axis
			// Offset axis window in parent
			frame = Frame();
			frame.top += 15;
			frame.left += 15;
			editWind = new WGAxisEditWind(frame,false);
			editWind->SetXDefaults(mPlot->mXAxis);
			editWind->SetYDefaults(mPlot->mYAxis);
			editWind->SetXLabel(mPlot->GetXLabel());
			editWind->SetYLabel(mPlot->GetYLabel());
			editWind->SetPlotMarkDefault(mPlot->PlotMark());
			editWind->SetPlotColorDefault(mPlot->PlotColor());
			// Set messenger here
			theMessenger = new BMessenger(this);
			editWind->SetTarget(theMessenger);
			break;
			
   		case WGAxisEditWind::class_ID :	// Data from axis editor comes here
 			if(theMessage->FindMessage(x_axis_name,&axisMesg) == B_OK) {
 				labelStr = axisMesg.FindString(axis_label);
 				if(labelStr != NULL) {
 					mPlot->SetXLabel(labelStr);
 				}
 				mPlot->mXAxis->SetValues(&axisMesg);
 			}
 			axisMesg.MakeEmpty();
 			if(theMessage->FindMessage(y_axis_name,&axisMesg) == B_OK) {
 				labelStr = axisMesg.FindString(axis_label);
 				if(labelStr != NULL) {
 					mPlot->SetYLabel(labelStr);
 				}
 				mPlot->mYAxis->SetValues(&axisMesg);
 			}
 			if(theMessage->FindInt32(plot_color_name,&colorVal) == B_OK) {
 				mPlot->SetPlotColor(::ValueToColor(colorVal));
 			}
 			if(theMessage->FindInt16(plot_mark_name,&markVal) == B_OK) {
 				mPlot->SetPlotMark((EPlotMark)markVal);
 			}
 			PositionOdometer();
			mPlot->ForceReDraw();
 			break;
			
		case data_allowMark :
			mAllowMarking = !mAllowMarkItem->IsMarked();
			mAllowMarkItem->SetMarked(mAllowMarking);
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
			
		default:
			BWindow::MessageReceived(theMessage);
	}
}
// -------------------------------------------------------------------
void
DRRegrWindow::OpenFile(const entry_ref* theRef, bool typeCheck)
{
	BNode theNode(theRef);
	if(theNode.InitCheck() == B_OK) {
		BNodeInfo theInfo(&theNode);
		if(theInfo.InitCheck() == B_OK) {
			bool validFile = false;
			if(typeCheck) {
				char typeStr[B_MIME_TYPE_LENGTH];
				if(theInfo.GetType(typeStr) == B_OK) {
					// Check MIME type for translator, if required
					BMimeType mType(typeStr);
					validFile = (mType.Type() != NULL && ::strcmp(mType.Type(),regrplot_mime_type) == 0);
				}
			} else {
				validFile = true;
				
			}	
			if(validFile) {
				mFile = new DPDataFile(theRef);
				if(mFile->InitCheck() == B_OK) {
					// Set default directory path
					BDirectory wDir;
					BEntry theEntry;
					if(mSavePanel == NULL) {
						CreateSavePanel();
					}
					status_t status = theEntry.SetTo(theRef);
					status = theEntry.GetParent(&wDir);
					status = mAppPath.SetTo(&wDir,NULL);
					mSavePanel->SetPanelDirectory(&wDir);
					
					// Get and add the data sets
					DPRegression* theApp = cast_as(be_app,DPRegression);
					for(int16 setNum = 0; setNum < mFile->NumSets(); setNum++) {
						DPData* theData = mFile->GetSet(setNum);
						if(theData != NULL) {
							theApp->AddDisplaySet(theData);
						}
					} 	// Of for(
					
					// If we are not typechecking, we need to get the default file name from the data
					if(typeCheck) {
						::strcpy(mFileName,theRef->name);
					} else {
						::strcpy(mFileName,"Untitled");
						const DPData* theData = theApp->DataSet(0);
						if(theData != NULL) {
							const char* defName = theData->SetName();
							if(defName != NULL) {
								::strcpy(mFileName,defName);
							}
						}
					}
					
					mSavePanel->SetSaveText(mFileName);
					
					Lock();
					mPlot->Rescale();
					Unlock();
					
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
}
// -------------------------------------------------------------------
void
DRRegrWindow::ReDraw()
{
	Lock();
	mDispFrame->RefreshDisplays();
	mPlot->Invalidate();
	Unlock();
}
// -------------------------------------------------------------------
//	* PROTECTED METHODS
// -------------------------------------------------------------------
bool
DRRegrWindow::CheckSave()
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
DRRegrWindow::ClearStatus(EDataStatus status)
{
	DPRegression* theApp = cast_as(be_app,DPRegression);
	int32 indx = 0;
	DPData* theData = (DPData*)theApp->DataSet(indx++);
	while(theData != NULL) {
		theData->ClearStatus(status);
		theData = (DPData*)theApp->DataSet(indx++);
	}
	ReDraw();
}
// -------------------------------------------------------------------
void
DRRegrWindow::CreateOpenPanel()
{
	mOpenPanel = new BFilePanel(B_OPEN_PANEL);
	BMessenger* theMessenger = new BMessenger(this);
	mOpenPanel->SetTarget(*theMessenger);
	mFileFilter = new DRFileFilter();
	mOpenPanel->SetRefFilter(mFileFilter);
	delete theMessenger;
	BDirectory theDirectory;
	if(theDirectory.SetTo(mAppPath.Path()) == B_OK) {
		mOpenPanel->SetPanelDirectory(&theDirectory);
	}
}		
// -------------------------------------------------------------------
void
DRRegrWindow::CreateSavePanel()
{
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
// -------------------------------------------------------------------
void
DRRegrWindow::FrameResized(float width, float height)
{
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
}
// -------------------------------------------------------------------
void
DRRegrWindow::LoadRegrData()
{
}
// -------------------------------------------------------------------
void
DRRegrWindow::MenusBeginning()
{
	DPRegression* theApp = cast_as(be_app,DPRegression);
	mSaveAsItem->SetEnabled(theApp->NumSets() > 0);
	//mSaveItem->SetEnabled(mDataChanged && mFile != NULL); 
	mPrintItem->SetEnabled(theApp->NumSets() > 0);
	bool haveMarks = false;
	bool haveSuppress = false;
	bool haveDelete = false;
	int32 setNum = 0;
	const DPData* theData = theApp->DataSet(setNum++);
	while(theData != NULL) {
		if(!haveMarks)  haveMarks = theData->HasMarkedData();
		if(!haveSuppress)  haveSuppress = theData->HasSuppressedData();
		if(!haveDelete)  haveDelete = theData->HasDeletedData();
		theData = theApp->DataSet(setNum++);
	}
	mUnMarkItem->SetEnabled(haveMarks);
	mUnSuppressItem->SetEnabled(haveSuppress);
	mUnDeleteItem->SetEnabled(haveDelete);
}
// -------------------------------------------------------------------
void
DRRegrWindow::OpenFile(BMessage* theMesg)
{
	delete mFile;	// Remove old file, if any
	mFile = NULL;
	entry_ref theRef;
	if(theMesg->FindRef("refs",&theRef) == B_OK) {
		OpenFile(&theRef);
	}	// if(FindRef
	mDataChanged = false;
}		
// -------------------------------------------------------------------
void
DRRegrWindow::PositionOdometer()
{
	mOdometer->ResizeToPreferred();
	mOdometer->MoveTo(mPlot->Frame().right - (mOdometer->Bounds().Width() + 20),5);
}
// -------------------------------------------------------------------
void
DRRegrWindow::Print()
{
	if(mDocData == NULL) {
		if(SetUpPage() != B_OK) {
			return;
		}
	}
	BAutolock wLock(this);
	// Set up for printing
	BPrintJob job("Regr Plot");
	job.SetSettings(new BMessage(*mDocData));
	if(job.ConfigJob() == B_OK) {
		job.BeginJob();
		// Draw the pages
		BRect pRect = job.PrintableRect();
		// Where to put the title
		BPoint tPt(0,pRect.bottom - 20);
		// Make a square graph
		BRect oldFrame = mPlot->Bounds();
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
		mPlot->ResizeTo(pWidth,pWidth);
		mPlot->SetFrame();
		job.DrawView(mPlot,pRect,pPt);
		// Restore graph size
		mPlot->ResizeTo(oldFrame.Width(),oldFrame.Height());
		mPlot->ForceReDraw();
		// Make a title for the page
		DPRegression* theApp = cast_as(be_app,DPRegression);
		char* temp = new char[theApp->NumSets()*128 + 64];
		::strcpy(temp,"File: ");
		int32 index = 0;
		while(index < theApp->NumSets()) {
			const DPData* theData = theApp->DataSet(index);
			const char* setName = theData->SetName();
			if(setName != NULL) {
				::strcat(temp,setName);
			} else {
				::strcat(temp,"No Name");
			}
			index++;
			if(index < theApp->NumSets()) {
				::strcat(temp," + ");
			}
		}
		::strcat(temp," - Printed: ");
		time_t theTime = ::time(NULL);
		::strftime(&temp[::strlen(temp)],32,"%m/%d/%y @ %H:%M",::localtime(&theTime));
		BRect tFrame(0,0,200,18);
		BStringView* theTitle = new BStringView(tFrame,"title",temp);
		theTitle->ResizeToPreferred();
		AddChild(theTitle);
		job.DrawView(theTitle,theTitle->Bounds(),tPt);
		job.SpoolPage();	// Print page
		
		// Draw as many regression data views as required
		const float top_margin = 50;
		BPoint vPt;
		vPt.y = top_margin;
		int32 setNum = 0;
		const DRRegrView* theDView = mDispFrame->DisplayView(setNum++);
		while(theDView != NULL) {
			BView* theView = theDView->DisplayView();
			if(theView != NULL) {
				float vWidth, vHeight;
				theView->GetPreferredSize(&vWidth,&vHeight);
				vPt.x = (pRect.Width() - vWidth)/2;
				job.DrawView(theView,theView->Bounds(),vPt);
				vPt.y += vHeight + top_margin;
			}
			theDView = mDispFrame->DisplayView(setNum++);
		}	// Of while(theDView...

		// Show title here too
		job.DrawView(theTitle,theTitle->Bounds(),tPt);
		RemoveChild(theTitle);
		delete theTitle;
		delete temp;
		
		job.SpoolPage();
		// Finish...
		job.CommitJob();
	}	// Of if(job.Config.. 
}
// -------------------------------------------------------------------
void
DRRegrWindow::SaveAsFile(BMessage* theMesg)
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
				// Set default path
				mAppPath.SetTo(&theDir,NULL);
				// Build file path
				thePath.SetTo(&theDir,mFileName);
				if(thePath.InitCheck() == B_OK) {
					mFile = new DPDataFile(thePath.Path());
					mFile->Clear();
					if(mFile->InitCheck() == B_OK) {
						Save();
						// Set the MIME type for this application
						BNodeInfo theInfo;
						theInfo.SetTo(mFile);
						theInfo.SetType(regrplot_mime_type);
						theInfo.SetPreferredApp(data_plus_regr_sig);
						/*
						// Set the document icons
						status_t status;
						BBitmap* lgIcon = new BBitmap(BRect(0,0,31,31),B_CMAP8);
						lgIcon->SetBits(largeIcon_bits,sizeof(largeIcon_bits),0,B_CMAP8);
						status = theInfo.SetIcon(lgIcon,B_LARGE_ICON);
						delete lgIcon;
						completed = true;
						BBitmap* miniIcon = new BBitmap(BRect(0,0,15,15),B_CMAP8);
						miniIcon->SetBits(smallIcon_bits,sizeof(smallIcon_bits),0,B_CMAP8);
						status = theInfo.SetIcon(miniIcon,B_MINI_ICON);
						delete miniIcon;
						*/
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
DRRegrWindow::Save()
{
	if(mFile != NULL) {
		mFile->Clear();
		int32 setNum = 0;
		DPRegression* theApp = cast_as(be_app,DPRegression);
		const DPData* theData = theApp->DataSet(setNum++);
		while(theData != NULL) {
			mFile->AddData(theData,true);
			theData = theApp->DataSet(setNum++);
		}
	}
}
// -------------------------------------------------------------------
status_t
DRRegrWindow::SetUpPage()
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
void
DRRegrWindow::ShowHideDisplay(bool show)
{
	float wLimit = min_wd;
	Lock();
	// This assumes that mDispFrame has been properly sized
	SetSizeLimits(0,9999,0,9999);
	mDispScroll->SetResizingMode(B_FOLLOW_TOP_BOTTOM);
	mPlot->SetResizingMode(B_FOLLOW_TOP_BOTTOM | B_FOLLOW_LEFT);
	if(show) {
		float width, height;
		mDispFrame->GetPreferredSize(&width, &height);
		mDispScroll->ResizeTo(width + B_V_SCROLL_BAR_WIDTH,Bounds().Height() - mb_height);
		mDispFrame->MoveTo(B_ORIGIN);
		ResizeTo(mPlot->Bounds().Width() + mDispScroll->Bounds().Width() + graf_margin + 2,Bounds().Height() + 1);
		mDispScroll->MoveTo(mPlot->Frame().right + graf_margin,mPlot->Frame().top);
		wLimit += mDispScroll->Bounds().Width();
	
	} else {	// Hide the display
		ResizeTo(mPlot->Bounds().Width() + graf_margin,Bounds().Height());
		mDispScroll->MoveTo(mPlot->Frame().right,mPlot->Frame().top);
	}
	mPlot->SetResizingMode(B_FOLLOW_TOP_BOTTOM | B_FOLLOW_LEFT_RIGHT);
	mDispScroll->SetResizingMode(B_FOLLOW_TOP_BOTTOM | B_FOLLOW_RIGHT);
	
	// Set the window's size limits
	SetSizeLimits(wLimit,9999,min_ht,9999);
	PositionOdometer();
	Unlock();
}
// -------------------------------------------------------------------
void
DRRegrWindow::SwapAxes()
{
	DPRegression* theApp = cast_as(be_app,DPRegression);
	int32 theSet = 0;
	DRRegrData* theRData = (DRRegrData*)theApp->DisplayData(theSet++);
	while(theRData != NULL) {
		theRData->SwapAxes();
		theRData = (DRRegrData*)theApp->DisplayData(theSet++);
	}
	Lock();
	mPlot->Rescale();
//	mPlot->Invalidate();
	mDispFrame->RefreshDisplays();
	Unlock();
}
// -------------------------------------------------------------------
bool 
DRRegrWindow::QuitRequested()
{
	be_app->PostMessage(B_QUIT_REQUESTED);
	return(TRUE);
}
// -------------------------------------------------------------------
