'
' test bup
'
Option Explicit
' On Error Resume Next

Const WindowStyleHide	= 0
Const WindowStyleNormal	= 1
Const WindowStyleMin	= 2
Const WindowStyleMax	= 3

Const PG_WAIT		= true
Const PG_NOWAIT		= false


''
Const PROGRAMS = "hardlink.exe"
Const TEST_SRC = "a0.txt"
Const TEST_DST = "a1.txt"


' main
Dim wso, we, cmd, i, r
Set wso = WScript.CreateObject( "WScript.Shell" )
If Err.Number = 0 Then
    cmd = PROGRAMS
    cmd = cmd & " " & TEST_SRC
    cmd = cmd & " " & TEST_DST
    ' WScript.Echo "cmd: " & cmd
    r = wso.Run( cmd, WindowStyleNormal, PG_WAIT )

    Set wso = Nothing
    ' WScript.Echo "-- end --"
End If



