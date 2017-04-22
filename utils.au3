#include-once
#include <Debug.au3>

Global $foobar=-1,$statusBar=-1,$stateBar=-1
Func showStatus($s)
	If $statusBar>-1 Then
	$dm.FoobarClearText($statusBar)
	$dm.FoobarPrintText($statusBar,$s,"ff0000")
	$dm.FoobarUpdate($statusBar)
	EndIf
EndFunc
Func appendStatus($s)
	If $statusBar>-1 Then
	;$dm.FoobarClearText($statusBar)
	$dm.FoobarPrintText($statusBar,$s,"ff0000")
	$dm.FoobarUpdate($statusBar)
	EndIf
EndFunc
Func appendState($s)
	If $stateBar>-1 Then
	;$dm.FoobarClearText($statusBar)
	$dm.FoobarPrintText($stateBar,$s,"ff0000")
	$dm.FoobarUpdate($stateBar)
	EndIf
EndFunc

Func setupDebug()
	If $isLogFile Then
		_DebugSetup("trace",True,4,$logFileName,True);
		trace("have log file")
	Else
		_DebugSetup("trace",True,2);
	EndIf
		;
		_DebugOut("***********Debug Info*****************")
EndFunc

Func foobarPrint($mess)
If $foobar>-1 Then
	$dm.FoobarPrintText($foobar,$mess,"ff0000")
	$dm.FoobarUpdate($foobar)
EndIf
EndFunc

Func trace($mess,$type=-1)
	foobarPrint(">"&$mess)
	_DebugOut(">" & $mess)
EndFunc

