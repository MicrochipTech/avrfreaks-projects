VERSION 5.00
Begin VB.Form Form1 
   BorderStyle     =   3  'Fixed Dialog
   Caption         =   "AVR音乐测试"
   ClientHeight    =   4110
   ClientLeft      =   45
   ClientTop       =   495
   ClientWidth     =   5685
   LinkTopic       =   "Form1"
   MaxButton       =   0   'False
   MinButton       =   0   'False
   ScaleHeight     =   4110
   ScaleWidth      =   5685
   ShowInTaskbar   =   0   'False
   StartUpPosition =   3  '窗口缺省
   Begin VB.CommandButton Command1 
      Caption         =   "播放"
      BeginProperty Font 
         Name            =   "宋体"
         Size            =   12
         Charset         =   134
         Weight          =   700
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      Height          =   615
      Left            =   3240
      TabIndex        =   6
      Top             =   2400
      Width           =   1335
   End
   Begin VB.CheckBox Check4 
      Caption         =   "Noise 杂波"
      BeginProperty Font 
         Name            =   "宋体"
         Size            =   12
         Charset         =   134
         Weight          =   400
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      Height          =   375
      Left            =   600
      TabIndex        =   5
      Top             =   2640
      Value           =   1  'Checked
      Width           =   1695
   End
   Begin VB.CheckBox Check3 
      Caption         =   "Tri 三角波"
      BeginProperty Font 
         Name            =   "宋体"
         Size            =   12
         Charset         =   134
         Weight          =   400
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      Height          =   495
      Left            =   600
      TabIndex        =   3
      Top             =   2040
      Value           =   1  'Checked
      Width           =   1695
   End
   Begin VB.CheckBox Check1 
      Caption         =   "Rect0 方波"
      BeginProperty Font 
         Name            =   "宋体"
         Size            =   12
         Charset         =   134
         Weight          =   400
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      Height          =   375
      Left            =   600
      TabIndex        =   2
      Top             =   1080
      Value           =   1  'Checked
      Width           =   1695
   End
   Begin VB.CheckBox Check2 
      Caption         =   "Rect1 方波"
      BeginProperty Font 
         Name            =   "宋体"
         Size            =   12
         Charset         =   134
         Weight          =   400
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      Height          =   375
      Left            =   600
      TabIndex        =   1
      Top             =   1560
      Value           =   1  'Checked
      Width           =   1695
   End
   Begin VB.Shape Shape1 
      FillStyle       =   0  'Solid
      Height          =   255
      Index           =   0
      Left            =   2280
      Top             =   1080
      Width           =   255
   End
   Begin VB.Shape Shape1 
      FillStyle       =   0  'Solid
      Height          =   255
      Index           =   1
      Left            =   2280
      Top             =   1605
      Width           =   255
   End
   Begin VB.Shape Shape1 
      FillStyle       =   0  'Solid
      Height          =   255
      Index           =   2
      Left            =   2280
      Top             =   2085
      Width           =   255
   End
   Begin VB.Shape Shape1 
      FillStyle       =   0  'Solid
      Height          =   255
      Index           =   3
      Left            =   2280
      Top             =   2685
      Width           =   255
   End
   Begin VB.Line Line1 
      X1              =   120
      X2              =   5520
      Y1              =   3600
      Y2              =   3600
   End
   Begin VB.Label Label5 
      Caption         =   "AVRNES Music Player1.2"
      BeginProperty Font 
         Name            =   "宋体"
         Size            =   18
         Charset         =   134
         Weight          =   700
         Underline       =   0   'False
         Italic          =   -1  'True
         Strikethrough   =   0   'False
      EndProperty
      ForeColor       =   &H000080FF&
      Height          =   375
      Left            =   600
      TabIndex        =   9
      Top             =   0
      Width           =   4695
   End
   Begin VB.Label Label4 
      AutoSize        =   -1  'True
      Caption         =   "1"
      BeginProperty Font 
         Name            =   "宋体"
         Size            =   12
         Charset         =   134
         Weight          =   700
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      Height          =   240
      Left            =   2400
      TabIndex        =   8
      Top             =   3720
      Width           =   135
   End
   Begin VB.Label Label3 
      AutoSize        =   -1  'True
      Caption         =   "消耗AVR FLASH容量："
      BeginProperty Font 
         Name            =   "宋体"
         Size            =   12
         Charset         =   134
         Weight          =   400
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      Height          =   240
      Left            =   120
      TabIndex        =   7
      Top             =   3720
      Width           =   2280
   End
   Begin VB.Label Label1 
      AutoSize        =   -1  'True
      Caption         =   "当前帧："
      BeginProperty Font 
         Name            =   "宋体"
         Size            =   12
         Charset         =   134
         Weight          =   400
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      Height          =   240
      Left            =   3840
      TabIndex        =   4
      Top             =   3720
      Width           =   960
   End
   Begin VB.Label Label2 
      AutoSize        =   -1  'True
      Caption         =   "1"
      BeginProperty Font 
         Name            =   "宋体"
         Size            =   12
         Charset         =   134
         Weight          =   700
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      Height          =   240
      Left            =   4920
      TabIndex        =   0
      Top             =   3720
      Width           =   135
   End
