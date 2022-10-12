scriptname JGWD_MCM extends SKI_ConfigBase

;Bools
int MASS_INDEPENDENT = 0
int BOOL_COUNT = 1

int[] bIDs
bool[] bDefaults
bool[] bValues
string[] bInfo

;Floats
int INTENSITY = 0
int OSC01_FORCE = 1
int OSC01_FREQ = 2
int OSC02_FORCE = 3
int OSC02_FREQ = 4
int OSC02_SPAN = 5
int NOISE = 6
int ALTITUDE_FACTOR = 7
int FLOAT_COUNT = 8

int[] ids
float[] values
float[] defaults
float[] ranges
float[] precisions
string[] formats
string[] infos

event OnConfigInit()
	initV1()
	initV2()
endevent

event OnOptionDefault(int id)
	int i = BOOL_COUNT
	while i
		i -= 1
		if id == bIDs[i]
			bValues[i] = bDefaults[i]
			SetToggleOptionValue(id, bValues[i])
			SetBool(i, bValues[i])
			return
		endif
	endwhile
	i = FLOAT_COUNT
	while i
		i -= 1
		if id == ids[i]
			values[i] = defaults[i]
			SetSliderOptionValue(id, values[i], formats[i])
			SetFloat(i, values[i])
			return
		endif
	endwhile
endevent

event OnOptionHighlight(int id)
	int i = BOOL_COUNT
	while i
		i -= 1
		if id == bIDs[i]
			SetInfoText(bInfo[i])
			return
		endif
	endwhile
	i = FLOAT_COUNT
	while i
		i -= 1
		if id == ids[i]
			SetInfoText(infos[i])
			return
		endif
	endwhile
endevent

event OnOptionSelect(int id)
	if id == bIDs[MASS_INDEPENDENT]
		bValues[MASS_INDEPENDENT] = !bValues[MASS_INDEPENDENT]
		SetToggleOptionValue(id, bValues[MASS_INDEPENDENT])
		SetBool(MASS_INDEPENDENT, bValues[MASS_INDEPENDENT])
	endif
endevent

event OnOptionSliderAccept(int id, float val)
	int i = FLOAT_COUNT
	while i
		i -= 1
		if id == ids[i]
			values[i] = val
			SetSliderOptionValue(id, val, formats[i])
			SetFloat(i, val)
			return
		endif
	endwhile
endevent

event OnOptionSliderOpen(int id)
	int i = FLOAT_COUNT
	while i
		i -= 1
		if id == ids[i]
			SetSliderDialogStartValue(values[i])
			SetSliderDialogDefaultValue(defaults[i])
			SetSliderDialogRange(0.0, ranges[i])
			SetSliderDialogInterval(precisions[i])
			return
		endif
	endwhile
endevent

event OnPageReset(string name)
	;Temporary solution, since we didn't add an autoupdater in v1.
	;Will be removed in a future version.
	CheckVersion()
	
	;Check for external edits to the config file
	ReadConfig()
	
	if name == ""
		SetCursorFillMode(TOP_TO_BOTTOM)
		
		ids[INTENSITY] = AddSliderOption("$Intensity", values[INTENSITY], formats[INTENSITY])
		ids[ALTITUDE_FACTOR] = AddSliderOption("$Altitude factor", values[ALTITUDE_FACTOR], formats[ALTITUDE_FACTOR])
		ids[NOISE] = AddSliderOption("$Noise", values[NOISE], formats[NOISE])
		bIDs[MASS_INDEPENDENT] = AddToggleOption("$Mass independent", bValues[MASS_INDEPENDENT])
		
		AddHeaderOption("$Intensity fluctuation")
		ids[OSC01_FORCE] = AddSliderOption("$Magnitude", values[OSC01_FORCE], formats[OSC01_FORCE])
		ids[OSC01_FREQ] = AddSliderOption("$Frequency", values[OSC01_FREQ], formats[OSC01_FREQ])
		
		AddHeaderOption("$Directional fluctuation")
		ids[OSC02_FORCE] = AddSliderOption("$Magnitude", values[OSC02_FORCE], formats[OSC02_FORCE])
		ids[OSC02_FREQ] = AddSliderOption("$Frequency", values[OSC02_FREQ], formats[OSC02_FREQ])
		ids[OSC02_SPAN] = AddSliderOption("$Span", values[OSC02_SPAN], formats[OSC02_SPAN])
	endif
endevent

event OnVersionUpdate(int newVersion)
	if CurrentVersion < 2 && newVersion >= 2
		initV2()
	endif
endevent

bool function GetBoolDefault(int id) global native
bool function GetBool(int id) global native
function SetBool(int id, bool val) global native

float function GetFloatDefault(int id) global native
float function GetFloat(int id) global native
function SetFloat(int id, float val) global native

int function GetVersion()
	return 2
endfunction

function InitV1()
	ids = new int[8]
	values = new float[8]
	defaults = new float[8]
	ranges = new float[8]
	precisions = new float[8]
	formats = new string[8]
	
	ranges[INTENSITY] = 100.0
	ranges[OSC01_FORCE] = 1.0
	ranges[OSC01_FREQ] = 20.0
	ranges[OSC02_FORCE] = 1.0
	ranges[OSC02_FREQ] = 20.0
	ranges[OSC02_SPAN] = 3.0
	ranges[NOISE] = 10.0
	ranges[ALTITUDE_FACTOR] = 10.0
	
	precisions[INTENSITY] = 0.1
	precisions[OSC01_FORCE] = 0.01
	precisions[OSC01_FREQ] = 0.01
	precisions[OSC02_FORCE] = 0.01
	precisions[OSC02_FREQ] = 0.01
	precisions[OSC02_SPAN] = 0.1
	precisions[NOISE] = 0.1
	precisions[ALTITUDE_FACTOR] = 0.1
	
	formats[INTENSITY] = "{1}"
	formats[OSC01_FORCE] = "{2}"
	formats[OSC01_FREQ] = "{2}"
	formats[OSC02_FORCE] = "{2}"
	formats[OSC02_FREQ] = "{2}"
	formats[OSC02_SPAN] = "{1}"
	formats[NOISE] = "{1}"
	formats[ALTITUDE_FACTOR] = "{1}"
endfunction

function InitV2()
	ModName = "$SMP Wind"
	
	bIDs = new int[1]
	bDefaults = new bool[1]
	bValues = new bool[1]
	bInfo = new string[1]
	
	infos = new string[8]
	
	bInfo[MASS_INDEPENDENT] = "$INFO_MASS_INDEPENDENT"
	
	infos[INTENSITY] = "$INFO_INTENSITY"
	infos[OSC01_FORCE] = "$INFO_MAGNITUDE"
	infos[OSC01_FREQ] = "$INFO_FREQUENCY"
	infos[OSC02_FORCE] = "$INFO_MAGNITUDE"
	infos[OSC02_FREQ] = "$INFO_FREQUENCY"
	infos[OSC02_SPAN] = "$INFO_SPAN"
	infos[NOISE] = "$INFO_NOISE"
	infos[ALTITUDE_FACTOR] = "$INFO_ALTITUDE_FACTOR"
endfunction

function ReadConfig()
	int i = BOOL_COUNT
	while i
		i -= 1
		bValues[i] = GetBool(i)
		bDefaults[i] = GetBoolDefault(i)
	endwhile
	i = FLOAT_COUNT
	while i
		i -= 1
		values[i] = GetFloat(i)
		defaults[i] = GetFloatDefault(i)
	endwhile
endfunction
