/* ------------------------------------------------------------------ 

	Title: DPIconPanel

	Description:  Shows and allows selection of a single data set

	Author :
		Stephen Wardlaw, M.D.
		Medical Research & Development
		Highrock
		Lyme, CT 06371

	Edit History:	
		30 August, 1999
			Created
		
------------------------------------------------------------------ */ 
#include "DPIconPanel.h"

#include "DPGlobals.h"
#include "DPData.h"
#include "DataPlus.h"

const uint32	mesg_launch		= 'mLau';
const char		str_appSig[8] 	= "appSig";
const char		str_name[8]		= "appName";
const char		str_whole[8]	= "appAll";

// -------------------------------------------------------------------
// *  Constructors & Destructor             
// -------------------------------------------------------------------
DPIconPanel::DPIconPanel(BRect frame,
			   			const char* title,
			   			uint32 mode, 
			   			uint32 flags):BView(frame,title,mode,flags)
{
	// Initialize the set of applications
	// Regression plotting
	::strcpy(mApps[panel_regr].appSig,data_plus_regr_sig);
	::strcpy(mApps[panel_regr].appName,"Regression Plot");
	mApps[panel_regr].bmpOut = 'RegO';
	mApps[panel_regr].bmpIn = 'RegI';
	mApps[panel_regr].bmpDis = 'RegD';
	mApps[panel_regr].wholeFile = false;
	// Distribution Plotting
	::strcpy(mApps[panel_dist].appSig,data_plus_dist_sig);
	::strcpy(mApps[panel_dist].appName,"Distribution Plot");
	mApps[panel_dist].bmpOut = 'DisO';
	mApps[panel_dist].bmpIn = 'DisI';
	mApps[panel_dist].bmpDis = 'DisD';
	mApps[panel_dist].wholeFile = false;
	// Polynomial plotting
	::strcpy(mApps[panel_poly].appSig,data_plus_poly_sig);
	::strcpy(mApps[panel_poly].appName,"Polynonial Plot");
	mApps[panel_poly].bmpOut = 'PolO';
	mApps[panel_poly].bmpIn = 'PolI';
	mApps[panel_poly].bmpDis = 'PolD';
	mApps[panel_poly].wholeFile = false;
	// 3D plotting
	::strcpy(mApps[panel_3d].appSig,data_plus_3dplot_sig);
	::strcpy(mApps[panel_3d].appName,"3-D Plot");
	mApps[panel_3d].bmpOut = 'De3O';
	mApps[panel_3d].bmpIn = 'De3I';
	mApps[panel_3d].bmpDis = 'De3D';
	mApps[panel_3d].wholeFile = false;
	// Multiple Regression
	::strcpy(mApps[panel_multRegr].appSig,data_plus_multregr_sig);
	::strcpy(mApps[panel_multRegr].appName,"Mult.Regression");
	mApps[panel_multRegr].bmpOut = 'MrsO';
	mApps[panel_multRegr].bmpIn = 'MrsI';
	mApps[panel_multRegr].bmpDis = 'MrsD';
	mApps[panel_multRegr].wholeFile = true;
	// Matrix sorting
	::strcpy(mApps[panel_sort].appSig,data_plus_sort_sig);
	::strcpy(mApps[panel_sort].appName,"Matrix Sort");
	mApps[panel_sort].bmpOut = 'SorO';
	mApps[panel_sort].bmpIn = 'SorI';
	mApps[panel_sort].bmpDis = 'SorD';
	mApps[panel_sort].wholeFile = true;
	// Formula entry
	::strcpy(mApps[panel_form].appSig,data_plus_calc_sig);
	::strcpy(mApps[panel_form].appName,"Data Calculations");
	mApps[panel_form].bmpOut = 'ForO';
	mApps[panel_form].bmpIn = 'ForI';
	mApps[panel_form].bmpDis = 'ForD';
	mApps[panel_form].wholeFile = true;
	// Data entry
	::strcpy(mApps[panel_enter].appSig,data_plus_entry_sig);
	::strcpy(mApps[panel_enter].appName,"Data Entry");
	mApps[panel_enter].bmpOut = 'EntO';
	mApps[panel_enter].bmpIn = 'EntI';
	mApps[panel_enter].bmpDis = 'EntD';
	mApps[panel_enter].wholeFile = true;
	
	
	const float icon_wd = 24;	// These buttons are 24X24
	const rgb_color dk_gray = {120,120,120};
	SetViewColor(dk_gray);
	BRect bFrame(0,0,icon_wd,icon_wd);
	float margin = (frame.Height() - 24)/2;
	bFrame.OffsetBy(margin,margin);
	for(int n = 0; n < max_panels; n++) {
		BMessage* theMesg = new BMessage(mesg_launch);
		theMesg->AddString(str_appSig,mApps[n].appSig);
		theMesg->AddString(str_name,mApps[n].appName);
		theMesg->AddBool(str_whole,mApps[n].wholeFile);
		mButton[n] = new WLaunchButton(bFrame,mApps[n].appName,theMesg);
		mButton[n]->SetBitmapIDs(mApps[n].bmpOut,mApps[n].bmpIn,mApps[n].bmpDis);
		mButton[n]->SetEnabled(false);
		AddChild(mButton[n]);
		bFrame.OffsetBy(icon_wd + margin,0);
	}
	ResizeTo(bFrame.left,frame.Height());
	SetEnabled(true);
}

