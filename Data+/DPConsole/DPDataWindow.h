/* ------------------------------------------------------------------ 

	Title: DPDataWindow

	Description:  The window for managing the application's data

	Author :
		Stephen Wardlaw, M.D.
		Medical Research & Development
		Highrock
		Lyme, CT 06371

	Edit History:	
		24 August, 1999
			Created
		
------------------------------------------------------------------ */ 

#ifndef _DPDataWindow
#define _DPDataWindow

#include "DPDataView.h"
#include "DPData.h"
#include "DPDataFile.h"
#include "WUPrefFile.h"
#include "DPFileFilter.h"
#include <StorageKit.h>

class DPDataWindow : public BWindow {

	public:
		DPDataWindow(BRect frame);
		
	virtual
		~DPDataWindow();
	
	void	
		AddData(entry_ref* theRef) {mDataView->AddDataSet(theRef);}
		
	bool
		AllowMarking() {return mAllowMarking;}
		
	BMessage*
		ArchiveAll(bool deep);
		
	void
		ArchiveToTemp(bool deep = true);
		
	void	
		MarkData(BMessage* theMesg);
		
	virtual void
		MessageReceived(BMessage* theMessage);
		
	const BEntry*
		TempEntry() {return (const BEntry*)&mTempFileRef;}
		
	protected :	// Methods
	
	bool
		CheckSave();
		
	void
		ClearStatus(EDataStatus status);
		
	void
		ConstructOpenPanel();
	
	void
		ConstructSavePanel();
	
	virtual void
		MenusBeginning();
		
	void
		OpenFile(BMessage* theMesg);
		
	void
		Save(DPDataFile* theFile,bool deep = true);
		
	void
		SaveAsFile(BMessage* theMesg);
		
	void
		SaveAsTextFile(BFile* theFile);
		
	virtual	bool	
		QuitRequested();
		
	protected:	// Data
		DPDataView*		mDataView;
		DPDataFile*		mFile;
		BFilePanel*		mOpenPanel;
		BFilePanel*		mSavePanel;
		BMenuItem*		mSaveItem;
		BMenuItem*		mSaveAsItem;
		BMenuItem*		mExportItem;
		BMenuItem*		mClearItem;
		BMenuItem*		mRemoveItem;
		BMenuItem*		mAllowMarkItem;
		BMenuItem*		mUnMarkItem;
		BMenuItem*		mUnSuppressItem;
		BMenuItem*		mUnDeleteItem;
		BPopUpMenu*		mSaveType;
		DPFileFilter*	mFileFilter;
		WUPrefFile*		mPrefFile;
		char			mFileName[64];
		BPath			mAppPath;
		BEntry			mTempFileRef;
		bool			mDataChanged;
		bool			mAllowMarking;
};

#endif

