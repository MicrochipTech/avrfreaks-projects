VERSION 5.00
Object = "{831FDD16-0C5C-11D2-A9FC-0000F8754DA1}#2.0#0"; "Mscomctl.ocx"
Object = "{4580EBBB-FE3D-45CF-8543-600A62B38A73}#1.1#0"; "NETComm.ocx"
Object = "{BDC217C8-ED16-11CD-956C-0000C04E4C0A}#1.1#0"; "TABCTL32.OCX"
Begin VB.Form Form1 
   BackColor       =   &H00E0E0E0&
   BorderStyle     =   1  'Fixed Single
   Caption         =   "Channel Level Control"
   ClientHeight    =   6840
   ClientLeft      =   150
   ClientTop       =   840
   ClientWidth     =   14355
   Icon            =   "Form1.frx":0000
   LinkTopic       =   "Form1"
   MaxButton       =   0   'False
   MinButton       =   0   'False
   ScaleHeight     =   6840
   ScaleWidth      =   14355
   StartUpPosition =   3  'Windows Default
   Begin VB.CommandButton BtnSIN 
      Caption         =   "SIN"
      Height          =   375
      Left            =   1800
      TabIndex        =   220
      Top             =   6240
      Width           =   1455
   End
   Begin TabDlg.SSTab SSTab1 
      Height          =   5055
      Left            =   600
      TabIndex        =   3
      Top             =   240
      Width           =   13335
      _ExtentX        =   23521
      _ExtentY        =   8916
      _Version        =   393216
      TabOrientation  =   1
      Tabs            =   6
      TabsPerRow      =   6
      TabHeight       =   520
      TabCaption(0)   =   "1 - 18"
      TabPicture(0)   =   "Form1.frx":0442
      Tab(0).ControlEnabled=   -1  'True
      Tab(0).Control(0)=   "Label1(17)"
      Tab(0).Control(0).Enabled=   0   'False
      Tab(0).Control(1)=   "Label1(16)"
      Tab(0).Control(1).Enabled=   0   'False
      Tab(0).Control(2)=   "Label1(15)"
      Tab(0).Control(2).Enabled=   0   'False
      Tab(0).Control(3)=   "Label1(14)"
      Tab(0).Control(3).Enabled=   0   'False
      Tab(0).Control(4)=   "Label1(13)"
      Tab(0).Control(4).Enabled=   0   'False
      Tab(0).Control(5)=   "Label1(12)"
      Tab(0).Control(5).Enabled=   0   'False
      Tab(0).Control(6)=   "Label1(11)"
      Tab(0).Control(6).Enabled=   0   'False
      Tab(0).Control(7)=   "Label1(10)"
      Tab(0).Control(7).Enabled=   0   'False
      Tab(0).Control(8)=   "Label1(9)"
      Tab(0).Control(8).Enabled=   0   'False
      Tab(0).Control(9)=   "Label1(8)"
      Tab(0).Control(9).Enabled=   0   'False
      Tab(0).Control(10)=   "Label1(7)"
      Tab(0).Control(10).Enabled=   0   'False
      Tab(0).Control(11)=   "Label1(6)"
      Tab(0).Control(11).Enabled=   0   'False
      Tab(0).Control(12)=   "Label1(5)"
      Tab(0).Control(12).Enabled=   0   'False
      Tab(0).Control(13)=   "Label1(4)"
      Tab(0).Control(13).Enabled=   0   'False
      Tab(0).Control(14)=   "Label1(3)"
      Tab(0).Control(14).Enabled=   0   'False
      Tab(0).Control(15)=   "Label1(2)"
      Tab(0).Control(15).Enabled=   0   'False
      Tab(0).Control(16)=   "Label1(1)"
      Tab(0).Control(16).Enabled=   0   'False
      Tab(0).Control(17)=   "Label1(0)"
      Tab(0).Control(17).Enabled=   0   'False
      Tab(0).Control(18)=   "DimmerLevel(17)"
      Tab(0).Control(18).Enabled=   0   'False
      Tab(0).Control(19)=   "DimmerLevel(16)"
      Tab(0).Control(19).Enabled=   0   'False
      Tab(0).Control(20)=   "DimmerLevel(15)"
      Tab(0).Control(20).Enabled=   0   'False
      Tab(0).Control(21)=   "DimmerLevel(14)"
      Tab(0).Control(21).Enabled=   0   'False
      Tab(0).Control(22)=   "DimmerLevel(13)"
      Tab(0).Control(22).Enabled=   0   'False
      Tab(0).Control(23)=   "DimmerLevel(12)"
      Tab(0).Control(23).Enabled=   0   'False
      Tab(0).Control(24)=   "DimmerLevel(11)"
      Tab(0).Control(24).Enabled=   0   'False
      Tab(0).Control(25)=   "DimmerLevel(10)"
      Tab(0).Control(25).Enabled=   0   'False
      Tab(0).Control(26)=   "DimmerLevel(9)"
      Tab(0).Control(26).Enabled=   0   'False
      Tab(0).Control(27)=   "DimmerLevel(8)"
      Tab(0).Control(27).Enabled=   0   'False
      Tab(0).Control(28)=   "DimmerLevel(7)"
      Tab(0).Control(28).Enabled=   0   'False
      Tab(0).Control(29)=   "DimmerLevel(6)"
      Tab(0).Control(29).Enabled=   0   'False
      Tab(0).Control(30)=   "DimmerLevel(5)"
      Tab(0).Control(30).Enabled=   0   'False
      Tab(0).Control(31)=   "DimmerLevel(4)"
      Tab(0).Control(31).Enabled=   0   'False
      Tab(0).Control(32)=   "DimmerLevel(3)"
      Tab(0).Control(32).Enabled=   0   'False
      Tab(0).Control(33)=   "DimmerLevel(2)"
      Tab(0).Control(33).Enabled=   0   'False
      Tab(0).Control(34)=   "DimmerLevel(1)"
      Tab(0).Control(34).Enabled=   0   'False
      Tab(0).Control(35)=   "DimmerLevel(0)"
      Tab(0).Control(35).Enabled=   0   'False
      Tab(0).ControlCount=   36
      TabCaption(1)   =   "19 - 36"
      TabPicture(1)   =   "Form1.frx":045E
      Tab(1).ControlEnabled=   0   'False
      Tab(1).Control(0)=   "Label1(18)"
      Tab(1).Control(1)=   "Label1(19)"
      Tab(1).Control(2)=   "Label1(20)"
      Tab(1).Control(3)=   "Label1(21)"
      Tab(1).Control(4)=   "Label1(22)"
      Tab(1).Control(5)=   "Label1(23)"
      Tab(1).Control(6)=   "Label1(24)"
      Tab(1).Control(7)=   "Label1(25)"
      Tab(1).Control(8)=   "Label1(26)"
      Tab(1).Control(9)=   "Label1(27)"
      Tab(1).Control(10)=   "Label1(28)"
      Tab(1).Control(11)=   "Label1(29)"
      Tab(1).Control(12)=   "Label1(30)"
      Tab(1).Control(13)=   "Label1(31)"
      Tab(1).Control(14)=   "Label1(32)"
      Tab(1).Control(15)=   "Label1(33)"
      Tab(1).Control(16)=   "Label1(34)"
      Tab(1).Control(17)=   "Label1(35)"
      Tab(1).Control(18)=   "DimmerLevel(35)"
      Tab(1).Control(19)=   "DimmerLevel(34)"
      Tab(1).Control(20)=   "DimmerLevel(33)"
      Tab(1).Control(21)=   "DimmerLevel(32)"
      Tab(1).Control(22)=   "DimmerLevel(31)"
      Tab(1).Control(23)=   "DimmerLevel(30)"
      Tab(1).Control(24)=   "DimmerLevel(29)"
      Tab(1).Control(25)=   "DimmerLevel(28)"
      Tab(1).Control(26)=   "DimmerLevel(27)"
      Tab(1).Control(27)=   "DimmerLevel(26)"
      Tab(1).Control(28)=   "DimmerLevel(25)"
      Tab(1).Control(29)=   "DimmerLevel(24)"
      Tab(1).Control(30)=   "DimmerLevel(23)"
      Tab(1).Control(31)=   "DimmerLevel(22)"
      Tab(1).Control(32)=   "DimmerLevel(21)"
      Tab(1).Control(33)=   "DimmerLevel(20)"
      Tab(1).Control(34)=   "DimmerLevel(19)"
      Tab(1).Control(35)=   "DimmerLevel(18)"
      Tab(1).ControlCount=   36
      TabCaption(2)   =   "37 - 54"
      TabPicture(2)   =   "Form1.frx":047A
      Tab(2).ControlEnabled=   0   'False
      Tab(2).Control(0)=   "Label1(36)"
      Tab(2).Control(1)=   "Label1(37)"
      Tab(2).Control(2)=   "Label1(38)"
      Tab(2).Control(3)=   "Label1(39)"
      Tab(2).Control(4)=   "Label1(40)"
      Tab(2).Control(5)=   "Label1(41)"
      Tab(2).Control(6)=   "Label1(42)"
      Tab(2).Control(7)=   "Label1(43)"
      Tab(2).Control(8)=   "Label1(44)"
      Tab(2).Control(9)=   "Label1(45)"
      Tab(2).Control(10)=   "Label1(46)"
      Tab(2).Control(11)=   "Label1(47)"
      Tab(2).Control(12)=   "Label1(48)"
      Tab(2).Control(13)=   "Label1(49)"
      Tab(2).Control(14)=   "Label1(50)"
      Tab(2).Control(15)=   "Label1(51)"
      Tab(2).Control(16)=   "Label1(52)"
      Tab(2).Control(17)=   "Label1(53)"
      Tab(2).Control(18)=   "DimmerLevel(53)"
      Tab(2).Control(19)=   "DimmerLevel(52)"
      Tab(2).Control(20)=   "DimmerLevel(51)"
      Tab(2).Control(21)=   "DimmerLevel(50)"
      Tab(2).Control(22)=   "DimmerLevel(49)"
      Tab(2).Control(23)=   "DimmerLevel(48)"
      Tab(2).Control(24)=   "DimmerLevel(47)"
      Tab(2).Control(25)=   "DimmerLevel(46)"
      Tab(2).Control(26)=   "DimmerLevel(45)"
      Tab(2).Control(27)=   "DimmerLevel(44)"
      Tab(2).Control(28)=   "DimmerLevel(43)"
      Tab(2).Control(29)=   "DimmerLevel(42)"
      Tab(2).Control(30)=   "DimmerLevel(41)"
      Tab(2).Control(31)=   "DimmerLevel(40)"
      Tab(2).Control(32)=   "DimmerLevel(39)"
      Tab(2).Control(33)=   "DimmerLevel(38)"
      Tab(2).Control(34)=   "DimmerLevel(37)"
      Tab(2).Control(35)=   "DimmerLevel(36)"
      Tab(2).ControlCount=   36
      TabCaption(3)   =   "55 - 72"
      TabPicture(3)   =   "Form1.frx":0496
      Tab(3).ControlEnabled=   0   'False
      Tab(3).Control(0)=   "Label1(54)"
      Tab(3).Control(1)=   "Label1(55)"
      Tab(3).Control(2)=   "Label1(56)"
      Tab(3).Control(3)=   "Label1(57)"
      Tab(3).Control(4)=   "Label1(58)"
      Tab(3).Control(5)=   "Label1(59)"
      Tab(3).Control(6)=   "Label1(60)"
      Tab(3).Control(7)=   "Label1(61)"
      Tab(3).Control(8)=   "Label1(62)"
      Tab(3).Control(9)=   "Label1(63)"
      Tab(3).Control(10)=   "Label1(64)"
      Tab(3).Control(11)=   "Label1(65)"
      Tab(3).Control(12)=   "Label1(66)"
      Tab(3).Control(13)=   "Label1(67)"
      Tab(3).Control(14)=   "Label1(68)"
      Tab(3).Control(15)=   "Label1(69)"
      Tab(3).Control(16)=   "Label1(70)"
      Tab(3).Control(17)=   "Label1(71)"
      Tab(3).Control(18)=   "DimmerLevel(71)"
      Tab(3).Control(19)=   "DimmerLevel(70)"
      Tab(3).Control(20)=   "DimmerLevel(69)"
      Tab(3).Control(21)=   "DimmerLevel(68)"
      Tab(3).Control(22)=   "DimmerLevel(67)"
      Tab(3).Control(23)=   "DimmerLevel(66)"
      Tab(3).Control(24)=   "DimmerLevel(65)"
      Tab(3).Control(25)=   "DimmerLevel(64)"
      Tab(3).Control(26)=   "DimmerLevel(63)"
      Tab(3).Control(27)=   "DimmerLevel(62)"
      Tab(3).Control(28)=   "DimmerLevel(61)"
      Tab(3).Control(29)=   "DimmerLevel(60)"
      Tab(3).Control(30)=   "DimmerLevel(59)"
      Tab(3).Control(31)=   "DimmerLevel(58)"
      Tab(3).Control(32)=   "DimmerLevel(57)"
      Tab(3).Control(33)=   "DimmerLevel(56)"
      Tab(3).Control(34)=   "DimmerLevel(55)"
      Tab(3).Control(35)=   "DimmerLevel(54)"
      Tab(3).ControlCount=   36
      TabCaption(4)   =   "73 - 90"
      TabPicture(4)   =   "Form1.frx":04B2
      Tab(4).ControlEnabled=   0   'False
      Tab(4).Control(0)=   "Label1(72)"
      Tab(4).Control(1)=   "Label1(73)"
      Tab(4).Control(2)=   "Label1(74)"
      Tab(4).Control(3)=   "Label1(75)"
      Tab(4).Control(4)=   "Label1(76)"
      Tab(4).Control(5)=   "Label1(77)"
      Tab(4).Control(6)=   "Label1(78)"
      Tab(4).Control(7)=   "Label1(79)"
      Tab(4).Control(8)=   "Label1(80)"
      Tab(4).Control(9)=   "Label1(81)"
      Tab(4).Control(10)=   "Label1(82)"
      Tab(4).Control(11)=   "Label1(83)"
      Tab(4).Control(12)=   "Label1(84)"
      Tab(4).Control(13)=   "Label1(85)"
      Tab(4).Control(14)=   "Label1(86)"
      Tab(4).Control(15)=   "Label1(87)"
      Tab(4).Control(16)=   "Label1(88)"
      Tab(4).Control(17)=   "Label1(89)"
      Tab(4).Control(18)=   "DimmerLevel(89)"
      Tab(4).Control(19)=   "DimmerLevel(88)"
      Tab(4).Control(20)=   "DimmerLevel(87)"
      Tab(4).Control(21)=   "DimmerLevel(86)"
      Tab(4).Control(22)=   "DimmerLevel(85)"
      Tab(4).Control(23)=   "DimmerLevel(84)"
      Tab(4).Control(24)=   "DimmerLevel(83)"
      Tab(4).Control(25)=   "DimmerLevel(82)"
      Tab(4).Control(26)=   "DimmerLevel(81)"
      Tab(4).Control(27)=   "DimmerLevel(80)"
      Tab(4).Control(28)=   "DimmerLevel(79)"
      Tab(4).Control(29)=   "DimmerLevel(78)"
      Tab(4).Control(30)=   "DimmerLevel(77)"
      Tab(4).Control(31)=   "DimmerLevel(76)"
      Tab(4).Control(32)=   "DimmerLevel(75)"
      Tab(4).Control(33)=   "DimmerLevel(74)"
      Tab(4).Control(34)=   "DimmerLevel(73)"
      Tab(4).Control(35)=   "DimmerLevel(72)"
      Tab(4).ControlCount=   36
      TabCaption(5)   =   "91 - 108"
      TabPicture(5)   =   "Form1.frx":04CE
      Tab(5).ControlEnabled=   0   'False
      Tab(5).Control(0)=   "Label1(90)"
      Tab(5).Control(1)=   "Label1(91)"
      Tab(5).Control(2)=   "Label1(92)"
      Tab(5).Control(3)=   "Label1(93)"
      Tab(5).Control(4)=   "Label1(94)"
      Tab(5).Control(5)=   "Label1(95)"
      Tab(5).Control(6)=   "Label1(96)"
      Tab(5).Control(7)=   "Label1(97)"
      Tab(5).Control(8)=   "Label1(98)"
      Tab(5).Control(9)=   "Label1(99)"
      Tab(5).Control(10)=   "Label1(100)"
      Tab(5).Control(11)=   "Label1(101)"
      Tab(5).Control(12)=   "Label1(102)"
      Tab(5).Control(13)=   "Label1(103)"
      Tab(5).Control(14)=   "Label1(104)"
      Tab(5).Control(15)=   "Label1(105)"
      Tab(5).Control(16)=   "Label1(106)"
      Tab(5).Control(17)=   "Label1(107)"
      Tab(5).Control(18)=   "DimmerLevel(107)"
      Tab(5).Control(19)=   "DimmerLevel(106)"
      Tab(5).Control(20)=   "DimmerLevel(105)"
      Tab(5).Control(21)=   "DimmerLevel(104)"
      Tab(5).Control(22)=   "DimmerLevel(103)"
      Tab(5).Control(23)=   "DimmerLevel(102)"
      Tab(5).Control(24)=   "DimmerLevel(101)"
      Tab(5).Control(25)=   "DimmerLevel(100)"
      Tab(5).Control(26)=   "DimmerLevel(99)"
      Tab(5).Control(27)=   "DimmerLevel(98)"
      Tab(5).Control(28)=   "DimmerLevel(97)"
      Tab(5).Control(29)=   "DimmerLevel(96)"
      Tab(5).Control(30)=   "DimmerLevel(95)"
      Tab(5).Control(31)=   "DimmerLevel(94)"
      Tab(5).Control(32)=   "DimmerLevel(93)"
      Tab(5).Control(33)=   "DimmerLevel(92)"
      Tab(5).Control(34)=   "DimmerLevel(91)"
      Tab(5).Control(35)=   "DimmerLevel(90)"
      Tab(5).ControlCount=   36
      Begin MSComctlLib.Slider DimmerLevel 
         Height          =   4095
         Index           =   0
         Left            =   240
         TabIndex        =   4
         Top             =   480
         Width           =   675
         _ExtentX        =   1191
         _ExtentY        =   7223
         _Version        =   393216
         Orientation     =   1
         LargeChange     =   10
         Max             =   255
         TickStyle       =   2
         TickFrequency   =   10
      End
      Begin MSComctlLib.Slider DimmerLevel 
         Height          =   4095
         Index           =   1
         Left            =   960
         TabIndex        =   5
         Top             =   480
         Width           =   675
         _ExtentX        =   1191
         _ExtentY        =   7223
         _Version        =   393216
         Orientation     =   1
         LargeChange     =   10
         Max             =   255
         TickStyle       =   2
         TickFrequency   =   10
      End
      Begin MSComctlLib.Slider DimmerLevel 
         Height          =   4095
         Index           =   2
         Left            =   1680
         TabIndex        =   6
         Top             =   480
         Width           =   675
         _ExtentX        =   1191
         _ExtentY        =   7223
         _Version        =   393216
         Orientation     =   1
         LargeChange     =   10
         Max             =   255
         TickStyle       =   2
         TickFrequency   =   10
      End
      Begin MSComctlLib.Slider DimmerLevel 
         Height          =   4095
         Index           =   3
         Left            =   2400
         TabIndex        =   7
         Top             =   480
         Width           =   675
         _ExtentX        =   1191
         _ExtentY        =   7223
         _Version        =   393216
         Orientation     =   1
         LargeChange     =   10
         Max             =   255
         TickStyle       =   2
         TickFrequency   =   10
      End
      Begin MSComctlLib.Slider DimmerLevel 
         Height          =   4095
         Index           =   4
         Left            =   3120
         TabIndex        =   8
         Top             =   480
         Width           =   675
         _ExtentX        =   1191
         _ExtentY        =   7223
         _Version        =   393216
         Orientation     =   1
         LargeChange     =   10
         Max             =   255
         TickStyle       =   2
         TickFrequency   =   10
      End
      Begin MSComctlLib.Slider DimmerLevel 
         Height          =   4095
         Index           =   5
         Left            =   3840
         TabIndex        =   9
         Top             =   480
         Width           =   675
         _ExtentX        =   1191
         _ExtentY        =   7223
         _Version        =   393216
         Orientation     =   1
         LargeChange     =   10
         Max             =   255
         TickStyle       =   2
         TickFrequency   =   10
      End
      Begin MSComctlLib.Slider DimmerLevel 
         Height          =   4095
         Index           =   6
         Left            =   4560
         TabIndex        =   10
         Top             =   480
         Width           =   675
         _ExtentX        =   1191
         _ExtentY        =   7223
         _Version        =   393216
         Orientation     =   1
         LargeChange     =   10
         Max             =   255
         TickStyle       =   2
         TickFrequency   =   10
      End
      Begin MSComctlLib.Slider DimmerLevel 
         Height          =   4095
         Index           =   7
         Left            =   5280
         TabIndex        =   11
         Top             =   480
         Width           =   675
         _ExtentX        =   1191
         _ExtentY        =   7223
         _Version        =   393216
         Orientation     =   1
         LargeChange     =   10
         Max             =   255
         TickStyle       =   2
         TickFrequency   =   10
      End
      Begin MSComctlLib.Slider DimmerLevel 
         Height          =   4095
         Index           =   8
         Left            =   6000
         TabIndex        =   12
         Top             =   480
         Width           =   675
         _ExtentX        =   1191
         _ExtentY        =   7223
         _Version        =   393216
         Orientation     =   1
         LargeChange     =   10
         Max             =   255
         TickStyle       =   2
         TickFrequency   =   10
      End
      Begin MSComctlLib.Slider DimmerLevel 
         Height          =   4095
         Index           =   9
         Left            =   6720
         TabIndex        =   13
         Top             =   480
         Width           =   675
         _ExtentX        =   1191
         _ExtentY        =   7223
         _Version        =   393216
         Orientation     =   1
         LargeChange     =   10
         Max             =   255
         TickStyle       =   2
         TickFrequency   =   10
      End
      Begin MSComctlLib.Slider DimmerLevel 
         Height          =   4095
         Index           =   10
         Left            =   7440
         TabIndex        =   14
         Top             =   480
         Width           =   675
         _ExtentX        =   1191
         _ExtentY        =   7223
         _Version        =   393216
         Orientation     =   1
         LargeChange     =   10
         Max             =   255
         TickStyle       =   2
         TickFrequency   =   10
      End
      Begin MSComctlLib.Slider DimmerLevel 
         Height          =   4095
         Index           =   11
         Left            =   8160
         TabIndex        =   15
         Top             =   480
         Width           =   675
         _ExtentX        =   1191
         _ExtentY        =   7223
         _Version        =   393216
         Orientation     =   1
         LargeChange     =   10
         Max             =   255
         TickStyle       =   2
         TickFrequency   =   10
      End
      Begin MSComctlLib.Slider DimmerLevel 
         Height          =   4095
         Index           =   12
         Left            =   8880
         TabIndex        =   16
         Top             =   480
         Width           =   675
         _ExtentX        =   1191
         _ExtentY        =   7223
         _Version        =   393216
         Orientation     =   1
         LargeChange     =   10
         Max             =   255
         TickStyle       =   2
         TickFrequency   =   10
      End
      Begin MSComctlLib.Slider DimmerLevel 
         Height          =   4095
         Index           =   13
         Left            =   9600
         TabIndex        =   17
         Top             =   480
         Width           =   675
         _ExtentX        =   1191
         _ExtentY        =   7223
         _Version        =   393216
         Orientation     =   1
         LargeChange     =   10
         Max             =   255
         TickStyle       =   2
         TickFrequency   =   10
      End
      Begin MSComctlLib.Slider DimmerLevel 
         Height          =   4095
         Index           =   14
         Left            =   10320
         TabIndex        =   18
         Top             =   480
         Width           =   675
         _ExtentX        =   1191
         _ExtentY        =   7223
         _Version        =   393216
         Orientation     =   1
         LargeChange     =   10
         Max             =   255
         TickStyle       =   2
         TickFrequency   =   10
      End
      Begin MSComctlLib.Slider DimmerLevel 
         Height          =   4095
         Index           =   15
         Left            =   11040
         TabIndex        =   19
         Top             =   480
         Width           =   675
         _ExtentX        =   1191
         _ExtentY        =   7223
         _Version        =   393216
         Orientation     =   1
         LargeChange     =   10
         Max             =   255
         TickStyle       =   2
         TickFrequency   =   10
      End
      Begin MSComctlLib.Slider DimmerLevel 
         Height          =   4095
         Index           =   16
         Left            =   11760
         TabIndex        =   20
         Top             =   480
         Width           =   675
         _ExtentX        =   1191
         _ExtentY        =   7223
         _Version        =   393216
         Orientation     =   1
         LargeChange     =   10
         Max             =   255
         TickStyle       =   2
         TickFrequency   =   10
      End
      Begin MSComctlLib.Slider DimmerLevel 
         Height          =   4095
         Index           =   17
         Left            =   12480
         TabIndex        =   21
         Top             =   480
         Width           =   675
         _ExtentX        =   1191
         _ExtentY        =   7223
         _Version        =   393216
         Orientation     =   1
         LargeChange     =   10
         Max             =   255
         TickStyle       =   2
         TickFrequency   =   10
      End
      Begin MSComctlLib.Slider DimmerLevel 
         Height          =   4095
         Index           =   18
         Left            =   -74760
         TabIndex        =   40
         Top             =   480
         Width           =   675
         _ExtentX        =   1191
         _ExtentY        =   7223
         _Version        =   393216
         Orientation     =   1
         LargeChange     =   10
         Max             =   255
         TickStyle       =   2
         TickFrequency   =   10
      End
      Begin MSComctlLib.Slider DimmerLevel 
         Height          =   4095
         Index           =   19
         Left            =   -74040
         TabIndex        =   41
         Top             =   480
         Width           =   675
         _ExtentX        =   1191
         _ExtentY        =   7223
         _Version        =   393216
         Orientation     =   1
         LargeChange     =   10
         Max             =   255
         TickStyle       =   2
         TickFrequency   =   10
      End
      Begin MSComctlLib.Slider DimmerLevel 
         Height          =   4095
         Index           =   20
         Left            =   -73320
         TabIndex        =   42
         Top             =   480
         Width           =   675
         _ExtentX        =   1191
         _ExtentY        =   7223
         _Version        =   393216
         Orientation     =   1
         LargeChange     =   10
         Max             =   255
         TickStyle       =   2
         TickFrequency   =   10
      End
      Begin MSComctlLib.Slider DimmerLevel 
         Height          =   4095
         Index           =   21
         Left            =   -72600
         TabIndex        =   43
         Top             =   480
         Width           =   675
         _ExtentX        =   1191
         _ExtentY        =   7223
         _Version        =   393216
         Orientation     =   1
         LargeChange     =   10
         Max             =   255
         TickStyle       =   2
         TickFrequency   =   10
      End
      Begin MSComctlLib.Slider DimmerLevel 
         Height          =   4095
         Index           =   22
         Left            =   -71880
         TabIndex        =   44
         Top             =   480
         Width           =   675
         _ExtentX        =   1191
         _ExtentY        =   7223
         _Version        =   393216
         Orientation     =   1
         LargeChange     =   10
         Max             =   255
         TickStyle       =   2
         TickFrequency   =   10
      End
      Begin MSComctlLib.Slider DimmerLevel 
         Height          =   4095
         Index           =   23
         Left            =   -71160
         TabIndex        =   45
         Top             =   480
         Width           =   675
         _ExtentX        =   1191
         _ExtentY        =   7223
         _Version        =   393216
         Orientation     =   1
         LargeChange     =   10
         Max             =   255
         TickStyle       =   2
         TickFrequency   =   10
      End
      Begin MSComctlLib.Slider DimmerLevel 
         Height          =   4095
         Index           =   24
         Left            =   -70440
         TabIndex        =   46
         Top             =   480
         Width           =   675
         _ExtentX        =   1191
         _ExtentY        =   7223
         _Version        =   393216
         Orientation     =   1
         LargeChange     =   10
         Max             =   255
         TickStyle       =   2
         TickFrequency   =   10
      End
      Begin MSComctlLib.Slider DimmerLevel 
         Height          =   4095
         Index           =   25
         Left            =   -69720
         TabIndex        =   47
         Top             =   480
         Width           =   675
         _ExtentX        =   1191
         _ExtentY        =   7223
         _Version        =   393216
         Orientation     =   1
         LargeChange     =   10
         Max             =   255
         TickStyle       =   2
         TickFrequency   =   10
      End
      Begin MSComctlLib.Slider DimmerLevel 
         Height          =   4095
         Index           =   26
         Left            =   -69000
         TabIndex        =   48
         Top             =   480
         Width           =   675
         _ExtentX        =   1191
         _ExtentY        =   7223
         _Version        =   393216
         Orientation     =   1
         LargeChange     =   10
         Max             =   255
         TickStyle       =   2
         TickFrequency   =   10
      End
      Begin MSComctlLib.Slider DimmerLevel 
         Height          =   4095
         Index           =   27
         Left            =   -68280
         TabIndex        =   49
         Top             =   480
         Width           =   675
         _ExtentX        =   1191
         _ExtentY        =   7223
         _Version        =   393216
         Orientation     =   1
         LargeChange     =   10
         Max             =   255
         TickStyle       =   2
         TickFrequency   =   10
      End
      Begin MSComctlLib.Slider DimmerLevel 
         Height          =   4095
         Index           =   28
         Left            =   -67560
         TabIndex        =   50
         Top             =   480
         Width           =   675
         _ExtentX        =   1191
         _ExtentY        =   7223
         _Version        =   393216
         Orientation     =   1
         LargeChange     =   10
         Max             =   255
         TickStyle       =   2
         TickFrequency   =   10
      End
      Begin MSComctlLib.Slider DimmerLevel 
         Height          =   4095
         Index           =   29
         Left            =   -66840
         TabIndex        =   51
         Top             =   480
         Width           =   675
         _ExtentX        =   1191
         _ExtentY        =   7223
         _Version        =   393216
         Orientation     =   1
         LargeChange     =   10
         Max             =   255
         TickStyle       =   2
         TickFrequency   =   10
      End
      Begin MSComctlLib.Slider DimmerLevel 
         Height          =   4095
         Index           =   30
         Left            =   -66120
         TabIndex        =   52
         Top             =   480
         Width           =   675
         _ExtentX        =   1191
         _ExtentY        =   7223
         _Version        =   393216
         Orientation     =   1
         LargeChange     =   10
         Max             =   255
         TickStyle       =   2
         TickFrequency   =   10
      End
      Begin MSComctlLib.Slider DimmerLevel 
         Height          =   4095
         Index           =   31
         Left            =   -65400
         TabIndex        =   53
         Top             =   480
         Width           =   675
         _ExtentX        =   1191
         _ExtentY        =   7223
         _Version        =   393216
         Orientation     =   1
         LargeChange     =   10
         Max             =   255
         TickStyle       =   2
         TickFrequency   =   10
      End
      Begin MSComctlLib.Slider DimmerLevel 
         Height          =   4095
         Index           =   32
         Left            =   -64680
         TabIndex        =   54
         Top             =   480
         Width           =   675
         _ExtentX        =   1191
         _ExtentY        =   7223
         _Version        =   393216
         Orientation     =   1
         LargeChange     =   10
         Max             =   255
         TickStyle       =   2
         TickFrequency   =   10
      End
      Begin MSComctlLib.Slider DimmerLevel 
         Height          =   4095
         Index           =   33
         Left            =   -63960
         TabIndex        =   55
         Top             =   480
         Width           =   675
         _ExtentX        =   1191
         _ExtentY        =   7223
         _Version        =   393216
         Orientation     =   1
         LargeChange     =   10
         Max             =   255
         TickStyle       =   2
         TickFrequency   =   10
      End
      Begin MSComctlLib.Slider DimmerLevel 
         Height          =   4095
         Index           =   34
         Left            =   -63240
         TabIndex        =   56
         Top             =   480
         Width           =   675
         _ExtentX        =   1191
         _ExtentY        =   7223
         _Version        =   393216
         Orientation     =   1
         LargeChange     =   10
         Max             =   255
         TickStyle       =   2
         TickFrequency   =   10
      End
      Begin MSComctlLib.Slider DimmerLevel 
         Height          =   4095
         Index           =   35
         Left            =   -62520
         TabIndex        =   57
         Top             =   480
         Width           =   675
         _ExtentX        =   1191
         _ExtentY        =   7223
         _Version        =   393216
         Orientation     =   1
         LargeChange     =   10
         Max             =   255
         TickStyle       =   2
         TickFrequency   =   10
      End
      Begin MSComctlLib.Slider DimmerLevel 
         Height          =   4095
         Index           =   36
         Left            =   -74760
         TabIndex        =   76
         Top             =   480
         Width           =   675
         _ExtentX        =   1191
         _ExtentY        =   7223
         _Version        =   393216
         Orientation     =   1
         LargeChange     =   10
         Max             =   255
         TickStyle       =   2
         TickFrequency   =   10
      End
      Begin MSComctlLib.Slider DimmerLevel 
         Height          =   4095
         Index           =   37
         Left            =   -74040
         TabIndex        =   77
         Top             =   480
         Width           =   675
         _ExtentX        =   1191
         _ExtentY        =   7223
         _Version        =   393216
         Orientation     =   1
         LargeChange     =   10
         Max             =   255
         TickStyle       =   2
         TickFrequency   =   10
      End
      Begin MSComctlLib.Slider DimmerLevel 
         Height          =   4095
         Index           =   38
         Left            =   -73320
         TabIndex        =   78
         Top             =   480
         Width           =   675
         _ExtentX        =   1191
         _ExtentY        =   7223
         _Version        =   393216
         Orientation     =   1
         LargeChange     =   10
         Max             =   255
         TickStyle       =   2
         TickFrequency   =   10
      End
      Begin MSComctlLib.Slider DimmerLevel 
         Height          =   4095
         Index           =   39
         Left            =   -72600
         TabIndex        =   79
         Top             =   480
         Width           =   675
         _ExtentX        =   1191
         _ExtentY        =   7223
         _Version        =   393216
         Orientation     =   1
         LargeChange     =   10
         Max             =   255
         TickStyle       =   2
         TickFrequency   =   10
      End
      Begin MSComctlLib.Slider DimmerLevel 
         Height          =   4095
         Index           =   40
         Left            =   -71880
         TabIndex        =   80
         Top             =   480
         Width           =   675
         _ExtentX        =   1191
         _ExtentY        =   7223
         _Version        =   393216
         Orientation     =   1
         LargeChange     =   10
         Max             =   255
         TickStyle       =   2
         TickFrequency   =   10
      End
      Begin MSComctlLib.Slider DimmerLevel 
         Height          =   4095
         Index           =   41
         Left            =   -71160
         TabIndex        =   81
         Top             =   480
         Width           =   675
         _ExtentX        =   1191
         _ExtentY        =   7223
         _Version        =   393216
         Orientation     =   1
         LargeChange     =   10
         Max             =   255
         TickStyle       =   2
         TickFrequency   =   10
      End
      Begin MSComctlLib.Slider DimmerLevel 
         Height          =   4095
         Index           =   42
         Left            =   -70440
         TabIndex        =   82
         Top             =   480
         Width           =   675
         _ExtentX        =   1191
         _ExtentY        =   7223
         _Version        =   393216
         Orientation     =   1
         LargeChange     =   10
         Max             =   255
         TickStyle       =   2
         TickFrequency   =   10
      End
      Begin MSComctlLib.Slider DimmerLevel 
         Height          =   4095
         Index           =   43
         Left            =   -69720
         TabIndex        =   83
         Top             =   480
         Width           =   675
         _ExtentX        =   1191
         _ExtentY        =   7223
         _Version        =   393216
         Orientation     =   1
         LargeChange     =   10
         Max             =   255
         TickStyle       =   2
         TickFrequency   =   10
      End
      Begin MSComctlLib.Slider DimmerLevel 
         Height          =   4095
         Index           =   44
         Left            =   -69000
         TabIndex        =   84
         Top             =   480
         Width           =   675
         _ExtentX        =   1191
         _ExtentY        =   7223
         _Version        =   393216
         Orientation     =   1
         LargeChange     =   10
         Max             =   255
         TickStyle       =   2
         TickFrequency   =   10
      End
      Begin MSComctlLib.Slider DimmerLevel 
         Height          =   4095
         Index           =   45
         Left            =   -68280
         TabIndex        =   85
         Top             =   480
         Width           =   675
         _ExtentX        =   1191
         _ExtentY        =   7223
         _Version        =   393216
         Orientation     =   1
         LargeChange     =   10
         Max             =   255
         TickStyle       =   2
         TickFrequency   =   10
      End
      Begin MSComctlLib.Slider DimmerLevel 
         Height          =   4095
         Index           =   46
         Left            =   -67560
         TabIndex        =   86
         Top             =   480
         Width           =   675
         _ExtentX        =   1191
         _ExtentY        =   7223
         _Version        =   393216
         Orientation     =   1
         LargeChange     =   10
         Max             =   255
         TickStyle       =   2
         TickFrequency   =   10
      End
      Begin MSComctlLib.Slider DimmerLevel 
         Height          =   4095
         Index           =   47
         Left            =   -66840
         TabIndex        =   87
         Top             =   480
         Width           =   675
         _ExtentX        =   1191
         _ExtentY        =   7223
         _Version        =   393216
         Orientation     =   1
         LargeChange     =   10
         Max             =   255
         TickStyle       =   2
         TickFrequency   =   10
      End
      Begin MSComctlLib.Slider DimmerLevel 
         Height          =   4095
         Index           =   48
         Left            =   -66120
         TabIndex        =   88
         Top             =   480
         Width           =   675
         _ExtentX        =   1191
         _ExtentY        =   7223
         _Version        =   393216
         Orientation     =   1
         LargeChange     =   10
         Max             =   255
         TickStyle       =   2
         TickFrequency   =   10
      End
      Begin MSComctlLib.Slider DimmerLevel 
         Height          =   4095
         Index           =   49
         Left            =   -65400
         TabIndex        =   89
         Top             =   480
         Width           =   675
         _ExtentX        =   1191
         _ExtentY        =   7223
         _Version        =   393216
         Orientation     =   1
         LargeChange     =   10
         Max             =   255
         TickStyle       =   2
         TickFrequency   =   10
      End
      Begin MSComctlLib.Slider DimmerLevel 
         Height          =   4095
         Index           =   50
         Left            =   -64680
         TabIndex        =   90
         Top             =   480
         Width           =   675
         _ExtentX        =   1191
         _ExtentY        =   7223
         _Version        =   393216
         Orientation     =   1
         LargeChange     =   10
         Max             =   255
         TickStyle       =   2
         TickFrequency   =   10
      End
      Begin MSComctlLib.Slider DimmerLevel 
         Height          =   4095
         Index           =   51
         Left            =   -63960
         TabIndex        =   91
         Top             =   480
         Width           =   675
         _ExtentX        =   1191
         _ExtentY        =   7223
         _Version        =   393216
         Orientation     =   1
         LargeChange     =   10
         Max             =   255
         TickStyle       =   2
         TickFrequency   =   10
      End
      Begin MSComctlLib.Slider DimmerLevel 
         Height          =   4095
         Index           =   52
         Left            =   -63240
         TabIndex        =   92
         Top             =   480
         Width           =   675
         _ExtentX        =   1191
         _ExtentY        =   7223
         _Version        =   393216
         Orientation     =   1
         LargeChange     =   10
         Max             =   255
         TickStyle       =   2
         TickFrequency   =   10
      End
      Begin MSComctlLib.Slider DimmerLevel 
         Height          =   4095
         Index           =   53
         Left            =   -62520
         TabIndex        =   93
         Top             =   480
         Width           =   675
         _ExtentX        =   1191
         _ExtentY        =   7223
         _Version        =   393216
         Orientation     =   1
         LargeChange     =   10
         Max             =   255
         TickStyle       =   2
         TickFrequency   =   10
      End
      Begin MSComctlLib.Slider DimmerLevel 
         Height          =   4095
         Index           =   54
         Left            =   -74760
         TabIndex        =   112
         Top             =   480
         Width           =   675
         _ExtentX        =   1191
         _ExtentY        =   7223
         _Version        =   393216
         Orientation     =   1
         LargeChange     =   10
         Max             =   255
         TickStyle       =   2
         TickFrequency   =   10
      End
      Begin MSComctlLib.Slider DimmerLevel 
         Height          =   4095
         Index           =   55
         Left            =   -74040
         TabIndex        =   113
         Top             =   480
         Width           =   675
         _ExtentX        =   1191
         _ExtentY        =   7223
         _Version        =   393216
         Orientation     =   1
         LargeChange     =   10
         Max             =   255
         TickStyle       =   2
         TickFrequency   =   10
      End
      Begin MSComctlLib.Slider DimmerLevel 
         Height          =   4095
         Index           =   56
         Left            =   -73320
         TabIndex        =   114
         Top             =   480
         Width           =   675
         _ExtentX        =   1191
         _ExtentY        =   7223
         _Version        =   393216
         Orientation     =   1
         LargeChange     =   10
         Max             =   255
         TickStyle       =   2
         TickFrequency   =   10
      End
      Begin MSComctlLib.Slider DimmerLevel 
         Height          =   4095
         Index           =   57
         Left            =   -72600
         TabIndex        =   115
         Top             =   480
         Width           =   675
         _ExtentX        =   1191
         _ExtentY        =   7223
         _Version        =   393216
         Orientation     =   1
         LargeChange     =   10
         Max             =   255
         TickStyle       =   2
         TickFrequency   =   10
      End
      Begin MSComctlLib.Slider DimmerLevel 
         Height          =   4095
         Index           =   58
         Left            =   -71880
         TabIndex        =   116
         Top             =   480
         Width           =   675
         _ExtentX        =   1191
         _ExtentY        =   7223
         _Version        =   393216
         Orientation     =   1
         LargeChange     =   10
         Max             =   255
         TickStyle       =   2
         TickFrequency   =   10
      End
      Begin MSComctlLib.Slider DimmerLevel 
         Height          =   4095
         Index           =   59
         Left            =   -71160
         TabIndex        =   117
         Top             =   480
         Width           =   675
         _ExtentX        =   1191
         _ExtentY        =   7223
         _Version        =   393216
         Orientation     =   1
         LargeChange     =   10
         Max             =   255
         TickStyle       =   2
         TickFrequency   =   10
      End
      Begin MSComctlLib.Slider DimmerLevel 
         Height          =   4095
         Index           =   60
         Left            =   -70440
         TabIndex        =   118
         Top             =   480
         Width           =   675
         _ExtentX        =   1191
         _ExtentY        =   7223
         _Version        =   393216
         Orientation     =   1
         LargeChange     =   10
         Max             =   255
         TickStyle       =   2
         TickFrequency   =   10
      End
      Begin MSComctlLib.Slider DimmerLevel 
         Height          =   4095
         Index           =   61
         Left            =   -69720
         TabIndex        =   119
         Top             =   480
         Width           =   675
         _ExtentX        =   1191
         _ExtentY        =   7223
         _Version        =   393216
         Orientation     =   1
         LargeChange     =   10
         Max             =   255
         TickStyle       =   2
         TickFrequency   =   10
      End
      Begin MSComctlLib.Slider DimmerLevel 
         Height          =   4095
         Index           =   62
         Left            =   -69000
         TabIndex        =   120
         Top             =   480
         Width           =   675
         _ExtentX        =   1191
         _ExtentY        =   7223
         _Version        =   393216
         Orientation     =   1
         LargeChange     =   10
         Max             =   255
         TickStyle       =   2
         TickFrequency   =   10
      End
      Begin MSComctlLib.Slider DimmerLevel 
         Height          =   4095
         Index           =   63
         Left            =   -68280
         TabIndex        =   121
         Top             =   480
         Width           =   675
         _ExtentX        =   1191
         _ExtentY        =   7223
         _Version        =   393216
         Orientation     =   1
         LargeChange     =   10
         Max             =   255
         TickStyle       =   2
         TickFrequency   =   10
      End
      Begin MSComctlLib.Slider DimmerLevel 
         Height          =   4095
         Index           =   64
         Left            =   -67560
         TabIndex        =   122
         Top             =   480
         Width           =   675
         _ExtentX        =   1191
         _ExtentY        =   7223
         _Version        =   393216
         Orientation     =   1
         LargeChange     =   10
         Max             =   255
         TickStyle       =   2
         TickFrequency   =   10
      End
      Begin MSComctlLib.Slider DimmerLevel 
         Height          =   4095
         Index           =   65
         Left            =   -66840
         TabIndex        =   123
         Top             =   480
         Width           =   675
         _ExtentX        =   1191
         _ExtentY        =   7223
         _Version        =   393216
         Orientation     =   1
         LargeChange     =   10
         Max             =   255
         TickStyle       =   2
         TickFrequency   =   10
      End
      Begin MSComctlLib.Slider DimmerLevel 
         Height          =   4095
         Index           =   66
         Left            =   -66120
         TabIndex        =   124
         Top             =   480
         Width           =   675
         _ExtentX        =   1191
         _ExtentY        =   7223
         _Version        =   393216
         Orientation     =   1
         LargeChange     =   10
         Max             =   255
         TickStyle       =   2
         TickFrequency   =   10
      End
      Begin MSComctlLib.Slider DimmerLevel 
         Height          =   4095
         Index           =   67
         Left            =   -65400
         TabIndex        =   125
         Top             =   480
         Width           =   675
         _ExtentX        =   1191
         _ExtentY        =   7223
         _Version        =   393216
         Orientation     =   1
         LargeChange     =   10
         Max             =   255
         TickStyle       =   2
         TickFrequency   =   10
      End
      Begin MSComctlLib.Slider DimmerLevel 
         Height          =   4095
         Index           =   68
         Left            =   -64680
         TabIndex        =   126
         Top             =   480
         Width           =   675
         _ExtentX        =   1191
         _ExtentY        =   7223
         _Version        =   393216
         Orientation     =   1
         LargeChange     =   10
         Max             =   255
         TickStyle       =   2
         TickFrequency   =   10
      End
      Begin MSComctlLib.Slider DimmerLevel 
         Height          =   4095
         Index           =   69
         Left            =   -63960
         TabIndex        =   127
         Top             =   480
         Width           =   675
         _ExtentX        =   1191
         _ExtentY        =   7223
         _Version        =   393216
         Orientation     =   1
         LargeChange     =   10
         Max             =   255
         TickStyle       =   2
         TickFrequency   =   10
      End
      Begin MSComctlLib.Slider DimmerLevel 
         Height          =   4095
         Index           =   70
         Left            =   -63240
         TabIndex        =   128
         Top             =   480
         Width           =   675
         _ExtentX        =   1191
         _ExtentY        =   7223
         _Version        =   393216
         Orientation     =   1
         LargeChange     =   10
         Max             =   255
         TickStyle       =   2
         TickFrequency   =   10
      End
      Begin MSComctlLib.Slider DimmerLevel 
         Height          =   4095
         Index           =   71
         Left            =   -62520
         TabIndex        =   129
         Top             =   480
         Width           =   675
         _ExtentX        =   1191
         _ExtentY        =   7223
         _Version        =   393216
         Orientation     =   1
         LargeChange     =   10
         Max             =   255
         TickStyle       =   2
         TickFrequency   =   10
      End
      Begin MSComctlLib.Slider DimmerLevel 
         Height          =   4095
         Index           =   72
         Left            =   -74760
         TabIndex        =   148
         Top             =   480
         Width           =   675
         _ExtentX        =   1191
         _ExtentY        =   7223
         _Version        =   393216
         Orientation     =   1
         LargeChange     =   10
         Max             =   255
         TickStyle       =   2
         TickFrequency   =   10
      End
      Begin MSComctlLib.Slider DimmerLevel 
         Height          =   4095
         Index           =   73
         Left            =   -74040
         TabIndex        =   149
         Top             =   480
         Width           =   675
         _ExtentX        =   1191
         _ExtentY        =   7223
         _Version        =   393216
         Orientation     =   1
         LargeChange     =   10
         Max             =   255
         TickStyle       =   2
         TickFrequency   =   10
      End
      Begin MSComctlLib.Slider DimmerLevel 
         Height          =   4095
         Index           =   74
         Left            =   -73320
         TabIndex        =   150
         Top             =   480
         Width           =   675
         _ExtentX        =   1191
         _ExtentY        =   7223
         _Version        =   393216
         Orientation     =   1
         LargeChange     =   10
         Max             =   255
         TickStyle       =   2
         TickFrequency   =   10
      End
      Begin MSComctlLib.Slider DimmerLevel 
         Height          =   4095
         Index           =   75
         Left            =   -72600
         TabIndex        =   151
         Top             =   480
         Width           =   675
         _ExtentX        =   1191
         _ExtentY        =   7223
         _Version        =   393216
         Orientation     =   1
         LargeChange     =   10
         Max             =   255
         TickStyle       =   2
         TickFrequency   =   10
      End
      Begin MSComctlLib.Slider DimmerLevel 
         Height          =   4095
         Index           =   76
         Left            =   -71880
         TabIndex        =   152
         Top             =   480
         Width           =   675
         _ExtentX        =   1191
         _ExtentY        =   7223
         _Version        =   393216
         Orientation     =   1
         LargeChange     =   10
         Max             =   255
         TickStyle       =   2
         TickFrequency   =   10
      End
      Begin MSComctlLib.Slider DimmerLevel 
         Height          =   4095
         Index           =   77
         Left            =   -71160
         TabIndex        =   153
         Top             =   480
         Width           =   675
         _ExtentX        =   1191
         _ExtentY        =   7223
         _Version        =   393216
         Orientation     =   1
         LargeChange     =   10
         Max             =   255
         TickStyle       =   2
         TickFrequency   =   10
      End
      Begin MSComctlLib.Slider DimmerLevel 
         Height          =   4095
         Index           =   78
         Left            =   -70440
         TabIndex        =   154
         Top             =   480
         Width           =   675
         _ExtentX        =   1191
         _ExtentY        =   7223
         _Version        =   393216
         Orientation     =   1
         LargeChange     =   10
         Max             =   255
         TickStyle       =   2
         TickFrequency   =   10
      End
      Begin MSComctlLib.Slider DimmerLevel 
         Height          =   4095
         Index           =   79
         Left            =   -69720
         TabIndex        =   155
         Top             =   480
         Width           =   675
         _ExtentX        =   1191
         _ExtentY        =   7223
         _Version        =   393216
         Orientation     =   1
         LargeChange     =   10
         Max             =   255
         TickStyle       =   2
         TickFrequency   =   10
      End
      Begin MSComctlLib.Slider DimmerLevel 
         Height          =   4095
         Index           =   80
         Left            =   -69000
         TabIndex        =   156
         Top             =   480
         Width           =   675
         _ExtentX        =   1191
         _ExtentY        =   7223
         _Version        =   393216
         Orientation     =   1
         LargeChange     =   10
         Max             =   255
         TickStyle       =   2
         TickFrequency   =   10
      End
      Begin MSComctlLib.Slider DimmerLevel 
         Height          =   4095
         Index           =   81
         Left            =   -68280
         TabIndex        =   157
         Top             =   480
         Width           =   675
         _ExtentX        =   1191
         _ExtentY        =   7223
         _Version        =   393216
         Orientation     =   1
         LargeChange     =   10
         Max             =   255
         TickStyle       =   2
         TickFrequency   =   10
      End
      Begin MSComctlLib.Slider DimmerLevel 
         Height          =   4095
         Index           =   82
         Left            =   -67560
         TabIndex        =   158
         Top             =   480
         Width           =   675
         _ExtentX        =   1191
         _ExtentY        =   7223
         _Version        =   393216
         Orientation     =   1
         LargeChange     =   10
         Max             =   255
         TickStyle       =   2
         TickFrequency   =   10
      End
      Begin MSComctlLib.Slider DimmerLevel 
         Height          =   4095
         Index           =   83
         Left            =   -66840
         TabIndex        =   159
         Top             =   480
         Width           =   675
         _ExtentX        =   1191
         _ExtentY        =   7223
         _Version        =   393216
         Orientation     =   1
         LargeChange     =   10
         Max             =   255
         TickStyle       =   2
         TickFrequency   =   10
      End
      Begin MSComctlLib.Slider DimmerLevel 
         Height          =   4095
         Index           =   84
         Left            =   -66120
         TabIndex        =   160
         Top             =   480
         Width           =   675
         _ExtentX        =   1191
         _ExtentY        =   7223
         _Version        =   393216
         Orientation     =   1
         LargeChange     =   10
         Max             =   255
         TickStyle       =   2
         TickFrequency   =   10
      End
      Begin MSComctlLib.Slider DimmerLevel 
         Height          =   4095
         Index           =   85
         Left            =   -65400
         TabIndex        =   161
         Top             =   480
         Width           =   675
         _ExtentX        =   1191
         _ExtentY        =   7223
         _Version        =   393216
         Orientation     =   1
         LargeChange     =   10
         Max             =   255
         TickStyle       =   2
         TickFrequency   =   10
      End
      Begin MSComctlLib.Slider DimmerLevel 
         Height          =   4095
         Index           =   86
         Left            =   -64680
         TabIndex        =   162
         Top             =   480
         Width           =   675
         _ExtentX        =   1191
         _ExtentY        =   7223
         _Version        =   393216
         Orientation     =   1
         LargeChange     =   10
         Max             =   255
         TickStyle       =   2
         TickFrequency   =   10
      End
      Begin MSComctlLib.Slider DimmerLevel 
         Height          =   4095
         Index           =   87
         Left            =   -63960
         TabIndex        =   163
         Top             =   480
         Width           =   675
         _ExtentX        =   1191
         _ExtentY        =   7223
         _Version        =   393216
         Orientation     =   1
         LargeChange     =   10
         Max             =   255
         TickStyle       =   2
         TickFrequency   =   10
      End
      Begin MSComctlLib.Slider DimmerLevel 
         Height          =   4095
         Index           =   88
         Left            =   -63240
         TabIndex        =   164
         Top             =   480
         Width           =   675
         _ExtentX        =   1191
         _ExtentY        =   7223
         _Version        =   393216
         Orientation     =   1
         LargeChange     =   10
         Max             =   255
         TickStyle       =   2
         TickFrequency   =   10
      End
      Begin MSComctlLib.Slider DimmerLevel 
         Height          =   4095
         Index           =   89
         Left            =   -62520
         TabIndex        =   165
         Top             =   480
         Width           =   675
         _ExtentX        =   1191
         _ExtentY        =   7223
         _Version        =   393216
         Orientation     =   1
         LargeChange     =   10
         Max             =   255
         TickStyle       =   2
         TickFrequency   =   10
      End
      Begin MSComctlLib.Slider DimmerLevel 
         Height          =   4095
         Index           =   90
         Left            =   -74760
         TabIndex        =   184
         Top             =   480
         Width           =   675
         _ExtentX        =   1191
         _ExtentY        =   7223
         _Version        =   393216
         Orientation     =   1
         LargeChange     =   10
         Max             =   255
         TickStyle       =   2
         TickFrequency   =   10
      End
      Begin MSComctlLib.Slider DimmerLevel 
         Height          =   4095
         Index           =   91
         Left            =   -74040
         TabIndex        =   185
         Top             =   480
         Width           =   675
         _ExtentX        =   1191
         _ExtentY        =   7223
         _Version        =   393216
         Orientation     =   1
         LargeChange     =   10
         Max             =   255
         TickStyle       =   2
         TickFrequency   =   10
      End
      Begin MSComctlLib.Slider DimmerLevel 
         Height          =   4095
         Index           =   92
         Left            =   -73320
         TabIndex        =   186
         Top             =   480
         Width           =   675
         _ExtentX        =   1191
         _ExtentY        =   7223
         _Version        =   393216
         Orientation     =   1
         LargeChange     =   10
         Max             =   255
         TickStyle       =   2
         TickFrequency   =   10
      End
      Begin MSComctlLib.Slider DimmerLevel 
         Height          =   4095
         Index           =   93
         Left            =   -72600
         TabIndex        =   187
         Top             =   480
         Width           =   675
         _ExtentX        =   1191
         _ExtentY        =   7223
         _Version        =   393216
         Orientation     =   1
         LargeChange     =   10
         Max             =   255
         TickStyle       =   2
         TickFrequency   =   10
      End
      Begin MSComctlLib.Slider DimmerLevel 
         Height          =   4095
         Index           =   94
         Left            =   -71880
         TabIndex        =   188
         Top             =   480
         Width           =   675
         _ExtentX        =   1191
         _ExtentY        =   7223
         _Version        =   393216
         Orientation     =   1
         LargeChange     =   10
         Max             =   255
         TickStyle       =   2
         TickFrequency   =   10
      End
      Begin MSComctlLib.Slider DimmerLevel 
         Height          =   4095
         Index           =   95
         Left            =   -71160
         TabIndex        =   189
         Top             =   480
         Width           =   675
         _ExtentX        =   1191
         _ExtentY        =   7223
         _Version        =   393216
         Orientation     =   1
         LargeChange     =   10
         Max             =   255
         TickStyle       =   2
         TickFrequency   =   10
      End
      Begin MSComctlLib.Slider DimmerLevel 
         Height          =   4095
         Index           =   96
         Left            =   -70440
         TabIndex        =   190
         Top             =   480
         Width           =   675
         _ExtentX        =   1191
         _ExtentY        =   7223
         _Version        =   393216
         Orientation     =   1
         LargeChange     =   10
         Max             =   255
         TickStyle       =   2
         TickFrequency   =   10
      End
      Begin MSComctlLib.Slider DimmerLevel 
         Height          =   4095
         Index           =   97
         Left            =   -69720
         TabIndex        =   191
         Top             =   480
         Width           =   675
         _ExtentX        =   1191
         _ExtentY        =   7223
         _Version        =   393216
         Orientation     =   1
         LargeChange     =   10
         Max             =   255
         TickStyle       =   2
         TickFrequency   =   10
      End
      Begin MSComctlLib.Slider DimmerLevel 
         Height          =   4095
         Index           =   98
         Left            =   -69000
         TabIndex        =   192
         Top             =   480
         Width           =   675
         _ExtentX        =   1191
         _ExtentY        =   7223
         _Version        =   393216
         Orientation     =   1
         LargeChange     =   10
         Max             =   255
         TickStyle       =   2
         TickFrequency   =   10
      End
      Begin MSComctlLib.Slider DimmerLevel 
         Height          =   4095
         Index           =   99
         Left            =   -68280
         TabIndex        =   193
         Top             =   480
         Width           =   675
         _ExtentX        =   1191
         _ExtentY        =   7223
         _Version        =   393216
         Orientation     =   1
         LargeChange     =   10
         Max             =   255
         TickStyle       =   2
         TickFrequency   =   10
      End
      Begin MSComctlLib.Slider DimmerLevel 
         Height          =   4095
         Index           =   100
         Left            =   -67560
         TabIndex        =   194
         Top             =   480
         Width           =   675
         _ExtentX        =   1191
         _ExtentY        =   7223
         _Version        =   393216
         Orientation     =   1
         LargeChange     =   10
         Max             =   255
         TickStyle       =   2
         TickFrequency   =   10
      End
      Begin MSComctlLib.Slider DimmerLevel 
         Height          =   4095
         Index           =   101
         Left            =   -66840
         TabIndex        =   195
         Top             =   480
         Width           =   675
         _ExtentX        =   1191
         _ExtentY        =   7223
         _Version        =   393216
         Orientation     =   1
         LargeChange     =   10
         Max             =   255
         TickStyle       =   2
         TickFrequency   =   10
      End
      Begin MSComctlLib.Slider DimmerLevel 
         Height          =   4095
         Index           =   102
         Left            =   -66120
         TabIndex        =   196
         Top             =   480
         Width           =   675
         _ExtentX        =   1191
         _ExtentY        =   7223
         _Version        =   393216
         Orientation     =   1
         LargeChange     =   10
         Max             =   255
         TickStyle       =   2
         TickFrequency   =   10
      End
      Begin MSComctlLib.Slider DimmerLevel 
         Height          =   4095
         Index           =   103
         Left            =   -65400
         TabIndex        =   197
         Top             =   480
         Width           =   675
         _ExtentX        =   1191
         _ExtentY        =   7223
         _Version        =   393216
         Orientation     =   1
         LargeChange     =   10
         Max             =   255
         TickStyle       =   2
         TickFrequency   =   10
      End
      Begin MSComctlLib.Slider DimmerLevel 
         Height          =   4095
         Index           =   104
         Left            =   -64680
         TabIndex        =   198
         Top             =   480
         Width           =   675
         _ExtentX        =   1191
         _ExtentY        =   7223
         _Version        =   393216
         Orientation     =   1
         LargeChange     =   10
         Max             =   255
         TickStyle       =   2
         TickFrequency   =   10
      End
      Begin MSComctlLib.Slider DimmerLevel 
         Height          =   4095
         Index           =   105
         Left            =   -63960
         TabIndex        =   199
         Top             =   480
         Width           =   675
         _ExtentX        =   1191
         _ExtentY        =   7223
         _Version        =   393216
         Orientation     =   1
         LargeChange     =   10
         Max             =   255
         TickStyle       =   2
         TickFrequency   =   10
      End
      Begin MSComctlLib.Slider DimmerLevel 
         Height          =   4095
         Index           =   106
         Left            =   -63240
         TabIndex        =   200
         Top             =   480
         Width           =   675
         _ExtentX        =   1191
         _ExtentY        =   7223
         _Version        =   393216
         Orientation     =   1
         LargeChange     =   10
         Max             =   255
         TickStyle       =   2
         TickFrequency   =   10
      End
      Begin MSComctlLib.Slider DimmerLevel 
         Height          =   4095
         Index           =   107
         Left            =   -62520
         TabIndex        =   201
         Top             =   480
         Width           =   675
         _ExtentX        =   1191
         _ExtentY        =   7223
         _Version        =   393216
         Orientation     =   1
         LargeChange     =   10
         Max             =   255
         TickStyle       =   2
         TickFrequency   =   10
      End
      Begin VB.Label Label1 
         Caption         =   "Ch 91"
         Height          =   255
         Index           =   107
         Left            =   -74640
         TabIndex        =   219
         Top             =   240
         Width           =   495
      End
      Begin VB.Label Label1 
         Caption         =   "Ch 92"
         Height          =   255
         Index           =   106
         Left            =   -73920
         TabIndex        =   218
         Top             =   240
         Width           =   495
      End
      Begin VB.Label Label1 
         Caption         =   "Ch 93"
         Height          =   255
         Index           =   105
         Left            =   -73200
         TabIndex        =   217
         Top             =   240
         Width           =   495
      End
      Begin VB.Label Label1 
         Caption         =   "Ch 94"
         Height          =   255
         Index           =   104
         Left            =   -72480
         TabIndex        =   216
         Top             =   240
         Width           =   495
      End
      Begin VB.Label Label1 
         Caption         =   "Ch 95"
         Height          =   255
         Index           =   103
         Left            =   -71760
         TabIndex        =   215
         Top             =   240
         Width           =   495
      End
      Begin VB.Label Label1 
         Caption         =   "Ch 96"
         Height          =   255
         Index           =   102
         Left            =   -71040
         TabIndex        =   214
         Top             =   240
         Width           =   495
      End
      Begin VB.Label Label1 
         Caption         =   "Ch 97"
         Height          =   255
         Index           =   101
         Left            =   -70320
         TabIndex        =   213
         Top             =   240
         Width           =   495
      End
      Begin VB.Label Label1 
         Caption         =   "Ch 98"
         Height          =   255
         Index           =   100
         Left            =   -69600
         TabIndex        =   212
         Top             =   240
         Width           =   495
      End
      Begin VB.Label Label1 
         Caption         =   "Ch 99"
         Height          =   255
         Index           =   99
         Left            =   -68880
         TabIndex        =   211
         Top             =   240
         Width           =   495
      End
      Begin VB.Label Label1 
         Caption         =   "Ch 100"
         Height          =   255
         Index           =   98
         Left            =   -68160
         TabIndex        =   210
         Top             =   240
         Width           =   615
      End
      Begin VB.Label Label1 
         Caption         =   "Ch 101"
         Height          =   255
         Index           =   97
         Left            =   -67440
         TabIndex        =   209
         Top             =   240
         Width           =   615
      End
      Begin VB.Label Label1 
         Caption         =   "Ch 102"
         Height          =   255
         Index           =   96
         Left            =   -66720
         TabIndex        =   208
         Top             =   240
         Width           =   615
      End
      Begin VB.Label Label1 
         Caption         =   "Ch 103"
         Height          =   255
         Index           =   95
         Left            =   -66000
         TabIndex        =   207
         Top             =   240
         Width           =   615
      End
      Begin VB.Label Label1 
         Caption         =   "Ch 104"
         Height          =   255
         Index           =   94
         Left            =   -65280
         TabIndex        =   206
         Top             =   240
         Width           =   615
      End
      Begin VB.Label Label1 
         Caption         =   "Ch 105"
         Height          =   255
         Index           =   93
         Left            =   -64560
         TabIndex        =   205
         Top             =   240
         Width           =   615
      End
      Begin VB.Label Label1 
         Caption         =   "Ch 106"
         Height          =   255
         Index           =   92
         Left            =   -63840
         TabIndex        =   204
         Top             =   240
         Width           =   615
      End
      Begin VB.Label Label1 
         Caption         =   "Ch 107"
         Height          =   255
         Index           =   91
         Left            =   -63120
         TabIndex        =   203
         Top             =   240
         Width           =   615
      End
      Begin VB.Label Label1 
         Caption         =   "Ch 108"
         Height          =   255
         Index           =   90
         Left            =   -62400
         TabIndex        =   202
         Top             =   240
         Width           =   615
      End
      Begin VB.Label Label1 
         Caption         =   "Ch 73"
         Height          =   255
         Index           =   89
         Left            =   -74640
         TabIndex        =   183
         Top             =   240
         Width           =   495
      End
      Begin VB.Label Label1 
         Caption         =   "Ch 74"
         Height          =   255
         Index           =   88
         Left            =   -73920
         TabIndex        =   182
         Top             =   240
         Width           =   495
      End
      Begin VB.Label Label1 
         Caption         =   "Ch 75"
         Height          =   255
         Index           =   87
         Left            =   -73200
         TabIndex        =   181
         Top             =   240
         Width           =   495
      End
      Begin VB.Label Label1 
         Caption         =   "Ch 76"
         Height          =   255
         Index           =   86
         Left            =   -72480
         TabIndex        =   180
         Top             =   240
         Width           =   495
      End
      Begin VB.Label Label1 
         Caption         =   "Ch 77"
         Height          =   255
         Index           =   85
         Left            =   -71760
         TabIndex        =   179
         Top             =   240
         Width           =   495
      End
      Begin VB.Label Label1 
         Caption         =   "Ch 78"
         Height          =   255
         Index           =   84
         Left            =   -71040
         TabIndex        =   178
         Top             =   240
         Width           =   495
      End
      Begin VB.Label Label1 
         Caption         =   "Ch 79"
         Height          =   255
         Index           =   83
         Left            =   -70320
         TabIndex        =   177
         Top             =   240
         Width           =   495
      End
      Begin VB.Label Label1 
         Caption         =   "Ch 80"
         Height          =   255
         Index           =   82
         Left            =   -69600
         TabIndex        =   176
         Top             =   240
         Width           =   495
      End
      Begin VB.Label Label1 
         Caption         =   "Ch 81"
         Height          =   255
         Index           =   81
         Left            =   -68880
         TabIndex        =   175
         Top             =   240
         Width           =   495
      End
      Begin VB.Label Label1 
         Caption         =   "Ch 82"
         Height          =   255
         Index           =   80
         Left            =   -68160
         TabIndex        =   174
         Top             =   240
         Width           =   495
      End
      Begin VB.Label Label1 
         Caption         =   "Ch 83"
         Height          =   255
         Index           =   79
         Left            =   -67440
         TabIndex        =   173
         Top             =   240
         Width           =   495
      End
      Begin VB.Label Label1 
         Caption         =   "Ch 84"
         Height          =   255
         Index           =   78
         Left            =   -66720
         TabIndex        =   172
         Top             =   240
         Width           =   495
      End
      Begin VB.Label Label1 
         Caption         =   "Ch 85"
         Height          =   255
         Index           =   77
         Left            =   -66000
         TabIndex        =   171
         Top             =   240
         Width           =   495
      End
      Begin VB.Label Label1 
         Caption         =   "Ch 86"
         Height          =   255
         Index           =   76
         Left            =   -65280
         TabIndex        =   170
         Top             =   240
         Width           =   495
      End
      Begin VB.Label Label1 
         Caption         =   "Ch 87"
         Height          =   255
         Index           =   75
         Left            =   -64560
         TabIndex        =   169
         Top             =   240
         Width           =   495
      End
      Begin VB.Label Label1 
         Caption         =   "Ch 88"
         Height          =   255
         Index           =   74
         Left            =   -63840
         TabIndex        =   168
         Top             =   240
         Width           =   495
      End
      Begin VB.Label Label1 
         Caption         =   "Ch 89"
         Height          =   255
         Index           =   73
         Left            =   -63120
         TabIndex        =   167
         Top             =   240
         Width           =   495
      End
      Begin VB.Label Label1 
         Caption         =   "Ch 90"
         Height          =   255
         Index           =   72
         Left            =   -62400
         TabIndex        =   166
         Top             =   240
         Width           =   495
      End
      Begin VB.Label Label1 
         Caption         =   "Ch 55"
         Height          =   255
         Index           =   71
         Left            =   -74640
         TabIndex        =   147
         Top             =   240
         Width           =   495
      End
      Begin VB.Label Label1 
         Caption         =   "Ch 56"
         Height          =   255
         Index           =   70
         Left            =   -73920
         TabIndex        =   146
         Top             =   240
         Width           =   495
      End
      Begin VB.Label Label1 
         Caption         =   "Ch 57"
         Height          =   255
         Index           =   69
         Left            =   -73200
         TabIndex        =   145
         Top             =   240
         Width           =   495
      End
      Begin VB.Label Label1 
         Caption         =   "Ch 58"
         Height          =   255
         Index           =   68
         Left            =   -72480
         TabIndex        =   144
         Top             =   240
         Width           =   495
      End
      Begin VB.Label Label1 
         Caption         =   "Ch 59"
         Height          =   255
         Index           =   67
         Left            =   -71760
         TabIndex        =   143
         Top             =   240
         Width           =   495
      End
      Begin VB.Label Label1 
         Caption         =   "Ch 60"
         Height          =   255
         Index           =   66
         Left            =   -71040
         TabIndex        =   142
         Top             =   240
         Width           =   495
      End
      Begin VB.Label Label1 
         Caption         =   "Ch 61"
         Height          =   255
         Index           =   65
         Left            =   -70320
         TabIndex        =   141
         Top             =   240
         Width           =   495
      End
      Begin VB.Label Label1 
         Caption         =   "Ch 62"
         Height          =   255
         Index           =   64
         Left            =   -69600
         TabIndex        =   140
         Top             =   240
         Width           =   495
      End
      Begin VB.Label Label1 
         Caption         =   "Ch 63"
         Height          =   255
         Index           =   63
         Left            =   -68880
         TabIndex        =   139
         Top             =   240
         Width           =   495
      End
      Begin VB.Label Label1 
         Caption         =   "Ch 64"
         Height          =   255
         Index           =   62
         Left            =   -68160
         TabIndex        =   138
         Top             =   240
         Width           =   495
      End
      Begin VB.Label Label1 
         Caption         =   "Ch 65"
         Height          =   255
         Index           =   61
         Left            =   -67440
         TabIndex        =   137
         Top             =   240
         Width           =   495
      End
      Begin VB.Label Label1 
         Caption         =   "Ch 66"
         Height          =   255
         Index           =   60
         Left            =   -66720
         TabIndex        =   136
         Top             =   240
         Width           =   495
      End
      Begin VB.Label Label1 
         Caption         =   "Ch 67"
         Height          =   255
         Index           =   59
         Left            =   -66000
         TabIndex        =   135
         Top             =   240
         Width           =   495
      End
      Begin VB.Label Label1 
         Caption         =   "Ch 68"
         Height          =   255
         Index           =   58
         Left            =   -65280
         TabIndex        =   134
         Top             =   240
         Width           =   495
      End
      Begin VB.Label Label1 
         Caption         =   "Ch 69"
         Height          =   255
         Index           =   57
         Left            =   -64560
         TabIndex        =   133
         Top             =   240
         Width           =   495
      End
      Begin VB.Label Label1 
         Caption         =   "Ch 70"
         Height          =   255
         Index           =   56
         Left            =   -63840
         TabIndex        =   132
         Top             =   240
         Width           =   495
      End
      Begin VB.Label Label1 
         Caption         =   "Ch 71"
         Height          =   255
         Index           =   55
         Left            =   -63120
         TabIndex        =   131
         Top             =   240
         Width           =   495
      End
      Begin VB.Label Label1 
         Caption         =   "Ch 72"
         Height          =   255
         Index           =   54
         Left            =   -62400
         TabIndex        =   130
         Top             =   240
         Width           =   495
      End
      Begin VB.Label Label1 
         Caption         =   "Ch 37"
         Height          =   255
         Index           =   53
         Left            =   -74640
         TabIndex        =   111
         Top             =   240
         Width           =   495
      End
      Begin VB.Label Label1 
         Caption         =   "Ch 38"
         Height          =   255
         Index           =   52
         Left            =   -73920
         TabIndex        =   110
         Top             =   240
         Width           =   495
      End
      Begin VB.Label Label1 
         Caption         =   "Ch 39"
         Height          =   255
         Index           =   51
         Left            =   -73200
         TabIndex        =   109
         Top             =   240
         Width           =   495
      End
      Begin VB.Label Label1 
         Caption         =   "Ch 40"
         Height          =   255
         Index           =   50
         Left            =   -72480
         TabIndex        =   108
         Top             =   240
         Width           =   495
      End
      Begin VB.Label Label1 
         Caption         =   "Ch 41"
         Height          =   255
         Index           =   49
         Left            =   -71760
         TabIndex        =   107
         Top             =   240
         Width           =   495
      End
      Begin VB.Label Label1 
         Caption         =   "Ch 42"
         Height          =   255
         Index           =   48
         Left            =   -71040
         TabIndex        =   106
         Top             =   240
         Width           =   495
      End
      Begin VB.Label Label1 
         Caption         =   "Ch 43"
         Height          =   255
         Index           =   47
         Left            =   -70320
         TabIndex        =   105
         Top             =   240
         Width           =   495
      End
      Begin VB.Label Label1 
         Caption         =   "Ch 44"
         Height          =   255
         Index           =   46
         Left            =   -69600
         TabIndex        =   104
         Top             =   240
         Width           =   495
      End
      Begin VB.Label Label1 
         Caption         =   "Ch 45"
         Height          =   255
         Index           =   45
         Left            =   -68880
         TabIndex        =   103
         Top             =   240
         Width           =   495
      End
      Begin VB.Label Label1 
         Caption         =   "Ch 46"
         Height          =   255
         Index           =   44
         Left            =   -68160
         TabIndex        =   102
         Top             =   240
         Width           =   495
      End
      Begin VB.Label Label1 
         Caption         =   "Ch 47"
         Height          =   255
         Index           =   43
         Left            =   -67440
         TabIndex        =   101
         Top             =   240
         Width           =   495
      End
      Begin VB.Label Label1 
         Caption         =   "Ch 48"
         Height          =   255
         Index           =   42
         Left            =   -66720
         TabIndex        =   100
         Top             =   240
         Width           =   495
      End
      Begin VB.Label Label1 
         Caption         =   "Ch 49"
         Height          =   255
         Index           =   41
         Left            =   -66000
         TabIndex        =   99
         Top             =   240
         Width           =   495
      End
      Begin VB.Label Label1 
         Caption         =   "Ch 50"
         Height          =   255
         Index           =   40
         Left            =   -65280
         TabIndex        =   98
         Top             =   240
         Width           =   495
      End
      Begin VB.Label Label1 
         Caption         =   "Ch 51"
         Height          =   255
         Index           =   39
         Left            =   -64560
         TabIndex        =   97
         Top             =   240
         Width           =   495
      End
      Begin VB.Label Label1 
         Caption         =   "Ch 52"
         Height          =   255
         Index           =   38
         Left            =   -63840
         TabIndex        =   96
         Top             =   240
         Width           =   495
      End
      Begin VB.Label Label1 
         Caption         =   "Ch 53"
         Height          =   255
         Index           =   37
         Left            =   -63120
         TabIndex        =   95
         Top             =   240
         Width           =   495
      End
      Begin VB.Label Label1 
         Caption         =   "Ch 54"
         Height          =   255
         Index           =   36
         Left            =   -62400
         TabIndex        =   94
         Top             =   240
         Width           =   495
      End
      Begin VB.Label Label1 
         Caption         =   "Ch 19"
         Height          =   255
         Index           =   35
         Left            =   -74640
         TabIndex        =   75
         Top             =   240
         Width           =   495
      End
      Begin VB.Label Label1 
         Caption         =   "Ch 20"
         Height          =   255
         Index           =   34
         Left            =   -73920
         TabIndex        =   74
         Top             =   240
         Width           =   495
      End
      Begin VB.Label Label1 
         Caption         =   "Ch 21"
         Height          =   255
         Index           =   33
         Left            =   -73200
         TabIndex        =   73
         Top             =   240
         Width           =   495
      End
      Begin VB.Label Label1 
         Caption         =   "Ch 22"
         Height          =   255
         Index           =   32
         Left            =   -72480
         TabIndex        =   72
         Top             =   240
         Width           =   495
      End
      Begin VB.Label Label1 
         Caption         =   "Ch 23"
         Height          =   255
         Index           =   31
         Left            =   -71760
         TabIndex        =   71
         Top             =   240
         Width           =   495
      End
      Begin VB.Label Label1 
         Caption         =   "Ch 24"
         Height          =   255
         Index           =   30
         Left            =   -71040
         TabIndex        =   70
         Top             =   240
         Width           =   495
      End
      Begin VB.Label Label1 
         Caption         =   "Ch 25"
         Height          =   255
         Index           =   29
         Left            =   -70320
         TabIndex        =   69
         Top             =   240
         Width           =   495
      End
      Begin VB.Label Label1 
         Caption         =   "Ch 26"
         Height          =   255
         Index           =   28
         Left            =   -69600
         TabIndex        =   68
         Top             =   240
         Width           =   495
      End
      Begin VB.Label Label1 
         Caption         =   "Ch 27"
         Height          =   255
         Index           =   27
         Left            =   -68880
         TabIndex        =   67
         Top             =   240
         Width           =   495
      End
      Begin VB.Label Label1 
         Caption         =   "Ch 28"
         Height          =   255
         Index           =   26
         Left            =   -68160
         TabIndex        =   66
         Top             =   240
         Width           =   495
      End
      Begin VB.Label Label1 
         Caption         =   "Ch 29"
         Height          =   255
         Index           =   25
         Left            =   -67440
         TabIndex        =   65
         Top             =   240
         Width           =   495
      End
      Begin VB.Label Label1 
         Caption         =   "Ch 30"
         Height          =   255
         Index           =   24
         Left            =   -66720
         TabIndex        =   64
         Top             =   240
         Width           =   495
      End
      Begin VB.Label Label1 
         Caption         =   "Ch 31"
         Height          =   255
         Index           =   23
         Left            =   -66000
         TabIndex        =   63
         Top             =   240
         Width           =   495
      End
      Begin VB.Label Label1 
         Caption         =   "Ch 32"
         Height          =   255
         Index           =   22
         Left            =   -65280
         TabIndex        =   62
         Top             =   240
         Width           =   495
      End
      Begin VB.Label Label1 
         Caption         =   "Ch 33"
         Height          =   255
         Index           =   21
         Left            =   -64560
         TabIndex        =   61
         Top             =   240
         Width           =   495
      End
      Begin VB.Label Label1 
         Caption         =   "Ch 34"
         Height          =   255
         Index           =   20
         Left            =   -63840
         TabIndex        =   60
         Top             =   240
         Width           =   495
      End
      Begin VB.Label Label1 
         Caption         =   "Ch 35"
         Height          =   255
         Index           =   19
         Left            =   -63120
         TabIndex        =   59
         Top             =   240
         Width           =   495
      End
      Begin VB.Label Label1 
         Caption         =   "Ch 36"
         Height          =   255
         Index           =   18
         Left            =   -62400
         TabIndex        =   58
         Top             =   240
         Width           =   495
      End
      Begin VB.Label Label1 
         Caption         =   "Ch 1"
         Height          =   255
         Index           =   0
         Left            =   360
         TabIndex        =   39
         Top             =   240
         Width           =   495
      End
      Begin VB.Label Label1 
         Caption         =   "Ch 2"
         Height          =   255
         Index           =   1
         Left            =   1080
         TabIndex        =   38
         Top             =   240
         Width           =   495
      End
      Begin VB.Label Label1 
         Caption         =   "Ch 3"
         Height          =   255
         Index           =   2
         Left            =   1800
         TabIndex        =   37
         Top             =   240
         Width           =   495
      End
      Begin VB.Label Label1 
         Caption         =   "Ch 4"
         Height          =   255
         Index           =   3
         Left            =   2520
         TabIndex        =   36
         Top             =   240
         Width           =   495
      End
      Begin VB.Label Label1 
         Caption         =   "Ch 5"
         Height          =   255
         Index           =   4
         Left            =   3240
         TabIndex        =   35
         Top             =   240
         Width           =   495
      End
      Begin VB.Label Label1 
         Caption         =   "Ch 6"
         Height          =   255
         Index           =   5
         Left            =   3960
         TabIndex        =   34
         Top             =   240
         Width           =   495
      End
      Begin VB.Label Label1 
         Caption         =   "Ch 7"
         Height          =   255
         Index           =   6
         Left            =   4680
         TabIndex        =   33
         Top             =   240
         Width           =   495
      End
      Begin VB.Label Label1 
         Caption         =   "Ch 8"
         Height          =   255
         Index           =   7
         Left            =   5400
         TabIndex        =   32
         Top             =   240
         Width           =   495
      End
      Begin VB.Label Label1 
         Caption         =   "Ch 9"
         Height          =   255
         Index           =   8
         Left            =   6120
         TabIndex        =   31
         Top             =   240
         Width           =   495
      End
      Begin VB.Label Label1 
         Caption         =   "Ch 10"
         Height          =   255
         Index           =   9
         Left            =   6840
         TabIndex        =   30
         Top             =   240
         Width           =   495
      End
      Begin VB.Label Label1 
         Caption         =   "Ch 11"
         Height          =   255
         Index           =   10
         Left            =   7560
         TabIndex        =   29
         Top             =   240
         Width           =   495
      End
      Begin VB.Label Label1 
         Caption         =   "Ch 12"
         Height          =   255
         Index           =   11
         Left            =   8280
         TabIndex        =   28
         Top             =   240
         Width           =   495
      End
      Begin VB.Label Label1 
         Caption         =   "Ch 13"
         Height          =   255
         Index           =   12
         Left            =   9000
         TabIndex        =   27
         Top             =   240
         Width           =   495
      End
      Begin VB.Label Label1 
         Caption         =   "Ch 14"
         Height          =   255
         Index           =   13
         Left            =   9720
         TabIndex        =   26
         Top             =   240
         Width           =   495
      End
      Begin VB.Label Label1 
         Caption         =   "Ch 15"
         Height          =   255
         Index           =   14
         Left            =   10440
         TabIndex        =   25
         Top             =   240
         Width           =   495
      End
      Begin VB.Label Label1 
         Caption         =   "Ch 16"
         Height          =   255
         Index           =   15
         Left            =   11160
         TabIndex        =   24
         Top             =   240
         Width           =   495
      End
      Begin VB.Label Label1 
         Caption         =   "Ch 17"
         Height          =   255
         Index           =   16
         Left            =   11880
         TabIndex        =   23
         Top             =   240
         Width           =   495
      End
      Begin VB.Label Label1 
         Caption         =   "Ch 18"
         Height          =   255
         Index           =   17
         Left            =   12600
         TabIndex        =   22
         Top             =   240
         Width           =   495
      End
   End
   Begin VB.CommandButton btnDBO 
      Caption         =   "DBO"
      Height          =   375
      Left            =   600
      TabIndex        =   2
      Top             =   6240
      Width           =   1095
   End
   Begin VB.Frame Frame1 
      Caption         =   "Status"
      Height          =   615
      Left            =   600
      TabIndex        =   0
      Top             =   5520
      Width           =   13095
      Begin VB.Label Status 
         Caption         =   "Not Connected"
         BeginProperty Font 
            Name            =   "MS Sans Serif"
            Size            =   8.25
            Charset         =   0
            Weight          =   700
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   255
         Left            =   240
         TabIndex        =   1
         Top             =   240
         Width           =   12615
      End
   End
   Begin NETCommOCX.NETComm NETComm1 
      Left            =   120
      Top             =   120
      _ExtentX        =   661
      _ExtentY        =   661
      Settings        =   "19200, N, 8, 1"
      CommPort        =   2
      Handshaking     =   0
      InputBufferSize =   10
      InputLength     =   1
      InputMode       =   0
      NullDiscard     =   0   'False
      OutputBufferSize=   512
      ParityReplace   =   "?"
      OnCommReceiveThreshold=   0
      RTSEnable       =   0   'False
      OnCommSendThreshold=   0
   End
   Begin VB.Menu mnuFile 
      Caption         =   "File"
      Begin VB.Menu mnuConnect 
         Caption         =   "Resync"
      End
      Begin VB.Menu mnuExit 
         Caption         =   "Exit"
      End
   End
