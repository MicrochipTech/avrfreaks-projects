'
'This class is necessary because the SerialPort class included with C# Express has a method
'GetPortNames that doesn't work right. The documentation even admits it doesn't work right - which is
'very unusual for Microsoft - the documentation says it reads a register key and if that key ain't right
'then the results ain't right (okay, the don't use 'ain't' but gist is the same). In my test case the key had
'an orphan port name in it, so I reverted to the DeviceInfo class that I'd made earlier before the
'SerialPort .NET namespace came along.

'The DevInfo class ignores the registry and looks at the ports to see what is there.

'If you are just getting started with C#, you don't want to read this since it uses all sorts of
'hard to follow techniques that violate the spirit of C# specifically and OOP in general.
'However, it does follow the usual Microsoft rule that anything that claims to be object oriented
'must have 'sort of' as a prefix.
 
'This code was derived from the DevInfo.cs module in the Code Project Article:
'.NET - Diving into System Programming - Part 3
'By Vladimir Afanasyev http://www.codeproject.com/csharp/DivingSysProg3.asp

'Joe Pardue added the GetPorts function 7/12/05
'Joe Pardue added the parsePorts function 7/14/05
'Joe Pardue added the parseFriendlyPorts function 7/21/05
'


Imports Microsoft.VisualBasic
Imports System
Imports System.Text
Imports System.Runtime.InteropServices

