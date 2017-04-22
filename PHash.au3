#include-once
;#include <utils.au3>
Func getHash($file)
Local $hash=-1;
Local $ret=DllCall("PHashDll.dll","UINT64","_getHash@4","str",$file);
if  (Not @error) And IsArray($ret) Then
	$hash=$ret[0];
EndIf
return $hash;
EndFunc

Func getBinDiff($a,$b)
Local $r=-1;
Local $ret=DllCall("PHashDll.dll","int","_getBinDiff@16","UINT64",$a,"UINT64",$b);
if  (Not @error) And IsArray($ret) Then
	$r=$ret[0];
EndIf
return $r;
EndFunc

Func isDiffImg($f1,$f2,$thre=5)
Local $r=-1;
Local $ret=DllCall("PHashDll.dll","int","_isDiffImg@12","str",$f1,"str",$f2,"int",$thre);
if  (Not @error) And IsArray($ret) Then
	$r=$ret[0];
EndIf
return $r;
EndFunc

Func parseMapToFile($file1, $file2)
Local $r=-1;
Local $ret=DllCall("PHashDll.dll","int","_parseMapToFile@8","str",$file1,"str",$file2);
if  (Not @error) And IsArray($ret) Then
	$r=$ret[0];
EndIf
return $r;
EndFunc


