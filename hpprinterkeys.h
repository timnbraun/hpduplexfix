//
// hpprinterkeys.h
//

using std::string;

// Registry keys

const string DriverKey = "SYSTEM\\CurrentControlSet\\Control\\Print\\Printers\\hplaser (HP LaserJet Professional P1606dn)\\PrinterDriverData";
const string DriverKey64 = "SOFTWARE\\WOW6432Node\\Microsoft\\Windows NT\\CurrentVersion\\Print\\Printers\\hplaser (HP LaserJet Professional P1606dn)\\PrinterDriverData";

const string FeatureKeyword = "FeatureKeyword";

// For powershell perspective
const string PrinterName = "hplaser (HP LaserJet Professional P1606dn)";
const string PropertyName = "Config:DuplexUnit";
