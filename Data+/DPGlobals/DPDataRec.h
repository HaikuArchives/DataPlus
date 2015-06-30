/* ------------------------------------------------------------------ 

	Title: DPDataRec

	Description:  The data records for Data+

	Author :
		Stephen Wardlaw, M.D.
		Medical Research & Development
		Highrock
		Lyme, CT 06371

	Edit History:	
		28 March, 2001
			Added file archiving and changed data structure
		26 August, 1999
			Modified as a BArchivable
		16 August, 1999
			Created
		
------------------------------------------------------------------ */ 

#ifndef _DPDataRec
#define _DPDataRec

#include <SupportKit.h>

typedef int64 record_key;
const record_key invalid_key = 0;

enum EDataStatus{
	data_normal,
	data_marked,
	data_suppress,
	data_delete
};

class DPDataRec : public BArchivable {

	public:	// Data structures
	
		enum {
			fld_code = 'DPDr',
			max_fld_name = 15
		};
	
	union UFldData {
		bigtime_t	timeVal;
		double		doubleVal;
		float			realVal;
		int32			longVal;
		int16			intVal;	
		char			charVal;
		bool			boolVal;
	};
	
	struct SRecFld {
		UFldData		fldData;
		bool			fldValid;
		bool			useFld;
		char			bytePad[4];
	};

	struct SRecData {
		record_key	key;
		rgb_color	markColor;
		char 			recName[max_fld_name + 1];
		SRecFld*	data;
		int16			numFlds;
		EDataStatus	status;
		bool			valid;
		char			bytePad[4];
	};
	
	public :	// Methods
	
	static record_key
		MakeKey();
		
	static BArchivable*
		Instantiate(BMessage* theArchive);
		
	static DPDataRec*	// Static creator from file
		Instantiate(BDataIO* theDataObj);

		// Constructor from a BMessage
		DPDataRec(BMessage* theArchive);
		
		// Constructor from a BData
		DPDataRec(BDataIO* theDataObj);
		
		// Deep copy constructor
		DPDataRec(const DPDataRec &theRec);
		
		// Selected field constructor
		DPDataRec(const DPDataRec &theRec, int16 xFld, int16 yFld, int16 zFld);
		
		// Blank constructor
		DPDataRec(int16 numFlds);
		
	virtual
		~DPDataRec();
		
	status_t
		Archive(BMessage* theArchive, bool deep = true) const;
		
	status_t
		Archive(BMessage* theArchive, int16 xFld, int16 yFld, int16 zFld) const;

	status_t
		Archive(BDataIO* theDataObj) const;
		
	status_t
		Archive(BDataIO* theDataObj, int16 xFld, int16 yFld, int16 zFld) const;
		
	void
		AddField();
		
	bool
		CanUseFld(int16 fldNum) const;
		
	EDataStatus
		DataStatus() const {return mData.status;}
		
	const SRecData*
		GetData() const {return (const SRecData*) &mData;}
		
	const SRecFld*
		GetDataField(int16 fldNum) const {if(fldNum >= 0 && fldNum < mData.numFlds) {
		return (const SRecFld*)&mData.data[fldNum];
	} else {
		return NULL;
	};}
		
	bool
		InitCheck() const {return mData.valid;}
		
	record_key
		Key() const {return mData.key;}
		
	rgb_color
		MarkColor() const {return mData.markColor;}
		
	int16
		NumFields() const {return mData.numFlds;}
		
	const char*
		RecordName() const {return (const char*)mData.recName;}
		
	void
		SetDataStatus(EDataStatus theStatus) {mData.status = theStatus;}
		
	void
		SetFieldValue(int16 fldNum, float val);
		
	void
		SetFieldValue(int16 fldNum, double val);
		
	void
		SetFieldValue(int16 fldNum, int32 val);
		
	void
		SetFieldValue(int16 fldNum, int16 val);
		
	void
		SetKey(record_key keyVal) {mData.key = keyVal;}
		
	void
		SetMarkColor(rgb_color color) {mData.markColor = color;}
		
	void
		SetRecordName(const char* rName);

	void
		SetUse(int16 fldNum, bool use);
		
	bool
		ValidField(int16 fldNum) const;
		
	bool
		Value(int16 fldNum, bigtime_t &val) const;

	bool
		Value(int16 fldNum, float &val) const;
		
	bool
		Value(int16 fldNum, double &val) const;
		
	bool
		Value(int16 fldNum, int32 &val) const;

	bool
		Value(int16 fldNum, int16 &val) const;

	protected :	// Methods
	void
		Initialize();
		
	protected :	// Data
	
	SRecData		mData;
};

#endif
