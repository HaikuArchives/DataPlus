/* ------------------------------------------------------------------ 

	Title: DPDistWindow

	Description:  The distribution graph window

	Author :
		Stephen Wardlaw, M.D.
		Medical Research & Development
		Highrock
		Lyme, CT 06371

	Edit History:	
		14 April, 2000
			Created
		
----------------------------------------------------------------- */ 
#include "DPDistWindow.h"
#include "DPDistribution.h"
#include "DPGlobals.h"
#include "DPData.h"
#include "DPGpSelect.h"
#include "WGAxisEditWind.h"
#include "WGAxisEdit.h"
#include "WU_AppUtils.h"
#include "U_Conversions.h"
#include "UGraph_Util.h"
#include <AppKit.h>
#include <SupportKit.h>

const char 	pref_file[16] 	= "distplot.prf";

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
const uint32 mesg_plotDist	= 'Pdst';
const uint32 mesg_plotPct	= 'Ppct';
const uint32 mesg_plotProb	= 'Ppro';
const uint32 mesg_fit		= 'Fdis';
const uint32 data_allowMark	= 'Damk';
const uint32 mesg_setGroup	= 'Sgrp';

const char group_name[8] = "grpnam";

// Positioning constants
const float graf_margin	= 3;	// Plot margin
const float min_ht		= 350;
const float min_wd		= 350;

// -------------------------------------------------------------------
//	* CONSTRUCTORS and DESTRUCTORS
// -------------------------------------------------------------------
DPDistWindow::DPDistWindow(BRect frame)
				: BWindow(frame, "Distribution", B_TITLED_WINDOW_LOOK,B_NORMAL_WINDOW_FEEL,0)
{
	mDataChanged = false;
	mAllowMarking = true;
	mDocData = NULL;
	mPaperRect.Set(0,0,0,0);
	mPrintableRect = mPaperRect;
	mSetsMin = 0;
	mSetsMax = 100;
	mMaxGroup = 20;
	
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
	BMenu* plotMenu = new BMenu("Plot Type");
	mPlotDist = new BMenuItem("Histogram",new BMessage(mesg_plotDist));
	plotMenu->AddItem(mPlotDist);
	mPlotPercent = new BMenuItem("Percentile", new BMessage(mesg_plotPct));
	plotMenu->AddItem(mPlotPercent);
	mPlotProbit = new BMenuItem("Std.Deviations", new BMessage(mesg_plotProb));
	plotMenu->AddItem(mPlotProbit);
	dataMenu->AddItem(plotMenu);
	mSetGpSize = new BMenuItem("Group Size" B_UTF8_ELLIPSIS, new BMessage(mesg_setGroup));
	dataMenu->AddItem(mSetGpSize);
	mFitDist = new BMenuItem("Fit Distribution", new BMessage(mesg_fit));
	dataMenu->AddItem(mFitDist);
	mFitDist->SetMarked(false);
	dataMenu->AddSeparatorItem();
	dataMenu->AddItem(new BMenuItem("Reset Axes",new BMessage(FLDS_RESET)));
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
	mSavePanel = NULL;
	::strcpy(mFileName,"Untitled");
	
	// Add the graph to the window
	BRect grafRect = Bounds();
	grafRect.top += mb_height;
	grafRect.InsetBy(graf_margin,graf_margin);
	mPlot = new DPDistPlot(grafRect);
	AddChild(mPlot);
	
	// Add the odometer
	mOdometer = new WGOdometer(mPlot,BRect(0,0,0,0),false);
	mOdometer->SetResizingMode(B_FOLLOW_RIGHT | B_FOLLOW_TOP);
	menuBar->AddChild(mOdometer);
	mOdometer->SetViewColor(menuBar->ViewColor());
	mOdometer->SetLowColor(menuBar->ViewColor());
	PositionOdometer();
	
	// Add the view for showing the distribution data, but do not size it
	BRect gFrame = mPlot->Frame();
	gFrame.OffsetBy(gFrame.Width(),0);
	gFrame.right = gFrame.left + 200;
	gFrame.bottom -= mb_height;
	mDistFrame = new DPDistFrame(gFrame);

	mDispScroll = new BScrollView("dispscroll",mDistFrame,B_FOLLOW_TOP_BOTTOM,0,false,true,B_NO_BORDER);
	AddChild(mDispScroll);
	ShowHideDisplay(false);
}

