scriptname JGWD_MCM extends SKI_ConfigBase

int GLOBALFORCE = 0
int OSC01FORCE = 1
int OSC01FREQ = 2
int OSC02FORCE = 3
int OSC02FREQ = 4
int OSC02SPAN = 5
int NOISEFORCE = 6
int HEIGHTFACTOR = 7
int COUNT = 8

int[] ids
float[] values
float[] defaults
float[] ranges
float[] precisions
string[] formats

float function GetFloatDefault(int id) global native
float function GetFloat(int id) global native
function SetFloat(int id, float f) global native

int function GetVersion()
	return 1
endfunction

event OnConfigInit()
	ids = new int[8]
	values = new float[8]
	defaults = new float[8]
	ranges = new float[8]
	precisions = new float[8]
	formats = new string[8]
	
	ranges[GLOBALFORCE] = 100.0
	ranges[OSC01FORCE] = 1.0
	ranges[OSC01FREQ] = 20.0
	ranges[OSC02FORCE] = 1.0
	ranges[OSC02FREQ] = 20.0
	ranges[OSC02SPAN] = 3.0
	ranges[NOISEFORCE] = 10.0
	ranges[HEIGHTFACTOR] = 10.0
	
	precisions[GLOBALFORCE] = 0.1
	precisions[OSC01FORCE] = 0.01
	precisions[OSC01FREQ] = 0.01
	precisions[OSC02FORCE] = 0.01
	precisions[OSC02FREQ] = 0.01
	precisions[OSC02SPAN] = 0.1
	precisions[NOISEFORCE] = 0.1
	precisions[HEIGHTFACTOR] = 0.1
	
	formats[GLOBALFORCE] = "{1}"
	formats[OSC01FORCE] = "{2}"
	formats[OSC01FREQ] = "{2}"
	formats[OSC02FORCE] = "{2}"
	formats[OSC02FREQ] = "{2}"
	formats[OSC02SPAN] = "{1}"
	formats[NOISEFORCE] = "{1}"
	formats[HEIGHTFACTOR] = "{1}"
endevent

event OnOptionDefault(int id)
	int i = COUNT
	while i
		i -= 1
		if id == ids[i]
			values[i] = defaults[i]
			SetFloat(i, values[i])
			SetSliderOptionValue(id, values[i], formats[i])
		endif
	endwhile
endevent

event OnOptionSliderAccept(int id, float val)
	int i = COUNT
	while i
		i -= 1
		if id == ids[i]
			values[i] = val
			SetFloat(i, val)
			SetSliderOptionValue(id, val, formats[i])
		endif
	endwhile
endevent

event OnOptionSliderOpen(int id)
	int i = COUNT
	while i
		i -= 1
		if id == ids[i]
			SetSliderDialogStartValue(values[i])
			SetSliderDialogDefaultValue(defaults[i])
			SetSliderDialogRange(0.0, ranges[i])
			SetSliderDialogInterval(precisions[i])
		endif
	endwhile
endevent

event OnPageReset(string name)
	if name == ""
		int i = COUNT
		while i
			i -= 1
			values[i] = GetFloat(i)
			defaults[i] = GetFloatDefault(i)
		endwhile
		
		SetCursorFillMode(TOP_TO_BOTTOM)
		
		ids[GLOBALFORCE] = AddSliderOption("Intensity", values[GLOBALFORCE], formats[GLOBALFORCE])
		ids[HEIGHTFACTOR] = AddSliderOption("Altitude factor", values[HEIGHTFACTOR], formats[HEIGHTFACTOR])
		ids[NOISEFORCE] = AddSliderOption("Noise", values[NOISEFORCE], formats[NOISEFORCE])
		
		AddHeaderOption("Intensity fluctuation")
		ids[OSC01FORCE] = AddSliderOption("Magnitude", values[OSC01FORCE], formats[OSC01FORCE])
		ids[OSC01FREQ] = AddSliderOption("Frequency", values[OSC01FREQ], formats[OSC01FREQ])
		
		AddHeaderOption("Directional fluctuation")
		ids[OSC02FORCE] = AddSliderOption("Magnitude", values[OSC02FORCE], formats[OSC02FORCE])
		ids[OSC02FREQ] = AddSliderOption("Frequency", values[OSC02FREQ], formats[OSC02FREQ])
		ids[OSC02SPAN] = AddSliderOption("Span", values[OSC02SPAN], formats[OSC02SPAN])
		
	endif
endevent
