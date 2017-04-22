#Region ;**** Directives created by AutoIt3Wrapper_GUI ****
#AutoIt3Wrapper_Outfile=playMineMain.exe
#EndRegion ;**** Directives created by AutoIt3Wrapper_GUI ****

#cs ----------------------------------------------------------------------------

 AutoIt Version: 3.3.14.2
 Author:         myName

 Script Function:
	Template AutoIt script.

#ce ----------------------------------------------------------------------------

; Script Start - Add your code below here


Global $mineProgram="C:\Program Files\Microsoft Games\Minesweeper\MineSweeper.exe";
Global $logFile="log.txt";
Global $imgPath="imgs\";
Global $defaultXY[]=[1100,500];

Global $isLogFile=False ; If true, the log file is: log.txt
Global $isDebug=True;
Global $isShowBasicInfo=True
Global $logFileName="log.log"
Global $hwnd=0;
Global $mapTL[]=[ 38, 80];
Global $mapBR[]=[ 578, 368];

Global $clientWidth=-1;
Global $clientHeight=-1;

Global $defaultDealy=300;

;***************
#include-once
#include <MsgBoxConstants.au3>

Global $isEnd=False
Global $startTimeHandle;
Global $runTime;
Global $myHero;

Global $dm = ObjCreate("dm.dmsoft");
If  @error Then
	;MsgBox($MB_SYSTEMMODAL, "objcreate fails:", "Error code:"&Hex(@error, 8));
EndIf



; start of headers.

#include <ScreenCapture.au3>
#include <Debug.au3>
#include <ImageSearch.au3>
#include <utils.au3>
#include <basicFuns.au3>
#include <Misc.au3>
; end of headers.

;*********************control of script *******************
; Press Esc to terminate script, Pause/Break to "pause"
Global $fPaused = False;
startThisScript()
While 1
	Sleep(100)
WEnd
ToolTip("");
Exit;

Func TogglePause()
	$fPaused = Not $fPaused
	While $fPaused
		Sleep(100)
		ToolTip('Script is "Paused"', 0, 0)
	WEnd
	ToolTip("")
EndFunc   ;==>TogglePause

Func Terminate()
	trace("exiting...");
	ToolTip("");
	Exit
EndFunc   ;==>Terminate

Func startThisScript()
	startApp()
EndFunc   ;==>ShowMessage

Func startApp()
	If _Singleton("playMine", 1) = 0 Then
		MsgBox($MB_SYSTEMMODAL, "Warning", "An occurence of playMine is already running");
		;trace(""An occurence of test is already running"")
		;Send("{F11}");end the previouse script.
		Sleep(1000);
		;Exit
	EndIf
	HotKeySet("{F10}", "TogglePause")
	HotKeySet("{F11}", "Terminate")
	HotKeySet("{F9}", "restartGame") ;
	HotKeySet("p", "mainLoop") ;
	$isEnd=False
	$startTimeHandle=TimerInit();
	;readSetting()
	setupDebug()
	;initScript()
	startGameProgram()
	;createFoobars()
	mainLoop()
EndFunc

Func readSetting()
$isLogFile=Int(IniRead("setting.ini","General","isLogFile",1))
$isDebug=Int(IniRead("setting.ini","General","isDebug",1))
;Local $test=IniRead("setting.ini","General","test","default")
;trace("$test"&$test)
;trace("$isDebug"&$isDebug)
$isShowBasicInfo=Int(IniRead("setting.ini","General","isShowBasicInfo",0))
$logFileName=IniRead("setting.ini","General","logFileName","log.log");
EndFunc
Func createFoobars()
	;***********test dm************/
	If $foobar>-1 Then
		$ret=$dm.FoobarClose($foobar)
		If $ret==0 Then
			trace("Error: $dm.FoobarClose($foobar) fails");
		EndIf
	EndIf
	If $statusBar>-1 Then
		$ret=$dm.FoobarClose($statusBar)
		If $ret==0 Then
			trace("Error: $dm.FoobarClose($statusBar) fails");
		EndIf
	EndIf

	If $isDebug Then
		$foobar = $dm.CreateFoobarRect($hwnd,10,100,200,250)
	EndIf
	If $isShowBasicInfo Then
		$statusBar=$dm.CreateFoobarRect($hwnd,3,514,399,80)
		$stateBar=$dm.CreateFoobarRect($hwnd,1143,506,116,42)
	EndIf
	;trace("dm.ret="&$ret);
;********
EndFunc



