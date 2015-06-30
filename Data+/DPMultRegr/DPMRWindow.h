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
		
------------------------------------------------------------------ */ 

#ifndef _DPMRWindow
#define _DPMRWindow

#include "DPDataRec.h"
#include <StorageKit.h>
#include <InterfaceKit.h>

class DPMRWindow : public BWindow {

	public:
		DPMRWindow(BRect frame); 
		
	virtual
		~DPMRWindow();
		
	BMessage*
		ArchiveAll();
		
	virtual void
		MessageReceived(BMessage* theMessage);
		
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
		
	virtual void
		MenusBeginning();
	
	void
		OpenFile(BMessage* theMesg);
		
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
		
	virtual	bool	
		QuitRequested();
		
	void
		UnmarkAll(EDataStatus chgStatus);
		
	protected:	// Data
		BRect				mPaperRect;
		BRect				mPrintableRect;
		BFile*			mFile;
		BFilePanel*		mOpenPanel;
		BFilePanel*		mSavePanel;
		BMenuItem*		mSaveItem;
		BMenuItem*		mSaveAsItem;
		BMenuItem*		mUnmarkItem;
		BMenuItem*		mFormatItem;
		BMenuItem*		mPrintItem;
		BMenuItem*		mAllowMarkItem;
		BMenuItem*		mUnSuppressItem;
		BMenuItem*		mUnDeleteItem;
		BMessage*		mDocData;
		char				mFileName[64];
		BPath				mAppPath;
		bool				mDataChanged;
		bool				mAllowMarking;
};

#endif

