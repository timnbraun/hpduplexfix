#! /usr/bin/python

from winreg import QueryValueEx, OpenKey, CloseKey, HKEY_LOCAL_MACHINE

DEBUG = False

def decode_key_val(keyword_bytes):
	keyword_list = keyword_bytes.split(b'\0')
	DEBUG and print(keyword_list)
	duplex_key = str( keyword_list[0], encoding='utf8' )
	duplex_val = str( keyword_list[1], encoding='utf8' )
	print(duplex_key, '=', duplex_val)

driverdata = OpenKey( HKEY_LOCAL_MACHINE, "SYSTEM\CurrentControlSet\Control\Print\Printers\hplaser (HP LaserJet Professional P1606dn)\PrinterDriverData" )
dup_val_type = QueryValueEx( driverdata, 'FeatureKeyword' )
CloseKey( driverdata )

DEBUG and print(dup_val_type[0])

decode_key_val( dup_val_type[0] )


driverdata = OpenKey( HKEY_LOCAL_MACHINE, 'SOFTWARE\WOW6432Node\Microsoft\Windows NT\CurrentVersion\Print\Printers\hplaser (HP LaserJet Professional P1606dn)\PrinterDriverData' )
dup_val_type = QueryValueEx( driverdata, 'FeatureKeyword' )
CloseKey( driverdata )

decode_key_val( dup_val_type[0] )

