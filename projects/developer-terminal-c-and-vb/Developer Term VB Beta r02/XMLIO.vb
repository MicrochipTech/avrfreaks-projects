Imports Microsoft.VisualBasic
Imports System
Imports System.Windows.Forms
Imports System.IO
Imports System.Xml.Serialization

Namespace DeveloperTerminal
	Public Class XMLIO
		Public appPath As String ' path to current directory

		Public Sub New()
			'
			' TODO: Add constructor logic here
			'
		End Sub

		Public Function OpenXMLDataFile() As XMLData
			Dim openFileDialog As OpenFileDialog = New OpenFileDialog()

			openFileDialog.Filter = "XML Files (*.xml)|*.xml"
			openFileDialog.Title = "Open"
			openFileDialog.DefaultExt = ".xml"

			If openFileDialog.ShowDialog() = System.Windows.Forms.DialogResult.OK Then
				Return XMLRead(openFileDialog.FileName)
			Else
				MessageBox.Show("Error: Couldn't get XML file name.")
				Return Nothing
			End If
		End Function

		' Use default test button data file name
		Public Function XMLRead() As XMLData
			Return XMLRead(appPath & "\XMLData.xml")
		End Function

		' Use any test button data file name
		Public Function XMLRead(ByVal FileName As String) As XMLData
			' Open an instance of the ButtonData class
			Dim xMLData As XMLData = New XMLData()

			Try
				' Construct an instance of the XmlSerializer with the type
				' of object that is being deserialized.
				Dim mySerializer As XmlSerializer = New XmlSerializer(GetType(XMLData))
				' To read the file, create a FileStream object.
				Dim myFileStream As FileStream = New FileStream(FileName, FileMode.Open)
				' Call the Deserialize method and cast to the object type.
				xMLData = CType(mySerializer.Deserialize(myFileStream), XMLData)

				myFileStream.Close()

			Catch
				MessageBox.Show("Error: couldn't read XMLData.xml")
				Return Nothing
			End Try

			Return xMLData
		End Function
	End Class
End Namespace
