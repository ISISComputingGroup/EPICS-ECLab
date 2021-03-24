#include "BLFunctions.h"
#include "ECLabCodeLookup.h"

struct EnumPair
{
    int val;
	const char* desc;
};
	
static const char* enumLookup(int val, EnumPair* enumPairs, int n)
{
   for(int i=0; i<n; ++i)
   {
        if (enumPairs[i].val == val)
	    {
           return enumPairs[i].desc;
        } 
    }
    return "unknown";
}

/** Channel State */
static EnumPair TChannelStateLookup[] = {
    { KBIO_STATE_STOP, "stopped" },
    { KBIO_STATE_RUN, "running" },
    { KBIO_STATE_PAUSE, "paused" }
};

const char* channelStateLookup(int val)
{
    return enumLookup(val, TChannelStateLookup, sizeof(TChannelStateLookup) / sizeof(EnumPair));
}
	
/** Voltage range */
static EnumPair TVoltageRangeLookup[] = {
    { KBIO_ERANGE_2_5, "+/- 2.5 V" },
    { KBIO_ERANGE_5, "+/- 5 V" },
    { KBIO_ERANGE_10, "+/- 10 V" },
    { KBIO_ERANGE_AUTO, "Auto range" }
};

const char* voltageRangeLookup(int val)
{
    return enumLookup(val, TVoltageRangeLookup, sizeof(TVoltageRangeLookup) / sizeof(EnumPair));
}

/** Intensity range */
static EnumPair TIntensityRangeLookup[] = {
    { KBIO_IRANGE_100pA, "I range 100 pA" },
    { KBIO_IRANGE_1nA, "I range 1 nA" },
    { KBIO_IRANGE_10nA, "I range 10 nA" },
    { KBIO_IRANGE_100nA, "I range 100 nA" },
    { KBIO_IRANGE_1uA, "I range 1 uA" },
    { KBIO_IRANGE_10uA, "I range 10 uA" },
    { KBIO_IRANGE_100uA, "I range 100 uA" },
    { KBIO_IRANGE_1mA, "I range 1 mA" },
    { KBIO_IRANGE_10mA, "I range 10 mA" },
    { KBIO_IRANGE_100mA, "I range 100 mA" },
    { KBIO_IRANGE_1A, "I range 1 A" },
    { KBIO_IRANGE_BOOSTER, "Booster" },
    { KBIO_IRANGE_AUTO, "Auto range" },
    { KBIO_IRANGE_10pA, "IRANGE_100pA + Igain x10" },
    { KBIO_IRANGE_1pA, "IRANGE_100pA + Igain x100" }
};

const char* intensityRangeLookup(int val)
{
    return enumLookup(val, TIntensityRangeLookup, sizeof(TIntensityRangeLookup) / sizeof(EnumPair));
}

/** Firmware codes */
static EnumPair TFirmwareCodeLookup[] = {
    { KIBIO_FIRM_NONE, "No firmware loaded" },
    { KIBIO_FIRM_INTERPR, "Firmware for EC-Lab software" },
    { KIBIO_FIRM_UNKNOWN, "Unknown firmware loaded" },
    { KIBIO_FIRM_KERNEL, "Firmware for the library" },
    { KIBIO_FIRM_INVALID, "Invalid firmware loaded" },
    { KIBIO_FIRM_ECAL, "Firmware for calibration software" }
};
	
const char* firmwareCodeLookup(int val)
{
    return enumLookup(val, TFirmwareCodeLookup, sizeof(TFirmwareCodeLookup) / sizeof(EnumPair));
}

/** Option error codes */
static EnumPair TOptionErrorLookup[] = {
   { KBIO_OPT_NOERR, "Option no error" },
   { KBIO_OPT_CHANGE, "Option change" },
   { KBIO_OPT_4A10V_ERR, "Amplifier 4A10V error" },
   { KBIO_OPT_4A10V_OVRTEMP, "Amplifier 4A10V overload temperature" },
   { KBIO_OPT_4A10V_BADPOW, "Amplifier 4A10V invalid power" },
   { KBIO_OPT_4A10V_POWFAIL, "Amplifier 4A10V power fail" },
   { KBIO_OPT_1A48V_ERR, "Amplifier 1A48V error" },
   { KBIO_OPT_1A48V_OVRTEMP, "Amplifier 1A48V overload temperature" },
   { KBIO_OPT_1A48V_BADPOW, "Amplifier 1A48V invalid power" },
   { KBIO_OPT_1A48V_POWFAIL, "Amplifier 1A48V power fail" },
   { KBIO_OPT_10A5V_ERR, "Amplifier 10A5V error" },
   { KBIO_OPT_10A5V_OVRTEMP, "Amplifier 10A5V overload temperature" },
   { KBIO_OPT_10A5V_BADPOW, "Amplifier 10A5V invalid power" },
   { KBIO_OPT_10A5V_POWFAIL, "Amplifier 10A5V power fail" },
};

const char* optionErrorLookup(int val)
{
    return enumLookup(val, TOptionErrorLookup, sizeof(TOptionErrorLookup) / sizeof(EnumPair));
}

/** Technique IDs enumeration */
static EnumPair TTechniqueIdentifierLookup[] = {
    { KBIO_TECHID_NONE, "None" },
	{ KBIO_TECHID_OCV, "Open Circuit Voltage (Rest)" },
    { KBIO_TECHID_CA, "Chrono-amperometry" },
    { KBIO_TECHID_CALIMIT, "Chrono-amperometry with limits identifier" },
	{ KBIO_TECHID_CP, "Chrono-potentiometry" },
	{ KBIO_TECHID_CPLIMIT, "Chrono-potentiometry with limits identifier" },
	{ KBIO_TECHID_CV, "Cyclic Voltammetry" },
	{ KBIO_TECHID_PEIS, "Potentio Electrochemical Impedance" },
    { KBIO_TECHID_TO, "Trigger Out identifier" },
    { KBIO_TECHID_TI, "Trigger In identifier" },
    { KBIO_TECHID_TOS, "Trigger Set identifier" },
	{ KBIO_TECHID_LOOP, "Loop" }
};

const char* techniqueIdentifierLookup(int val)
{
    return enumLookup(val, TTechniqueIdentifierLookup, sizeof(TTechniqueIdentifierLookup) / sizeof(EnumPair));
}
