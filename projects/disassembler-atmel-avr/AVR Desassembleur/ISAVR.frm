VERSION 5.00
Object = "{F9043C88-F6F2-101A-A3C9-08002B2F49FB}#1.2#0"; "COMDLG32.OCX"
Begin VB.Form Feuille 
   BorderStyle     =   4  'Fixed ToolWindow
   Caption         =   "D�sassembleur AVR �C        V1.5               G.Henreaux"
   ClientHeight    =   3090
   ClientLeft      =   45
   ClientTop       =   315
   ClientWidth     =   4680
   LinkTopic       =   "Form1"
   MaxButton       =   0   'False
   MinButton       =   0   'False
   ScaleHeight     =   3090
   ScaleWidth      =   4680
   ShowInTaskbar   =   0   'False
   StartUpPosition =   3  'Windows Default
   Begin VB.PictureBox Picture2 
      Appearance      =   0  'Flat
      BorderStyle     =   0  'None
      ForeColor       =   &H80000008&
      Height          =   1335
      Left            =   240
      Picture         =   "DISAVR.frx":0000
      ScaleHeight     =   1335
      ScaleWidth      =   1455
      TabIndex        =   7
      Top             =   1320
      Width           =   1455
   End
   Begin VB.TextBox Text1 
      Height          =   1095
      Left            =   2040
      MultiLine       =   -1  'True
      TabIndex        =   6
      Text            =   "DISAVR.frx":4C6E
      Top             =   360
      Visible         =   0   'False
      Width           =   2535
   End
   Begin VB.PictureBox Picture1 
      Appearance      =   0  'Flat
      BackColor       =   &H80000005&
      BorderStyle     =   0  'None
      ForeColor       =   &H80000008&
      Height          =   855
      Left            =   120
      Picture         =   "DISAVR.frx":4D09
      ScaleHeight     =   855
      ScaleWidth      =   1695
      TabIndex        =   5
      Top             =   120
      Width           =   1695
   End
   Begin VB.CommandButton BoutonStop 
      Appearance      =   0  'Flat
      Height          =   735
      Left            =   3000
      MaskColor       =   &H8000000F&
      Picture         =   "DISAVR.frx":9FC3
      Style           =   1  'Graphical
      TabIndex        =   4
      ToolTipText     =   "Arr�t du programme"
      Top             =   840
      Visible         =   0   'False
      Width           =   735
   End
   Begin VB.ComboBox Combo1 
      Height          =   315
      Left            =   1800
      TabIndex        =   3
      Text            =   "Combo1"
      Top             =   2400
      Visible         =   0   'False
      Width           =   2655
   End
   Begin VB.TextBox Message 
      Alignment       =   2  'Center
      Appearance      =   0  'Flat
      BackColor       =   &H8000000F&
      BorderStyle     =   0  'None
      CausesValidation=   0   'False
      ForeColor       =   &H000000FF&
      Height          =   285
      Left            =   2400
      Locked          =   -1  'True
      TabIndex        =   2
      Text            =   "D�sassemblage en cours"
      Top             =   1800
      Visible         =   0   'False
      Width           =   1935
   End
   Begin MSComDlg.CommonDialog CommonDialog1 
      Left            =   360
      Top             =   2160
      _ExtentX        =   847
      _ExtentY        =   847
      _Version        =   393216
   End
   Begin VB.TextBox TexteOuvrir 
      Alignment       =   2  'Center
      Appearance      =   0  'Flat
      BackColor       =   &H8000000F&
      BorderStyle     =   0  'None
      CausesValidation=   0   'False
      Height          =   285
      Left            =   2640
      TabIndex        =   1
      Text            =   "Ouvrir fichier HEX"
      Top             =   720
      Width           =   1455
   End
   Begin VB.CommandButton Ouvrir 
      Height          =   375
      Left            =   3000
      Picture         =   "DISAVR.frx":B24D
      Style           =   1  'Graphical
      TabIndex        =   0
      Top             =   1080
      Width           =   495
   End
End
Attribute VB_Name = "Feuille"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
'V1.5 D�sassembleur 2 passes pour �C Atmel AVR

Dim Ligne As String, Depart As Boolean, DerniereAdrActu As Long, EmplErreur As Byte
Dim RepresentType_X_Y_Z As Boolean, NbredePointsAcces As Long, FichierBinaire() As Byte
Dim RepresentRegister As Boolean, RegInclude() As String, LabelInclude() As String, Modele As String
Dim NbreRegistres As Byte, TypeMicro() As String, NomFichierInc() As String, IndexProcesseur As Byte
Dim ExtendedRegExist As Boolean, ExtRegInclude() As String, ExtLabelInclude() As String
Dim NbreExtRegist As Byte, FichInc As String, NbreDigitsAdresse As Byte
Dim Provis() As String, ComptProvis As Long, Effaces() As String, ComptEffaces As Long
Dim ResultatDesassemblage() As Variant, CompteurPartiesDesassembl As Byte, CpteurMnemos As Byte
Dim DifferentsMnemo1() As String, DifferentsMnemo2() As String, SymboleCommentaire As String
Dim TailleFLASH As Long             'Taille (en Mots) de la m�moire FLASH du �C
Dim TailleFlashAdressable As Long   'Taille (en Mots) de la FLASH th�oriquement adressable par le PC du �C
Dim AddrDebut As Long               '1�re adresse (Mot)d'implantation du code dans la FLASH
Dim Nbre_Bytes_Fichier As Long      'Nombre de Bytes du code � d�sassembler
Dim DerniereAdresse As Long         'Derni�re adresse (en Mot) du code � d�sassembler
Const CheminRepertoire = "C:\Program Files\Atmel\AVR Tools\AvrAssembler2\Appnotes\"
        'R�pertoire des fichiers Include de Atmel AVR studio
Const LargeurTab = 8: Const NbreTab = 4
        'Param�trisation des tabulations dans NotePad pour le fichier de sortie cr��
Private Declare Function GetTickCount Lib "kernel32" () As Long

Private Sub Sleep(lgMSec As Long)
' Proc�dure de temporisation
' Le temps d'attente donn� en param�tre en millisecondes est approximatif
Dim lgTime As Long
lgTime = GetTickCount
Do While lgTime + lgMSec > GetTickCount
    DoEvents
    DoEvents
    DoEvents
Loop
End Sub

Private Sub BoutonStop_Click()
    'Terminer le programme
    End
End Sub

Private Sub Form_Load()
    Message.Visible = False 'Cacher le message "D�sassemblage en cours"
    TailleFlashAdressable = 0
    RepresentType_X_Y_Z = True 'repr�sentation type XL, XH, YL, YH, ZL, ZH pour R26, R27, R28, R29, R30, R31
    Call Recherche_Includes    'Parcourir r�pertoire des fichiers include
End Sub

Private Sub Ouvrir_Click()

'Recherche d'un fichier Include d'ATxmega (pas g�rable par cette version)
If InStr((NomFichierInc(Combo1.ListIndex)), "ATxmega") <> 0 Then
    MsgBox "D�sol�, mais ATxmega pas g�r� dans cette version", vbExclamation, "!!! D�sol� !!!"
    End
End If

Depart = 0 'Signaler que l'on va ouvrir le Fichier
EmplErreur = 0 'Initialiser signalisation Erreur dans Fichier
    Dim strNomFichier As String
        With Me.CommonDialog1
            .CancelError = True
            .Filter = "Fichiers Hexa (*.hex)|*.hex"  'le filtre
            .FilterIndex = 1
            .InitDir = App.Path  'le premier dossier visible sera celui de l'application
        On Error GoTo Annuler
        .ShowOpen  'lancer la boite de dialogue OUVRIR
        End With
        
        strNomFichier = Me.CommonDialog1.FileName  '** pour r�cup�rer le nom du fichier s�lectionn�

        Call Sleep(100)

        Message.Visible = True 'D�voiler le message "D�sassemblage en cours"
        
        'Cr�er un tableau correspondant au contenu du fichier ouvert ci-dessus
        Open strNomFichier For Input As 1
        Nbre_Bytes_Fichier = 0 'Initialiser le nbre de bytes contenus dans le tableau
        ReDim FichierBinaire(Nbre_Bytes_Fichier)
        Do Until EOF(1)
            EmplErreur = 0 'Initialiser signalisation Erreur dans Fichier
            Input #1, Ligne
                Call TransformBin(Ligne) 'R�enregistrer le Fichier sous forme de tableau de valeurs
        Loop
        Close
            

            EmplErreur = 0 'Initialiser signalisation Erreur dans Fichier
            
            DerniereAdresse = AddrDebut + (Nbre_Bytes_Fichier / 2) - 1
            
        Call LireInclude 'Lire et m�moriser fichier include (si n�cessaire)
        
        NbreDigitsAdresse = 4       'Par d�faut, adresses sur 4 digits
        If TailleFlashAdressable > (2 ^ 16) Then NbreDigitsAdresse = 6  'Corriger la valeur du
                                                                        'nombre de digits selon �C
            
        Call MemodesSauts 'M�moriser les divers points d'acc�s dans la routine � d�sassembler
                            '(C"est la 1�re passe du d�sassemblage)
                            
            
            Ouvrir.Visible = False 'Faire disparaitre bouton et liste combo
            Combo1.Visible = False '
            TexteOuvrir.Visible = False
            
        Call Desassemble(strNomFichier) 'D�sassembler le code
                            '(C"est la 2�me passe du d�sassemblage)
                            'Note: au passage, on fourni le nom du fichier original

        
        Message.Visible = False 'Cacher le message "D�sassemblage en cours"
        
        'On lance et on configure ConTEXT et on y colle le d�sassemblage
        Dim RetVal As Double, Bcl As Long
        RetVal = Shell("C:\Program Files\ConTEXT" & "\ConTEXT.exe", vbNormalFocus)
        AppActivate RetVal
        Call Sleep(100)
        SendKeys "^n" ' CTRL-N pour cr�er nouveau document vierge
        Call Sleep(100)
        'ALT-U-D-A pour choisir visu AtmelAVR
        SendKeys "%(uda)"
        Call Sleep(100)
        For Bcl = 0 To CompteurPartiesDesassembl
            Clipboard.Clear
            Call Sleep(100)
            Clipboard.SetText ResultatDesassemblage(Bcl)
            Call Sleep(100)
            SendKeys "^v" ' CTRL-V pour coller contenu du presse-papier
            Call Sleep(100)
        Next Bcl
        

        Feuille.Show
        BoutonStop.Visible = True


Annuler:
'Pas utilis� pour l'instant

        'End 'Sortir de la feuille
End Sub

Private Function FileExists(sFileName As String) As Boolean
    'Tester existence fichier
    On Error Resume Next
    FileExists = ((GetAttr(sFileName) And vbDirectory) = 0)
End Function

Private Sub LireInclude()
    'Lecture et m�morisation du fichier "include" s�lectionn�
    Dim LocaleVar As String, IndexVar As Byte, IndexVar2 As Byte: Modele = "": RepresentRegister = False
    IndexVar = 0: IndexVar2 = 0
    Dim Endroit As Byte, Parcoureur As Byte, PortionLabel As String, DernAdrFlash As String, PC As Byte
    RepresentRegister = False   'repr�senter les registres sous leurs formes brutes ($xx)
    ExtendedRegExist = False    'Ce processeur ne poss�de pas de registres �tendus
    
    If Combo1.Visible = False Then 'Pas de ComboBox donc pas de processeur s�lectionn�
        IndexProcesseur = 0: GoTo Passe
    End If
        'On a s�lectionn� un processeur dans la ComboList
    IndexProcesseur = Combo1.ListIndex
Passe:
    
    If IndexProcesseur = 0 Then
        'Pas de �C s�lectionn�:
        'Rechercher la taille de m�moire FLASH qui conviendrait juste bien
        If DerniereAdresse < 511 Then DerniereAdresse = 511
        TailleFlashAdressable = DerniereAdresse + 1
        For PC = 9 To 22    'Taille du Program Counter (de 9 � 22 bits)
            If (DerniereAdresse + 1) > (2 ^ (PC - 1)) And (DerniereAdresse + 1) < (2 ^ PC) Then
                TailleFlashAdressable = 2 ^ PC: GoTo PasseRechercheTailleFlash
            End If
        Next PC
PasseRechercheTailleFlash:
        TailleFLASH = TailleFlashAdressable
        Exit Sub 'Pas de fichier include � lire ==> repr�sentation brute
    End If
    
        RepresentRegister = True   'repr�senter les registres connus sous leurs labels (ex: PORTA)
        FichInc = NomFichierInc(IndexProcesseur)
        Open CheminRepertoire & FichInc For Input As #5 'Ouvrir fichier include
            Do Until EOF(5)
                Input #5, LocaleVar
                If InStr(LocaleVar, "I/O REGISTER DEFINITIONS") <> 0 Then GoTo incPt1
            Loop
            Call ErreurInclude 'Erreur de format du fichier inc
incPt1:
            Do Until EOF(5)
                Input #5, LocaleVar
                If InStr(LocaleVar, ".equ") <> 0 Then GoTo incPt2
            Loop
            Call ErreurInclude 'Erreur de format du fichier inc
