Attribute VB_Name = "udpemail"
Function FormatEmail(emailData As String)
    On Error Resume Next
    'Place your routine to parse and format UDP data here
    emailData = "Received " & Now & ": " & emailData & vbCrLf
    FormatEmail = emailData
End Function