End
Attribute VB_Name = "Form1"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Option Explicit
'NES音乐播放器   AVRNES Music Player1.2

'          BY张皓
'http://www.lotusinvention.cn/ART/nes/NESMusic.htm

Private Declare Function midiOutShortMsg Lib "winmm.dll" (ByVal hMidiOut As Long, ByVal dwMsg As Long) As Long
Private Declare Function midiOutOpen Lib "winmm.dll" (lphMidiOut As Long, ByVal uDeviceID As Long, ByVal dwCallback As Long, ByVal dwInstance As Long, ByVal dwFlags As Long) As Long
Private Declare Function midiOutClose Lib "winmm.dll" (ByVal hMidiOut As Long) As Long
Private Declare Sub Sleep Lib "kernel32" (ByVal dwMilliseconds As Long)
Private hMidi As Long

Private UseMemory As Long '使用的AVR FLASH容量大小

'格式：  频率 | 音量 | 开始时间
Private AVRRect0data(-1 To 5000, 3) As Integer
Private AVRRect1data(-1 To 5000, 3) As Integer
Private AVRTriangledata(-1 To 5000, 2) As Integer
Private AVRNoisedata(-1 To 5000, 2) As Integer
Dim t As Long

Private Sub Command1_Click()
    playMusic
End Sub

Private Sub Form_Load()
    Show
    midiOutOpen hMidi, -1, 0, 0, 0
    SelectInstrument 0, 80 'Square wave 方波0使用方波
    SelectInstrument 1, 80 'Square wave 方波1使用方波
    SelectInstrument 2, 80 'Triangle wave 三角波
    SelectInstrument 3, 80 'Noise wave 杂波

    LoadFile
    Label4 = Round(UseMemory / 1000, 1) & "K"

    playMusic
End Sub

Private Sub Form_Unload(Cancel As Integer)
    midiOutClose hMidi
    End
End Sub

'演奏音乐
Public Sub playMusic()
    t = 0
    '频率 | 音量 | 开始时间
    Dim i0 As Long, i1 As Long, i2 As Long, i3 As Long
    Dim Tone(3) As Long '频率
    Dim volume(3) As Long '音量
    '    Dim channel(3) As Long '通道

    Do

        TimeDelay2 1 * 1000 / 60
        t = t + 1
        Label2 = t

        '&&&&&&&&&&&&&$$$$$ 到尾部了，从头开始
        If AVRRect0data(i0, 0) = 9999 Then
            Call midiOutShortMsg(hMidi, 0 + 127 * &H10000 + Tone(0) * &H100 + &H80) '关闭最后的
            Call midiOutShortMsg(hMidi, 1 + 127 * &H10000 + Tone(1) * &H100 + &H80) '关闭最后的
            Call midiOutShortMsg(hMidi, 2 + 127 * &H10000 + Tone(2) * &H100 + &H80)
            Call midiOutShortMsg(hMidi, 3 + 127 * &H10000 + Tone(3) * &H100 + &H80)

            Exit Do '结束
        End If

        '处理方波0  &&&&&&&&&&&&&$$$$$
        processRect Check1.Value, AVRRect0data, i0, 0, Tone(0), volume(0)
        '方波1 &&&&&&&&&&&&&$$$$$
        processRect Check2.Value, AVRRect1data, i1, 1, Tone(1), volume(1)

        '三角波&&&&&&&&&&&&&$$$$$
        processTriangleNoise Check3.Value, AVRTriangledata, i2, 2, Tone(2)
        '杂波
        processTriangleNoise Check4.Value, AVRNoisedata, i3, 3, Tone(3)

    Loop

