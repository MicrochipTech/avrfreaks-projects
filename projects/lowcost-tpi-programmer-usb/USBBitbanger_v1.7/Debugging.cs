using Microsoft.VisualBasic;
using System;
using System.Collections;
using System.Data;
using System.Diagnostics;
using System.Drawing;
using System.Windows.Forms;

///  <summary>
///  Used only in Debug.Write statements.
///  </summary>
  
namespace GenericHid
{    
    internal sealed partial class Debugging  
    {         
        ///  <summary>
        ///  Get text that describes the result of an API call.
        ///  </summary>
        ///  
        ///  <param name="FunctionName"> the name of the API function. </param>
        ///  
        ///  <returns>
        ///  The text.
        ///  </returns>
          
        internal String ResultOfAPICall( String functionName ) 
        {             
            Int32 bytes = 0; 
            Int32 resultCode = 0; 
            String resultString = ""; 
            
            resultString = new String(Convert.ToChar( 0 ), 129 ); 
            
            // Returns the result code for the last API call.
            
            resultCode = System.Runtime.InteropServices.Marshal.GetLastWin32Error(); 
            
            // Get the result message that corresponds to the code.

            Int64 temp = 0;          
            bytes = FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, ref temp, resultCode, 0, resultString, 128, 0); 
            
            // Subtract two characters from the message to strip the CR and LF.
            
            if ( bytes > 2 ) 
            { 
                resultString = resultString.Remove( bytes - 2, 2 ); 
            }             
            // Create the String to return.
            
            resultString = "\r\n" + functionName + "\r\n" + "Result = " + resultString + "\r\n"; 
            
            return resultString;             
        }         
    }  
} 
