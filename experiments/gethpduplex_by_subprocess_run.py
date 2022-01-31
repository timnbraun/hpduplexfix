#! /usr/bin/python

from subprocess import run
from winreg import QueryValueEx, OpenKey, HKEY_LOCAL_MACHINE

duplex = run( [ "reg", "query", 
		"HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Control\Print\Printers\hplaser (HP LaserJet Professional P1606dn)\PrinterDriverData", 
		'/v', 'FeatureKeyword' ], 
	capture_output=True, universal_newlines=True )

DEBUG = False
output = duplex.stdout.split()
DEBUG and print(output)

keyword_blob = output[-1]

DEBUG and print(keyword_blob)

keyword_bytes = bytes.fromhex( keyword_blob )

DEBUG and print(keyword_bytes)

def decode_key_val(keyword_bytes):
	keyword_list = keyword_bytes.split(b'\0')
	DEBUG and print(keyword_list)
	duplex_key = str( keyword_list[0], encoding='utf8' )
	duplex_val = str( keyword_list[1], encoding='utf8' )
	print(duplex_key, '=', duplex_val)

driverdata = OpenKey( HKEY_LOCAL_MACHINE, "SYSTEM\CurrentControlSet\Control\Print\Printers\hplaser (HP LaserJet Professional P1606dn)\PrinterDriverData" )
dup_val_type = QueryValueEx( driverdata, 'FeatureKeyword' )
DEBUG and print(dup_val_type[0])

decode_key_val( keyword_bytes )
decode_key_val( dup_val_type[0] )