End
Attribute VB_Name = "Form1"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Private Declare Sub Sleep Lib "kernel32" (ByVal dwMilliseconds As Long)

Private Sub btnDBO_Click()
Call DBO
End Sub

Private Sub BtnSIN_Click()
While A < 2048
    DoEvents
    X = Int(((Sin((A / 255)) * 128)) + 128)
    Y = Int(((Sin(((A + 1024) / 255)) * 128)) + 128)
    Call SetLevel(8, X)
    Call SetLevel(10, Y)
    A = A + 2
Wend
End Sub

Private Sub DimmerLevel_Scroll(Index As Integer)
Let Channel = (Index + 1)
Let Level = DimmerLevel(Index).Value
NETComm1.Output = Chr$(&HFF) & Chr$(Channel) & Chr$(Level)
Let Flag = False
Do Until (Flag = True)
    DoEvents
    If NETComm1.InputData = Chr$(&H0) Then
        Let Flag = True
    End If
    Sleep (2)
Loop
End Sub

Private Sub mnuConnect_Click()
Status.Caption = "Resyncing"
Let Flag = False
Do Until (Flag = True)
    DoEvents
    NETComm1.Output = Chr$(&HFF)
    Sleep (100)
    If NETComm1.InputData = Chr$(&HFF) Then
        Let Flag = True
        Status.Caption = "Connected"
    End If