;**************************Main function******************
Func ScreenCapture_CaptureWnd($file,$hWnd,$iLeft=0,$iTop=0,$iRight=-1,$iBottom=-1,$bCursor=True)
_ScreenCapture_CaptureWnd($imgPath&$file,$hWnd,$iLeft,$iTop,$iRight,$iBottom,$bCursor)
EndFunc

Func ImageSearch($findImage,$resultPosition, ByRef $x, ByRef $y,$tolerance, $HBMP=0)
	;trace("search for "&$findImage)


	;trace("w:"&@DesktopWidth)
	;trace("@DesktopHeight:"&@DesktopHeight)
   return _ImageSearchArea($imgPath&$findImage,$resultPosition,0,0,@DesktopWidth,@DesktopHeight,$x,$y,$tolerance,$HBMP)
EndFunc
Func isProgramExist($p)
	Local $h=WinGetHandle($p);
	If @error Then
		return False
	EndIf
	If IsHungAppWindow($h) Then
		WinKill($h)
		Sleep(300);
		Return False
	EndIf
	Return True
EndFunc


Func startGameProgram()
	trace("check for mineSweeper program exist?");
	If  isProgramExist("扫雷") Then
		Sleep(100);
	Else
		Run($mineProgram);
		trace("Wait to 扫雷程序　to appears...");
		WinWait("扫雷");
		Sleep(1000);
		trace("open 扫雷 finished");
	EndIf
	trace("mineSweeper had been opened, start play1.")
	$hwnd=WinGetHandle("扫雷")
	WinSetState($hwnd,"",@SW_RESTORE)

	WinMove($hwnd,"",0,0, 100, 100);move to (0,0) , and set it's window size to the minimized size.
	WinActivate($hwnd)
	Local $clientSize=WinGetClientSize($hwnd)
	$clientWidth=$clientSize[0]
	$clientHeight=$clientSize[1]
	trace("end start game.");
EndFunc
Func restMouse()
	mclick($defaultXY[0],$defaultXY[1]);
EndFunc

Func mmove($x,$y,$s=10)
	$x=$x+Random(-2,2,1)
	$y=$y+Random(-2,2,1)
	MouseMove($x,$y,$s)
	Sleep(50)
EndFunc

Func mclick($x,$y,$isLeft=True)
	$x=$x+Random(-2,2,1)
	$y=$y+Random(-2,2,1)
	If $isLeft Then
	MouseClick("left",$x,$y,1,5)
Else
	MouseClick("right",$x,$y,1,5)
EndIf
	Sleep(50)
EndFunc





	; the map get mine
; (38, 80)  (578, 368)
; 16*30
; 288*540
;
; tile size: 18px  18px.
Func varInit()
Global $NumRow=16;
Global $NumCol=30;
Global $tileSize=18;
Global $baseX=38;
Global $baseY=80;
Global $mapXY[16][30][2];
Global $mapData[16][30]; -1: unMined, 0~8 for num,
For $r=0 to ($NumRow-1)
		For $c=0 to ($NumCol -1)
			$mapXY[$r][$c][0]=$mapTL[0]+$c*$tileSize; x.
			$mapXY[$r][$c][1]=$mapTL[1]+$r*$tileSize;y
		Next
Next

EndFunc

Func toXY($r, $c, ByRef $x, ByRef $y)
If $r <0 Or $r >= $NumRow Or $c<0 Or $c>=$NumCol  Then
	trace("Error: $r, $c fails. $r="&$r&" $c="&$c);
	$x=0;
	$y=0;
Return
EndIf
$x=$c*$tileSize+$baseX;
$y=$r*$tileSize+$baseY;
EndFunc

Func clickMine($r, $c)
Local $x, $y;
toXY($r, $c, $x, $y);
WinActivate($hwnd);
mclick($x+$tileSize/2, $y+$tileSize/2);
EndFunc

Func getType($x1, $y1, $x2, $y2)
trace("inside get type");



	Local $type=-2;// for unknow.
	Local $ans=0;
	For $x= $x1 to $x2 Step 5
		For $y= $y1 to $y2 Step 5
			;trace("x,y="&$x&","&$y);
			$ans=$ans+PixelGetColor($x, $y, $hwnd);
		Next
	Next
	Return $ans;
	Return $type;
EndFunc