incPt2:
            Modele = TypeMicro(IndexProcesseur) 'C'est le processeur s�lectionn�
            
incPt3:
                If InStr(LocaleVar, "MEMORY MAPPED") <> 0 Then
                    ExtendedRegExist = True 'Ce processeur poss�de des registres �tendus
                    IndexVar = IndexVar + 1: ReDim Preserve ExtRegInclude(IndexVar): ReDim Preserve _
                                                                                    ExtLabelInclude(IndexVar)
                    Endroit = InStr(LocaleVar, "0x") + 2
                    ExtRegInclude(IndexVar) = "": Parcoureur = 0
                        Do
                            PortionLabel = Mid(LocaleVar, Endroit + Parcoureur, 1)      '1 caractere
                            If PortionLabel = " " Or PortionLabel = Chr(9) Then Exit Do 'Rechercher caract�re = " "
                            Parcoureur = Parcoureur + 1
                            ExtRegInclude(IndexVar) = ExtRegInclude(IndexVar) + PortionLabel
                        Loop
                        ExtRegInclude(IndexVar) = DecToHex(HexToDec(ExtRegInclude(IndexVar)), 4, "0x")
                        Endroit = InStr(LocaleVar, ".equ") + 4
                        ExtLabelInclude(IndexVar) = "": Parcoureur = 0
                        Do
                            PortionLabel = Mid(LocaleVar, Endroit + Parcoureur, 1)          '1 caractere
                            If PortionLabel <> " " And PortionLabel <> Chr(9) Then Exit Do  'Rechercher caract�re <> " "
                            Parcoureur = Parcoureur + 1
                        Loop
                        Do
                            ExtLabelInclude(IndexVar) = ExtLabelInclude(IndexVar) + PortionLabel
                            Parcoureur = Parcoureur + 1
                            PortionLabel = Mid(LocaleVar, Endroit + Parcoureur, 1)          '1 caractere
                            If PortionLabel = " " Or PortionLabel = Chr(9) Then Exit Do     'Rechercher caract�re = " "
                        Loop
                End If
                If InStr(LocaleVar, "MEMORY MAPPED") = 0 Then
                    IndexVar2 = IndexVar2 + 1: ReDim Preserve RegInclude(IndexVar2): ReDim Preserve _
                                                                                    LabelInclude(IndexVar2)
                    Endroit = InStr(LocaleVar, "0x") + 2
                    RegInclude(IndexVar2) = "$" & UCase(Mid(LocaleVar, Endroit, 2))
                        Endroit = InStr(LocaleVar, ".equ") + 4
                        LabelInclude(IndexVar2) = "": Parcoureur = 0
                        Do
                            PortionLabel = Mid(LocaleVar, Endroit + Parcoureur, 1)          '1 caractere
                            If PortionLabel <> " " And PortionLabel <> Chr(9) Then Exit Do  'Rechercher caract�re <> " "
                            Parcoureur = Parcoureur + 1
                        Loop
                        Do
                            LabelInclude(IndexVar2) = LabelInclude(IndexVar2) + PortionLabel
                            Parcoureur = Parcoureur + 1
                            PortionLabel = Mid(LocaleVar, Endroit + Parcoureur, 1)          '1 caractere
                            If PortionLabel = " " Or PortionLabel = Chr(9) Then Exit Do     'Rechercher caract�re = " "
                        Loop
                End If
                
                Input #5, LocaleVar: If InStr(LocaleVar, ".equ") <> 0 Then GoTo incPt3 'Tester si fin des d�finitions
               
                Do Until EOF(5)
                    Input #5, LocaleVar
                    If InStr(LocaleVar, "FLASHEND") <> 0 Then GoTo incPt4
                Loop
                Call ErreurInclude 'Erreur de format du fichier inc
incPt4:
                    If InStr(LocaleVar, "Word address") = 0 Then Call ErreurInclude 'Erreur de format du fichier inc
                    If InStr(LocaleVar, ".equ") = 0 Then Call ErreurInclude         'Erreur de format du fichier inc
                    
                        Endroit = InStr(LocaleVar, "0x") + 2
                        DernAdrFlash = "": Parcoureur = 0
                        Do
                            PortionLabel = Mid(LocaleVar, Endroit + Parcoureur, 1)          '1 caractere
                            If PortionLabel <> " " And PortionLabel <> Chr(9) Then Exit Do  'Rechercher caract�re <> " "
                            Parcoureur = Parcoureur + 1
                        Loop
                        Do
                            DernAdrFlash = DernAdrFlash + UCase(PortionLabel)
                            Parcoureur = Parcoureur + 1
                            PortionLabel = Mid(LocaleVar, Endroit + Parcoureur, 1)          '1 caractere
                            If PortionLabel = " " Or PortionLabel = Chr(9) Then Exit Do     'Rechercher caract�re = " "
                        Loop
                        TailleFLASH = HexToDec(DernAdrFlash) + 1 'On r�cup�re ici la taille de la FLASH (en Mots)
                        
                        
        Close 5
        
        'Corriger les tailles de m�moires FLASH ambig�es
        TailleFlashAdressable = TailleFLASH
        For PC = 9 To 22    'Taille du Program Counter (de 9 � 22 bits)
            If TailleFLASH > (2 ^ (PC - 1)) And TailleFLASH < (2 ^ PC) Then
                TailleFlashAdressable = 2 ^ PC: GoTo PasseCorrTailleFlash
            End If
        Next PC
PasseCorrTailleFlash:
        

    NbreRegistres = IndexVar2   'C'est le nbre de registres d�finis dans le fichier include
    NbreExtRegist = IndexVar    'C'est le nbre de registres �tendus d�finis dans le fichier include
End Sub

Private Sub TransformBin(Entree As String)
'R�enregistrer le Fichier sous forme d'un tableau de valeurs
Dim TypedeDonnees As Long, NbredeDonnees As Long, PremAdresseLigne As String, AdrActu As Long
Dim Adr As Byte
EmplErreur = 1 'Initialiser signalisation Erreur dans Fichier
        If Left(Entree, 1) <> ":" Then Call GestErreurFormat(EmplErreur) 'Erreur Fichier
        TypedeDonnees = HexToDec(Mid(Entree, 8, 2)): If TypedeDonnees <> 0 Then Exit Sub
        'Note: Type 0 = donn�es utiles
        NbredeDonnees = HexToDec(Mid(Entree, 2, 2))
        'Note: Nbre de donnees valides dans la ligne
            If Len(Entree) <> ((NbredeDonnees * 2) + 11) Then Call GestErreurFormat(EmplErreur) 'Erreur Fichier
        PremAdresseLigne = (Mid(Entree, 4, 4)): AdrActu = HexToDec(PremAdresseLigne)
        'Note: Premi�re adresse m�moire o� loger les donn�es ci-apr�s
        
            
            If Depart = -1 Then 'On a d�j� lu des donn�es dans le Fichier
                If AdrActu < DerniereAdrActu Then
                    Call GestErreurFormat(EmplErreur) 'Erreur Fichier
                                          'Erreur ==> ceci n'est pas possible
                End If
Test:
                If AdrActu > DerniereAdrActu Then 'Il y a un "blanc" � combler
                    FichierBinaire(Nbre_Bytes_Fichier) = &HFF
                    DerniereAdrActu = DerniereAdrActu + 1
                    Nbre_Bytes_Fichier = Nbre_Bytes_Fichier + 1 'Mettre � jour le nbre de bytes du tableau
                    ReDim Preserve FichierBinaire(Nbre_Bytes_Fichier)
                    GoTo Test 'Encore � remplir ?
                End If
            End If
            
            If Depart = 0 Then 'On lit la 1�re ligne de donn�es du fichier
                Depart = -1
                AddrDebut = Int((HexToDec(PremAdresseLigne)) / 2)   'On r�cup�re valeur 1�re adresse du code
                                                                    ' (format Mots)
                If (AddrDebut * 2) <> (HexToDec(PremAdresseLigne)) Then Call GestErreurFormat(EmplErreur) 'Erreur Fichier
            End If
            
            For Adr = 1 To NbredeDonnees
                FichierBinaire(Nbre_Bytes_Fichier) = HexToDec(Mid(Entree, 8 + (Adr * 2), 2))
                Nbre_Bytes_Fichier = Nbre_Bytes_Fichier + 1 'Mettre � jour le nbre de bytes du tableau
                AdrActu = AdrActu + 1 'Mettre � jour le surveillant d'adresse m�moire
                ReDim Preserve FichierBinaire(Nbre_Bytes_Fichier)
            Next Adr
            DerniereAdrActu = AdrActu 'M�moriser derni�re adresse �crite (+1)
End Sub

Private Sub Memoris_Sortie(Chaine As String)
        'M�moriser le r�sultat d�sassembl� dans un tableau de variables STRING
        If Len(ResultatDesassemblage(CompteurPartiesDesassembl)) > 60000 Then
            CompteurPartiesDesassembl = CompteurPartiesDesassembl + 1
            ReDim Preserve ResultatDesassemblage(CompteurPartiesDesassembl)
            ResultatDesassemblage(CompteurPartiesDesassembl) = ""
        End If
        ResultatDesassemblage(CompteurPartiesDesassembl) = ResultatDesassemblage(CompteurPartiesDesassembl) _
                                                                                    & Chaine & Chr(13) & Chr(10)
End Sub

Private Sub Memoris_Sortie2(Chaine As String)
        'M�moriser le r�sultat d�sassembl� dans un tableau de variables STRING
        If Len(ResultatDesassemblage(CompteurPartiesDesassembl)) > 60000 Then
            CompteurPartiesDesassembl = CompteurPartiesDesassembl + 1
            ReDim Preserve ResultatDesassemblage(CompteurPartiesDesassembl)
            ResultatDesassemblage(CompteurPartiesDesassembl) = ""
        End If
        ResultatDesassemblage(CompteurPartiesDesassembl) = ResultatDesassemblage(CompteurPartiesDesassembl) & Chaine
End Sub

Private Sub MemorisationDiversMnemoniques(Mnemo1 As String, Mnemo2 As String)
        ' Pour m�mo des divers Mn�moniques d'un m�me code
        ' NOTE: on ajoute "; " devant mn�monique si ce n'est pas le premier trouv�
        If (Mnemo1 = "0") And (Mnemo2 = "0") Then 'Initialisation
            CpteurMnemos = 0
            ReDim DifferentsMnemo1(0): ReDim DifferentsMnemo2(0)
            SymboleCommentaire = ""
            Exit Sub
        End If
            
        DifferentsMnemo1(CpteurMnemos) = SymboleCommentaire & Mnemo1: DifferentsMnemo2(CpteurMnemos) = Mnemo2
        CpteurMnemos = CpteurMnemos + 1
        ReDim Preserve DifferentsMnemo1(CpteurMnemos): ReDim Preserve DifferentsMnemo2(CpteurMnemos)
        SymboleCommentaire = "; "
End Sub

