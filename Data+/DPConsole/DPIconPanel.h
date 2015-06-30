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

#ifndef _DPIconPanel
#define _DPIconPanel

#include "DPData.h"
#include <InterfaceKit.h>
#include "WLaunchButton.h"

// A structure to hold launchable app info
struct SAppSet {
	char appSig[B_MIME_TYPE_LENGTH];
	char appName[32];
	int32 bmpOut;
	int32 bmpIn;
	int32 bmpDis;
	bool wholeFile;
};

class DPIconPanel : public BView{

	public:
		DPIconPanel(BRect frame,
					const char* title	= "DPIconPanel",
					uint32 mode			= B_FOLLOW_NONE,
					uint32 flags		= B_WILL_DRAW + 
										  B_FRAME_EVENTS); 
	virtual
		~DPIconPanel();
		
	void
		SetEnabled(bool enabled);
		
	void
		SetButtonsFor(int16 numSets, int16 numFields);
		
		
	protected: 	// Methods
	
	void
		AttachedToWindow();
		
	virtual void
		Draw(BRect theRect);
		
	bool
		IsPresent(int16 panelApp);
		
	void
		LaunchApp(BMessage* theMesg);
		
	virtual void
		MessageReceived(BMessage* theMessage);
		
	protected:	// Data
	
	enum {
		panel_dist,
		panel_regr,
		panel_poly,
		panel_3d,
		panel_multRegr,
		panel_sort,
		panel_form,
		panel_enter,
		max_panels
	};
	SAppSet			mApps[max_panels];
	WLaunchButton*	mButton[max_panels];
};

#endif