Func parseMap()
	For $r=0 to 15
		For $c=0 to 29
			trace($r&"-"&$c);
			Local $startX=$mapXY[$r][$c][0];
			Local $startY=$mapXY[$r][$c][1];
			Local $x1=$startX;
			Local $x2=$x1+$tileSize-1;
			Local $y1=$startY;
			Local $y2=$y1+$tileSize-1;
			ScreenCapture_CaptureWnd("tmp\"&$r&"-"&$c&".bmp", $hwnd,  $x1, $y1, $x2, $y2);
	;		$mapData[$r][$c]=getType($startX, $startY, $startX+$tileSize-1, $startY+$tileSize-1);
		Next
	Next

	trace("end parse map");
	printMap();
EndFunc

Func printMap()
	Local $res="";
For $r=0 to ($NumRow-1)
		For $c=0 to ($NumCol -1)
			$res=$res&$mapData[$r][$c];
		Next
		$res=$res&"\n";
Next
trace("Map: \n" &$res);

EndFunc




Func ImageRSearch($findImage,$resultPosition, ByRef $x, ByRef $y, $w, $h, $tolerance, $HBMP=0)
	;trace("search for "&$findImage)
	;trace("w:"&@DesktopWidth)
	;trace("@DesktopHeight:"&@DesktopHeight)

   return _ImageSearchArea($imgPath&$findImage,$resultPosition,0,0,$w,$h,$x,$y,$tolerance,$HBMP)
EndFunc

Func hasPics($img, $w=0, $h=0)
	Local $x,$y;
	Local $imgs=StringSplit($img,"|");
	For $i=1 to $imgs[0]
		If $w=0 Then

		Local $ret=ImageSearch($imgs[$i],0,$x,$y,20);
		If $ret=1 Then
			Return True
		EndIf
		Else
		Local $ret=ImageRSearch($imgs[$i],0,$x,$y,$w, $h,20);
		If $ret=1 Then
			Return True
		EndIf


		EndIf

	Next
	return False
EndFunc
Func clickPics($img, $w=0, $h=0)
	Local $x=-1,$y=-1;

	Local $isFound=False
	Local $imgs=StringSplit($img,"|");
		For $i=1 to $imgs[0]
			Local $ret;
			If $w=0 Then
			 $ret=ImageSearch($imgs[$i],1,$x,$y,20);
			Else
			  $ret=ImageRSearch($imgs[$i],0,$x,$y,$w, $h,20);
			EndIf

			If $ret=1 Then
				;found that pic.
				mclick($x,$y)
				Return True
			EndIf
		Next

	Return False

EndFunc

Func getGameState()
	;1 for fails.
	;2
	if hasPics("m0.bmp|m1.bmp|m2.bmp|m3.bmp|lose.bmp", 580, 369) Then
		return 1;
	EndIf
	If hasPics("numZeroMine.bmp|win.bmp",614, 402) Then

		return 2;
	EndIf
	return 0;

EndFunc

Func basicNotify($mess)
ToolTip($mess,0, 0);
EndFunc

Func restartGame()
trace("restart gaming...");
startGameProgram();

trace("try click play again button");
basicNotify("Please wait 2s, program's going to click the play again button");
Sleep(200);
If not clickPics("playAgain2.bmp|playAgain1.bmp|playAgain.bmp") Then
	basicNotify("Auto click faisl, Please ress button: play again manually!");
Else

	trace("click paly again button successfully!");
	Sleep($defaultDealy);
	trace("wait to play");
EndIf

mainLoop();
EndFunc


#include<PHash.au3>
Func mainLoop()
	varInit();
	trace("enter main");
	;Sleep($defaultDealy);
	Local $state=-1;
	Local $isEnd=false;
	while Not $isEnd
		$state=getGameState();
		trace("game state:"&$state);
		Switch($state)
			Case 0 ; playing.
				basicNotify("playing, press <F10> to pause/continue, <F11> to stop...");
				ScreenCapture_CaptureWnd("a.bmp", $hwnd,  $mapTL[0], $mapTL[1], $mapBR[0]-1, $mapBR[1]-1);
				parseMapToFile("imgs\a.bmp", "map.txt");
				RunWait("TrivalMineAI.exe map.txt operation.txt","",false);
				Local $arr=FileReadToArray("operation.txt");
				if @error Then
					MsgBox($MB_SYSTEMMODAL, "", "There was an error reading the file. @error: " & @error) ;
				Else
					Local $len=UBound($arr);
					for $i =0 to $len-2 Step 2
						Sleep(200);
						$r=int($arr[$i]);
						$c=int($arr[$i+1]);
						clickMine($r, $c);
						mmove($clientWidth,$clientHeight);
					Next

				EndIf
				Sleep($defaultDealy);
			case 1; fails.
				basicNotify("You lose, press <F9> to play again.!!!!");
				$isEnd=true;
			case 2; wins.
				basicNotify("You win, press <F9> to play again.!!!!");
				$isEnd=true;
		EndSwitch

	WEnd
	trace("exit main.");
EndFunc