End Sub

Public Sub SelectInstrument(ByVal channel As Long, ByVal patch As Long) '改变音乐乐器
    midiOutShortMsg hMidi, &HC0 Or patch * 256 Or channel
End Sub

'处理方波
Public Sub processRect(Value As Integer, ByRef da() As Integer, ByRef i As Long, ByRef channel As Long, ByRef Tone As Long, ByRef volume As Long)
    If (t >= da(i - 1, 3)) Then
    Call midiOutShortMsg(hMidi, channel + 127 * &H10000 + Tone * &H100 + &H80) '是否|结束时间|了
    Shape1(channel).FillColor = 0 '标签变黑色
    End If
    
    If (t >= da(i, 2)) Then  '到了
        Call midiOutShortMsg(hMidi, channel + 127 * &H10000 + Tone * &H100 + &H80) '先关闭先前的

        Tone = getMIDInum(da(i, 0))
        volume = da(i, 1) * 8
        If Value Then Call midiOutShortMsg(hMidi, channel + volume * &H10000 + Tone * &H100 + &H90)
        Shape1(channel).FillColor = &HC000& '标签变绿色
        i = i + 1
    End If
End Sub

'处理三角波
Public Sub processTriangleNoise(Value As Integer, ByRef da() As Integer, ByRef i As Long, ByRef channel As Long, ByRef Tone As Long)
    If (t >= da(i - 1, 2)) Then
    Call midiOutShortMsg(hMidi, channel + 127 * &H10000 + Tone * &H100 + &H80) '是否|结束时间|了
    Shape1(channel).FillColor = 0 '标签变黑色
    End If
    
    If (t >= da(i, 1)) Then
        Call midiOutShortMsg(hMidi, channel + 127 * &H10000 + Tone * &H100 + &H80) '先关闭先前的
        Tone = getMIDInum(da(i, 0))
        If Value Then Call midiOutShortMsg(hMidi, channel + 6 * 8 * &H10000 + Tone * &H100 + &H90)
        Shape1(channel).FillColor = &HC000& '标签变绿色
        i = i + 1
    End If
End Sub

Public Function getMIDInum(ByVal AVRtone As Long) As Long
    Dim freq As Single
    If AVRtone = 0 Then Exit Function

    freq = 20000 / (AVRtone * 2 / 2) '0.05ms 音频采样
    getMIDInum = CLng(Log(freq / 8.176) * 17.31234) '计算出midi的编号

End Function

'音乐文件读取 load Music
Public Sub LoadFile()

    Open App.Path & "\" & "..\AVRdata.txt" For Input As #1   ' 打开输入文件。

    LoadRectWave AVRRect0data '读取方波0
    LoadRectWave AVRRect1data '读取方波1
    LoadTriangNoiseWave AVRTriangledata '读取三角波
    LoadTriangNoiseWave AVRNoisedata '读取杂波
    Close #1
End Sub

'读取方波
Public Sub LoadRectWave(ByRef da() As Integer)
    Dim MyString1 As String, MyString2 As String, MyString3 As String, MyString4 As String
    Dim i As Long
    Do While Not EOF(1)
        Input #1, MyString1, MyString2, MyString3, MyString4
        Clearupstr MyString1
        da(i, 0) = Val(MyString1)
        da(i, 1) = Val(MyString2)
        da(i, 2) = Val(MyString3)
        da(i, 3) = Val(MyString4)
        UseMemory = UseMemory + 8
        If InStr(MyString4, "}") Then Exit Do
        i = i + 1
    Loop
End Sub

'读取三角波/杂波
Public Sub LoadTriangNoiseWave(ByRef da() As Integer)
    Dim MyString1 As String, MyString2 As String, MyString3 As String
    Dim i As Long
    Do While Not EOF(1)
        Input #1, MyString1, MyString2, MyString3
        Clearupstr MyString1
        da(i, 0) = Val(MyString1)
        da(i, 1) = Val(MyString2)
        da(i, 2) = Val(MyString3)
        UseMemory = UseMemory + 6
        If InStr(MyString3, "}") Then Exit Do
        i = i + 1
    Loop
End Sub

Public Sub Clearupstr(ByRef s As String) '消除 {
    Dim MyPos As Long, x
    MyPos = InStr(s, "{")
    If MyPos Then
        x = Split(s, "{")
        s = x(1) '取右边
    End If
End Sub
