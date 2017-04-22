Func IsHungAppWindow($h)
Local $ret=DllCall("user32.dll","BOOL","IsHungAppWindow","HWND",$h);
if  (Not @error) And IsArray($ret) Then
	Return $ret[0];
EndIf
return 0
EndFunc