Private Sub Desassemble(FichierHEX As String)
        'D�sassemblage du contenu du tableau de valeurs Bytes
        'Note: au passage, on r�cup�re le nom du fichier original
        Dim a1 As Long, a2 As Long, a3 As Long, a4 As Long, Mnemonique1 As String, Parametre_s As Byte
        Dim Parametre_d As Byte, Partie1 As Byte, Partie2 As Byte, Parametre_r As Byte, AdrActu As Long
        Dim Parametre_k As Long, Paramtre_K As Long, Nbre_Bytes_Lus As Long, NbredeMots As Byte
        Dim Parametre_A As Long, Parametre_b As Byte, Parametre_q As Byte, PosDsMnemo As Byte, Cpteur1 As Byte
        Dim Label_a_placer As String, Nbre_Labels_Lus As Long, Addr_Label As Long, Mnemonique2 As String
        Dim Label_a_Imprimer As String, Parcoureur_ As Byte, LieuduSaut As Long, Avertissement As Boolean
        Dim NombredeTab As Byte, IndexTab As Byte, Anoter As Boolean, LocaleVar As String, LocaleVar2 As String
        Dim SREG(7) As String: SREG(0) = "SREG_C": SREG(1) = "SREG_Z": SREG(2) = "SREG_N": SREG(3) = "SREG_V"
        SREG(4) = "SREG_S": SREG(5) = "SREG_H": SREG(6) = "SREG_T": SREG(7) = "SREG_I"
        
        'Initialisation variables qui contiendront le texte final
        CompteurPartiesDesassembl = 0 'Initialiser compteur de Parties r�sultat d�sassemblage
        ReDim ResultatDesassemblage(CompteurPartiesDesassembl)
        ResultatDesassemblage(CompteurPartiesDesassembl) = ""
        
        Nbre_Bytes_Lus = 0: AdrActu = AddrDebut
        
        If NbredePointsAcces > 0 Then
            Nbre_Labels_Lus = 1: Label_a_placer = Provis(Nbre_Labels_Lus - 1) 'Le 1er Label � placer
        End If                                                                '(Provient de 1�re passe)
        
        
        Call Memoris_Sortie("; D�sassemblage de " & FichierHEX)             'Ajouter le nom du Fichier source
        If Modele <> "" Then Call Memoris_Sortie("; Processeur " & Modele)  'Ajouter le type du processeur
                                                                            's'il est connu
        If RepresentRegister = True Then Call Memoris_Sortie(Chr(9) & ".include " & Chr(34) & FichInc & Chr(34))
                                                                            'Nom fichier include
        Call Memoris_Sortie(Chr(9) & ".cseg")
        Call Memoris_Sortie(Chr(9) & ".org" & DecToHex(AddrDebut, NbreDigitsAdresse, " 0x")) 'L'adresse de d�but du d�sassemblage
        
        Call Memoris_Sortie(";============================================")
        
        ' ==================
        ' D�but de la boucle
        ' ==================
        Do
        ' ==================
        ' D�but de la boucle
        ' ==================
        
            If (AdrActu >= TailleFLASH) And Avertissement = False Then
                MsgBox "Erreur de taille par rapport au �C s�lectionn�", vbExclamation, "Note pour d�sassemblage"
                Avertissement = True 'Signaler que l'avertissement a �t� vu
                Call Memoris_Sortie(Chr(13))
                Call Memoris_Sortie(Chr(9) & ";* * * derni�re adresses m�m FLASH �C atteinte * * *")
                Call Memoris_Sortie(Chr(9) & ";* * *   le �C s�lectionn� n'est pas le bon    * * *")
                Call Memoris_Sortie((chr13))
            End If
        
            Mnemonique1 = "unknow": Mnemonique2 = "" 'Mn�monique par d�faut (subsistera si pas instruction)
            Call MemorisationDiversMnemoniques("0", "0") 'Initialiser compteur de m�morisation divers mn�mos
            NbredeMots = 1 'Nbre de Mots de l'instruction que l'on va lire = 1 par d�faut
            'Lire un mot � d�sassembler dans le tableau
            a1 = FichierBinaire(Nbre_Bytes_Lus): Nbre_Bytes_Lus = Nbre_Bytes_Lus + 1
            a2 = FichierBinaire(Nbre_Bytes_Lus): Nbre_Bytes_Lus = Nbre_Bytes_Lus + 1
            
            If AdrActu = DerniereAdresse Then GoTo EmpecherReaquerir2 'Pas d'instructions sur 2 Mots (c'est la fin)
            Partie1 = a2 And &HFE: Partie2 = a1 And &HF
            If Partie1 = &H90 And Partie2 = 0 Then GoTo ReAcquerir '1 Mot en plus (2 Mots)
            If Partie1 = &H92 And Partie2 = 0 Then GoTo ReAcquerir '1 Mot en plus (2 Mots)
            Partie2 = a1 And &HC
            If Partie1 = &H94 And Partie2 = &HC Then GoTo ReAcquerir '1 Mot en plus (2 Mots)
EmpecherReaquerir2:

            If a2 = &H0 And a1 = &H0 Then
                Mnemonique1 = "nop": Call MemorisationDiversMnemoniques(Mnemonique1, Mnemonique2)
            End If
            If a2 = &H94 Then
                If a1 = &H8 Then
                    Mnemonique1 = "sec": Call MemorisationDiversMnemoniques(Mnemonique1, Mnemonique2)
                End If
                If a1 = &H9 Then
                    Mnemonique1 = "ijmp": Call MemorisationDiversMnemoniques(Mnemonique1, Mnemonique2)
                End If
                If a1 = &H18 Then
                    Mnemonique1 = "sez": Call MemorisationDiversMnemoniques(Mnemonique1, Mnemonique2)
                End If
                If a1 = &H19 Then
                    Mnemonique1 = "eijmp": Call MemorisationDiversMnemoniques(Mnemonique1, Mnemonique2)
                End If
                If a1 = &H28 Then
                    Mnemonique1 = "sen": Call MemorisationDiversMnemoniques(Mnemonique1, Mnemonique2)
                End If
                If a1 = &H38 Then
                    Mnemonique1 = "sev": Call MemorisationDiversMnemoniques(Mnemonique1, Mnemonique2)
                End If
                If a1 = &H48 Then
                    Mnemonique1 = "ses": Call MemorisationDiversMnemoniques(Mnemonique1, Mnemonique2)
                End If
                If a1 = &H58 Then
                    Mnemonique1 = "seh": Call MemorisationDiversMnemoniques(Mnemonique1, Mnemonique2)
                End If
                If a1 = &H68 Then
                    Mnemonique1 = "set": Call MemorisationDiversMnemoniques(Mnemonique1, Mnemonique2)
                End If
                If a1 = &H78 Then
                    Mnemonique1 = "sei": Call MemorisationDiversMnemoniques(Mnemonique1, Mnemonique2)
                End If
                If a1 = &H88 Then
                    Mnemonique1 = "clc": Call MemorisationDiversMnemoniques(Mnemonique1, Mnemonique2)
                End If
                If a1 = &H98 Then
                    Mnemonique1 = "clz": Call MemorisationDiversMnemoniques(Mnemonique1, Mnemonique2)
                End If
                If a1 = &HA8 Then
                    Mnemonique1 = "cln": Call MemorisationDiversMnemoniques(Mnemonique1, Mnemonique2)
                End If
                If a1 = &HB8 Then
                    Mnemonique1 = "clv": Call MemorisationDiversMnemoniques(Mnemonique1, Mnemonique2)
                End If
                If a1 = &HC8 Then
                    Mnemonique1 = "cls": Call MemorisationDiversMnemoniques(Mnemonique1, Mnemonique2)
                End If
                If a1 = &HD8 Then
                    Mnemonique1 = "clh": Call MemorisationDiversMnemoniques(Mnemonique1, Mnemonique2)
                End If
                If a1 = &HE8 Then
                    Mnemonique1 = "clt": Call MemorisationDiversMnemoniques(Mnemonique1, Mnemonique2)
                End If
                If a1 = &HF8 Then
                    Mnemonique1 = "cli": Call MemorisationDiversMnemoniques(Mnemonique1, Mnemonique2)
                End If
            End If
            If a2 = &H95 Then
                If a1 = &H8 Then
                    Mnemonique1 = "ret": Call MemorisationDiversMnemoniques(Mnemonique1, Mnemonique2)
                End If
                If a1 = &H9 Then
                    Mnemonique1 = "icall": Call MemorisationDiversMnemoniques(Mnemonique1, Mnemonique2)
                End If
                If a1 = &H18 Then
                    Mnemonique1 = "reti": Call MemorisationDiversMnemoniques(Mnemonique1, Mnemonique2)
                End If
                If a1 = &H19 Then
                    Mnemonique1 = "eicall": Call MemorisationDiversMnemoniques(Mnemonique1, Mnemonique2)
                End If
                If a1 = &H88 Then
                    Mnemonique1 = "sleep": Call MemorisationDiversMnemoniques(Mnemonique1, Mnemonique2)
                End If
                If a1 = &H98 Then
                    Mnemonique1 = "break": Call MemorisationDiversMnemoniques(Mnemonique1, Mnemonique2)
                End If
                If a1 = &HA8 Then
                    Mnemonique1 = "wdr": Call MemorisationDiversMnemoniques(Mnemonique1, Mnemonique2)
                End If
                If a1 = &HC8 Then
                    Mnemonique1 = "lpm": Call MemorisationDiversMnemoniques(Mnemonique1, Mnemonique2)
                End If
                If a1 = &HD8 Then
                    Mnemonique1 = "elpm": Call MemorisationDiversMnemoniques(Mnemonique1, Mnemonique2)
                End If
                If a1 = &HE8 Then
                    Mnemonique1 = "spm": Call MemorisationDiversMnemoniques(Mnemonique1, Mnemonique2)
                End If
            End If
        
            If a2 = &H94 Then
                    Parametre_s = ((a1 And &H70) / 16)
                If (a1 And &H88) = &H8 Then
                    Mnemonique1 = "bset": Mnemonique2 = Parametre_s
                    Call MemorisationDiversMnemoniques(Mnemonique1, Mnemonique2)
                End If
                If (a1 And &H88) = &H88 Then
                    Mnemonique1 = "bclr": Mnemonique2 = Parametre_s
                    Call MemorisationDiversMnemoniques(Mnemonique1, Mnemonique2)
                End If
            End If
            If a2 = &HEF Then
                    Parametre_d = ((a1 And &HF0) / 16) + 16
                If (a1 And &HF) = &HF Then
                    Mnemonique1 = "ser": Mnemonique2 = "R" & Parametre_d
                    Call MemorisationDiversMnemoniques(Mnemonique1, Mnemonique2)
                End If
            End If
            Partie1 = (a2 And &HFE): Partie2 = (a1 And &HF)
                    Parametre_d = ((a2 And 1) * 16) + ((a1 And &HF0) / 16): Parametre_r = Parametre_d
            If Partie1 = &H80 Then
                If Partie2 = &H0 Then
                    Mnemonique1 = "ld": Mnemonique2 = "R" & Parametre_d & ", Z"
                    Call MemorisationDiversMnemoniques(Mnemonique1, Mnemonique2)
                End If
                If Partie2 = &H8 Then
                    Mnemonique1 = "ld": Mnemonique2 = "R" & Parametre_d & ", Y"
                    Call MemorisationDiversMnemoniques(Mnemonique1, Mnemonique2)
                End If
            End If
            If Partie1 = &H82 Then
                If Partie2 = &H0 Then
                    Mnemonique1 = "st": Mnemonique2 = "Z, R" & Parametre_r
                    Call MemorisationDiversMnemoniques(Mnemonique1, Mnemonique2)
                End If
                If Partie2 = &H8 Then
                    Mnemonique1 = "st": Mnemonique2 = "Y, R" & Parametre_r
                    Call MemorisationDiversMnemoniques(Mnemonique1, Mnemonique2)
                End If
            End If
            If Partie1 = &H90 Then
                If Partie2 = &H1 Then
                    Mnemonique1 = "ld": Mnemonique2 = "R" & Parametre_d & ", Z+"
                    Call MemorisationDiversMnemoniques(Mnemonique1, Mnemonique2)
                End If
                If Partie2 = &H2 Then
                    Mnemonique1 = "ld": Mnemonique2 = "R" & Parametre_d & ", -Z"
                    Call MemorisationDiversMnemoniques(Mnemonique1, Mnemonique2)
                End If
                If Partie2 = &H4 Then
                    Mnemonique1 = "lpm": Mnemonique2 = "R" & Parametre_d & ", Z"
                    Call MemorisationDiversMnemoniques(Mnemonique1, Mnemonique2)
                End If
                If Partie2 = &H5 Then
                    Mnemonique1 = "lpm": Mnemonique2 = "R" & Parametre_d & ", Z+"
                    Call MemorisationDiversMnemoniques(Mnemonique1, Mnemonique2)
                End If
                If Partie2 = &H6 Then
                    Mnemonique1 = "elpm": Mnemonique2 = "R" & Parametre_d & ", Z"
                    Call MemorisationDiversMnemoniques(Mnemonique1, Mnemonique2)
                End If
                If Partie2 = &H7 Then
                    Mnemonique1 = "elpm": Mnemonique2 = "R" & Parametre_d & ", Z+"
                    Call MemorisationDiversMnemoniques(Mnemonique1, Mnemonique2)
                End If
                If Partie2 = &H9 Then
                    Mnemonique1 = "ld": Mnemonique2 = "R" & Parametre_d & ", Y+"
                    Call MemorisationDiversMnemoniques(Mnemonique1, Mnemonique2)
                End If
                If Partie2 = &HA Then
                    Mnemonique1 = "ld": Mnemonique2 = "R" & Parametre_d & ", -Y"
                    Call MemorisationDiversMnemoniques(Mnemonique1, Mnemonique2)
                End If
                If Partie2 = &HC Then
                    Mnemonique1 = "ld": Mnemonique2 = "R" & Parametre_d & ", X"
                    Call MemorisationDiversMnemoniques(Mnemonique1, Mnemonique2)
                End If
                If Partie2 = &HD Then
                    Mnemonique1 = "ld": Mnemonique2 = "R" & Parametre_d & ", X+"
                    Call MemorisationDiversMnemoniques(Mnemonique1, Mnemonique2)
                End If
                If Partie2 = &HE Then
                    Mnemonique1 = "ld": Mnemonique2 = "R" & Parametre_d & ", -X"
                    Call MemorisationDiversMnemoniques(Mnemonique1, Mnemonique2)
                End If
                If Partie2 = &HF Then
                    Mnemonique1 = "pop": Mnemonique2 = "R" & Parametre_d
                    Call MemorisationDiversMnemoniques(Mnemonique1, Mnemonique2)
                End If
            End If
            If Partie1 = &H92 Then
                If Partie2 = &H1 Then
                    Mnemonique1 = "st": Mnemonique2 = "Z+, R" & Parametre_r
                    Call MemorisationDiversMnemoniques(Mnemonique1, Mnemonique2)
                End If
                If Partie2 = &H2 Then
                    Mnemonique1 = "st": Mnemonique2 = "-Z, R" & Parametre_r
                    Call MemorisationDiversMnemoniques(Mnemonique1, Mnemonique2)
                End If
                If Partie2 = &H9 Then
                    Mnemonique1 = "st": Mnemonique2 = "Y+, R" & Parametre_r
                    Call MemorisationDiversMnemoniques(Mnemonique1, Mnemonique2)
                End If
                If Partie2 = &HA Then
                    Mnemonique1 = "st": Mnemonique2 = "-Y, R" & Parametre_r
                    Call MemorisationDiversMnemoniques(Mnemonique1, Mnemonique2)
                End If
                If Partie2 = &HC Then
                    Mnemonique1 = "st": Mnemonique2 = "X, R" & Parametre_r
                    Call MemorisationDiversMnemoniques(Mnemonique1, Mnemonique2)
                End If
                If Partie2 = &HD Then
                    Mnemonique1 = "st": Mnemonique2 = "X+, R" & Parametre_r
                    Call MemorisationDiversMnemoniques(Mnemonique1, Mnemonique2)
                End If
                If Partie2 = &HE Then
                    Mnemonique1 = "st": Mnemonique2 = "-X, R" & Parametre_r
                    Call MemorisationDiversMnemoniques(Mnemonique1, Mnemonique2)
                End If
                If Partie2 = &HF Then
                    Mnemonique1 = "push": Mnemonique2 = "R" & Parametre_r
                    Call MemorisationDiversMnemoniques(Mnemonique1, Mnemonique2)
                End If
            End If
            If Partie1 = &H94 Then
                If Partie2 = &H0 Then
                    Mnemonique1 = "com": Mnemonique2 = "R" & Parametre_d
                    Call MemorisationDiversMnemoniques(Mnemonique1, Mnemonique2)
                End If
                If Partie2 = &H1 Then
                    Mnemonique1 = "neg": Mnemonique2 = "R" & Parametre_d
                    Call MemorisationDiversMnemoniques(Mnemonique1, Mnemonique2)
                End If
                If Partie2 = &H2 Then
                    Mnemonique1 = "swap": Mnemonique2 = "R" & Parametre_d
                    Call MemorisationDiversMnemoniques(Mnemonique1, Mnemonique2)
                End If
                If Partie2 = &H3 Then
                    Mnemonique1 = "inc": Mnemonique2 = "R" & Parametre_d
                    Call MemorisationDiversMnemoniques(Mnemonique1, Mnemonique2)
                End If
                If Partie2 = &H5 Then
                    Mnemonique1 = "asr": Mnemonique2 = "R" & Parametre_d
                    Call MemorisationDiversMnemoniques(Mnemonique1, Mnemonique2)
                End If
                If Partie2 = &H6 Then
                    Mnemonique1 = "lsr": Mnemonique2 = "R" & Parametre_d
                    Call MemorisationDiversMnemoniques(Mnemonique1, Mnemonique2)
                End If
                If Partie2 = &H7 Then
                    Mnemonique1 = "ror": Mnemonique2 = "R" & Parametre_d
                    Call MemorisationDiversMnemoniques(Mnemonique1, Mnemonique2)
                End If
                If Partie2 = &HA Then
                    Mnemonique1 = "dec": Mnemonique2 = "R" & Parametre_d
                    Call MemorisationDiversMnemoniques(Mnemonique1, Mnemonique2)
                End If
            End If
        
            If a2 = &H3 Then
            Partie2 = (a1 And &H88)
                    Parametre_d = ((a1 And &H70) / 16): Parametre_r = (a1 And &H7)
                If Partie2 = &H0 Then
                    Mnemonique1 = "mulsu": Mnemonique2 = "R" & (Parametre_d + 16) & "," & " R" & (Parametre_r + 16)
                    Call MemorisationDiversMnemoniques(Mnemonique1, Mnemonique2)
                End If
                If Partie2 = &H8 Then
                    Mnemonique1 = "fmul": Mnemonique2 = "R" & (Parametre_d + 16) & "," & " R" & (Parametre_r + 16)
                    Call MemorisationDiversMnemoniques(Mnemonique1, Mnemonique2)
                End If
                If Partie2 = &H80 Then
                    Mnemonique1 = "fmuls": Mnemonique2 = "R" & (Parametre_d + 16) & "," & " R" & (Parametre_r + 16)
                    Call MemorisationDiversMnemoniques(Mnemonique1, Mnemonique2)
                End If
                If Partie2 = &H88 Then
                    Mnemonique1 = "fmulsu": Mnemonique2 = "R" & (Parametre_d + 16) & "," & " R" & (Parametre_r + 16)
                    Call MemorisationDiversMnemoniques(Mnemonique1, Mnemonique2)
                End If
            End If
            Partie1 = a2 And &HFC: Partie2 = a1 And 7
                    Parametre_k = ((a2 And 1) * 32) + ((a1 And &HF8) / 8)
                        'Arranger valeur (sign�e) de "k"
                        If ((a2 And 2) = 2) Then Parametre_k = (64 - Parametre_k) * (-1)
                        Parametre_k = Parametre_k + 1 'Arranger valeur (sign�e) de "k"
                        LieuduSaut = AdrActu + Parametre_k: Call CorrectPtMemFlash(LieuduSaut, Anoter)
                        If Anoter = True Then Label_a_Imprimer = DecToHex((LieuduSaut), NbreDigitsAdresse, "AVR")
                        If Anoter = False Then Label_a_Imprimer = DecToHex((LieuduSaut), NbreDigitsAdresse, "0x")
            If Partie1 = &HF0 Then
                If Partie2 = &H0 Then
                    Mnemonique1 = "brlo": Mnemonique2 = Label_a_Imprimer  'Note: idem � "brcs "
                    Call MemorisationDiversMnemoniques(Mnemonique1, Mnemonique2)
                    Mnemonique1 = "brcs": Mnemonique2 = Label_a_Imprimer
                    Call MemorisationDiversMnemoniques(Mnemonique1, Mnemonique2)
                End If
                If Partie2 = &H1 Then
                    Mnemonique1 = "breq": Mnemonique2 = Label_a_Imprimer
                    Call MemorisationDiversMnemoniques(Mnemonique1, Mnemonique2)
                End If
                If Partie2 = &H2 Then
                    Mnemonique1 = "brmi": Mnemonique2 = Label_a_Imprimer
                    Call MemorisationDiversMnemoniques(Mnemonique1, Mnemonique2)
                End If
                If Partie2 = &H3 Then
                    Mnemonique1 = "brvs": Mnemonique2 = Label_a_Imprimer
                    Call MemorisationDiversMnemoniques(Mnemonique1, Mnemonique2)
                End If
                If Partie2 = &H4 Then
                    Mnemonique1 = "brlt": Mnemonique2 = Label_a_Imprimer
                    Call MemorisationDiversMnemoniques(Mnemonique1, Mnemonique2)
                End If
                If Partie2 = &H5 Then
                    Mnemonique1 = "brhs": Mnemonique2 = Label_a_Imprimer
                    Call MemorisationDiversMnemoniques(Mnemonique1, Mnemonique2)
                End If
                If Partie2 = &H6 Then
                    Mnemonique1 = "brts": Mnemonique2 = Label_a_Imprimer
                    Call MemorisationDiversMnemoniques(Mnemonique1, Mnemonique2)
                End If
                If Partie2 = &H7 Then
                    Mnemonique1 = "brie": Mnemonique2 = Label_a_Imprimer
                    Call MemorisationDiversMnemoniques(Mnemonique1, Mnemonique2)
                End If
            End If
            If Partie1 = &HF4 Then
                If Partie2 = &H0 Then
                    Mnemonique1 = "brsh": Mnemonique2 = Label_a_Imprimer  'Note: idem � "brcc "
                    Call MemorisationDiversMnemoniques(Mnemonique1, Mnemonique2)
                    Mnemonique1 = "brcc": Mnemonique2 = Label_a_Imprimer
                    Call MemorisationDiversMnemoniques(Mnemonique1, Mnemonique2)
                End If
                If Partie2 = &H1 Then
                    Mnemonique1 = "brne": Mnemonique2 = Label_a_Imprimer
                    Call MemorisationDiversMnemoniques(Mnemonique1, Mnemonique2)
                End If
                If Partie2 = &H2 Then
                    Mnemonique1 = "brpl": Mnemonique2 = Label_a_Imprimer
                    Call MemorisationDiversMnemoniques(Mnemonique1, Mnemonique2)
                End If
                If Partie2 = &H3 Then
                    Mnemonique1 = "brvc": Mnemonique2 = Label_a_Imprimer
                    Call MemorisationDiversMnemoniques(Mnemonique1, Mnemonique2)
                End If
                If Partie2 = &H4 Then
                    Mnemonique1 = "brge": Mnemonique2 = Label_a_Imprimer
                    Call MemorisationDiversMnemoniques(Mnemonique1, Mnemonique2)
                End If
                If Partie2 = &H5 Then
                    Mnemonique1 = "brhc": Mnemonique2 = Label_a_Imprimer
                    Call MemorisationDiversMnemoniques(Mnemonique1, Mnemonique2)
                End If
                If Partie2 = &H6 Then
                    Mnemonique1 = "brtc": Mnemonique2 = Label_a_Imprimer
                    Call MemorisationDiversMnemoniques(Mnemonique1, Mnemonique2)
                End If
                If Partie2 = &H7 Then
                    Mnemonique1 = "brid": Mnemonique2 = Label_a_Imprimer
                    Call MemorisationDiversMnemoniques(Mnemonique1, Mnemonique2)
                End If
            End If
        
                    Parametre_d = (((a1 And &HF0) / 16) * 2): Parametre_r = ((a1 And &HF) * 2)
            If a2 = &H1 Then
                Mnemonique1 = "movw": Mnemonique2 = "R" & (Parametre_d + 1) & ":R" & Parametre_d & ", R" & _
                        (Parametre_r + 1) & ":R" & Parametre_r
                Call MemorisationDiversMnemoniques(Mnemonique1, Mnemonique2)
            End If
                    Parametre_d = (((a1 And &HF0) / 16) + 16): Parametre_r = ((a1 And &HF) + 16)
            If a2 = &H2 Then
                Mnemonique1 = "muls": Mnemonique2 = "R" & Parametre_d & ", R" & Parametre_r
                Call MemorisationDiversMnemoniques(Mnemonique1, Mnemonique2)
            End If
                    Parametre_d = (((a1 And &H30) / 8) + 24): Paramtre_K = (a1 And &HF) + ((a1 And &HC0) / 4)
            If a2 = &H96 Then
                Mnemonique1 = "adiw": Mnemonique2 = "R" & (Parametre_d + 1) & ":R" & Parametre_d & ", " & _
                        DecToHex(Paramtre_K, 2, "0x")
                Call MemorisationDiversMnemoniques(Mnemonique1, Mnemonique2)
            End If
            If a2 = &H97 Then
                Mnemonique1 = "sbiw": Mnemonique2 = "R" & (Parametre_d + 1) & ":R" & Parametre_d & ", " & _
                        DecToHex(Paramtre_K, 2, "0x")
                Call MemorisationDiversMnemoniques(Mnemonique1, Mnemonique2)
            End If
                    Parametre_A = (a1 And &HF8) / 8: Parametre_b = a1 And &H7
            If a2 = &H98 Then
                Mnemonique1 = "cbi": Mnemonique2 = DecToHex(Parametre_A, 2, "$") & ", " & Parametre_b
                Call MemorisationDiversMnemoniques(Mnemonique1, Mnemonique2)
            End If
            If a2 = &H99 Then
                Mnemonique1 = "sbic": Mnemonique2 = DecToHex(Parametre_A, 2, "$") & ", " & Parametre_b
                Call MemorisationDiversMnemoniques(Mnemonique1, Mnemonique2)
            End If
            If a2 = &H9A Then
                Mnemonique1 = "sbi": Mnemonique2 = DecToHex(Parametre_A, 2, "$") & ", " & Parametre_b
                Call MemorisationDiversMnemoniques(Mnemonique1, Mnemonique2)
            End If
            If a2 = &H9B Then
                Mnemonique1 = "sbis": Mnemonique2 = DecToHex(Parametre_A, 2, "$") & ", " & Parametre_b
                Call MemorisationDiversMnemoniques(Mnemonique1, Mnemonique2)
            End If
            If ((a2 And &HF8) = &HF8) And ((a1 And &H8) = 0) Then
                Parametre_d = ((a1 And &HF0) / 16) + ((a2 And &H1) * 16): Parametre_r = Parametre_d
                If (a2 And &H6) = 0 Then
                    Mnemonique1 = "bld": Mnemonique2 = "R" & Parametre_d & ", " & Parametre_b
                    Call MemorisationDiversMnemoniques(Mnemonique1, Mnemonique2)
                End If
                If (a2 And &H6) = 2 Then
                    Mnemonique1 = "bst": Mnemonique2 = "R" & Parametre_d & ", " & Parametre_b
                    Call MemorisationDiversMnemoniques(Mnemonique1, Mnemonique2)
                End If
                If (a2 And &H6) = 4 Then
                    Mnemonique1 = "sbrc": Mnemonique2 = "R" & Parametre_r & ", " & Parametre_b
                    Call MemorisationDiversMnemoniques(Mnemonique1, Mnemonique2)
                End If
                If (a2 And &H6) = 6 Then
                    Mnemonique1 = "sbrs": Mnemonique2 = "R" & Parametre_r & ", " & Parametre_b
                    Call MemorisationDiversMnemoniques(Mnemonique1, Mnemonique2)
                End If
            End If
        
            Partie1 = a2 And &HFC
                Parametre_r = (a1 And &HF) + ((a2 And 2) * 8)
                Parametre_d = ((a1 And &HF0) / 16) + ((a2 And 1) * 16)
            If Partie1 = &H4 Then
                Mnemonique1 = "cpc": Mnemonique2 = "R" & Parametre_d & ", R" & Parametre_r
                Call MemorisationDiversMnemoniques(Mnemonique1, Mnemonique2)
            End If
            If Partie1 = &H8 Then
                Mnemonique1 = "sbc": Mnemonique2 = "R" & Parametre_d & ", R" & Parametre_r
                Call MemorisationDiversMnemoniques(Mnemonique1, Mnemonique2)
            End If
            If Partie1 = &HC Then
                    If Parametre_d = Parametre_r Then
                        Mnemonique1 = "lsl": Mnemonique2 = "R" & Parametre_d
                        Call MemorisationDiversMnemoniques(Mnemonique1, Mnemonique2)
                        Mnemonique1 = "add": Mnemonique2 = "R" & Parametre_d & ", R" & Parametre_r
                        Call MemorisationDiversMnemoniques(Mnemonique1, Mnemonique2)
                    End If
                    If Parametre_d <> Parametre_r Then
                        Mnemonique1 = "add": Mnemonique2 = "R" & Parametre_d & ", R" & Parametre_r
                        Call MemorisationDiversMnemoniques(Mnemonique1, Mnemonique2)
                    End If
            End If
            If Partie1 = &H10 Then
                Mnemonique1 = "cpse": Mnemonique2 = "R" & Parametre_d & ", R" & Parametre_r
                Call MemorisationDiversMnemoniques(Mnemonique1, Mnemonique2)
            End If
            If Partie1 = &H14 Then
                Mnemonique1 = "cp": Mnemonique2 = "R" & Parametre_d & ", R" & Parametre_r
                Call MemorisationDiversMnemoniques(Mnemonique1, Mnemonique2)
            End If
            If Partie1 = &H18 Then
                Mnemonique1 = "sub": Mnemonique2 = "R" & Parametre_d & ", R" & Parametre_r
                Call MemorisationDiversMnemoniques(Mnemonique1, Mnemonique2)
            End If
            If Partie1 = &H1C Then
                    If Parametre_d = Parametre_r Then
                        Mnemonique1 = "rol": Mnemonique2 = "R" & Parametre_d
                        Call MemorisationDiversMnemoniques(Mnemonique1, Mnemonique2)
                        Mnemonique1 = "adc": Mnemonique2 = "R" & Parametre_d & ", R" & Parametre_r
                        Call MemorisationDiversMnemoniques(Mnemonique1, Mnemonique2)
                    End If
                    If Parametre_d <> Parametre_r Then
                        Mnemonique1 = "adc": Mnemonique2 = "R" & Parametre_d & ", R" & Parametre_r
                        Call MemorisationDiversMnemoniques(Mnemonique1, Mnemonique2)
                    End If
            End If
            If Partie1 = &H20 Then
                    If Parametre_d = Parametre_r Then
                        Mnemonique1 = "tst": Mnemonique2 = "R" & Parametre_d
                        Call MemorisationDiversMnemoniques(Mnemonique1, Mnemonique2)
                        Mnemonique1 = "and": Mnemonique2 = "R" & Parametre_d & ", R" & Parametre_r
                        Call MemorisationDiversMnemoniques(Mnemonique1, Mnemonique2)
                    End If
                    If Parametre_d <> Parametre_r Then
                        Mnemonique1 = "and": Mnemonique2 = "R" & Parametre_d & ", R" & Parametre_r
                        Call MemorisationDiversMnemoniques(Mnemonique1, Mnemonique2)
                    End If
            End If
            If Partie1 = &H24 Then
                    If Parametre_d = Parametre_r Then
                        Mnemonique1 = "clr": Mnemonique2 = "R" & Parametre_d
                        Call MemorisationDiversMnemoniques(Mnemonique1, Mnemonique2)
                        Mnemonique1 = "eor": Mnemonique2 = "R" & Parametre_d & ", R" & Parametre_r
                        Call MemorisationDiversMnemoniques(Mnemonique1, Mnemonique2)
                    End If
                    If Parametre_d <> Parametre_r Then
                        Mnemonique1 = "eor": Mnemonique2 = "R" & Parametre_d & ", R" & Parametre_r
                        Call MemorisationDiversMnemoniques(Mnemonique1, Mnemonique2)
                    End If
            End If
            If Partie1 = &H28 Then
                Mnemonique1 = "or": Mnemonique2 = "R" & Parametre_d & ", R" & Parametre_r
                Call MemorisationDiversMnemoniques(Mnemonique1, Mnemonique2)
            End If
            If Partie1 = &H2C Then
                Mnemonique1 = "mov": Mnemonique2 = "R" & Parametre_d & ", R" & Parametre_r
                Call MemorisationDiversMnemoniques(Mnemonique1, Mnemonique2)
            End If
            If Partie1 = &H9C Then
                Mnemonique1 = "mul": Mnemonique2 = "R" & Parametre_d & ", R" & Parametre_r
                Call MemorisationDiversMnemoniques(Mnemonique1, Mnemonique2)
            End If
                Parametre_s = (a1 And 7)
                Parametre_k = ((a2 And &H1) * 32) + ((a1 And &HF8) / 8)
                        'Arranger valeur (sign�e) de "k"
                        If ((a2 And 2) = 2) Then
                            Parametre_k = (64 - Parametre_k) * (-1)
                        End If
                        Parametre_k = Parametre_k + 1 'Arranger valeur (sign�e) de "k"
                        LieuduSaut = AdrActu + Parametre_k: Call CorrectPtMemFlash(LieuduSaut, Anoter)
                        If Anoter = True Then Label_a_Imprimer = DecToHex((LieuduSaut), NbreDigitsAdresse, "AVR")
                        If Anoter = False Then Label_a_Imprimer = DecToHex((LieuduSaut), NbreDigitsAdresse, "0x")
            If Partie1 = &HF0 Then
                Mnemonique1 = "brbs": Mnemonique2 = Parametre_s & ", " & Label_a_Imprimer
                Call MemorisationDiversMnemoniques(Mnemonique1, Mnemonique2)
            End If
            If Partie1 = &HF4 Then
                Mnemonique1 = "brbc": Mnemonique2 = Parametre_s & ", " & Label_a_Imprimer
                Call MemorisationDiversMnemoniques(Mnemonique1, Mnemonique2)
            End If
        
            If (a2 And &HF0) = &HB0 Then
                Parametre_A = (a1 And &HF) + ((a2 And &H6) * 8)
                Parametre_d = ((a1 And &HF0) / 16) + ((a2 And 1) * 16): Parametre_r = Parametre_d
                If (a2 And 8) = 0 Then
                    Mnemonique1 = "in": Mnemonique2 = "R" & Parametre_d & ", " & DecToHex(Parametre_A, 2, "$")
                    Call MemorisationDiversMnemoniques(Mnemonique1, Mnemonique2)
                End If
                If (a2 And 8) = 8 Then
                    Mnemonique1 = "out": Mnemonique2 = DecToHex(Parametre_A, 2, "$") & ", R" & Parametre_r
                    Call MemorisationDiversMnemoniques(Mnemonique1, Mnemonique2)
                End If
            End If
                Parametre_q = (a1 And 7) + ((a2 And &HC) * 2) + (a2 And &H20)
                Parametre_d = ((a1 And &HF0) / 16) + ((a2 And 1) * 16): Parametre_r = Parametre_d
                If (a2 And &HD2) = &H80 Then
                    If (a1 And 8) = 0 Then
                        Mnemonique1 = "ldd": Mnemonique2 = "R" & Parametre_d & ", Z+" & Parametre_q
                        Call MemorisationDiversMnemoniques(Mnemonique1, Mnemonique2)
                    End If
                    If (a1 And 8) = 8 Then
                        Mnemonique1 = "ldd": Mnemonique2 = "R" & Parametre_d & ", Y+" & Parametre_q
                        Call MemorisationDiversMnemoniques(Mnemonique1, Mnemonique2)
                    End If
                End If
                If (a2 And &HD2) = &H82 Then
                    If (a1 And 8) = 0 Then
                        Mnemonique1 = "std": Mnemonique2 = "Z+" & Parametre_q & ", R" & Parametre_r
                        Call MemorisationDiversMnemoniques(Mnemonique1, Mnemonique2)
                    End If
                    If (a1 And 8) = 8 Then
                        Mnemonique1 = "std": Mnemonique2 = "Y+" & Parametre_q & ", R" & Parametre_r
                        Call MemorisationDiversMnemoniques(Mnemonique1, Mnemonique2)
                    End If
                End If
            Partie1 = a2 And &HF0
                Paramtre_K = (a1 And &HF) + ((a2 And &HF) * 16): Parametre_d = ((a1 And &HF0) / 16) + 16
                Parametre_k = a1 + ((a2 And 7) * 256)
                        'Arranger valeur (sign�e) de "k"
                        If ((a2 And 8) = 8) Then
                            Parametre_k = (2048 - Parametre_k) * (-1)
                        End If
                        Parametre_k = Parametre_k + 1 'Arranger valeur (sign�e) de "k"
                        LieuduSaut = AdrActu + Parametre_k: Call CorrectPtMemFlash(LieuduSaut, Anoter)
            If Partie1 = &H30 Then
                Mnemonique1 = "cpi": Mnemonique2 = "R" & Parametre_d & ", " & DecToHex(Paramtre_K, 2, "0x")
                Call MemorisationDiversMnemoniques(Mnemonique1, Mnemonique2)
            End If
            If Partie1 = &H40 Then
                Mnemonique1 = "sbci": Mnemonique2 = "R" & Parametre_d & ", " & DecToHex(Paramtre_K, 2, "0x")
                Call MemorisationDiversMnemoniques(Mnemonique1, Mnemonique2)
            End If
            If Partie1 = &H50 Then
                Mnemonique1 = "subi": Mnemonique2 = "R" & Parametre_d & ", " & DecToHex(Paramtre_K, 2, "0x")
                Call MemorisationDiversMnemoniques(Mnemonique1, Mnemonique2)
            End If
            If Partie1 = &H60 Then
                Mnemonique1 = "ori": Mnemonique2 = "R" & Parametre_d & ", " & DecToHex(Paramtre_K, 2, "0x")
                                            'Note: "ORI" est idem � "SBR"
                Call MemorisationDiversMnemoniques(Mnemonique1, Mnemonique2)
                Mnemonique1 = "sbr": Mnemonique2 = "R" & Parametre_d & ", " & DecToHex(Paramtre_K, 2, "0x")
                Call MemorisationDiversMnemoniques(Mnemonique1, Mnemonique2)
            End If
            If Partie1 = &H70 Then
                Mnemonique1 = "andi": Mnemonique2 = "R" & Parametre_d & ", " & DecToHex(Paramtre_K, 2, "0x")
                Call MemorisationDiversMnemoniques(Mnemonique1, Mnemonique2)
                Mnemonique1 = "cbr": Mnemonique2 = "R" & Parametre_d & ", " & DecToHex((255 - Paramtre_K), 2, "0x")
                Call MemorisationDiversMnemoniques(Mnemonique1, Mnemonique2)
            End If
            If Partie1 = &HC0 Then
                Mnemonique1 = "rjmp"
                If Anoter = True Then Mnemonique2 = DecToHex((LieuduSaut), NbreDigitsAdresse, "AVR")
                If Anoter = False Then Mnemonique2 = DecToHex((LieuduSaut), NbreDigitsAdresse, "0x")
                Call MemorisationDiversMnemoniques(Mnemonique1, Mnemonique2)
            End If
            If Partie1 = &HD0 Then
                Mnemonique1 = "rcall"
                If Anoter = True Then Mnemonique2 = DecToHex((LieuduSaut), NbreDigitsAdresse, "SUB")
                If Anoter = False Then Mnemonique2 = DecToHex((LieuduSaut), NbreDigitsAdresse, "0x")
                Call MemorisationDiversMnemoniques(Mnemonique1, Mnemonique2)
            End If
            If Partie1 = &HE0 Then
                Mnemonique1 = "ldi": Mnemonique2 = "R" & Parametre_d & ", " & DecToHex(Paramtre_K, 2, "0x")
                Call MemorisationDiversMnemoniques(Mnemonique1, Mnemonique2)
            End If
            
            If Mnemonique1 = "unknow" Then 'Par d�faut si aucun mn�monique trouv�
                Mnemonique1 = ".db": Mnemonique2 = DecToHex(a1, 2, "0x") & "," & DecToHex(a2, 2, " 0x")
                Call MemorisationDiversMnemoniques(Mnemonique1, Mnemonique2)
            End If
'=========
Poursuite:
'=========
            If RepresentType_X_Y_Z = False Then GoTo Bypass1
            'Modifier R26....R31 en XL....ZH si d�sir�
            
            For Parcoureur_ = 0 To (CpteurMnemos - 1) 'Parcourir les diverses formulations du mn�monique
            PosDsMnemo = InStr(DifferentsMnemo2(Parcoureur_), "R26")
            If PosDsMnemo <> 0 Then DifferentsMnemo2(Parcoureur_) = Left(DifferentsMnemo2(Parcoureur_), PosDsMnemo - 1) & "XL" & _
                Right(DifferentsMnemo2(Parcoureur_), Len(DifferentsMnemo2(Parcoureur_)) - PosDsMnemo - 2)
            PosDsMnemo = InStr(DifferentsMnemo2(Parcoureur_), "R26")
            If PosDsMnemo <> 0 Then DifferentsMnemo2(Parcoureur_) = Left(DifferentsMnemo2(Parcoureur_), PosDsMnemo - 1) & "XL" & _
                Right(DifferentsMnemo2(Parcoureur_), Len(DifferentsMnemo2(Parcoureur_)) - PosDsMnemo - 2)
            PosDsMnemo = InStr(DifferentsMnemo2(Parcoureur_), "R27")
            If PosDsMnemo <> 0 Then DifferentsMnemo2(Parcoureur_) = Left(DifferentsMnemo2(Parcoureur_), PosDsMnemo - 1) & "XH" & _
                Right(DifferentsMnemo2(Parcoureur_), Len(DifferentsMnemo2(Parcoureur_)) - PosDsMnemo - 2)
            PosDsMnemo = InStr(DifferentsMnemo2(Parcoureur_), "R27")
            If PosDsMnemo <> 0 Then DifferentsMnemo2(Parcoureur_) = Left(DifferentsMnemo2(Parcoureur_), PosDsMnemo - 1) & "XH" & _
                Right(DifferentsMnemo2(Parcoureur_), Len(DifferentsMnemo2(Parcoureur_)) - PosDsMnemo - 2)
            PosDsMnemo = InStr(DifferentsMnemo2(Parcoureur_), "R28")
            If PosDsMnemo <> 0 Then DifferentsMnemo2(Parcoureur_) = Left(DifferentsMnemo2(Parcoureur_), PosDsMnemo - 1) & "YL" & _
                Right(DifferentsMnemo2(Parcoureur_), Len(DifferentsMnemo2(Parcoureur_)) - PosDsMnemo - 2)
            PosDsMnemo = InStr(DifferentsMnemo2(Parcoureur_), "R28")
            If PosDsMnemo <> 0 Then DifferentsMnemo2(Parcoureur_) = Left(DifferentsMnemo2(Parcoureur_), PosDsMnemo - 1) & "YL" & _
                Right(DifferentsMnemo2(Parcoureur_), Len(DifferentsMnemo2(Parcoureur_)) - PosDsMnemo - 2)
            PosDsMnemo = InStr(DifferentsMnemo2(Parcoureur_), "R29")
            If PosDsMnemo <> 0 Then DifferentsMnemo2(Parcoureur_) = Left(DifferentsMnemo2(Parcoureur_), PosDsMnemo - 1) & "YH" & _
                Right(DifferentsMnemo2(Parcoureur_), Len(DifferentsMnemo2(Parcoureur_)) - PosDsMnemo - 2)
            PosDsMnemo = InStr(DifferentsMnemo2(Parcoureur_), "R29")
            If PosDsMnemo <> 0 Then DifferentsMnemo2(Parcoureur_) = Left(DifferentsMnemo2(Parcoureur_), PosDsMnemo - 1) & "YH" & _
                Right(DifferentsMnemo2(Parcoureur_), Len(DifferentsMnemo2(Parcoureur_)) - PosDsMnemo - 2)
            PosDsMnemo = InStr(DifferentsMnemo2(Parcoureur_), "R30")
            If PosDsMnemo <> 0 Then DifferentsMnemo2(Parcoureur_) = Left(DifferentsMnemo2(Parcoureur_), PosDsMnemo - 1) & "ZL" & _
                Right(DifferentsMnemo2(Parcoureur_), Len(DifferentsMnemo2(Parcoureur_)) - PosDsMnemo - 2)
            PosDsMnemo = InStr(DifferentsMnemo2(Parcoureur_), "R30")
            If PosDsMnemo <> 0 Then DifferentsMnemo2(Parcoureur_) = Left(DifferentsMnemo2(Parcoureur_), PosDsMnemo - 1) & "ZL" & _
                Right(DifferentsMnemo2(Parcoureur_), Len(DifferentsMnemo2(Parcoureur_)) - PosDsMnemo - 2)
            PosDsMnemo = InStr(DifferentsMnemo2(Parcoureur_), "R31")
            If PosDsMnemo <> 0 Then DifferentsMnemo2(Parcoureur_) = Left(DifferentsMnemo2(Parcoureur_), PosDsMnemo - 1) & "ZH" & _
                Right(DifferentsMnemo2(Parcoureur_), Len(DifferentsMnemo2(Parcoureur_)) - PosDsMnemo - 2)
            PosDsMnemo = InStr(DifferentsMnemo2(Parcoureur_), "R31")
            If PosDsMnemo <> 0 Then DifferentsMnemo2(Parcoureur_) = Left(DifferentsMnemo2(Parcoureur_), PosDsMnemo - 1) & "ZH" & _
                Right(DifferentsMnemo2(Parcoureur_), Len(DifferentsMnemo2(Parcoureur_)) - PosDsMnemo - 2)
            PosDsMnemo = InStr(DifferentsMnemo2(Parcoureur_), "XH:XL")
            If PosDsMnemo <> 0 Then DifferentsMnemo2(Parcoureur_) = Left(DifferentsMnemo2(Parcoureur_), PosDsMnemo - 1) & "X" & _
                Right(DifferentsMnemo2(Parcoureur_), Len(DifferentsMnemo2(Parcoureur_)) - PosDsMnemo - 4)
            PosDsMnemo = InStr(DifferentsMnemo2(Parcoureur_), "XH:XL")
            If PosDsMnemo <> 0 Then DifferentsMnemo2(Parcoureur_) = Left(DifferentsMnemo2(Parcoureur_), PosDsMnemo - 1) & "X" & _
                Right(DifferentsMnemo2(Parcoureur_), Len(DifferentsMnemo2(Parcoureur_)) - PosDsMnemo - 4)
            PosDsMnemo = InStr(DifferentsMnemo2(Parcoureur_), "YH:YL")
            If PosDsMnemo <> 0 Then DifferentsMnemo2(Parcoureur_) = Left(DifferentsMnemo2(Parcoureur_), PosDsMnemo - 1) & "Y" & _
                Right(DifferentsMnemo2(Parcoureur_), Len(DifferentsMnemo2(Parcoureur_)) - PosDsMnemo - 4)
            PosDsMnemo = InStr(DifferentsMnemo2(Parcoureur_), "YH:YL")
            If PosDsMnemo <> 0 Then DifferentsMnemo2(Parcoureur_) = Left(DifferentsMnemo2(Parcoureur_), PosDsMnemo - 1) & "Y" & _
                Right(DifferentsMnemo2(Parcoureur_), Len(DifferentsMnemo2(Parcoureur_)) - PosDsMnemo - 4)
            PosDsMnemo = InStr(DifferentsMnemo2(Parcoureur_), "ZH:ZL")
            If PosDsMnemo <> 0 Then DifferentsMnemo2(Parcoureur_) = Left(DifferentsMnemo2(Parcoureur_), PosDsMnemo - 1) & "Z" & _
                Right(DifferentsMnemo2(Parcoureur_), Len(DifferentsMnemo2(Parcoureur_)) - PosDsMnemo - 4)
            PosDsMnemo = InStr(DifferentsMnemo2(Parcoureur_), "ZH:ZL")
            If PosDsMnemo <> 0 Then DifferentsMnemo2(Parcoureur_) = Left(DifferentsMnemo2(Parcoureur_), PosDsMnemo - 1) & "Z" & _
                Right(DifferentsMnemo2(Parcoureur_), Len(DifferentsMnemo2(Parcoureur_)) - PosDsMnemo - 4)
            Next Parcoureur_
            
Bypass1:
            If RepresentRegister = False Then GoTo Bypass3
            'S'il existe une repr�sentation symbolique des registres et que ce type de repr�sentation est d�sir�e
            
            For Cpteur1 = 0 To (CpteurMnemos - 1) 'Parcourir les diverses formulations du mn�monique
            
            For Parcoureur_ = 1 To NbreRegistres
                PosDsMnemo = InStr(DifferentsMnemo2(Cpteur1), RegInclude(Parcoureur_))
                If PosDsMnemo <> 0 Then DifferentsMnemo2(Cpteur1) = Left(DifferentsMnemo2(Cpteur1), PosDsMnemo - 1) & _
                        LabelInclude(Parcoureur_) & Right(DifferentsMnemo2(Cpteur1), Len(DifferentsMnemo2(Cpteur1)) - PosDsMnemo - 2)
            Next
            'Idem pour les registres �tendus
            If ExtendedRegExist = True Then
                If InStr(DifferentsMnemo1(Cpteur1), "lds") <> 0 Or InStr(DifferentsMnemo1(Cpteur1), "sts") <> 0 Then
                    For Parcoureur_ = 1 To NbreExtRegist
                        PosDsMnemo = InStr(DifferentsMnemo2(Cpteur1), ExtRegInclude(Parcoureur_))
                        If PosDsMnemo <> 0 Then DifferentsMnemo2(Cpteur1) = Left(DifferentsMnemo2(Cpteur1), PosDsMnemo - 1) & _
                                ExtLabelInclude(Parcoureur_) & Right(DifferentsMnemo2(Cpteur1), Len(DifferentsMnemo2(Cpteur1)) - PosDsMnemo - 5)
                    Next
                End If
            End If
            
            
            'Placement des d�finitions de bits
            If (DifferentsMnemo1(Cpteur1) = "bset") Or (DifferentsMnemo1(Cpteur1) = "bclr") Or _
                            (DifferentsMnemo1(Cpteur1) = "; bset") Or (DifferentsMnemo1(Cpteur1) = "; bclr") Then
                    'Recherche du label de bit "s" (bit dans "Status Register")
                DifferentsMnemo2(Cpteur1) = SREG(Val(DifferentsMnemo2(Cpteur1)))
                GoTo Bypass3
            End If
            
            If (DifferentsMnemo1(Cpteur1) = "brbs") Or (DifferentsMnemo1(Cpteur1) = "brbc") Or _
                            (DifferentsMnemo1(Cpteur1) = "; brbs") Or (DifferentsMnemo1(Cpteur1) = "; brbc") Then
                    'Recherche du label de bit "s" (bit dans "Status Register")
                    '(Encore � r�aliser)
                DifferentsMnemo2(Cpteur1) = SREG(Val(Left(DifferentsMnemo2(Cpteur1), 1))) & Right(DifferentsMnemo2(Cpteur1), (Len(DifferentsMnemo2(Cpteur1)) - 1))
                GoTo Bypass3
            End If
            
            If (DifferentsMnemo1(Cpteur1) = "cbi") Or (DifferentsMnemo1(Cpteur1) = "sbi") Or (DifferentsMnemo1(Cpteur1) = "sbic") Or _
                    (DifferentsMnemo1(Cpteur1) = "sbis") Or (DifferentsMnemo1(Cpteur1) = "; cbi") Or (DifferentsMnemo1(Cpteur1) = "; sbi") Or _
                        (DifferentsMnemo1(Cpteur1) = "; sbic") Or (DifferentsMnemo1(Cpteur1) = "; sbis") Then
                    'Recherche du label de bit "b"
                Open CheminRepertoire & FichInc For Input As #1 'Ouvrir fichier include
                    Do Until EOF(1)
                    Input #1, LocaleVar
                    If InStr(LocaleVar, "* BIT DEFINITIONS *") <> 0 Then GoTo PlacemLabelBitPt1
                    Loop
                    Close 1
                    GoTo Bypass3
PlacemLabelBitPt1:
                    LocaleVar2 = Left(DifferentsMnemo2(Cpteur1), Len(DifferentsMnemo2(Cpteur1)) - 3) 'R�-extraire label I/O Register
                    Do Until EOF(1)
                    Input #1, LocaleVar 'Et rechercher celui-ci dans d�finition des bits
                    If InStr(LocaleVar, "; " & LocaleVar2) <> 0 Then GoTo PlacemLabelBitPt2
                    Loop
                    Close 1
                    GoTo Bypass3
PlacemLabelBitPt2:
                    Do Until EOF(1)
                    Input #1, LocaleVar
                    If InStr(LocaleVar, ".equ") <> 0 And InStr(LocaleVar, "= " & Right(DifferentsMnemo2(Cpteur1), 1)) <> 0 _
                                                                                        Then GoTo PlacemLabelBitPt3
                    Loop
                    
                    Close 1
                    GoTo Bypass3
PlacemLabelBitPt3:
                    LocaleVar2 = ""
                    For Parcoureur_ = 5 To Len(LocaleVar)
                        If (Mid(LocaleVar, Parcoureur_, 1) <> " ") And (Mid(LocaleVar, Parcoureur_, 1) <> Chr(9)) _
                                                                                        Then GoTo PlacemLabelBitPt4
                    Next Parcoureur_
                    Close 1
                    GoTo Bypass3
PlacemLabelBitPt4:
                    LocaleVar2 = LocaleVar2 + Mid(LocaleVar, Parcoureur_, 1) 'On lit le label de bit
                    Parcoureur_ = Parcoureur_ + 1
                    If (Mid(LocaleVar, Parcoureur_, 1) <> " ") And (Mid(LocaleVar, Parcoureur_, 1) <> Chr(9)) _
                                                                                        Then GoTo PlacemLabelBitPt4:

                    
                    DifferentsMnemo2(Cpteur1) = Left(DifferentsMnemo2(Cpteur1), Len(DifferentsMnemo2(Cpteur1)) - 1) & LocaleVar2
                Close 1
            End If
            
           Next Cpteur1
           
Bypass3:


            '---------------------------------------------------------------------------------
            'Placement des labels de points d'acc�s, issus de la 1�re passe
            If NbredePointsAcces = 0 Then GoTo BypassLabel
            Addr_Label = Val(Right(Label_a_placer, (Len(Label_a_placer) - 4)))
            
                If Addr_Label = AdrActu Then
                    If Left(Label_a_placer, 3) = "SUB" Then _
                                                Call Memoris_Sortie(";============================================")
                    Call Memoris_Sortie(Left(Label_a_placer, 3) & DecToHex(Addr_Label, NbreDigitsAdresse, "") & ":")
                End If
                If (Nbre_Labels_Lus < NbredePointsAcces) And (AdrActu >= Addr_Label) Then
                        Label_a_placer = Provis(Nbre_Labels_Lus)
                        Nbre_Labels_Lus = Nbre_Labels_Lus + 1
                End If
                
BypassLabel:
            '---------------------------------------------------------------------------------
                
            '---------------------------------------------------------------------------------
            'Dans le cas de sauts, les labels par d�faut sont de type "AVR ,mais il y a exception
            'si cet endroit de saut est aussi un endroit appell� par une sous-routine. Dans ce
            'cas, le label pr�pond�rant (celui qui a �t� retenu durant la 1�re passe) est "SUB".
            'Ce petit bo�t de programmation ne sert qu'� d�tecter s'il faut placer "AVR" ou "SUB"
            'derri�re le mn�monique.
            
            For Cpteur1 = 0 To (CpteurMnemos - 1) 'Parcourir les diverses formulations du mn�monique
            If ComptEffaces = 0 Then GoTo Bypass2 'Sauter si pas de valeurs d'effacements
            If Left((Right(DifferentsMnemo2(Cpteur1), 3 + NbreDigitsAdresse)), 3) <> "AVR" Then GoTo Bypass2 'Rien de sp�cial �
                                                                                               'faire, dans ce cas
                ComptProvis = 0
                Do
                    If ComptProvis = ComptEffaces Then Exit Do
                    label_test = Effaces(ComptProvis): ComptProvis = ComptProvis + 1
                    If label_test = Right(DifferentsMnemo2(Cpteur1), 3 + NbreDigitsAdresse) Then
                        DifferentsMnemo2(Cpteur1) = Left(DifferentsMnemo2(Cpteur1), Len(DifferentsMnemo2(Cpteur1)) - (3 + NbreDigitsAdresse)) & _
                                                                "SUB" & Right(DifferentsMnemo2(Cpteur1), NbreDigitsAdresse)
                        Exit Do
                    End If
                Loop
            Next Cpteur1
            '---------------------------------------------------------------------------------
            
Bypass2:
            
            For Cpteur1 = 0 To (CpteurMnemos - 1) 'Parcourir les diverses formulations du mn�monique
            Call Memoris_Sortie2(Chr$(9) & DifferentsMnemo1(Cpteur1) & Chr(9) & DifferentsMnemo2(Cpteur1))
            NombredeTab = NbreTab - Int(Len(DifferentsMnemo2(Cpteur1)) / LargeurTab)                   'Placer le nbre de tabulations
            For IndexTab = 0 To NombredeTab: Call Memoris_Sortie2(Chr(9)): Next IndexTab 'n�cessaires pour une belle pr�sentation
            If Left(DifferentsMnemo1(Cpteur1), 1) <> ";" Then
                Call Memoris_Sortie2(DecToHex(AdrActu, NbreDigitsAdresse, "; "))
                Call Memoris_Sortie2(DecToHex(a2, 2, " ") & DecToHex(a1, 2, ""))
                If NbredeMots = 2 Then Call Memoris_Sortie(DecToHex(a4, 2, " ") & DecToHex(a3, 2, "")) Else _
                                                                                            Call Memoris_Sortie("")
            End If
            If Left(DifferentsMnemo1(Cpteur1), 1) = ";" Then Call Memoris_Sortie("")
                                                    'Note: chr$(9) = tabulation horizontale
            If DifferentsMnemo1(Cpteur1) = "ret" Or DifferentsMnemo1(Cpteur1) = "reti" Then Call Memoris_Sortie2(Chr$(13) & Chr$(10)) 'Ajouter ligne vide
                                                                                               'sous "ret" et "reti"
            Next Cpteur1
            
            
            AdrActu = AdrActu + 1: If NbredeMots = 2 Then AdrActu = AdrActu + 1
            If Nbre_Bytes_Lus = Nbre_Bytes_Fichier Then Exit Do 'Sortie de la boucle
            
        ' ================
        ' Fin de la boucle
        ' ================
        Loop
        ' ================
        ' Fin de la boucle
        ' ================
        
        Call Memoris_Sortie(";============================================")
        Call Memoris_Sortie(";=====================FIN====================")
        Close 'Lire suivant

        ' ~~~~~~~~~~~~~~~~~
        ' Fin de la routine
        ' ~~~~~~~~~~~~~~~~~
        Exit Sub 'Sortir
        ' ~~~~~~~~~~~~~~~~~
        ' Fin de la routine
        ' ~~~~~~~~~~~~~~~~~
        
ReAcquerir:
            '1 Mot en plus (2 Mots) ==> 4 instructions concern�es
            NbredeMots = 2 'Dans ce cas, l'instruction est compos�e de 2 Mots
            
            'Lire 2�me Mot � d�sassembler
            a3 = FichierBinaire(Nbre_Bytes_Lus): Nbre_Bytes_Lus = Nbre_Bytes_Lus + 1
            a4 = FichierBinaire(Nbre_Bytes_Lus): Nbre_Bytes_Lus = Nbre_Bytes_Lus + 1
            
            Partie1 = a2 And &HFE: Partie2 = a1 And &HF
                Parametre_k = a3 + (a4 * 256): Parametre_d = ((a2 And &H1) * 16) + ((a1 And &HF0) / 16)
            If Partie1 = &H90 And Partie2 = &H0 Then
                Mnemonique1 = "lds": Mnemonique2 = "R" & Parametre_d & ", " & DecToHex(Parametre_k, 4, "0x")
                Call MemorisationDiversMnemoniques(Mnemonique1, Mnemonique2)
            End If
            If Partie1 = &H92 And Partie2 = &H0 Then
                Mnemonique1 = "sts": Mnemonique2 = DecToHex(Parametre_k, 4, "0x") & ", R" & Parametre_d
                Call MemorisationDiversMnemoniques(Mnemonique1, Mnemonique2)
            End If
            Partie2 = a1 And &HE
                Parametre_k = a3 + (a4 * 256) + ((a1 And 1) * 65536) + ((a1 And &HF0) * 8192) + _
                    ((a2 And 1) * 2097152)
            If Partie1 = &H94 And Partie2 = &HC Then
                Mnemonique1 = "jmp"
                Mnemonique2 = DecToHex(Parametre_k, NbreDigitsAdresse, "AVR")
                If (Parametre_k < AddrDebut) Or (Parametre_k > DerniereAdresse) Then Mnemonique2 = _
                                                                            DecToHex(Parametre_k, NbreDigitsAdresse, "0x")
                If Parametre_k > 65535 Then Mnemonique2 = DecToHex(Parametre_k, 6, "0x")
                Call MemorisationDiversMnemoniques(Mnemonique1, Mnemonique2)
            End If
            If Partie1 = &H94 And Partie2 = &HE Then
                Mnemonique1 = "call"
                Mnemonique2 = DecToHex(Parametre_k, NbreDigitsAdresse, "SUB")
                If (Parametre_k < AddrDebut) Or (Parametre_k > DerniereAdresse) Then Mnemonique2 = _
                                                                            DecToHex(Parametre_k, NbreDigitsAdresse, "0x")
                If Parametre_k > 65535 Then Mnemonique2 = DecToHex(Parametre_k, 6, "0x")
                Call MemorisationDiversMnemoniques(Mnemonique1, Mnemonique2)
            End If
            GoTo Poursuite
        
End Sub

Private Sub MemodesSauts()
            'Routine m�morisant les divers sauts dans le programme � d�sassembler
        Dim a1 As Long, a2 As Long, a3 As Long, a4 As Long, Saut As String
        Dim Partie1 As Byte, Partie2 As Byte, AdrActu As Long, LieuduSaut As Long
        Dim Parametre_k As Long, Nbre_Bytes_Lus As Long, NbredeMots As Byte, Anoter As Boolean
        
        ComptProvis = 0 'Initialiser valeur compteur associ� au tableau des labels de points d'acc�s
        ReDim Provis(ComptProvis) 'Initialiser le tableau des labels de points d'acc�s
        
        Nbre_Bytes_Lus = 0: AdrActu = AddrDebut: NbredePointsAcces = 0
        
        Do
        
            Saut = "" 'Initialiser valeur de l'annotation adresse de saut �ventuel
            Anoter = True 'Par d�faut, le saut doit �tre anot�
            
            NbredeMots = 1 'Nbre de Mots de l'instruction que l'on va lire = 1 par d�faut
            
            'Lire un mot � d�sassembler dans le tableau
            a1 = FichierBinaire(Nbre_Bytes_Lus): Nbre_Bytes_Lus = Nbre_Bytes_Lus + 1
            a2 = FichierBinaire(Nbre_Bytes_Lus): Nbre_Bytes_Lus = Nbre_Bytes_Lus + 1
            
            If AdrActu = DerniereAdresse Then GoTo EmpecherReaquerir1 'Pas d'instructions sur 2 Mots (c'est la fin)
            Partie1 = a2 And &HFE: Partie2 = a1 And &HF
            If Partie1 = &H90 And Partie2 = 0 Then GoTo ReAcquerir2 '1 Mot en plus (2 Mots)
            If Partie1 = &H92 And Partie2 = 0 Then GoTo ReAcquerir2 '1 Mot en plus (2 Mots)
            Partie2 = a1 And &HC
            If Partie1 = &H94 And Partie2 = &HC Then GoTo ReAcquerir2 '1 Mot en plus (2 Mots)
EmpecherReaquerir1:
            
            Partie1 = a2 And &HFC: Partie2 = a1 And 7
                    Parametre_k = ((a2 And 1) * 32) + ((a1 And &HF8) / 8)
                        'Arranger valeur (sign�e) de "k"
                        If ((a2 And 2) = 2) Then Parametre_k = (64 - Parametre_k) * (-1)
                        Parametre_k = Parametre_k + 1 'Arranger valeur (sign�e) de "k"
                        LieuduSaut = AdrActu + Parametre_k: Call CorrectPtMemFlash(LieuduSaut, Anoter)
                            
            If (Partie1 = &HF0) Or (Partie1 = &HF4) Then
                    Saut = "AVR" & Str(LieuduSaut)
                    GoTo Poursuite2
            End If

            Partie1 = a2 And &HF0
                Parametre_k = a1 + ((a2 And 7) * 256)
                        'Arranger valeur (sign�e) de "k"
                        If ((a2 And 8) = 8) Then
                            Parametre_k = (2048 - Parametre_k) * (-1)
                        End If
                        Parametre_k = Parametre_k + 1 'Arranger valeur (sign�e) de "k"
                        LieuduSaut = AdrActu + Parametre_k: Call CorrectPtMemFlash(LieuduSaut, Anoter)
            If Partie1 = &HC0 Then Saut = "AVR" & Str(LieuduSaut)
            If Partie1 = &HD0 Then Saut = "SUB" & Str(LieuduSaut)

Poursuite2:
            If (Saut <> "") And (Anoter = True) Then
                    ReDim Preserve Provis(ComptProvis)
                    Provis(ComptProvis) = Saut: ComptProvis = ComptProvis + 1
                    NbredePointsAcces = NbredePointsAcces + 1
            End If
            AdrActu = AdrActu + 1: If NbredeMots = 2 Then AdrActu = AdrActu + 1
            If Nbre_Bytes_Lus = Nbre_Bytes_Fichier Then Exit Do
        Loop
        
                '=========================================================
                    'Effacement des doublons de points d'acc�s:
        If NbredePointsAcces < 2 Then GoTo Passer_ce_Point_1 'Dans ce cas, rien � effacer
        'Relire le tableau qui vient d'�tre �crit
        ReDim variabletableau(NbredePointsAcces) As String: Dim VarBcl1 As Long, VarBcl2 As Long
        For VarBcl1 = 1 To NbredePointsAcces
                variabletableau(VarBcl1) = Provis(VarBcl1 - 1)
        Next
        
        For VarBcl1 = 1 To (NbredePointsAcces - 1)
            For VarBcl2 = (VarBcl1 + 1) To NbredePointsAcces
                If variabletableau(VarBcl2) = variabletableau(VarBcl1) Then variabletableau(VarBcl2) = "doublon"
            Next VarBcl2
        Next VarBcl1
        
        'On va r�-�crire le tableau nettoy� de ses doublons
        ComptProvis = 0: ReDim Provis(ComptProvis)
        VarBcl2 = 0
        For VarBcl1 = 1 To NbredePointsAcces
                If variabletableau(VarBcl1) <> "doublon" Then
                    ReDim Preserve Provis(ComptProvis): Provis(ComptProvis) = _
                                                        variabletableau(VarBcl1): ComptProvis = ComptProvis + 1
                    VarBcl2 = VarBcl2 + 1 'Totaliser le nbre r�el de points d'acc�s diff�rents
                End If
        Next

        NbredePointsAcces = VarBcl2 'M�moriser la nvlle valeur (�pur�e)
Passer_ce_Point_1:
                '=========================================================
        
                '=========================================================
                    'Effacement des doublons de points d'acc�s (si "AVR" et "SUB" pr m�me adresse)
        If NbredePointsAcces < 2 Then GoTo Passer_ce_Point_2 'Dans ce cas, rien � effacer
        'R�-ouvrir le tableau qui vient d'�tre r�-�crit
        ReDim variabletableau(NbredePointsAcces) As String: Dim Provisoire1 As Long, Provisoire2 As Long
        For VarBcl1 = 1 To NbredePointsAcces
                variabletableau(VarBcl1) = Provis(VarBcl1 - 1)
        Next
        
        'Table cr��e pour m�moriser les labels effac�s de type "AVR"
        ComptEffaces = 0: ReDim Effaces(ComptEffaces)
        For VarBcl1 = 1 To (NbredePointsAcces - 1)
            Provisoire1 = Val(Right(variabletableau(VarBcl1), (Len(variabletableau(VarBcl1)) - 4)))
            For VarBcl2 = (VarBcl1 + 1) To NbredePointsAcces
                Provisoire2 = Val(Right(variabletableau(VarBcl2), (Len(variabletableau(VarBcl2)) - 4)))
                If (Provisoire1 = Provisoire2) And (variabletableau(VarBcl1) <> "doublon") And _
                                                            (variabletableau(VarBcl2) <> "doublon") Then
                    'S'il existe le label "AVR" et le label "SUB" avec la m�me adresse,
                    'on ne laisse subsister que l'�tiquette portant le label "SUB"
                    'Notifier le label "AVR" effac�
                    ReDim Preserve Effaces(ComptEffaces): Effaces(ComptEffaces) = _
                                                                    DecToHex(Provisoire1, NbreDigitsAdresse, "AVR")
                                                                            ComptEffaces = ComptEffaces + 1
                    If Left(variabletableau(VarBcl1), 3) = "AVR" Then variabletableau(VarBcl1) = "doublon"
                    If Left(variabletableau(VarBcl2), 3) = "AVR" Then variabletableau(VarBcl2) = "doublon"
                End If
                
            Next VarBcl2
        Next VarBcl1
        
        'On va r�-�crire le tableau nettoy� de ses doublons
        ComptProvis = 0: ReDim Provis(ComptProvis)
        VarBcl2 = 0
        For VarBcl1 = 1 To NbredePointsAcces
                If variabletableau(VarBcl1) <> "doublon" Then
                    ReDim Preserve Provis(ComptProvis): Provis(ComptProvis) = _
                                                        variabletableau(VarBcl1): ComptProvis = ComptProvis + 1
                    VarBcl2 = VarBcl2 + 1 'Totaliser le nbre r�el de points d'acc�s diff�rents
                End If
        Next
        
        NbredePointsAcces = VarBcl2 'M�moriser la nvlle valeur (�pur�e)
        
Passer_ce_Point_2:
                '=========================================================
            
                '=========================================================
                    'Trier par ordre croissant d'adresses les labels des points d'acc�s
        If NbredePointsAcces < 2 Then GoTo Passer_ce_Point_3 'Dans ce cas, rien � trier
        'R�-ouvrir le tableau qui vient d'�tre r�-�crit
        ReDim variabletableau(NbredePointsAcces) As String
        For VarBcl1 = 1 To NbredePointsAcces
                variabletableau(VarBcl1) = Provis(VarBcl1 - 1)
        Next

        Call Tri_Dichotomique(NbredePointsAcces, variabletableau()) 'Trier par adresses croissantes

        ComptProvis = 0: ReDim Provis(ComptProvis)
        For VarBcl1 = 1 To NbredePointsAcces
                ReDim Preserve Provis(ComptProvis): Provis(ComptProvis) = _
                                                        variabletableau(VarBcl1): ComptProvis = ComptProvis + 1
        Next
Passer_ce_Point_3:
                '=========================================================
        
        Exit Sub
        
        
ReAcquerir2:
            '1 Mot en plus (2 Mots) ==> 4 instructions concern�es
            NbredeMots = 2 'Dans ce cas, l'instruction est compos�e de 2 Mots
            'Lire un nveau Mot � d�sassembler ds le tableau
            a3 = FichierBinaire(Nbre_Bytes_Lus): Nbre_Bytes_Lus = Nbre_Bytes_Lus + 1
            a4 = FichierBinaire(Nbre_Bytes_Lus): Nbre_Bytes_Lus = Nbre_Bytes_Lus + 1
            Partie1 = a2 And &HFE: Partie2 = a1 And &HE

                Parametre_k = a3 + (a4 * 256) + ((a1 And 1) * 65536) + ((a1 And &HF0) * 8192) + _
                    ((a2 And 1) * 2097152)
            If (Parametre_k > DerniereAdresse) Or (Parametre_k < AddrDebut) Then Anoter = False
            If Partie1 = &H94 And Partie2 = &HC Then Saut = "AVR" & Str(Parametre_k)
            If Partie1 = &H94 And Partie2 = &HE Then Saut = "SUB" & Str(Parametre_k)
            GoTo Poursuite2
End Sub

Private Sub CorrectPtMemFlash(AddrVisee As Long, Anotation As Boolean)
            'Si l'adresse FLASH vis�e par un saut relatif sort du domaine autoris�, corriger celle-ci
            If AddrVisee < 0 Then
                AddrVisee = TailleFlashAdressable + AddrVisee
                GoTo TestSuivant
            End If
            If AddrVisee >= TailleFlashAdressable Then
                AddrVisee = AddrVisee - TailleFlashAdressable
                GoTo TestSuivant
            End If
TestSuivant:
            'Tester si les sauts et branchements sont dans le domaine des adresses du code d�sassembl�
            Anotation = True
            If (AddrVisee < AddrDebut) Or (AddrVisee > DerniereAdresse) Then Anotation = False
End Sub

Private Sub GestErreurFormat(RecupErreur As Byte)
            'Erreur dans le Format INTELHEX
            MsgBox "Erreur dans le Format INTELHEX", vbExclamation, "Note pour d�sassemblage"
            End
End Sub

Private Sub Tri_Dichotomique(ByVal Nb_Element As Long, Tableau() As String)
'Cette routine est un tri dichotomique modifi� pour trier uniquement sur la partie num�rique
'contenue dans les labels

' le param�tre Nb_Element correspond au nombre d'�l�ments du tableau, sa valeur n'est donc pas modifi�e
' le param�tre Tableau() est le tableau � trier, il est modifi� puis retourn�
Dim P As Long
' P repr�sente la position � laquelle il faut placer l'�l�ment � classer
Dim I As Long
Dim J As Long
' I et J sont des variables interm�diaires utilis�es pour les compteurs de boucles
Dim Ligne_Tampon As String
' Ligne_Tampon est une variable interm�diaire utilis�e pour la permutation d'�l�ments
Dim Provisoire1 As Long, Provisoire2 As Long
' On effectue l'op�ration pour tous les �l�ments
For I = 2 To Nb_Element

Provisoire1 = Val(Right(Tableau(I), (Len(Tableau(I)) - 4)))
Provisoire2 = Val(Right(Tableau(I - 1), (Len(Tableau(I - 1)) - 4)))
If (Provisoire1 < Provisoire2) Then

' Si l'�l�ment I est sup�rieur � l'�l�ment 1
' alors il faut d�terminer sa place
' Sinon l'�l�ment I doit prendre la premi�re place
Provisoire2 = Val(Right(Tableau(1), (Len(Tableau(1)) - 4)))
If (Provisoire1 > Provisoire2) Then

P = DichoPlacer(I, Tableau())

Else

P = 1

End If
' D�calage du tableau
Ligne_Tampon = Tableau(I)
For J = I To P + 1 Step -1


Tableau(J) = Tableau(J - 1)

Next J
Tableau(P) = Ligne_Tampon

End If

Next I

End Sub

Private Function DichoPlacer(I As Long, Tableau() As String) As Long


' le param�tre I repr�sente la position de l'�l�ment � classer
' le param�tre Tableau() est le tableau � trier, il est modifi� puis retourn�
' DichoPlacer renvoie la nouvelle position de l'�l�ment � classer
Dim inf As Long
Dim sup As Long
Dim milieu As Long
Dim Provisoire1 As Long, Provisoire2 As Long
inf = 1
sup = I - 1

Do While inf <> sup


milieu = (inf + sup) \ 2
Provisoire1 = Val(Right(Tableau(milieu), (Len(Tableau(milieu)) - 4)))
Provisoire2 = Val(Right(Tableau(I), (Len(Tableau(I)) - 4)))
If (Provisoire1 <= Provisoire2) Then

inf = milieu + 1

Else

sup = milieu

End If

Loop
DichoPlacer = inf


End Function

Private Sub Recherche_Includes()
    'Parcourir le r�pertoire des fichiers include de Atmel AVR studio (s'il existe)
    Dim a As String, pos1 As Byte, pos2 As Byte, NbreMicrosDansListe As Byte, DirName As String
    
    NbreMicrosDansListe = 0
    If Verif(CheminRepertoire) = True Then
    'Le r�pertoire existe: afficher liste des �C disponibles dans combobox
        Combo1.Visible = True: Combo1.Clear: Combo1.AddItem "Affichage brut Registres"
        DirName = Dir(CheminRepertoire, 6)          ' Acqu�rir le 1er nom de fichier
        Do While DirName <> ""
            'If dirname <> "." And dirname <> ".." And LCase(Right(dirname, 4)) = ".inc" Then
            If LCase(Right(DirName, 4)) = ".inc" Then
            
                NbreMicrosDansListe = NbreMicrosDansListe + 1
                ReDim Preserve NomFichierInc(NbreMicrosDansListe)
                ReDim Preserve TypeMicro(NbreMicrosDansListe)
                '        'Lire 2�me ligne du fichier inc:
                'Open CheminRepertoire & DirName For Input As 1: Input #1, a: Input #1, a
                'If (InStr(a, "Source: ") = 0) Or (InStr(a, ".xml") = 0) Then Call _
                '                                                    ErreurInclude 'Erreur dans fichier include
                'pos1 = InStr(a, "Source: ") + 8: pos2 = InStr(a, ".xml") 'D�limiter le texte utile (type de �C)
                'TypeMicro(NbreMicrosDansListe) = Mid(a, pos1, (pos2 - pos1))
                Open CheminRepertoire & DirName For Input As 1
                    Do Until EOF(1)
                        Input #1, a
                        If (InStr(a, ".device ") <> 0) Then GoTo SortieRecherche
                    Loop
                    Call ErreurInclude 'Erreur de format du fichier inc
SortieRecherche:
                TypeMicro(NbreMicrosDansListe) = Mid(a, 9, (Len(a) - 8))
                NomFichierInc(NbreMicrosDansListe) = DirName
                Combo1.AddItem TypeMicro(NbreMicrosDansListe)             'Ajouter le type de �C � la liste
                Close 1
            End If
            
        DirName = Dir                               ' Acqu�rir un autre nom de fichier
        Loop
        Combo1.ListIndex = 0    'Valeur par d�faut = "Affichage brut Registres"
        Exit Sub
    End If
    
    If Verif(CheminRepertoire) = False Then
        'Le r�pertoire n'existe pas
        Feuille.Show
        Text1.Visible = True 'Afficher message de conseil pendant 3 secondes
        DoEvents
        DoEvents
        DoEvents
        Call Sleep(3000)
        Text1.Visible = False
    End If
End Sub

Private Function Verif(Repertoire_Teste As String) As Boolean
    'Tester existence d'un r�pertoire
Dim MyPath As String 'D�finition des variables
Dim myName As String
  
MyPath = Repertoire_Teste
  
If (myName = Dir(MyPath, vbDirectory)) = vbEmpty Then Verif = True Else Verif = False
End Function

Private Sub ErreurInclude()
    'Erreur dans le format du fichier include
    MsgBox "Erreur dans le format du fichier include", vbExclamation, "Note pour d�sassemblage"
    End
End Sub
