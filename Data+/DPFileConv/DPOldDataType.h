/* ------------------------------------------------------------------ 

	Title: DPOldDataType

	Description:  Field definitions and methods for DPOldDataFld

	Author :
		Stephen Wardlaw, M.D.
		Medical Research & Development
		Highrock
		Lyme, CT 06371

	Edit History:	
		26 August, 1999
			Created
		
------------------------------------------------------------------ */ 

#ifndef _DPOldDataType
#define _DPOldDataType

#include <SupportKit.h>
#include "URealFmt_Utils.h"
#include "DPOldDataRec.h"
#include "DPDataType.h"

class DPOldDataRec;	// Forward declaration

class DPOldDataType : public BArchivable {

	public :
		enum {
			fld_code = 'DPTy',
			max_fld_name = 16,
			max_enum_name = 12,
			max_result_len = 12
		};
		
	static BArchivable*	// Static creator
		Instantiate(BMessage* theArchive);
	
		DPOldDataType(BMessage* theArchive);
		
		DPOldDataType(DPOldDataType &theType);

		DPOldDataType(type_code dataType);
		
	virtual
		~DPOldDataType();
		
	void
		AddEnum(const char* itemStr, int32 val) const;
		
	virtual status_t
		Archive(BMessage* theArchive,bool deep = true) const;
		
	const char*
		DataString(const DPOldDataRec*, int16 fldNum);
		
	type_code
		DataType() {return mType;}
		
	const char*
		FieldName(){return (const char*)mFldName;}
		
	void
		GetRealFormat(ERealFormat &fmt, int16 &precision) const {fmt = mRealFmt; precision = mRealPrec;}
		
	void
		SetFieldName(const char* title);
		
	void
		SetRealFormat(ERealFormat format, int16 prec);

	protected :	// Data
		char 		mFldName[max_fld_name + 1];
		char		mRsltStr[max_result_len + 1];
		type_code	mType;
		BList*		mEnumStr;
		BList*		mEnumVal;		
		int16		mRealPrec;
		ERealFormat	mRealFmt;
};

#endif
