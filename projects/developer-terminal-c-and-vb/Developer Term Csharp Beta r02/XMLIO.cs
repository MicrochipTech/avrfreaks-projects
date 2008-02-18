using System;
using System.Windows.Forms;
using System.IO;
using System.Xml.Serialization;

namespace DeveloperTerminal
{
	public class XMLIO
	{
		public string appPath; // path to current directory
		
		public XMLIO()
		{
			//
			// TODO: Add constructor logic here
			//
		}

		public XMLData OpenXMLDataFile()
		{
			OpenFileDialog openFileDialog = new OpenFileDialog();

			openFileDialog.Filter = "XML Files (*.xml)|*.xml";
			openFileDialog.Title = "Open";
			openFileDialog.DefaultExt = ".xml";

			if (openFileDialog.ShowDialog() == System.Windows.Forms.DialogResult.OK) 
			{
				return XMLRead(openFileDialog.FileName);
			}
			else
			{
				MessageBox.Show("Error: Couldn't get XML file name.");
				return null;
			}
		}

		// Use default test button data file name
		public XMLData XMLRead()
		{
			return XMLRead(appPath + "\\XMLData.xml");
		}
		
		// Use any test button data file name
		public XMLData XMLRead(string FileName)
		{
			// Open an instance of the ButtonData class
			XMLData xMLData = new XMLData();

			try
			{
				// Construct an instance of the XmlSerializer with the type
				// of object that is being deserialized.
				XmlSerializer mySerializer = 
					new XmlSerializer(typeof(XMLData));
				// To read the file, create a FileStream object.
				FileStream myFileStream = 
					new FileStream(FileName, FileMode.Open);
				// Call the Deserialize method and cast to the object type.
				xMLData = (XMLData) 
					mySerializer.Deserialize(myFileStream);	

				myFileStream.Close();

			}
			catch
			{
				MessageBox.Show("Error: couldn't read XMLData.xml");
				return null;
			}

			return xMLData;
		}
	}
}
