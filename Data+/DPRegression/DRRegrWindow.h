/* ------------------------------------------------------------------ 

	Title: DRRegrWindow

	Description:  The graph window

	Author :
		Stephen Wardlaw, M.D.
		Medical Research & Development
		Highrock
		Lyme, CT 06371

	Edit History:	
		27 March, 2001
			Added file filter and new file structure
		08 Dec, 2000
			Added preference file
		29 November, 1999
			Modified for single-window
		14 August, 1999
			Created
		
------------------------------------------------------------------ */ 

#ifndef _DRRegrWindow
#define _DRRegrWindow
#include "DRRegrPlot.h"
#include "DRDisplayFrame.h"
#include "DRFileFilter.h"
#include "DPDataFile.h"
#include "WUPrefFile.h"
#include <StorageKit.h>

class DRRegrWindow : public BWindow {

	public:
		DRRegrWindow(BRect frame); 
		
	virtual
		~DRRegrWindow();
		
	void
		AddDisplay(DRRegrData* theData);
		
	bool
		AllowMarking() const {return mAllowMarking;}
		
	virtual void
		MessageReceived(BMessage* theMessage);
		
	void
		OpenFile(const entry_ref* theRef, bool typeCheck = true);
		
	void
		ReDraw();
		
	protected :	// Methods
	
	bool
		CheckSave();
		
	void
		ClearStatus(EDataStatus status);
		
	void
		CreateOpenPanel();
		
	void
		CreateSavePanel();
		
	virtual void
		FrameResized(float width, float height);
		
	void
		LoadRegrData();
		
	virtual void
		MenusBeginning();
	
	void
		OpenFile(BMessage* theMesg);
		
	void
		PositionOdometer();
		
	void
		Print();
		
	void
		Save();
		
	void
		SaveAsFile(BMessage* theMesg);
		
	status_t
		SetUpPage();
		
	void
		ShowHideDisplay(bool show);
		
	void
		SwapAxes();
		
	virtual	bool	
		QuitRequested();
		
	void
		UnmarkAll(EDataStatus chgStatus);
		
	protected:	// Data
		BRect				mPaperRect;
		BRect				mPrintableRect;
		DRRegrPlot*		mPlot;
		DRDisplayFrame*	mDispFrame;
		BScrollView*	mDispScroll;
		DPDataFile*		mFile;
		BFilePanel*		mOpenPanel;
		BFilePanel*		mSavePanel;
		//BMenuItem*		mSaveItem;
		BMenuItem*		mSaveAsItem;
		BMenuItem*		mUnmarkItem;
		BMenuItem*		mFormatItem;
		BMenuItem*		mPrintItem;
		BMenuItem*		mAllowMarkItem;
		BMenuItem*		mUnMarkItem;
		BMenuItem*		mUnSuppressItem;
		BMenuItem*		mUnDeleteItem;
		WUPrefFile*		mPrefFile;
		DRFileFilter*	mFileFilter;
		BPath			mAppPath;
		WGOdometer*		mOdometer;
		BPictureButton*	mPaneSel;
		BMessage*		mDocData;
		char				mFileName[64];
		bool				mDataChanged;
		bool				mAllowMarking;
};

#endif