Loop
End Sub

Private Sub mnuExit_Click()
If NETComm1.PortOpen = True Then
    NETComm1.PortOpen = False
End If
Unload Me
End Sub

Private Sub Form_Load()
Dim DataIn As Variant
Dim Flag As Boolean
Dim A As Integer
Dim Y As Integer
Dim X As Integer
Dim Channel As Integer
Dim Level As Integer
Form1.Show
Status.Caption = "Connecting"
NETComm1.CommPort = 1
NETComm1.Settings = "19200,N,8,1"
NETComm1.InputLen = 1
NETComm1.InputMode = comInputModeText
NETComm1.PortOpen = True
Call mnuConnect_Click
End Sub

Private Sub DBO()
If NETComm1.PortOpen = False Then
    MsgBox ("Not Connected")
End If
Status.Caption = "Sending Blackout"
For A = 1 To 108
    DoEvents
    Call SetLevel(A, 0)
Next A
Status.Caption = "Blackout Done"
End Sub

Private Sub SetLevel(Channel, Level)
DimmerLevel((Channel - 1)).Value = Level
NETComm1.Output = Chr$(&HFF) & Chr$(Channel) & Chr$(Level)
Let Flag = False
Do Until (Flag = True)
    DoEvents
    If NETComm1.InputData = Chr$(&H0) Then
        Let Flag = True
    End If
    Sleep (2)
Loop
End Sub
