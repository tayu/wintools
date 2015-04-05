' 画像格納フォルダに移動する
' ファイル名が重複する場合はリネームする
Option Explicit

Const PIC_DIR = "D:\home\doc\pictures"


Function BuildFilename( base, ext )
    BuildFilename = base & "." & ext
End Function


Function make_range_string( s, e )
    Dim st, i
    st = ""
    For i = Asc( s ) To Asc( e )
        st = st & Chr( i )
    Next
    make_range_string = st
End Function


Function NAME_STR_RANGE()
    NAME_STR_RANGE = "" _
        + make_range_string( "0", "9" ) _
        + make_range_string( "a", "z" ) _
        + make_range_string( "A", "Z" )
End Function


Sub do_stock( filename, destdir )
    Dim fso, base, ext, destname, dm, ofname, nfname

    Set fso = CreateObject( "Scripting.FileSystemObject" )

    base = fso.GetBaseName( filename )
    ext = fso.GetExtensionName( filename )
    ofname = BuildFilename( base, ext )
    destname = fso.BuildPath( destdir, ofname )
    Randomize
    While fso.FileExists( destname )
	base = base _
	    & Mid( NAME_STR_RANGE, 1 + Int( Rnd() * Len( NAME_STR_RANGE )), 1 )
    	destname = fso.BuildPath( destdir, BuildFilename( base, ext ) )
    Wend
    nfname = BuildFilename( base, ext )

    If ofname <> nfname Then
        WScript.Echo( "Rename: " & ofname & " --> " & nfname )
    End If

    dm = fso.MoveFile( filename, destname )

    Set fso = Nothing
End Sub


Sub main( argv )
    Dim fso, arg, ext

    If 0 = argv.Count Then
        WScript.Echo( "No args !!!" )
       	Exit Sub
    End If

    Set fso = CreateObject( "Scripting.FileSystemObject" )
    For Each arg In argv
        If Not fso.FileExists( arg ) Then
       	    WScript.Echo( "Err: Not exist" & arg )
    	    Set fso = Nothing
       	    Exit Sub
    	Else
    	    Call do_stock( arg, PIC_DIR )
	End If
    Next
    Set fso = Nothing

    ' WScript.Echo( "-- end --" )
End Sub
main( WScript.Arguments )
