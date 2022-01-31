# Start-Process powershell -ArgumentList '-NoExit', "-Command cd '$pwd'; & .\actual\script\you\want\to\run.ps1" -Verb runAs

$oIdent= [Security.Principal.WindowsIdentity]::GetCurrent()
$oPrincipal = New-Object Security.Principal.WindowsPrincipal($oIdent)
if(!$oPrincipal.IsInRole([Security.Principal.WindowsBuiltInRole]::Administrator )){
    Write-Warning "Please start script with Administrator rights!"
	# Start-Process powershell.exe "-File",('"{0}"' -f $MyInvocation.MyCommand.Path) -Verb RunAs
    # exit
}
Set-PrinterProperty -PrinterName 'hplaser (HP LaserJet Professional P1606dn)' -PropertyName Config:DuplexUnit -Value TRUE