DPIconPanel::~DPIconPanel()
{
	for(int n = 0; n < max_panels; n++) {
		delete mButton[n];
	}
}

// -------------------------------------------------------------------
// *  Public Methods             
// -------------------------------------------------------------------
void
DPIconPanel::SetButtonsFor(int16 numSets, int16 numFields)
{
	// N.B. Negative field means no data sets
	mButton[panel_regr]->SetEnabled(numFields >= 2);
	mButton[panel_dist]->SetEnabled(numFields == 1);
	mButton[panel_poly]->SetEnabled(numFields == 2);
	mButton[panel_3d]->SetEnabled(numFields >= 3);
	
	// These types only work with a single data set
	mButton[panel_multRegr]->SetEnabled(numSets == 1 && numFields >= 0);
	mButton[panel_sort]->SetEnabled(numSets == 1 && numFields >= 0);
	mButton[panel_form]->SetEnabled(numSets == 1 && numFields >= 0);
	mButton[panel_enter]->SetEnabled(numSets == 1 && true);
}
// -------------------------------------------------------------------
void
DPIconPanel::SetEnabled(bool enabled)
{
	for(int n = 0; n < max_panels; n++) {
		mButton[n]->SetEnabled(enabled);
	}
}
// -------------------------------------------------------------------
// *  Protected Methods             
// -------------------------------------------------------------------
void
DPIconPanel::AttachedToWindow()
{
	for(int n = 0; n < max_panels; n++) {
		mButton[n]->SetTarget(this);
	}
}
// -------------------------------------------------------------------
void
DPIconPanel::Draw(BRect theRect)
{
}
// -------------------------------------------------------------------
bool
DPIconPanel::IsPresent(int16 panelApp)
{
	return false;
}
// -------------------------------------------------------------------
void
DPIconPanel::LaunchApp(BMessage* theMesg)
{	
	char mesgStr[128];
	status_t status;
	const char* appStr = theMesg->FindString(str_appSig);
	if(appStr != NULL) {
		// Make sure we are installed
		BMimeType mime;
		status = mime.SetTo(appStr);
		if(status == B_OK && !mime.IsInstalled()) {
			status = mime.Install();
		}
	}	
	
	entry_ref theRef;
	if(be_roster->FindApp(appStr,&theRef) != B_OK){
		::strcpy(mesgStr,"The Tool '");
		::strcat(mesgStr,theMesg->FindString(str_name));
		::strcat(mesgStr,"' Is Not Yet Available.");
		BAlert* theAlert = new BAlert("Launch",mesgStr,"OK");
		theAlert->Go();
		return;
	}
	
	DPDataWindow* dWind = cast_as(Window(),DPDataWindow);
	// Save the data to a temporary file, either partial or complete..
	bool wholeFile = true;
	theMesg->FindBool(str_whole, &wholeFile);
	dWind->ArchiveToTemp(wholeFile);
	const BEntry* theEntry = dWind->TempEntry();
	BMessage* theData = new BMessage(launch_mesg);
	if(theEntry->Exists()) {
		// Send a referenct to that file to the application
		theEntry->GetRef(&theRef);
		theData->AddRef("refs",&theRef);
	} else {
		BAlert* theAlert = new BAlert("fileAlrt","Couldn't Find Temp Data File","OK");
		theAlert->Go();
		return;
	}
	// Launch the application			
	status = be_roster->Launch(appStr,theData);
	delete theData;
	if(status != B_OK) {
		::strcpy(mesgStr,"Couldn't Launch '");
		::strcat(mesgStr,theMesg->FindString(str_name));
		::strcat(mesgStr,"'.");
		BAlert* theAlert = new BAlert("toolAlrt",mesgStr,"OK");
		theAlert->Go();
	}	
}
// -------------------------------------------------------------------
void
DPIconPanel::MessageReceived(BMessage* theMessage)
{
 	switch ( theMessage->what ){
 	
    	case  mesg_launch :
    		LaunchApp(theMessage);
    		break;
    		
		default :
			this->BView::MessageReceived(theMessage);
	}
}
// -------------------------------------------------------------------

		
