/* ------------------------------------------------------------------ 

	Title: DPGlobals

	Description:  Global declarations for Graph_It

	Author :
		Stephen Wardlaw, M.D.
		Medical Research & Development
		Highrock
		Lyme, CT 06371

	Edit History:	
		12 April, 2001
			Better organization of MIME types
		14 August, 1999
			Created
		
------------------------------------------------------------------ */ 

#ifndef _DPGlobals
#define _DPGlobals

#include <AppKit.h>

const char data_plus_sig[64] = "application/x-DataPlus";
const char data_plus_regr_sig[64] = "application/x-DataPlus.RegrPlot";
const char data_plus_poly_sig[64] = "application/x-DataPlus.PolyPlot";
const char data_plus_dist_sig[64] = "application/x-DataPlus.DistPlot";
const char data_plus_3dplot_sig[64] = "application/x-DataPlus.3DPlot";
const char data_plus_multregr_sig[64] = "application/x-DataPlus.MultRegr";
const char data_plus_sort_sig[64] = "application/x-DataPlus.Sort";
const char data_plus_calc_sig[64] = "application/x-DataPlus.Calcs";
const char data_plus_entry_sig[64] = "application/x-DataPlus.Entry";
const char data_plus_marker_sig[64] = "application/x-DPMarker";
const char data_plus_mark_client_sig[32] = "DataPlus";

// MIME types for data files
const char old_mime_type[32] = "application/x-dataplus-native";
const char native_mime_type[32] = "application/x-dataplus.V2";
const char dptempfile_type[32] = "application/x-dptempfile";
const char regrplot_mime_type[32] = "application/x-regrplot";
const char distplot_mime_type[32] = "application/x-distplot";
const char mr_mime_type[32] = "application/x-multregr";
const char text_mime_type[16] = "text/plain";
const char style_edit_app[32] = "application/x-vnd.Be-STEE";

// Temporary data file for DPConsole
const char dp_temp_file[8] = "dptemp";

// Tool launch message
const uint32 launch_mesg	= 'DPLa';

// Preference files
const char 	pref_dir[16] 		= "DataPlus_Prefs";
const char	name_posn[16]		= "windposn";
const char	name_dataDir[16]	= "datadir";

// Menu Constants
const float 	mb_height 		= 17;
const uint32 	B_MENU_OPEN		= 'BMop';
const uint32 	B_MENU_CLOSE	= 'BMcl';
const uint32 	B_MENU_SAVE		= 'BMsv';
const uint32 	B_MENU_SAVEAS	= 'BMsa';

// Data messages
const char		data_set_str[16] 	= "dpdataset";
const uint32 	FLDS_RESET		= 'fRst';
const uint32 	select_data		= 'DatS';
const uint32 	comb_data		= 'Cdat';
const char		select_str[8]	= "selNum";
const uint32 	clear_data		= 'DatC';

// Message sent to mark/suppress/delete data
const uint32	mesg_mark		= 'Mmrk';
const char		mark_col_name[8] = "mkcolor";
const char		status_name[8]	= "mkstat";
const char		keys_names[8]	= "reckeys";

// Messages sent to change a data set characteristic
const uint32	refresh_disp	= 'Rdat';
const uint32	set_display		= 'SDsp';
const char		disp_set_name[8] = "dispset";
const char		disp_mark_name[8] = "dismark";
const char		disp_line_name[8]	= "dpline";
// Use 'mark_col_name' from above for color

const int16 max_data_sets = 8;	// Max # of data sets in console

#endif