Namespace DevInfo

	Friend Class DeviceInfo

		Private Const DIGCF_PRESENT As Integer = (&H00000002)
		Private Const MAX_DEV_LEN As Integer = 1000
		Private Const SPDRP_FRIENDLYNAME As Integer = (&H0000000C) ' FriendlyName (R/W)
		Private Const SPDRP_DEVICEDESC As Integer = (&H00000000) ' DeviceDesc (R/W)

		<StructLayout(LayoutKind.Sequential)> _
		Private Class SP_DEVINFO_DATA
				 Public cbSize As Integer
				 Public ClassGuid As Guid
				 Public DevInst As Integer ' DEVINST handle
				 Public Reserved As ULong
		End Class

		<DllImport("setupapi.dll")> _
		Private Shared Function SetupDiClassGuidsFromNameA(ByVal ClassN As String, ByRef guids As Guid, ByVal ClassNameSize As UInt32, ByRef ReqSize As UInt32) As Boolean
		End Function

		<DllImport("setupapi.dll")> _
		Private Shared Function SetupDiGetClassDevsA(ByRef ClassGuid As Guid, ByVal Enumerator As UInt32, ByVal hwndParent As IntPtr, ByVal Flags As UInt32) As IntPtr
		End Function

		<DllImport("setupapi.dll")> _
		Private Shared Function SetupDiEnumDeviceInfo(ByVal DeviceInfoSet As IntPtr, ByVal MemberIndex As UInt32, ByVal DeviceInfoData As SP_DEVINFO_DATA) As Boolean
		End Function

		<DllImport("setupapi.dll")> _
		Private Shared Function SetupDiDestroyDeviceInfoList(ByVal DeviceInfoSet As IntPtr) As Boolean
		End Function

        <DllImport("setupapi.dll")> _
        Private Shared Function SetupDiGetDeviceRegistryPropertyA(ByVal DeviceInfoSet As IntPtr, ByVal DeviceInfoData As SP_DEVINFO_DATA, ByVal Propert As UInt32, ByVal PropertyRegDataType As UInt32, ByVal PropertyBuffer As StringBuilder, ByVal PropertyBufferSize As UInt32, ByVal RequiredSize As IntPtr) As Boolean
        End Function


		Public Shared Function EnumerateDevices(ByVal DeviceIndex As UInt32, ByVal ClassName As String, ByVal DeviceName As StringBuilder) As Integer
		 Dim RequiredSize As UInt32 = 0
		 Dim guid As Guid=Guid.Empty
		 Dim guids As Guid() = New Guid(0){}
		 Dim NewDeviceInfoSet As IntPtr
		 Dim DeviceInfoData As SP_DEVINFO_DATA= New SP_DEVINFO_DATA()


		 Dim res As Boolean=SetupDiClassGuidsFromNameA(ClassName,guids(0),RequiredSize,RequiredSize)
		 If RequiredSize=0 Then
				'incorrect class name:
				DeviceName = New StringBuilder("")
				Return -2
		 End If

		 If (Not res) Then
                guids = New Guid(System.Convert.ToInt32(RequiredSize) - 1) {}
                res = SetupDiClassGuidsFromNameA(ClassName, guids(0), RequiredSize, RequiredSize)

		   If (Not res) OrElse RequiredSize=0 Then
				'incorrect class name:
				DeviceName = New StringBuilder("")
				Return -2
		   End If
		 End If

		 'get device info set for our device class
		 NewDeviceInfoSet=SetupDiGetClassDevsA(guids(0),0,IntPtr.Zero,DIGCF_PRESENT)
		 If NewDeviceInfoSet.ToInt32() = -1 Then
		  'device information is unavailable:
				DeviceName = New StringBuilder("")
				Return -3
		 End If

			DeviceInfoData.cbSize = 28
			'is devices exist for class
			DeviceInfoData.DevInst=0
			DeviceInfoData.ClassGuid=System.Guid.Empty
			DeviceInfoData.Reserved=0

			res=SetupDiEnumDeviceInfo(NewDeviceInfoSet, DeviceIndex,DeviceInfoData)
			If (Not res) Then
		 'no such device:
				SetupDiDestroyDeviceInfoList(NewDeviceInfoSet)
				DeviceName = New StringBuilder("")
				Return -1
			End If

			DeviceName.Capacity=MAX_DEV_LEN
			If (Not SetupDiGetDeviceRegistryPropertyA(NewDeviceInfoSet,DeviceInfoData, SPDRP_FRIENDLYNAME,0,DeviceName,MAX_DEV_LEN,IntPtr.Zero)) Then
			res = SetupDiGetDeviceRegistryPropertyA(NewDeviceInfoSet, DeviceInfoData,SPDRP_DEVICEDESC,0,DeviceName,MAX_DEV_LEN, IntPtr.Zero)
			If (Not res) Then
			'incorrect device name:
					SetupDiDestroyDeviceInfoList(NewDeviceInfoSet)
					DeviceName = New StringBuilder("")
					Return -4
			End If
			End If
		 Return 0
		End Function

		' Joe Pardue 7/14/05
		Public Shared Function ParsePorts() As String()
			Dim str As String = GetPorts()

			' Maximum number of possible COM ports is 256
			' create and intialize array
			Dim strArray As String() = New String(255){}
			For i As Integer = 0 To 255
				strArray(i) = ""
			Next i

			Dim cArray As Char() = str.ToCharArray()

			Dim k As Integer = 0
			Dim j As Integer = 0
			For i As Integer = 0 To cArray.Length - 1
				If cArray(i) = "("c Then
					For j = 0 To 7 ' JWP 12/20/05 changed 6 to 8 to accomodate up to COM255
						If cArray(i+j) = ")"c Then
							i+=j
							k += 1
							Exit For
						End If
						If cArray(i+j) <> "("c Then
						strArray(k) += cArray(i + j)
						End If
					Next j
					If cArray(i-j+1) = "L"c Then ' dump an Lpt port
						k -= 1
						strArray(k) = ""
					End If
				End If
			Next i
			Return strArray
		End Function

		' Joe Pardue 7/21/05 - gets the 'Friendly' port name
		Public Shared Function ParseFriendlyPorts() As String()
			Dim str As String = GetPorts()

			' Maximum number of possible COM ports is 256
			' create and intialize array
			Dim strArray As String() = New String(255){}
			For i As Integer = 0 To 255
				strArray(i) = ""
			Next i

			Dim cArray As Char() = str.ToCharArray()

			Dim k As Integer = 0
			For i As Integer = 0 To cArray.Length - 1
					strArray(k) += cArray(i)
					If cArray(i) = ")"c Then
						' Hack to get rid of LPT string
						If (i > 5) AndAlso (cArray(i - 4) <> "L"c) Then
							k += 1
						Else
							strArray(k) = ""

						End If
						i += 1 ' skip the trailing Constants.vbLf
					End If
			Next i

			' Debugging
			'string temp = "";
'for(int i = 0; i < strArray.Length; i++)
'{
'temp += strArray[i];
'if(strArray[i] == ')')
'{
'temp +=  "\n";
'}
'}
			
'System.Windows.Forms.MessageBox.Show("ParseFriendlyPorts:\n" + temp);
'
			Return strArray
		End Function


		' Joe Pardue 7/12/05
		Public Shared Function GetPorts() As String
			Dim devices As StringBuilder = New StringBuilder("")
			Dim Index As UInt32= 0
			Dim result As Integer=0

			Dim str As String = ""

			Do
				'enumerate devices of selected device class
'TODO: INSTANT VB TODO TASK: Assignments within expressions are not supported in VB.NET
'ORIGINAL LINE: result=DevInfo.DeviceInfo.EnumerateDevices(Index++, "Ports", devices);
                result = DevInfo.DeviceInfo.EnumerateDevices(Index, "Ports", devices)
                Index += CUInt(1)
				If result <> 0 Then
				Exit Do
				Else
					str &= devices.ToString() & Constants.vbLf
				End If
			Loop
			Return str
		End Function
	End Class
End Namespace