DPDistWindow::~DPDistWindow()
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
	delete mDocData;
}
// -------------------------------------------------------------------
//	* PUBLIC METHODS
// -------------------------------------------------------------------
void
DPDistWindow::AddDisplay(DPDistData* theData)
{
	Lock();
	mDistFrame->AddDisplay(theData);
	ShowHideDisplay(true);
	mPaneSel->SetEnabled(true);
	mPaneSel->SetValue(1);
	Unlock();
}		
// -------------------------------------------------------------------
BMessage*
DPDistWindow::ArchiveAll()
{
	BMessage* fileArchive = new BMessage(DPData::fld_code);
	int32 setNum = 0;
	DPDistribution* theApp = cast_as(be_app,DPDistribution);
	const DPDistData* theData = theApp->DataSet(setNum++);
	while(theData != NULL) {
		BMessage* setArchive = new BMessage();
		theData->Data()->Archive(setArchive,true);
		fileArchive->AddMessage(data_set_str,setArchive);
		theData = theApp->DataSet(setNum++);
		delete setArchive;
	}
	return fileArchive;
}
// -------------------------------------------------------------------
void
DPDistWindow::MessageReceived(BMessage* theMessage)
{
	const char* labelStr;
	WGAxisEditWind* editWind;
	DPDistribution* theApp;
	BRect frame;
	BMessenger* theMessenger;
	BMessage axisMesg;
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
			Recalculate();
			RecalcGroups();
			break;
			
		case mesg_format :
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
 			
 			SetDisplays();
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
			
		case mesg_plotDist :
			theApp = cast_as(be_app,DPDistribution);
			theApp->SetDistType(distType_distribution);
			SetPlotType();
			break;
			
		case mesg_plotPct :
			theApp =  cast_as(be_app,DPDistribution);
			theApp->SetDistType(distType_percentile);
			SetPlotType();
			break;
			
		case mesg_plotProb :
			theApp =  cast_as(be_app,DPDistribution);
			theApp->SetDistType(distType_probit);
			SetPlotType();
			break;
			
		case mesg_setGroup :
			new DPGpSelect(mGroupWidth,BRect(100,100,100,100));
			break;
			
		case mesg_fit :
			mFitDist->SetMarked(!mFitDist->IsMarked());
			theApp =  cast_as(be_app,DPDistribution);
			theApp->SetFitFlag(mFitDist->IsMarked());
			RecalcGroups();
			break;
			
		case mesg_gpSize :
			if(!theMessage->FindFloat("",&mGroupWidth) == B_OK) {
				mGroupWidth = 0;
			}
			theApp = cast_as(be_app,DPDistribution);
			theApp->SetGroupWidth(mGroupWidth);	// For use by other classes
			RecalcGroups();
			break;
			
		default:
			BWindow::MessageReceived(theMessage);
	}
}
// -------------------------------------------------------------------
void
DPDistWindow::OpenFile(const entry_ref* theRef)
{
	bool isNative = false;
	BNode theNode(theRef);
	if(theNode.InitCheck() == B_OK) {
		BNodeInfo theInfo(&theNode);
		if(theInfo.InitCheck() == B_OK) {
			char typeStr[B_MIME_TYPE_LENGTH];
			if(theInfo.GetType(typeStr) == B_OK) {
				BMimeType mType(typeStr);
				if(mType.Type() != NULL) {
					isNative = ::strcmp(mType.Type(),distplot_mime_type) == 0;
					if(!isNative && !(::strcmp(mType.Type(),dptempfile_type) == 0)) {
						BAlert* theAlert = new BAlert("OFile","Can't Open This File","OK");
						theAlert->Go();
						return;	// EXIT HERE if bad file type
					}
				} else {	
					BAlert* theAlert = new BAlert("OFile","File is Unidentified","OK");
					theAlert->Go();
					return;	// EXIT HERE if no file type
				}
			} else {
				BAlert* theAlert = new BAlert("OFile","Can't Get File's MIME Type","OK");
				theAlert->Go();
				return;	// EXIT HERE if no file type
			}	
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
				DPDistribution* theApp = cast_as(be_app,DPDistribution);
				for(int16 setNum = 0; setNum < mFile->NumSets(); setNum++) {
					DPData* theData = mFile->GetSet(setNum);
					if(theData != NULL) {
						theApp->AddDisplaySet(theData);
					}
				} 	// Of for(
				
				// If it is not a native file, we need to get the default file name from the data
				if(!isNative) {
					::strcpy(mFileName,theRef->name);
				} else {
					::strcpy(mFileName,"Untitled");
					const DPData* theData = theApp->Data(0);
					if(theData != NULL) {
						const char* defName = theData->SetName();
						if(defName != NULL) {
							::strcpy(mFileName,defName);
						}
					}
				}
				
				mSavePanel->SetSaveText(mFileName);
				Recalculate();
				
				// If we have formatting data saved, we need to get it here and apply it
				if(isNative) {
					off_t posn = mFile->PrivateData();
					mFile->Seek(posn,SEEK_SET);
					BMessage* theArchive = new BMessage();
					// Look for X-axis data
					status = theArchive->Unflatten(mFile);
					if(status == B_OK) {
						mPlot->mXAxis->SetValues(theArchive);
					}
					// Look for Y-axis data
					status = theArchive->Unflatten(mFile);
					if(status == B_OK) {
						mPlot->mYAxis->SetValues(theArchive);
					}
					// Look for group size
					status = theArchive->Unflatten(mFile);
					if(status == B_OK) {
						if(theArchive->FindFloat(group_name,&mGroupWidth) != B_OK) {
							mGroupWidth = 0;
						}
					}
					delete theArchive;
				}
				
				RecalcGroups();
				SetDisplays();
				
			} else {	// Of if(mFile->InitCheck
				BAlert* theAlert = new BAlert("OFile","Can't Open This File","OK");
				theAlert->Go();
			}
		}	// if(theInfo.InitCheck()
	}	// if(theNode.InitCheck()
}		
// -------------------------------------------------------------------
void
DPDistWindow::ReDraw()
{
	Lock();
	mDistFrame->RefreshDisplays();
	mPlot->Refresh();
	Unlock();
}
// -------------------------------------------------------------------
//	* PROTECTED METHODS
// -------------------------------------------------------------------
bool
DPDistWindow::CheckSave()
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
DPDistWindow::ClearStatus(EDataStatus status)
{
	DPDistribution* theApp = cast_as(be_app,DPDistribution);
	int32 indx = 0;
	DPData* theData = (DPData*)theApp->Data(indx++);
	while(theData != NULL) {
		theData->ClearStatus(status);
		theData = (DPData*)theApp->DataSet(indx++);
	}
	Recalculate();
	RecalcGroups();
	ReDraw();
}
// -------------------------------------------------------------------
void
DPDistWindow::CreateOpenPanel()
{
	if(mOpenPanel == NULL) {
		mOpenPanel = new BFilePanel(B_OPEN_PANEL);
		BMessenger* theMessenger = new BMessenger(this);
		mOpenPanel->SetTarget(*theMessenger);
		mFileFilter = new DPDFileFilter();
		mOpenPanel->SetRefFilter(mFileFilter);
		delete theMessenger;
		BDirectory theDirectory;
		if(theDirectory.SetTo(mAppPath.Path()) == B_OK) {
			mOpenPanel->SetPanelDirectory(&theDirectory);
		}
	}
}		
// -------------------------------------------------------------------
void
DPDistWindow::CreateSavePanel()
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
}
// -------------------------------------------------------------------
void
DPDistWindow::FrameResized(float width, float height)
{
	if(mPaneSel->Value() > 0) {	// If opened up
		float width, height;
		BScrollBar* theScroll = mDispScroll->ScrollBar(B_VERTICAL);
		mDistFrame->GetPreferredSize(&width, &height);
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
DPDistWindow::MenusBeginning()
{
	DPDistribution* theApp = cast_as(be_app,DPDistribution);
	mSaveAsItem->SetEnabled(theApp->NumSets() > 0);
	//mSaveItem->SetEnabled(mDataChanged && mFile != NULL); 
	mPrintItem->SetEnabled(theApp->NumSets() > 0);
	bool haveMarks = false;
	bool haveSuppress = false;
	bool haveDelete = false;
	int32 setNum = 0;
	const DPDistData* theData = theApp->DataSet(setNum++);
	while(theData != NULL) {
		if(!haveMarks)  haveMarks = theData->Data()->HasMarkedData();
		if(!haveSuppress)  haveSuppress = theData->Data()->HasSuppressedData();
		if(!haveDelete)  haveDelete = theData->Data()->HasDeletedData();
		theData = theApp->DataSet(setNum++);
	}
	mUnMarkItem->SetEnabled(haveMarks);
	mUnSuppressItem->SetEnabled(haveSuppress);
	mUnDeleteItem->SetEnabled(haveDelete);
	mPlotDist->SetMarked(false);
	mPlotPercent->SetMarked(false);
	mPlotProbit->SetMarked(false);
	switch(theApp->DistType()) {
		case distType_distribution :
			mPlotDist->SetMarked(true);
			break;
		case distType_percentile :
			mPlotPercent->SetMarked(true);
			break;
		case distType_probit :
			mPlotProbit->SetMarked(true);
			break;
	}
}
// -------------------------------------------------------------------
void
DPDistWindow::OpenFile(BMessage* theMesg)
{
	delete mFile;	// Remove old file, if any
	mFile = NULL;
	entry_ref theRef;
	if(theMesg->FindRef("refs",&theRef) == B_OK) {
		OpenFile(&theRef);
	}	// if(FindRef
}		
// -------------------------------------------------------------------
void
DPDistWindow::PositionOdometer()
{
	mOdometer->ResizeToPreferred();
	mOdometer->MoveTo(mPlot->Frame().right - (mOdometer->Bounds().Width() + 20),5);
}
// -------------------------------------------------------------------
void
DPDistWindow::Print()
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
		job.DrawView(mPlot,mPlot->Bounds(),pPt);
		// Restore graph size
		mPlot->ResizeTo(oldFrame.Width(),oldFrame.Height());
		mPlot->ForceReDraw();
		// Make a title for the page
		DPDistribution* theApp = cast_as(be_app,DPDistribution);
		char* temp = new char[theApp->NumSets()*128 + 64];
		::strcpy(temp,"File: ");
		int32 index = 0;
		while(index < theApp->NumSets()) {
			const DPData* theData = theApp->Data(index);
			const char* fName = theData->SetName();
			if(fName != NULL) {
				::strcat(temp,fName);
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
		job.SpoolPage();
		
		// Draw as many distribution data views as required
		const float top_margin = 50;
		BPoint vPt;
		vPt.y = top_margin;
		int32 setNum = 0;
		const DPDistView* theDView = mDistFrame->DisplayView(setNum++);
		while(theDView != NULL) {
			BView* theView = theDView->DisplayView();
			if(theView != NULL) {
				float vWidth, vHeight;
				theView->GetPreferredSize(&vWidth,&vHeight);
				vPt.x = (pRect.Width() - vWidth)/2;
				job.DrawView(theView,theView->Bounds(),vPt);
				vPt.y += vHeight + top_margin;
			}
			theDView = mDistFrame->DisplayView(setNum++);
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
// ------------------------------------------------------------------ 
void
DPDistWindow::Recalculate()
{
	BAutolock lock(this);
	int32 dispNum = 0;
	mSetsMin = 1E10;
	mSetsMax = -1E10;
	DPDistView* theView = (DPDistView*)mDistFrame->DisplayView(dispNum++);
	while(theView != NULL) {
		DPDistData* theData = (DPDistData*)theView->Data();
		theData->CalcDistribution();
		if(theData->Max() > mSetsMax) mSetsMax = theData->Max() ;
		if(theData->Min() < mSetsMin) mSetsMin = theData->Min() ;
		theView = (DPDistView*)mDistFrame->DisplayView(dispNum++);
	}
	mGroupWidth = 0;
}
// ------------------------------------------------------------------ 
void
DPDistWindow::RecalcGroups()
{
	const uint32 default_groups = 64;
	const uint32 min_gp_size = 10;
	int32 dispNum = 0;
	BAutolock lock(this);
	if(mGroupWidth == 0) {	// If not set to a specific number
	// Now, set up the limits and groups
		uint32 numGroups = default_groups;
		mGroupWidth = (mSetsMax - mSetsMin)/numGroups;
		uint32 gpMax = 0;
		while(gpMax < min_gp_size && numGroups > 0) {
			dispNum = 0;
			DPDistView* theView = (DPDistView*)mDistFrame->DisplayView(dispNum++);
			while(theView != NULL) {
				DPDistData* theData = (DPDistData*)theView->Data();
				uint32 gSize = theData->MaxGroupSize(mGroupWidth);
				if(gSize > gpMax) gpMax = gSize ;
				theView = (DPDistView*)mDistFrame->DisplayView(dispNum++);
			}
			mGroupWidth *=2.0;
			numGroups /= 2;
		}
	}
	
	// Set width and minimum to a "good number" and set max as a multiple..
	UGraph_Util::AutoScale(mPlot->mXAxis,mSetsMin,mSetsMax);
	mGroupWidth = UGraph_Util::ConvertUp(mGroupWidth);
	float theMin = mPlot->mXAxis->Min();
	//mSetsMin = mPlot->mXAxis->Min();
	//mSetsMax = mPlot->mXAxis->Max();
	DPDistribution* theApp = cast_as(be_app,DPDistribution);
	theApp->SetGroupWidth(mGroupWidth);
	
	// Transfer the information
	dispNum = 0;
	mMaxGroup = 0;
	DPDistView* theView = (DPDistView*)mDistFrame->DisplayView(dispNum++);
	while(theView != NULL) {
		DPDistData* theData = (DPDistData*)theView->Data();
		theData->CreateGroups(theMin,mGroupWidth);
		int32 maxGp = theData->MaxGroup();
		if(maxGp > mMaxGroup) mMaxGroup = maxGp ;
		theView = (DPDistView*)mDistFrame->DisplayView(dispNum++);
	}
	mMaxGroup = UGraph_Util::ConvertUp(mMaxGroup);
	mDispScroll->Invalidate();
	SetPlotType();
}
// -------------------------------------------------------------------
void
DPDistWindow::SaveAsFile(BMessage* theMesg)
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
						theInfo.SetType(distplot_mime_type);
						theInfo.SetPreferredApp(data_plus_dist_sig);
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
DPDistWindow::Save()
{
	if(mFile != NULL) {
		mFile->Clear();
		int32 setNum = 0;
		DPDistribution* theApp = cast_as(be_app,DPDistribution);
		const DPData* theData = theApp->Data(setNum++);
		while(theData != NULL) {
			mFile->AddData(theData,true);
			theData = theApp->Data(setNum++);
		}
		// Archive the axis data and save it
		off_t posn = mFile->PrivateData();
		mFile->Seek(posn,SEEK_SET);
		BMessage* theArchive = new BMessage();
		status_t status = mPlot->mXAxis->WGGraphAxis::Archive(theArchive);
		theArchive->Flatten(mFile);
		theArchive->MakeEmpty();
		status = mPlot->mYAxis->WGGraphAxis::Archive(theArchive);
		theArchive->Flatten(mFile);
		theArchive->MakeEmpty();
		theArchive->AddFloat(group_name,mGroupWidth);
		theArchive->Flatten(mFile);
		delete theArchive;
	}
}
// -------------------------------------------------------------------
void
DPDistWindow::SetDisplays()
{
	ERealFormat theFmt = mPlot->mXAxis->NumberFormat();
	int16 thePrec = mPlot->mXAxis->NumberPrecision();
	int16 frameNum = 0;
	const DPDistView* theView = mDistFrame->DisplayView(frameNum++);
	while (theView != NULL) {
		WSDistributionView* theDist = theView->DisplayView();
		theDist->SetDataFormat(theFmt,thePrec);
		theView = mDistFrame->DisplayView(frameNum++);	
	}
	mDistFrame->RefreshDisplays();
}
// -------------------------------------------------------------------
void
DPDistWindow::SetPlotType()
{
	DPDistribution* theApp = cast_as(be_app, DPDistribution);

	Lock();
	EDistType theType = theApp->DistType();
	//mPlot->mXAxis->SetScale(scale_linear);
	mPlot->mXAxis->SetMinorDivisions(1);
	//mPlot->mYAxis->SetScale(scale_linear);
	mPlot->mYAxis->SetMinorDivisions(1);
	// Set field name
	switch(theType) {
		case distType_distribution :
			UGraph_Util::AutoScale(mPlot->mYAxis,0,mMaxGroup);
			UGraph_Util::AutoScale(mPlot->mXAxis,mSetsMin,mSetsMax);
			mPlot->mYAxis->SetLabelFormat(realFmt_fixed,0);
			mPlot->SetYLabel("Number");
			break;
			
		case distType_percentile :
			UGraph_Util::AutoScale(mPlot->mXAxis,mSetsMin,mSetsMax);
			mPlot->mYAxis->SetSpan(0,100);
			mPlot->mYAxis->SetInterval(10);
			mPlot->mYAxis->SetLabelFormat(realFmt_fixed,0);
			mPlot->SetYLabel("Percentile");
			break;

		case distType_probit :
			UGraph_Util::AutoScale(mPlot->mXAxis,mSetsMin,mSetsMax);
			mPlot->mYAxis->SetSpan(-4,4);
			mPlot->mYAxis->SetInterval(.5);
			mPlot->mYAxis->SetLabelFormat(realFmt_fixed,1);
			mPlot->SetYLabel("Standard Deviations");
			break;
	}
	
	mPlot->SetXLabel("Data");
	if(theApp->Data(0) != NULL) {
		const DPData* theData = theApp->Data(0);
		if(theData != NULL) {
			const DPDataType* theType = theData->FieldType(0);
			if(theType != NULL) {
				const char* fName = theType->FieldName();
				if(fName != NULL) {
					mPlot->SetXLabel(fName);
				}
			}
		}
	}

	ReDraw();
	Unlock();
}
// -------------------------------------------------------------------
status_t
DPDistWindow::SetUpPage()
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
DPDistWindow::ShowHideDisplay(bool show)
{
	float wLimit = min_wd;
	Lock();
	// This assumes that mDistFrame has been properly sized
	SetSizeLimits(0,9999,0,9999);
	mDispScroll->SetResizingMode(B_FOLLOW_TOP_BOTTOM);
	mPlot->SetResizingMode(B_FOLLOW_TOP_BOTTOM | B_FOLLOW_LEFT);
	if(show) {
		float width, height;
		mDistFrame->GetPreferredSize(&width, &height);
		mDispScroll->ResizeTo(width + B_V_SCROLL_BAR_WIDTH,Bounds().Height() - mb_height);
		mDistFrame->MoveTo(B_ORIGIN);
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
	Unlock();
}
// -------------------------------------------------------------------
bool 
DPDistWindow::QuitRequested()
{
	be_app->PostMessage(B_QUIT_REQUESTED);
	return(TRUE);
}
// -------------------------------------------------------------------
