//
// printerutils.h
//
//    name_value
//    dumpDriverFeatures() - get the list of driver features from the registry
//    setPrinterProperty() - call powershell to set a property
//    enableDuplex() - call setPrinterProperty to set property to TRUE
//
//  Copyright (c) 2022 Tim Braun <tim.n.braun@gmail.com>
//

// Convenient name / value structure
typedef pair<string, string> name_value;

// Looks in registry for current feature blob
vector<name_value> dumpDriverFeatures( const string &key, const string &feature );

// Uses powershell to set a printer property
LSTATUS setPrinterProperty( const string &printer, const string &name, const string &value );

inline bool enableDuplex( const string &PrinterName, const string &PropertyName)
{
	LSTATUS rc = setPrinterProperty( PrinterName, PropertyName, "TRUE" );

	return rc == ERROR_SUCCESS;
}
