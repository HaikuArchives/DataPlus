/* ------------------------------------------------------------------ 	Title: 		LMSmoothingFiltersT	Description: 		Template class of digital smoothing filters	Author :		Stephen Wardlaw, M.D.		Yale University School of Medicine		20 York St.		New Haven, CT  06504	Edit History:			10 February, 1997			Adapted to template class			11 March, 1995			Created------------------------------------------------------------------ */ #pragma oncetemplate <class Ary1, class Ary2> class LMSmoothingFiltersT {	public:		LMSmoothingFiltersT();		virtual		~LMSmoothingFiltersT();			// This is a simple filter which averages the data over (span/2)	// positions on either side of the index datum.  The filter handles	// the ends of the array by mirroring the points.  This preserves any	// constant slope but can cause complications if the span is large	// and the slope is not uniform.	// 'span' must be an odd number	void			AverageData(Ary1* dArray, int aryLen, int span);	// These filters use a separate destination filter, instead	// of modifying the source		void			AverageData(Ary1* const sourceArray, Ary2* destArray, int aryLen, int span);	// This filter assumes a gaussian distribution of the signal about the	// index datum and truncates the datum to the expected value.		// 'span' must be an odd number	void		GaussFilter(Ary1* dArray, int aryLen, int span);			// These filters use a separate destination filter, instead	// of modifying the source		void		GaussFilter(Ary1* const sourceArray, Ary2* destArray, int aryLen, int span);			};