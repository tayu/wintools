' Šg’£Žq‚Ì•ÏX
Option Explicit


Function get_ext( argv )
    Dim fso, arg, ext, input, fname, msg

    Set fso = CreateObject( "Scripting.FileSystemObject" )
    ext = fso.GetExtensionName( argv( 0 ) )
    msg = "Šg’£Žq‚Ì•ÏX: " & vbCrLf
    For Each fname In argv
    	msg = msg & vbCrLf  & "  " & fname
    Next
    input = InputBox( msg,, "" &  ext )
    If 0 >= Len( input ) Or input = ext Then
        get_ext = ""
    Else
	get_ext = input
    End If
    Set fso = Nothing
End Function


Sub set_ext( argv, ext )
    Dim fso, file, f, base

    Set fso = CreateObject( "Scripting.FileSystemObject" )
    For Each file In argv
    	base = fso.GetBaseName( file )
        Set f = fso.GetFile( file )
    	f.Name = base & "." & ext
    	Set f = Nothing
    Next
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
    	End If
    Next

    ext = get_ext( argv )
    If 0 = Len( ext ) Then
        WScript.Echo( "No Change ..." )
	Exit Sub
    End If

    Set fso = Nothing
    set_ext argv, ext

    WScript.Echo( "-- end --" )
End Sub
main( WScript.Arguments )

