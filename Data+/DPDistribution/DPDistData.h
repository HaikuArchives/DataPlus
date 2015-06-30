/* ------------------------------------------------------------------ 

	Title: DPDistData

	Description:  A wrapper class for distribution statistics
	Author :
		Stephen Wardlaw, M.D.
		MDInventions
		Highrock
		Lyme, CT 06371

	Edit History:	
		21 April, 2000
			Created
		
------------------------------------------------------------------ */
#ifndef _DPDistData
#define _DPDistData

#include "DPData.h"
#include "WSDistribution.h"

enum EDistType {
	distType_distribution,
	distType_percentile,
	distType_probit
};

class DPDistFit;	// Foward declaration
// A class containing the group data	
class DistGroup {

	public :
	
		DistGroup(float begin, float end);
		
	virtual
		~DistGroup();
		
	void
		AddMarked(rgb_color markColor);
		
	float
		Beginning() const {return mBegin;}
		
	float 
		End() const {return mEnd;}
		
	int32
		NumSets() const {return mMarkList.CountItems();}
		
	bool
		SetInfo(int32 index, int32 &number, rgb_color &color) const;
		
	int32
		Points() const {return mTotalPoints;}
		
	void
		SortColors();
		
	protected :	// Methods
	
	static int
		Compare(const void* ptr1, const void* ptr2);
	
	protected :	// Data
	
	struct SDistSet {
		int32 number;
		rgb_color color;
	};
	float		mBegin;
	float		mEnd;
	int32		mTotalPoints;
	BList		mMarkList;
	
};

class DPDistData : public BHandler {

	public :
	
		DPDistData(DPData* theData);
		
	virtual
		~DPDistData();
		
	virtual void
		CalcDistribution();
		
	rgb_color
		Color() const {return mDistColor;}
		
	void
		CreateGroups(float min, float width);
		
	const DPData*
		Data() const {return (const DPData*)mData;}
		
	int32
		DataPoints() const {return mNumPoints;}
		
	const WSDistribution*
		DistributionData() const {return (const WSDistribution*)&mDist;}
		
	const DPDistFit*
		FitFunct() const {return (const DPDistFit*)mDistFit;}
		
	const DistGroup*
		GroupInfo(int16 groupNum) const {return (const DistGroup*)mGroups.ItemAt(groupNum);}
		
	float
		Max();
		
	int32
		MaxGroup() const {return mMaxGroup;}
		
	int32
		MaxGroupSize(float width);
		
	void
		MessageReceived(BMessage* theMesg);
		
	float
		Min();
		
	int32
		NumGroups() const {return mGroups.CountItems();}
		
	bool
		Percentile(int32 index, float &val);
		
	void
		SetDataColor(rgb_color theColor) {mDistColor = theColor;}
		
	void
		SetDistType(EDistType theType) {mDistType = theType;}
		
	bool
		Value(int32 index, float &val, bool &marked);
		
	protected :	// Methods
	static int
		CompareValues(const void* sPtr1, const void* sPtr2);
		
	protected:	// Data
	
	BList					mGroups;
	DPData*				mData;
	SDistData*			mWorkData;
	DPDistFit*			mDistFit;
	WSDistribution		mDist;
	int32					mNumPoints;
	int32					mMaxGroup;
	rgb_color			mDistColor;
	EDistType			mDistType;
};	
	
#endif

