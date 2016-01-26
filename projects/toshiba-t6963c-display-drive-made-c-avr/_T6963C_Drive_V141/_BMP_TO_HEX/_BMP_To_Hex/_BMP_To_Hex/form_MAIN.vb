Public Class form_MAIN

    Dim BMP As Bitmap
    Dim Pict_Sample_Image_Width As Integer
    Dim Pict_Sample_Image_Height As Integer
    Dim openDialog As New OpenFileDialog
    Dim imageLoaded As Boolean = False
    Private Declare Function Beep Lib "kernel32" (ByVal soundFrequency As Int32, ByVal soundDuration As Int32) As Int32


    Private Sub form_MAIN_Load(sender As Object, e As EventArgs) Handles MyBase.Load
        'set Program Title
        Me.Text = Application.ProductName + ""
        'Set pictures positions
        pict_CPLUSPLUS.Left = pict_ASM.Left
        pict_CPLUSPLUS.Top = pict_ASM.Top
    End Sub

    Private Sub form_MAIN_Paint(sender As Object, e As PaintEventArgs) Handles Me.Paint
        'Line a Black Line in Client Area
        Dim g As Graphics = e.Graphics
        g.Clear(Me.BackColor)
        g.DrawRectangle(Pens.Black, 0, 0, Me.ClientSize.Width - 1, Me.ClientSize.Height - 1)
        g.Dispose()
    End Sub

    Private Sub form_MAIN_Resize(sender As Object, e As EventArgs) Handles Me.Resize
        'Recall Paint Event
        Invalidate()
        text_HEX_OUTPUT.Left = groupBox_IMAGE.Width + groupBox_IMAGE.Left * 2
        text_HEX_OUTPUT.Top = groupBox_IMAGE.Top + 6
        text_HEX_OUTPUT.Width = (Me.ClientSize.Width - (groupBox_IMAGE.Left + groupBox_IMAGE.Width)) - groupBox_IMAGE.Left * 2
        text_HEX_OUTPUT.Height = (Me.ClientSize.Height - groupBox_IMAGE.Top) - groupBox_IMAGE.Top
    End Sub

    Private Sub LoadImageToolStripMenuItem_Click(sender As Object, e As EventArgs) Handles LoadImageToolStripMenuItem.Click
        'Load Image
        Load_Image()
    End Sub

    Private Sub ExitToolStripMenuItem_Click(sender As Object, e As EventArgs) Handles ExitToolStripMenuItem.Click
        'Terminate Program
        End
    End Sub

    Public Sub New()

        ' This call is required by the designer.
        InitializeComponent()

        ' Add any initialization after the InitializeComponent() call.

        'Set Default Picture Sample Image Size
        Pict_Sample_Image_Width = pict_SAMPLE_IMAGE.Width
        Pict_Sample_Image_Height = pict_SAMPLE_IMAGE.Height
    End Sub
    '
    '+#######################################################################################
    '# BMP_TO_ASSEMBLER_HEX_1_BIT
    '# 
    '# Convert BMP to string of Hex data for assembler code for Atmel AVR Studio 4..7
    '+#######################################################################################
    '
    Sub BMP_To_Assembler_Hex_1_Bit(ByRef bmp As Bitmap, ByRef openDialog As OpenFileDialog)
        Dim w As Integer
        Dim X As Integer
        Dim Y As Integer
        Dim f As Integer
        Dim s As String
        Dim so As String
        Dim T As Object
        Dim r As Byte
        Dim b As Byte
        Dim g As Byte
        Dim l As Integer
        Dim bb As Byte
        Dim mask As Byte
        Dim rs As Integer
        Dim sc As Integer

        Dim Width_Pixels As Long
        Dim Width_Bytes As Long
        Dim Height_Pixels As Long
        Dim Color_Bits As Integer
        Dim SizeBytes As Long
        Dim b_UPDOWN As Boolean
        Dim sCursor As Cursor

        sCursor = Me.Cursor

        If imageLoaded = False Then
            text_HEX_OUTPUT.Text = "Image File not Loaded!!!"
            Beep(1000, 250)
            Exit Sub
        End If
        Me.Cursor = Cursors.WaitCursor

        Width_Pixels = bmp.Width
        Height_Pixels = bmp.Height
        Width_Bytes = IIf((bmp.Width Mod 8) = 0, bmp.Width \ 8, bmp.Width \ 8 + 1)
        Color_Bits = 1
        SizeBytes = Width_Bytes * Height_Pixels

        'Information Fields
        Dim sBar = New String("-", Len(openDialog.FileName.ToString))
        so = ""
        so = so + ";+" + sBar + vbCrLf
        so = so + ";| Generate Assembler File for Atmel AVR Studio 4..7" + vbCrLf
        so = so + ";|" + vbCrLf
        so = so + ";| Source" + vbCrLf
        so = so + ";|     Path : " + ExtractFilePath(openDialog.FileName) + vbCrLf
        so = so + ";|     File : " + ExtractFileName(openDialog.FileName) + "." + ExtractFileNameExtension(openDialog.FileName) + vbCrLf
        so = so + ";|" + vbCrLf
        so = so + ";| Destiny" + vbCrLf
        so = so + ";|     Path : " + ExtractFilePath(openDialog.FileName) + vbCrLf
        so = so + ";|     File : " + "BMP_" + ExtractFileName(openDialog.FileName) + ".INC" + vbCrLf
        so = so + ";|" + vbCrLf
        so = so + ";| Destiny Image Properties" + vbCrLf
        so = so + String.Format(";|     Size   : {0,5} Bytes", SizeBytes) + vbCrLf
        so = so + String.Format(";|     Width  : {0,5} Pixels", Width_Pixels) + vbCrLf
        so = so + String.Format(";|     Height : {0,5} Pixels", Height_Pixels) + vbCrLf
        so = so + String.Format(";|     WidthB : {0,5} Bytes", Width_Bytes) + vbCrLf
        so = so + String.Format(";|     Colors : {0,5} Levels({1}) Bit", 2 ^ Color_Bits, Color_Bits) + vbCrLf
        so = so + ";+" + sBar + vbCrLf + vbCrLf
        so = so + ";( IMAGE CONTENTS )" + vbCrLf + vbCrLf
        'Draw upper X axis
        Draw_Axis_X_1_Bit(so, bmp, True)
        'Draw Text Image
        Draw_Text_Image(so, bmp, Width_Bytes)
        'Draw lower X axis
        Draw_Axis_X_1_Bit(so, bmp, False)
        'DW Fields
        so = so + vbCrLf
        so = so + ";( IMAGE DATA )" + vbCrLf + vbCrLf
        so = so + "BMP_" + ExtractFileName(openDialog.FileName) + ":" + vbCrLf + vbCrLf
        so = so + vbTab + ";+------------+" + vbCrLf
        so = so + vbTab + ";| BMP Header |" + vbCrLf
        so = so + vbTab + ";+------------+" + vbCrLf
        so = so + vbTab + ".DW  " + String.Format("{0,5} Bits Color Bit", Color_Bits) + vbCrLf
        so = so + vbTab + ".DW  " + String.Format("{0,5} Width Pixels", Width_Pixels) + vbCrLf
        so = so + vbTab + ".DW  " + String.Format("{0,5} Height Pixels", Height_Pixels) + vbCrLf
        so = so + vbTab + ".DW  " + String.Format("{0,5} Width Bytes ", Width_Bytes) + vbCrLf
        so = so + vbTab + ".DW  " + String.Format("{0,5} Size Bytes", SizeBytes) + vbCrLf
        so = so + vbTab + ";+------------+" + vbCrLf
        so = so + vbTab + ";|  BMP Data  |" + vbCrLf
        so = so + vbTab + ";+------------+" + vbCrLf
        'Draw DBs
        Draw_DBs(so, bmp, Width_Bytes)
        'Set Hex To Output Text Control
        text_HEX_OUTPUT.Text = so
        'restore cursor state
        Me.Cursor = sCursor
    End Sub
    Function ExtractFileName(ByVal fileName As String) As String
        'FileName cases
        '
        ' 1 - c:\teste.txt
        ' 2 - c:\alo\teste.txt
        ' 3 - c:\sub\sub2\teste
        ' 4 - teste
        ' 5 - teste.txt
        Dim rBackSlash, _
            rPoint As Integer
        'Extract a File Name from path
        rBackSlash = InStrRev(fileName, "\")
        rPoint = InStrRev(fileName, ".")
        If rBackSlash > 0 And rPoint > 0 Then
            'case 1,2
            Return fileName.Substring(rBackSlash, rPoint - rBackSlash - 1)
        Else
            If rBackSlash > 0 And rPoint = 0 Then
                'case 3
                Return fileName.Substring(rBackSlash, Len(fileName) - rBackSlash)
            Else
                If rBackSlash = 0 And rPoint > 0 Then
                    'case 5
                    Return fileName.Substring(0, rPoint - 1)
                Else
                    If rBackSlash = 0 And rPoint = 0 Then
                        'case 4
                        Return fileName
                    End If
                End If
            End If
        End If
        Return ""
    End Function
    Function ExtractFileNameExtension(ByVal fileName As String) As String
        Dim rPoint As Integer

        rPoint = InStrRev(fileName, ".")
        If rPoint > 0 Then
            Return fileName.Substring(rPoint, Len(fileName) - rPoint)
        End If
        Return ""
    End Function
    Function ExtractFilePath(ByVal fileName As String) As String
        Dim rBackSlash As Integer
        rBackSlash = InStrRev(fileName, "\")
        If rBackSlash > 0 Then
            Return fileName.Substring(0, rBackSlash)
        End If
        Return ""
    End Function
    Sub Draw_Axis_X_1_Bit(ByRef so As String, ByRef BMP As Bitmap, ByVal upDown As Boolean)
        'Draw X Axis
        If Not (upDown) Then
            so = so + ";" + Space(5)
            For X = 0 To BMP.Width - 1
                If (X Mod 10) = 0 Then
                    so = so + "|"
                Else
                    so = so + " "
                End If
            Next
            so = so + vbCrLf
        End If
        so = so + ";" + Space(5)
        For X = 0 To BMP.Width - 1
            If (X Mod 100) = 0 Then
                If X = 0 Then
                    so = so + " "
                Else
                    'so = so + Mid(Right$("0000" + Trim(X), 4), 2, 1)
                    so = so + Format(X, "0000").Substring(1, 1)
                End If
            Else
                so = so + " "
            End If
        Next
        so = so + vbCrLf
        so = so + ";" + Space(5)
        For X = 0 To BMP.Width - 1
            If (X Mod 10) = 0 Then
                If X = 0 Then
                    so = so + " "
                Else
                    'so = so + Mid(Right$("0000" + Trim(X), 4), 3, 1)
                    so = so + Format(X, "0000").Substring(2, 1)
                End If
            Else
                so = so + " "
            End If
        Next
        so = so + vbCrLf
        so = so + ";" + Space(5)
        For X = 0 To BMP.Width - 1
            'so = so + Mid(Right$("0000" + Trim(X), 4), 4, 1)
            so = so + Format(X, "0000").Substring(3, 1)
        Next
        so = so + vbCrLf
        If upDown Then
            so = so + ";" + Space(5)
            For X = 0 To BMP.Width - 1
                If (X Mod 10) = 0 Then
                    so = so + "|"
                Else
                    so = so + " "
                End If
            Next
            so = so + vbCrLf
        End If
        Return
    End Sub
    Sub Draw_Text_Image(ByRef so As String, ByRef bmp As Bitmap, ByVal widthBytes As Integer)
        Dim x, _
            y, _
            l, _
            K As Integer
        Dim s As String
        Dim C As Color
        Dim bInvert As Boolean

        bInvert = InvertedToolStripMenuItem.Checked

        C = bmp.GetPixel(0, bmp.Height - 1)
        'Draw Image in Text Control
        For y = 0 To bmp.Height - 1
            ProgressBar.Value = y / bmp.Height * 50
            so = so + ";" + Format(y, "0000") + "-"
            s = ""
            'For X = 0 To im.SPRITE_WIDTH(sprite_I) Step 8
            For w = 0 To widthBytes - 1
                x = w * 8
                For K = x To x + 7
                    'get rgb value
                    'im.SPRITE_PIXEL_GET_RGB(sprite_I, K, im.SPRITE_HEIGHT(sprite_I) - y - 1, r, g, b)
                    If (K < bmp.Width) Then
                        C = bmp.GetPixel(K, y)
                    Else
                        C = Color.Black
                    End If
                    'compute luminance
                    l = (CInt(C.R) * 33 + CInt(C.G) * 56 + CInt(C.B) * 11) / 100
                    If l < 128 And K < bmp.Width Then
                        s = s + IIf(bInvert, " ", "#")
                    Else
                        s = s + IIf(bInvert, "#", " ")
                    End If
                Next
            Next
            'so = so + s + "-" + Format(y, "@@@@") + vbCrLf
            so = so + s + String.Format("-{0,4}", y) + vbCrLf
        Next
    End Sub
    '
    '
    'DRAW DBS
    '
    Sub Draw_DBs(ByRef so As String, ByRef bmp As Bitmap, ByVal widthBytes As Integer, Optional ByVal cPluPlus As Boolean = False)
        'Draw DBs and DWs
        Dim s As String
        Dim X,
            Y,
            K,
            w,
            l,
            bb,
            sc,
            ct,
            mask As Integer
        Dim C As Color
        Dim bInvert As Boolean

        bInvert = InvertedToolStripMenuItem.Checked
        s = ""
        sc = 0
        ct = 0
        For Y = 0 To bmp.Height - 1
            ProgressBar.Value = Y / bmp.Height * 50 + 50
            'For X = 0 To im.SPRITE_WIDTH(sprite_I) - 1 Step 8
            For w = 0 To widthBytes - 1
                X = w * 8
                mask = &H80
                bb = 0
                For K = X To X + 7
                    'get rgb value
                    'im.SPRITE_PIXEL_GET_RGB(sprite_I, K, im.SPRITE_HEIGHT(sprite_I) - Y - 1, r, g, b)
                    If K < bmp.Width Then
                        C = bmp.GetPixel(K, Y)
                    Else
                        C = Color.Black
                    End If
                    'get luminance
                    l = (CInt(C.R) * 33 + CInt(C.G) * 56 + CInt(C.B) * 11) / 100
                    'check if inverted

                    If bInvert Then
                        If l > 128 Then
                            mask = mask \ 2
                        Else
                            bb = bb Or mask
                            mask = mask \ 2
                        End If
                    Else
                        If l < 128 Then
                            mask = mask \ 2
                        Else
                            bb = bb Or mask
                            mask = mask \ 2
                        End If

                    End If
                Next
                's = s + "0x" + Right$("00" + Hex$(bb), 2) + ","
                s = s + Hex2(bb) + ","
                sc = sc + 1
                If sc > 15 Then
                    sc = 0
                    'so = so + vbTab + ".DB " + Left(s, Len(s) - 1) + vbCrLf
                    so = so + vbTab + IIf(cPluPlus = False, ".DB ", "") + s.Substring(0, Len(s) - 1) + IIf(cPluPlus = False, ";    ", "," + vbTab + "//") + Y.ToString + vbCrLf
                    s = ""
                    ct += 1
                End If
            Next
        Next
        If s <> "" Then
            If (sc And 1) = 0 Then
                'last line even case
                'so = so + vbTab + ".DB " + Left(s, Len(s) - 1) + vbCrLf
                so = so + vbTab + IIf(cPluPlus = False, ".DB ", "") + s.Substring(0, Len(s) - 1) + vbCrLf
            Else
                'last line odd case
                'so = so + vbTab + ".DB " + Left(s, Len(s) - 1) + ",0x00" + vbCrLf
                so = so + vbTab + IIf(cPluPlus = False, ".DB ", "") + s.Substring(0, Len(s) - 1) + ",0x00" + vbCrLf
            End If
        End If
        ProgressBar.Value = 0
    End Sub
    Function Load_Image() As Boolean
        'Load Image
        'Set Filer
        openDialog.Filter = "JPG File (*.jpg)|*.jpg|Bit Map File (*.bmp)|*.bmp|GIF File (*.gif)|*.gif|PNG File (*.png)|*.png"
        Try
            'call dialog for open file
            openDialog.ShowDialog()
            'set BMP with this image
            BMP = New Bitmap(openDialog.FileName)
            'check if is a indexed image
            If (BMP.PixelFormat = Imaging.PixelFormat.Format1bppIndexed Or _
                BMP.PixelFormat = Imaging.PixelFormat.Format4bppIndexed Or _
                BMP.PixelFormat = Imaging.PixelFormat.Format8bppIndexed Or _
                BMP.PixelFormat = Imaging.PixelFormat.Indexed) Then
                'if yes, show message and exit
                'MsgBox("Indexed Image not supported", vbOK, "Warning !!!")
                text_HEX_OUTPUT.Text = "Indexed Image not supported"
                Return False
            End If
            'Adjust proportion
            If BMP.Width >= BMP.Height Then
                pict_SAMPLE_IMAGE.Width = Pict_Sample_Image_Width
                pict_SAMPLE_IMAGE.Height = Pict_Sample_Image_Width * BMP.Height / BMP.Width
            Else
                pict_SAMPLE_IMAGE.Height = pict_SAMPLE_IMAGE.Height
                pict_SAMPLE_IMAGE.Width = Pict_Sample_Image_Height * BMP.Width / BMP.Height
            End If
            'show file in use
            Me.Text = Application.ProductName + ",File(" + ExtractFileName(openDialog.FileName) + "." + ExtractFileNameExtension(openDialog.FileName) + ")"
            'show Sample image
            pict_SAMPLE_IMAGE.Image = BMP
            'check load ok
            imageLoaded = True
            Return True
        Catch
            imageLoaded = False
            Return False
        End Try
    End Function

    Private Sub buttom_LOAD_IMAGE_Click(sender As Object, e As EventArgs) Handles buttom_LOAD_IMAGE.Click
        'Load Image
        Load_Image()
    End Sub

    Private Sub buttom_GENERATE_Click(sender As Object, e As EventArgs) Handles buttom_GENERATE.Click
        'Generate hex
        text_HEX_OUTPUT.Text = ""
        '1 Bit Color
        If BitToolStripMenuItem.Checked Then
            'Save Assembler Type File?
            If AssemblerToolStripMenuItem.Checked Then
                'Convert BMP to assembler Hex String 1 Bit
                BMP_To_Assembler_Hex_1_Bit(BMP, openDialog)
            End If
            'Save C,C++ Type File?
            If CCToolStripMenuItem.Checked Then
                'Convert BMP to C,C++ Hex String 1 Bit
                BMP_To_CPlusPlus_Hex_1_Bit(BMP, openDialog)
            End If
        End If
    End Sub
    Function Hex2(ByVal value As Integer)
        If Len(Hex(value)) = 1 Then
            Return "0x0" + Hex(value)
        Else
            Return "0x" + Hex(value)
        End If

    End Function
    Private Sub BitToolStripMenuItem3_Click(sender As Object, e As EventArgs) Handles BitToolStripMenuItem3.Click

    End Sub

    Private Sub BitToolStripMenuItem_Click(sender As Object, e As EventArgs) Handles BitToolStripMenuItem.Click

    End Sub

    Private Sub AssemblerToolStripMenuItem_Click(sender As Object, e As EventArgs) Handles AssemblerToolStripMenuItem.Click
        'Set Assembler Generation
        AssemblerToolStripMenuItem.Checked = True
        CCToolStripMenuItem.Checked = False
        'Show Information Picture According
        pict_ASM.Visible = True
        pict_CPLUSPLUS.Visible = False
        'call GENERATE Button Event
        buttom_GENERATE_Click(sender, New EventArgs)
    End Sub

    Private Sub CCToolStripMenuItem_Click(sender As Object, e As EventArgs) Handles CCToolStripMenuItem.Click
        'Set C,C++ Generation
        AssemblerToolStripMenuItem.Checked = False
        CCToolStripMenuItem.Checked = True
        'Show Information Picture According
        pict_ASM.Visible = False
        pict_CPLUSPLUS.Visible = True
        'call GENERATE Button Event
        buttom_GENERATE_Click(sender, New EventArgs)
    End Sub
    '
    '+#######################################################################################
    '# BMP_TO_CPLUSPLUS_HEX_1_BIT
    '# 
    '# Convert BMP to string of Hex data for C++ code for Atmel AVR Studio 4..7
    '+#######################################################################################
    '
    Sub BMP_To_CPlusPlus_Hex_1_Bit(ByRef bmp As Bitmap, ByRef openDialog As OpenFileDialog)
        Dim w As Integer
        Dim X As Integer
        Dim Y As Integer
        Dim f As Integer
        Dim s As String
        Dim so As String
        Dim T As Object
        Dim r As Byte
        Dim b As Byte
        Dim g As Byte
        Dim l As Integer
        Dim bb As Byte
        Dim mask As Byte
        Dim rs As Integer
        Dim sc As Integer

        Dim Width_Pixels As Long
        Dim Width_Bytes As Long
        Dim Height_Pixels As Long
        Dim Color_Bits As Integer
        Dim SizeBytes As Long
        Dim b_UPDOWN As Boolean
        Dim sCursor As Cursor

        sCursor = Me.Cursor

        If imageLoaded = False Then
            text_HEX_OUTPUT.Text = "Image File not Loaded!!!"
            Beep(1000, 250)
            Exit Sub
        End If
        Me.Cursor = Cursors.WaitCursor

        Width_Pixels = bmp.Width
        Height_Pixels = bmp.Height
        Width_Bytes = IIf((bmp.Width Mod 8) = 0, bmp.Width \ 8, bmp.Width \ 8 + 1)
        Color_Bits = 1
        SizeBytes = Width_Bytes * Height_Pixels

        'Information Fields
        Dim sBar = New String("-", Len(openDialog.FileName.ToString))
        so = "/*" + vbCrLf
        so = so + ";+" + sBar + vbCrLf
        so = so + ";| Generate C,C++ File for Atmel AVR Studio 4..7" + vbCrLf
        so = so + ";|" + vbCrLf
        so = so + ";| Source" + vbCrLf
        so = so + ";|     Path : " + ExtractFilePath(openDialog.FileName) + vbCrLf
        so = so + ";|     File : " + ExtractFileName(openDialog.FileName) + "." + ExtractFileNameExtension(openDialog.FileName) + vbCrLf
        so = so + ";|" + vbCrLf
        so = so + ";| Destiny" + vbCrLf
        so = so + ";|     Path : " + ExtractFilePath(openDialog.FileName) + vbCrLf
        so = so + ";|     File : " + "BMP_" + ExtractFileName(openDialog.FileName) + ".INC" + vbCrLf
        so = so + ";|" + vbCrLf
        so = so + ";| Destiny Image Properties" + vbCrLf
        so = so + String.Format(";|     Size   : {0,5} Bytes", SizeBytes) + vbCrLf
        so = so + String.Format(";|     Width  : {0,5} Pixels", Width_Pixels) + vbCrLf
        so = so + String.Format(";|     Height : {0,5} Pixels", Height_Pixels) + vbCrLf
        so = so + String.Format(";|     WidthB : {0,5} Bytes", Width_Bytes) + vbCrLf
        so = so + String.Format(";|     Colors : {0,5} Levels({1}) Bit", 2 ^ Color_Bits, Color_Bits) + vbCrLf
        so = so + ";|" + vbCrLf
        so = so + ";| Define a below typedef in C,C++ code to accept BitMap resource" + vbCrLf
        so = so + ";|" + vbCrLf
        so = so + ";|" + vbTab + "typedef const uint8_t BitMap;" + vbCrLf
        so = so + ";|" + vbCrLf
        so = so + ";+" + sBar + vbCrLf + vbCrLf
        so = so + ";( IMAGE CONTENTS )" + vbCrLf + vbCrLf
        'Draw upper X axis
        Draw_Axis_X_1_Bit(so, bmp, True)
        'Draw Text Image
        Draw_Text_Image(so, bmp, Width_Bytes)
        'Draw lower X axis
        Draw_Axis_X_1_Bit(so, bmp, False)
        'End Comments
        so = so + "*/" + vbCrLf
        'DW Fields
        so = so + vbCrLf
        so = so + "BitMap BMP_" + ExtractFileName(openDialog.FileName) + "[] PROGMEM =" + vbCrLf
        so = so + "{" + vbCrLf
        so = so + vbTab + "//+------------+" + vbCrLf
        so = so + vbTab + "//| BMP Header |" + vbCrLf
        so = so + vbTab + "//+------------+" + vbCrLf
        so = so + vbTab + Hex2(Asc("B")) + "," + Hex2(Asc("M")) + "," + vbTab + "// BitMap Header ASCII(B)+ASCII(M)" + vbCrLf
        so = so + vbTab + Hex2(Color_Bits And &HFF) + "," + Hex2(Color_Bits \ 256) + "," + vbTab + String.Format("// {0,5} Bits Deep", Color_Bits) + vbCrLf
        so = so + vbTab + Hex2(Width_Pixels And &HFF) + "," + Hex2(Width_Pixels \ 256) + "," + vbTab + String.Format("// {0,5} Width in Pixels", Width_Pixels) + vbCrLf
        so = so + vbTab + Hex2(Height_Pixels And &HFF) + "," + Hex2(Height_Pixels \ 256) + "," + vbTab + String.Format("// {0,5} Height in Pixels", Height_Pixels) + vbCrLf
        so = so + vbTab + Hex2(Width_Bytes And &HFF) + "," + Hex2(Width_Bytes \ 256) + "," + vbTab + String.Format("// {0,5} Width in Bytes", Width_Bytes) + vbCrLf
        so = so + vbTab + Hex2(SizeBytes And &HFF) + "," + Hex2(SizeBytes \ 256) + "," + vbTab + String.Format("// {0,5} Size in Bytes", SizeBytes) + vbCrLf
        so = so + vbTab + "//+------------+" + vbCrLf
        so = so + vbTab + "//|  BMP Data  |" + vbCrLf
        so = so + vbTab + "//+------------+" + vbCrLf
        'Draw DBs
        Draw_DBs(so, bmp, Width_Bytes, True)
        'extract last comma(,)
        w = InStrRev(so, ",")
        If (w > 0) Then
            Mid(so, w, 1) = " "
        End If
        'close }
        so = so + "};" + vbCrLf
        'Set Hex To Output Text Control
        text_HEX_OUTPUT.Text = so
        'restore cursor state
        Me.Cursor = sCursor
    End Sub
    '
    '+#######################################################################################
    '# SAVE HEX GENERATE FILE IN FILE BMP_[FILE_NAME].INC IF ASSEMBLER FILE OR 
    '# BMP_[FILE_NAME].H IF C,C++ FILE
    '# 
    '+#######################################################################################
    '
    Private Sub buttom_SAVE_Click(sender As Object, e As EventArgs) Handles buttom_SAVE.Click
        Dim File_Out As String = ExtractFilePath(openDialog.FileName) + "BMP_" + ExtractFileName(openDialog.FileName)
        Dim sExt As String = ""
        Dim r As Integer
        'Check if no file 
        If (ExtractFileName(openDialog.FileName) = "") Then
            Beep(1000, 250)
            MsgBox("Image Not Loaded", vbOKOnly, "Warning!!!")
            Exit Sub
        End If
        'Save Assembler File?
        If AssemblerToolStripMenuItem.Checked Then
            sExt = ".INC"
        End If
        'Save C,C++ File?
        If CCToolStripMenuItem.Checked Then
            sExt = ".H"
        End If
        'check if file already exit and question if overwhite
        If IO.File.Exists(File_Out + sExt) Then
            Beep(1000, 250)
            r = MsgBox("File " + ExtractFileName(openDialog.FileName) + sExt + " ?", vbYesNo, "File Already Exist, Overwrite File ?")
            If (r = vbNo) Then Exit Sub
        End If
        'Write File
        Beep(1000, 200)
        IO.File.WriteAllText(File_Out + sExt, text_HEX_OUTPUT.Text)
        'Show Save Message
        MsgBox("BMP_" + ExtractFileName(openDialog.FileName) + sExt, vbOKOnly, "File Saved")
    End Sub
    '
    '+#######################################################################################
    '# Show Author
    '# 
    '+#######################################################################################
    '
    Private Sub ByToolStripMenuItem_Click(sender As Object, e As EventArgs) Handles ByToolStripMenuItem.Click
        MsgBox("Version " + Application.ProductVersion.ToString + vbCrLf + _
               "By " + "João D´Artagnan A. Oliveira", vbOKOnly, Application.ProductName)
    End Sub
    '
    '+#######################################################################################
    '# Copy Text Output to Clipboard
    '# 
    '+#######################################################################################
    '
    Private Sub buttom_COPY_TO_CLIPBOARD_Click(sender As Object, e As EventArgs) Handles buttom_COPY_TO_CLIPBOARD.Click
        If Trim(text_HEX_OUTPUT.Text) <> "" Then
            Clipboard.SetText(text_HEX_OUTPUT.Text)
            Beep(1000, 125)
            MsgBox("Copied to Clipboard", vbOKOnly, "Message!!!")
        End If
    End Sub

    Private Sub ConvertToolStripMenuItem_Click(sender As Object, e As EventArgs) Handles ConvertToolStripMenuItem.Click

    End Sub

    Private Sub ToolStripMenuItem1_Click(sender As Object, e As EventArgs) Handles ToolStripMenuItem1.Click

    End Sub

    Private Sub MenuStrip_ItemClicked(sender As Object, e As ToolStripItemClickedEventArgs) Handles MenuStrip.ItemClicked

    End Sub

    Private Sub ToolStripMenuItem2_Click(sender As Object, e As EventArgs) Handles ToolStripMenuItem2.Click
        'call GENERATE Button Event
        buttom_GENERATE_Click(sender, New EventArgs)
    End Sub

    Private Sub ToolStripMenuItem3_Click(sender As Object, e As EventArgs) Handles ToolStripMenuItem3.Click
        'call GENERATE Button Event
        buttom_COPY_TO_CLIPBOARD_Click(sender, New EventArgs)
    End Sub

    Private Sub ToolStripMenuItem4_Click(sender As Object, e As EventArgs) Handles ToolStripMenuItem4.Click
        'call GENERATE Button Event
        buttom_SAVE_Click(sender, New EventArgs)
    End Sub

    Private Sub NormalToolStripMenuItem_Click(sender As Object, e As EventArgs) Handles NormalToolStripMenuItem.Click
        NormalToolStripMenuItem.Checked = True
        InvertedToolStripMenuItem.Checked = False
        'call GENERATE Button Event
        buttom_GENERATE_Click(sender, New EventArgs)
    End Sub

    Private Sub InvertedToolStripMenuItem_Click(sender As Object, e As EventArgs) Handles InvertedToolStripMenuItem.Click
        NormalToolStripMenuItem.Checked = False
        InvertedToolStripMenuItem.Checked = True
        'call GENERATE Button Event
        buttom_GENERATE_Click(sender, New EventArgs)
    End Sub
End Class
