/* ------------------------------------------------------------------ 

	Title: DPDistWindow

	Description:  The distribution graph window

	Author :
		Stephen Wardlaw, M.D.
		Medical Research & Development
		Highrock
		Lyme, CT 06371

	Edit History:	
		13 April, 2001
			File and data restructuring
		08 Dec, 2000
			Added preference file
		14 April, 2000
			Created
		
------------------------------------------------------------------ */ 

#ifndef _DPDistWindow
#define _DPDistWindow

#include "DPData.h"
#include "DPDistPlot.h"
#include "DPDistFrame.h"
#include "DPDataFile.h"
#include "DPDFileFilter.h"
#include "WGOdometer.h"
#include "WUPrefFile.h"
#include <StorageKit.h>

class DPDistWindow : public BWindow {

	public:
		DPDistWindow(BRect frame); 
		
	virtual
		~DPDistWindow();
		
	BMessage*
		ArchiveAll();
		
	void
		AddDisplay(DPDistData* theData);
		
	bool
		AllowMarking() const {return mAllowMarking;}
		
	virtual void
		MessageReceived(BMessage* theMessage);
		
	void
		OpenFile(const entry_ref* theRef);
	
	void
		ReDraw();
		
	bool
		UseFit() const {return mFitDist->IsMarked();}
		
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
		
	virtual void
		MenusBeginning();
		
	void
		OpenFile(BMessage* theMesg);
		
	void
		PositionOdometer();
		
	void
		Print();
		
	void
		Recalculate();
		
	void
		RecalcGroups();
		
	void
		Save();
		
	void
		SaveAsFile(BMessage* theMesg);
		
	void
		SetDisplays();
		
	void
		SetPlotType();
		
	status_t
		SetUpPage();
		
	void
		ShowHideDisplay(bool show);
		
	virtual	bool	
		QuitRequested();
		
	void
		UnmarkAll(EDataStatus chgStatus);
		
	protected:	// Data
		BRect				mPaperRect;
		BRect				mPrintableRect;
		DPDistPlot*		mPlot;
		DPDistFrame*	mDistFrame;
		BScrollView*	mDispScroll;
		DPDataFile*		mFile;
		BFilePanel*		mOpenPanel;
		BFilePanel*		mSavePanel;
		DPDFileFilter*	mFileFilter;
		//BMenuItem*		mSaveItem;
		BMenuItem*		mSaveAsItem;
		BMenuItem*		mUnmarkItem;
		BMenuItem*		mFormatItem;
		BMenuItem*		mPrintItem;
		BMenuItem*		mAllowMarkItem;
		BMenuItem*		mUnMarkItem;
		BMenuItem*		mUnSuppressItem;
		BMenuItem*		mUnDeleteItem;
		BMenuItem*		mPlotDist;
		BMenuItem*		mPlotPercent;
		BMenuItem*		mPlotProbit;
		BMenuItem*		mSetGpSize;
		BMenuItem*		mFitDist;
		WUPrefFile*		mPrefFile;
		BPictureButton*	mPaneSel;
		WGOdometer*		mOdometer;
		BMessage*		mDocData;
		char				mFileName[64];
		BPath				mAppPath;
		float 			mSetsMax;
		float 			mSetsMin;
		float 			mGroupWidth;
		int32			mMaxGroup;
		bool				mDataChanged;
		bool				mAllowMarking;
};

#endif

