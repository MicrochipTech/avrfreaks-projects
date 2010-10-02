using Microsoft.VisualBasic;
using Microsoft.Win32.SafeHandles; 
using System;
using System.Data; 
using System.Diagnostics;
using System.Runtime.InteropServices;
using System.Threading;
using System.Windows.Forms;

///  <summary>
///  For communicating with HID-class USB devices.
///  The ReportIn class handles Input reports and Feature reports that carry data to the host.
///  The ReportOut class handles Output reports and Feature reports that that carry data to the device.
///  Other routines retrieve information about and configure the HID.
///  </summary>
///  

namespace GenericHid
{    
    internal sealed partial class Hid  
    {         
        //  Used in error messages.
        
        private const String MODULE_NAME = "Hid"; 
        
        internal HIDP_CAPS Capabilities; 
        internal HIDD_ATTRIBUTES DeviceAttributes; 
        
        //  For viewing results of API calls in debug.write statements:
        
        internal static Debugging MyDebugging = new Debugging(); 
        
        ///  <summary>
        ///  For reports the device sends to the host.
        ///  </summary>
        
        internal abstract class ReportIn  
        {             
            ///  <summary>
            ///  Each class that handles reading reports defines a Read method for reading 
            ///  a type of report. Read is declared as a Sub rather
            ///  than as a Function because asynchronous reads use a callback method 
            ///  that can access parameters passed by ByRef but not Function return values.
            ///  </summary>
            
            internal abstract void Read( SafeFileHandle hidHandle, SafeFileHandle readHandle, SafeFileHandle writeHandle, ref Boolean myDeviceDetected, ref Byte[] readBuffer, ref Boolean success );           
         }      
        
        ///  <summary>
        ///  For reading Feature reports.
        ///  </summary>
        
        internal class InFeatureReport : ReportIn 
        {             
            ///  <summary>
            ///  reads a Feature report from the device.
            ///  </summary>
            ///  
            ///  <param name="hidHandle"> the handle for learning about the device and exchanging Feature reports. </param>
            ///  <param name="readHandle"> the handle for reading Input reports from the device. </param>
            ///  <param name="writeHandle"> the handle for writing Output reports to the device. </param>
            ///  <param name="myDeviceDetected"> tells whether the device is currently attached.</param>
            ///  <param name="inFeatureReportBuffer"> contains the requested report.</param>
            ///  <param name="success"> read success</param>
            
            internal override void Read( SafeFileHandle hidHandle, SafeFileHandle readHandle, SafeFileHandle writeHandle, ref Boolean myDeviceDetected, ref Byte[] inFeatureReportBuffer, ref Boolean success ) 
            {                 
                try 
                { 
                    //  ***
                    //  API function: HidD_GetFeature
                    //  Attempts to read a Feature report from the device.
                    
                    //  Requires:
                    //  A handle to a HID
                    //  A pointer to a buffer containing the report ID and report
                    //  The size of the buffer. 
                    
                    //  Returns: true on success, false on failure.
                    //  ***                    
                   
                    success = HidD_GetFeature(hidHandle, inFeatureReportBuffer, inFeatureReportBuffer.Length); 
                                        
                    //Debug.Print( "HidD_GetFeature success = " + success );                     
                } 
                catch ( Exception ex ) 
                { 
                    DisplayException( MODULE_NAME, ex ); 
                    throw ; 
                }                 
            }             
        }         
        
        ///  <summary>
        ///  For reading Input reports via control transfers
        ///  </summary>
        
        internal class InputReportViaControlTransfer : ReportIn 
        {             
            ///  <summary>
            ///  reads an Input report from the device using a control transfer.
            ///  </summary>
            ///  
            ///  <param name="hidHandle"> the handle for learning about the device and exchanging Feature reports. </param>
            ///  <param name="readHandle"> the handle for reading Input reports from the device. </param>
            ///  <param name="writeHandle"> the handle for writing Output reports to the device. </param>
            ///  <param name="myDeviceDetected"> tells whether the device is currently attached. </param>
            ///  <param name="inputReportBuffer"> contains the requested report. </param>
            ///  <param name="success"> read success </param>
            
            internal override void Read( SafeFileHandle hidHandle, SafeFileHandle readHandle, SafeFileHandle writeHandle, ref Boolean myDeviceDetected, ref Byte[] inputReportBuffer, ref Boolean success ) 
            {                 
                try 
                {                     
                    //  ***
                    //  API function: HidD_GetInputReport
                    
                    //  Purpose: Attempts to read an Input report from the device using a control transfer.
                    //  Supported under Windows XP and later only.
                    
                    //  Requires:
                    //  A handle to a HID
                    //  A pointer to a buffer containing the report ID and report
                    //  The size of the buffer. 
                    
                    //  Returns: true on success, false on failure.
                    //  ***
                    
                    success = HidD_GetInputReport(hidHandle, inputReportBuffer, inputReportBuffer.Length + 1); 
                    
                    Debug.Print( "HidD_GetInputReport success = " + success );                     
                } 
                catch ( Exception ex ) 
                { 
                    DisplayException( MODULE_NAME, ex ); 
                    throw ; 
                }                 
            }      
        }         
        
        ///  <summary>
        ///  For reading Input reports.
        ///  </summary>
        
        internal class InputReportViaInterruptTransfer : ReportIn 
        {          
            ///  <summary>
            ///  closes open handles to a device.
            ///  </summary>
            ///  
            ///  <param name="hidHandle"> the handle for learning about the device and exchanging Feature reports. </param>
            ///  <param name="readHandle"> the handle for reading Input reports from the device. </param>
            ///  <param name="writeHandle"> the handle for writing Output reports to the device. </param>

            internal void CancelTransfer(SafeFileHandle hidHandle, SafeFileHandle readHandle, SafeFileHandle writeHandle, IntPtr eventObject) 
            {                 
                try 
                { 
                    //  ***
                    //  API function: CancelIo
                    
                    //  Purpose: Cancels a call to ReadFile
                    
                    //  Accepts: the device handle.
                    
                    //  Returns: True on success, False on failure.
                    //  ***
                    
                    FileIO.CancelIo(readHandle);               
                                        
                    Debug.WriteLine( "************ReadFile error*************" ); 
                    String functionName = "CancelIo";
                    Debug.WriteLine(MyDebugging.ResultOfAPICall(functionName)); 
                    Debug.WriteLine( "" ); 
                    
                    //  The failure may have been because the device was removed,
                    //  so close any open handles and
                    //  set myDeviceDetected=False to cause the application to
                    //  look for the device on the next attempt.
                    
                    if ( ( !( hidHandle.IsInvalid ) ) ) 
                    { 
                        hidHandle.Close(); 
                    } 
                    
                    if ( ( !( readHandle.IsInvalid ) ) ) 
                    { 
                        readHandle.Close(); 
                    } 
                    
                    if ( ( !( writeHandle.IsInvalid ) ) ) 
                    { 
                        writeHandle.Close(); 
                    }                           
                } 
                catch ( Exception ex ) 
                { 
                    DisplayException( MODULE_NAME, ex ); 
                    throw ; 
                }                 
            }             
            
            ///  <summary>
            ///  Creates an event object for the overlapped structure used with ReadFile. 
            ///  </summary>
            ///  
            ///  <param name="hidOverlapped"> the overlapped structure </param>
            ///  <param name="eventObject"> the event object </param>

			internal void PrepareForOverlappedTransfer(ref NativeOverlapped hidOverlapped, ref IntPtr eventObject) 
            {                 
                try 
                { 
                    //  ***
                    //  API function: CreateEvent
                    
                    //  Purpose: Creates an event object for the overlapped structure used with ReadFile.
                    
                    //  Accepts:
                    //  A security attributes structure or IntPtr.Zero.
                    //  Manual Reset = False (The system automatically resets the state to nonsignaled 
                    //  after a waiting thread has been released.)
                    //  Initial state = False (not signaled)
                    //  An event object name (optional)
                    
                    //  Returns: a handle to the event object
                    //  ***

                    eventObject = FileIO.CreateEvent(IntPtr.Zero, false, false, "");                     
                                    
                    //  Set the members of the overlapped structure.
                    
                    hidOverlapped.OffsetLow = 0; 
                    hidOverlapped.OffsetHigh = 0; 
                    hidOverlapped.EventHandle = eventObject;                                     
                } 
                catch ( Exception ex ) 
                { 
                    DisplayException( MODULE_NAME, ex ); 
                    throw ; 
                }                 
            }             
            
            ///  <summary>
            ///  reads an Input report from the device using interrupt transfers.
            ///  </summary>
            ///  
            ///  <param name="hidHandle"> the handle for learning about the device and exchanging Feature reports. </param>
            ///  <param name="readHandle"> the handle for reading Input reports from the device. </param>
            ///  <param name="writeHandle"> the handle for writing Output reports to the device. </param>
            ///  <param name="myDeviceDetected"> tells whether the device is currently attached. </param>
            ///  <param name="inputReportBuffer"> contains the requested report. </param>
            ///  <param name="success"> read success </param>
            
            internal override void Read( SafeFileHandle hidHandle, SafeFileHandle readHandle, SafeFileHandle writeHandle, ref Boolean myDeviceDetected, ref Byte[] inputReportBuffer, ref Boolean success ) 
            {                 
                IntPtr eventObject = IntPtr.Zero;
				NativeOverlapped HidOverlapped = new NativeOverlapped();
				IntPtr nonManagedBuffer = IntPtr.Zero;
				IntPtr nonManagedOverlapped = IntPtr.Zero;
                Int32 numberOfBytesRead = 0; 
                Int32 result = 0;   
              
                try 
                { 
                    //  Set up the overlapped structure for ReadFile.
                    
                    PrepareForOverlappedTransfer( ref HidOverlapped, ref eventObject );

					// Allocate memory for the input buffer and overlapped structure. 

					nonManagedBuffer = Marshal.AllocHGlobal(inputReportBuffer.Length);
					nonManagedOverlapped = Marshal.AllocHGlobal(Marshal.SizeOf(HidOverlapped));
					Marshal.StructureToPtr(HidOverlapped, nonManagedOverlapped, false);			
                                        
                    //  ***
                    //  API function: ReadFile
                    //  Purpose: Attempts to read an Input report from the device.
                    
                    //  Accepts:
                    //  A device handle returned by CreateFile
                    //  (for overlapped I/O, CreateFile must have been called with FILE_FLAG_OVERLAPPED),
                    //  A pointer to a buffer for storing the report.
                    //  The Input report length in bytes returned by HidP_GetCaps,
                    //  A pointer to a variable that will hold the number of bytes read. 
                    //  An overlapped structure whose hEvent member is set to an event object.
                    
                    //  Returns: the report in ReadBuffer.
                    
                    //  The overlapped call returns immediately, even if the data hasn't been received yet.
                    
                    //  To read multiple reports with one ReadFile, increase the size of ReadBuffer
                    //  and use NumberOfBytesRead to determine how many reports were returned.
                    //  Use a larger buffer if the application can't keep up with reading each report
                    //  individually. 
                    //  ***                    
					
					success = FileIO.ReadFile(readHandle, nonManagedBuffer, inputReportBuffer.Length, ref numberOfBytesRead, nonManagedOverlapped);
 
					if (!success)
                    {
                        Debug.WriteLine("waiting for ReadFile");

                        //  API function: WaitForSingleObject

                        //  Purpose: waits for at least one report or a timeout.
                        //  Used with overlapped ReadFile.

                        //  Accepts:
                        //  An event object created with CreateEvent
                        //  A timeout value in milliseconds.

                        //  Returns: A result code.

                        result = FileIO.WaitForSingleObject(eventObject, 3000);

                        //  Find out if ReadFile completed or timeout.

                        switch (result)
                        {
                            case (System.Int32)FileIO.WAIT_OBJECT_0:

                                //  ReadFile has completed

                                success = true;
                                Debug.WriteLine("ReadFile completed successfully.");

								// Get the number of bytes read.

								//  API function: GetOverlappedResult

								//  Purpose: gets the result of an overlapped operation.
								
								//  Accepts:
								//  A device handle returned by CreateFile.
								//  A pointer to an overlapped structure.
								//  A pointer to a variable to hold the number of bytes read.
								//  False to return immediately.
								
								//  Returns: non-zero on success and the number of bytes read.	

								FileIO.GetOverlappedResult(readHandle, nonManagedOverlapped, ref numberOfBytesRead, false);

                                break;

                            case FileIO.WAIT_TIMEOUT:

                                //  Cancel the operation on timeout

                                CancelTransfer(hidHandle, readHandle, writeHandle, eventObject);
                                Debug.WriteLine("Readfile timeout");
                                success = false;
                                myDeviceDetected = false;
                                break;
                            default:

                                //  Cancel the operation on other error.

                                CancelTransfer(hidHandle, readHandle, writeHandle, eventObject);
                                Debug.WriteLine("Readfile undefined error");
                                success = false;
                                myDeviceDetected = false;
                                break;
                        }
						
                    }
					if (success)
					{
						// A report was received.
						// Copy the received data to inputReportBuffer for the application to use.

						Marshal.Copy(nonManagedBuffer, inputReportBuffer, 0, numberOfBytesRead);
					}
                } 
                catch ( Exception ex ) 
                { 
                    DisplayException( MODULE_NAME, ex ); 
                    throw ; 
                }                 
            }             
        } 
                
        ///  <summary>
        ///  For reports the host sends to the device.
        ///  </summary>
        
        internal abstract class ReportOut  
        {            
            ///  <summary>
            ///  Each class that handles writing reports defines a Write method for 
            ///  writing a type of report.
            ///  </summary>
            ///  
            ///  <param name="reportBuffer"> contains the report ID and report data. </param>
            ///   <param name="deviceHandle"> handle to the device.  </param>
            ///  
            ///  <returns>
            ///   True on success. False on failure.
            ///  </returns>             
            
            internal abstract Boolean Write( Byte[] reportBuffer, SafeFileHandle deviceHandle );      
        } 
        
        ///  <summary>
        ///  For Feature reports the host sends to the device.
        ///  </summary>
        
        internal class OutFeatureReport : ReportOut 
        {            
            ///  <summary>
            ///  writes a Feature report to the device.
            ///  </summary>
            ///  
            ///  <param name="outFeatureReportBuffer"> contains the report ID and report data. </param>
            ///  <param name="hidHandle"> handle to the device.  </param>
            ///  
            ///  <returns>
            ///   True on success. False on failure.
            ///  </returns>            
            
            internal override Boolean Write( Byte[] outFeatureReportBuffer, SafeFileHandle hidHandle ) 
            {                 
                Boolean success = false; 
                
                try 
                { 
                    //  ***
                    //  API function: HidD_SetFeature
                    
                    //  Purpose: Attempts to send a Feature report to the device.
                    
                    //  Accepts:
                    //  A handle to a HID
                    //  A pointer to a buffer containing the report ID and report
                    //  The size of the buffer. 
                    
                    //  Returns: true on success, false on failure.
                    //  ***
                                      
                    success = HidD_SetFeature(hidHandle, outFeatureReportBuffer, outFeatureReportBuffer.Length); 
                    
                    //Debug.Print( "HidD_SetFeature success = " + success ); 
                    
                    return success;                     
                } 
                catch ( Exception ex ) 
                { 
                    DisplayException( MODULE_NAME, ex ); 
                    throw ; 
                }                 
            }             
        } 
                
        ///  <summary>
        ///  For writing Output reports via control transfers
        ///  </summary>
        
        internal class OutputReportViaControlTransfer : ReportOut 
        {             
            ///  <summary>
            ///  writes an Output report to the device using a control transfer.
            ///  </summary>
            ///  
            ///  <param name="outputReportBuffer"> contains the report ID and report data. </param>
            ///  <param name="hidHandle"> handle to the device.  </param>
            ///  
            ///  <returns>
            ///   True on success. False on failure.
            ///  </returns>            
            
            internal override Boolean Write( Byte[] outputReportBuffer, SafeFileHandle hidHandle ) 
            {                 
                Boolean success = false; 
                
                try 
                { 
                    //  ***
                    //  API function: HidD_SetOutputReport
                    
                    //  Purpose: 
                    //  Attempts to send an Output report to the device using a control transfer.
                    //  Requires Windows XP or later.
                    
                    //  Accepts:
                    //  A handle to a HID
                    //  A pointer to a buffer containing the report ID and report
                    //  The size of the buffer. 
                    
                    //  Returns: true on success, false on failure.
                    //  ***                    
                   
                    success = HidD_SetOutputReport(hidHandle, outputReportBuffer, outputReportBuffer.Length + 1); 
                    
                    Debug.Print( "HidD_SetOutputReport success = " + success ); 
                    
                    return success;                     
                } 
                catch ( Exception ex ) 
                { 
                    DisplayException( MODULE_NAME, ex ); 
                    throw ; 
                }                 
            }           
        }       
        
        ///  <summary>
        ///  For Output reports the host sends to the device.
        ///  Uses interrupt or control transfers depending on the device and OS.
        ///  </summary>
        
        internal class OutputReportViaInterruptTransfer : ReportOut 
        {             
            ///  <summary>
            ///  writes an Output report to the device.
            ///  </summary>
            ///  
            ///  <param name="outputReportBuffer"> contains the report ID and report data. </param>
            ///  <param name="writeHandle"> handle to the device.  </param>
            ///  
            ///  <returns>
            ///   True on success. False on failure.
            ///  </returns>            
            
            internal override Boolean Write( Byte[] outputReportBuffer, SafeFileHandle writeHandle ) 
            {                 
                Int32 numberOfBytesWritten = 0; 
                Boolean success = false; 
                
                try 
                { 
                    //  The host will use an interrupt transfer if the the HID has an interrupt OUT
                    //  endpoint (requires USB 1.1 or later) AND the OS is NOT Windows 98 Gold (original version). 
                    //  Otherwise the the host will use a control transfer.
                    //  The application doesn't have to know or care which type of transfer is used.
                    
                    numberOfBytesWritten = 0; 
                    
                    //  ***
                    //  API function: WriteFile
                    
                    //  Purpose: writes an Output report to the device.
                    
                    //  Accepts:
                    //  A handle returned by CreateFile
                    //  An integer to hold the number of bytes written.
                    
                    //  Returns: True on success, False on failure.
                    //  ***
                    
                    success = FileIO.WriteFile(writeHandle, outputReportBuffer, outputReportBuffer.Length, ref numberOfBytesWritten, IntPtr.Zero);
                    
                    Debug.Print( "WriteFile success = " + success ); 
                    
                    if ( !( ( success ) ) ) 
                    { 
                        
                        if ( ( !( writeHandle.IsInvalid ) ) ) 
                        { 
                            writeHandle.Close(); 
                        } 
                    }                     
                    return success;                     
                } 
                catch ( Exception ex ) 
                { 
                    DisplayException( MODULE_NAME, ex ); 
                    throw ; 
                }                 
            }           
        } 
     
        ///  <summary>
        ///  Remove any Input reports waiting in the buffer.
        ///  </summary>
        ///  
        ///  <param name="hidHandle"> a handle to a device.   </param>
        ///  
        ///  <returns>
        ///  True on success, False on failure.
        ///  </returns>
        
        internal Boolean FlushQueue( SafeFileHandle hidHandle ) 
        {             
            Boolean success = false; 
            
            try 
            { 
                //  ***
                //  API function: HidD_FlushQueue
                
                //  Purpose: Removes any Input reports waiting in the buffer.
                
                //  Accepts: a handle to the device.
                
                //  Returns: True on success, False on failure.
                //  ***
                
                success = HidD_FlushQueue( hidHandle ); 
                
                return success;                 
            } 
            catch ( Exception ex ) 
            { 
                DisplayException( MODULE_NAME, ex ); 
                throw ; 
            }             
        }         
        
        ///  <summary>
        ///  Retrieves a structure with information about a device's capabilities. 
        ///  </summary>
        ///  
        ///  <param name="hidHandle"> a handle to a device. </param>
        ///  
        ///  <returns>
        ///  An HIDP_CAPS structure.
        ///  </returns>
        
        internal HIDP_CAPS GetDeviceCapabilities( SafeFileHandle hidHandle ) 
        {             
            IntPtr preparsedData = new System.IntPtr(); 
            Int32 result = 0; 
            Boolean success = false; 
            //Byte[] valueCaps = new Byte[ 1024 ]; // (the array size is a guess)

			try
			{
				//  ***
				//  API function: HidD_GetPreparsedData

				//  Purpose: retrieves a pointer to a buffer containing information about the device's capabilities.
				//  HidP_GetCaps and other API functions require a pointer to the buffer.

				//  Requires: 
				//  A handle returned by CreateFile.
				//  A pointer to a buffer.

				//  Returns:
				//  True on success, False on failure.
				//  ***

				success = HidD_GetPreparsedData(hidHandle, ref preparsedData);

				//  ***
				//  API function: HidP_GetCaps

				//  Purpose: find out a device's capabilities.
				//  For standard devices such as joysticks, you can find out the specific
				//  capabilities of the device.
				//  For a custom device where the software knows what the device is capable of,
				//  this call may be unneeded.

				//  Accepts:
				//  A pointer returned by HidD_GetPreparsedData
				//  A pointer to a HIDP_CAPS structure.

				//  Returns: True on success, False on failure.
				//  ***

				result = HidP_GetCaps(preparsedData, ref Capabilities);
				if ((result != 0))
				{
					Debug.WriteLine("");
					Debug.WriteLine("  Usage: " + Convert.ToString(Capabilities.Usage, 16));
					Debug.WriteLine("  Usage Page: " + Convert.ToString(Capabilities.UsagePage, 16));
					Debug.WriteLine("  Input Report Byte Length: " + Capabilities.InputReportByteLength);
					Debug.WriteLine("  Output Report Byte Length: " + Capabilities.OutputReportByteLength);
					Debug.WriteLine("  Feature Report Byte Length: " + Capabilities.FeatureReportByteLength);
					Debug.WriteLine("  Number of Link Collection Nodes: " + Capabilities.NumberLinkCollectionNodes);
					Debug.WriteLine("  Number of Input Button Caps: " + Capabilities.NumberInputButtonCaps);
					Debug.WriteLine("  Number of Input Value Caps: " + Capabilities.NumberInputValueCaps);
					Debug.WriteLine("  Number of Input Data Indices: " + Capabilities.NumberInputDataIndices);
					Debug.WriteLine("  Number of Output Button Caps: " + Capabilities.NumberOutputButtonCaps);
					Debug.WriteLine("  Number of Output Value Caps: " + Capabilities.NumberOutputValueCaps);
					Debug.WriteLine("  Number of Output Data Indices: " + Capabilities.NumberOutputDataIndices);
					Debug.WriteLine("  Number of Feature Button Caps: " + Capabilities.NumberFeatureButtonCaps);
					Debug.WriteLine("  Number of Feature Value Caps: " + Capabilities.NumberFeatureValueCaps);
					Debug.WriteLine("  Number of Feature Data Indices: " + Capabilities.NumberFeatureDataIndices);

					//  ***
					//  API function: HidP_GetValueCaps

					//  Purpose: retrieves a buffer containing an array of HidP_ValueCaps structures.
					//  Each structure defines the capabilities of one value.
					//  This application doesn't use this data.

					//  Accepts:
					//  A report type enumerator from hidpi.h,
					//  A pointer to a buffer for the returned array,
					//  The NumberInputValueCaps member of the device's HidP_Caps structure,
					//  A pointer to the PreparsedData structure returned by HidD_GetPreparsedData.

					//  Returns: True on success, False on failure.
					//  ***                    


					Int32 vcSize = Capabilities.NumberInputValueCaps;
					Byte[] valueCaps = new Byte[vcSize];

					
					result = HidP_GetValueCaps(HidP_Input, valueCaps, ref vcSize, preparsedData);

					//result = HidP_GetValueCaps(HidP_Input, ref valueCaps[0], ref Capabilities.NumberInputValueCaps, preparsedData); 

					// (To use this data, copy the ValueCaps byte array into an array of structures.)                   

				}
			}
			catch (Exception ex)
			{
				DisplayException(MODULE_NAME, ex);
				throw;
			}
			finally
			{
				 //  ***
					//  API function: HidD_FreePreparsedData
                    
					//  Purpose: frees the buffer reserved by HidD_GetPreparsedData.
                    
					//  Accepts: A pointer to the PreparsedData structure returned by HidD_GetPreparsedData.
                    
					//  Returns: True on success, False on failure.
					//  ***

				if (preparsedData != IntPtr.Zero)
				{
					success = HidD_FreePreparsedData(preparsedData);
				}
			} 
            
            return Capabilities;             
        }         
        
        ///  <summary>
        ///  Creates a 32-bit Usage from the Usage Page and Usage ID. 
        ///  Determines whether the Usage is a system mouse or keyboard.
        ///  Can be modified to detect other Usages.
        ///  </summary>
        ///  
        ///  <param name="MyCapabilities"> a HIDP_CAPS structure retrieved with HidP_GetCaps. </param>
        ///  
        ///  <returns>
        ///  A String describing the Usage.
        ///  </returns>
        
        internal String GetHidUsage( HIDP_CAPS MyCapabilities ) 
        {             
            Int32 usage = 0; 
            String usageDescription = ""; 
            
            try 
            { 
                //  Create32-bit Usage from Usage Page and Usage ID.
                
                usage = MyCapabilities.UsagePage * 256 + MyCapabilities.Usage; 
                
                if ( usage == Convert.ToInt32( 0X102 ) )
                 { 
                    usageDescription = "mouse"; } 
                
                if ( usage == Convert.ToInt32( 0X106 ) )
                 { 
                    usageDescription = "keyboard"; }                   
            } 
            catch ( Exception ex ) 
            { 
                DisplayException( MODULE_NAME, ex ); 
                throw ; 
            } 
            
            return usageDescription;             
        }         
        
        ///  <summary>
        ///  Retrieves the number of Input reports the host can store.
        ///  </summary>
        ///  
        ///  <param name="hidDeviceObject"> a handle to a device  </param>
        ///  <param name="numberOfInputBuffers"> an integer to hold the returned value. </param>
        ///  
        ///  <returns>
        ///  True on success, False on failure.
        ///  </returns>
        
        internal Boolean GetNumberOfInputBuffers( SafeFileHandle hidDeviceObject, ref Int32 numberOfInputBuffers ) 
        {             
            Boolean success = false;

            try
            {
                if (!((IsWindows98Gold())))
                {
                    //  ***
                    //  API function: HidD_GetNumInputBuffers

                    //  Purpose: retrieves the number of Input reports the host can store.
                    //  Not supported by Windows 98 Gold.
                    //  If the buffer is full and another report arrives, the host drops the 
                    //  ldest report.

                    //  Accepts: a handle to a device and an integer to hold the number of buffers. 

                    //  Returns: True on success, False on failure.
                    //  ***

                    success = HidD_GetNumInputBuffers(hidDeviceObject, ref numberOfInputBuffers);
                }
                else
                {
                    //  Under Windows 98 Gold, the number of buffers is fixed at 2.

                    numberOfInputBuffers = 2;
                    success = true;
                }

                return success;
            }
            catch (Exception ex)
            {
                DisplayException( MODULE_NAME, ex ); 
                throw ; 
            }                       
        } 
                
        ///  <summary>
        ///  sets the number of input reports the host will store.
        ///  Requires Windows XP or later.
        ///  </summary>
        ///  
        ///  <param name="hidDeviceObject"> a handle to the device.</param>
        ///  <param name="numberBuffers"> the requested number of input reports.  </param>
        ///  
        ///  <returns>
        ///  True on success. False on failure.
        ///  </returns>
        
        internal Boolean SetNumberOfInputBuffers( SafeFileHandle hidDeviceObject, Int32 numberBuffers ) 
        {              
            try 
            { 
                if ( !IsWindows98Gold() ) 
                {                     
                    //  ***
                    //  API function: HidD_SetNumInputBuffers
                    
                    //  Purpose: Sets the number of Input reports the host can store.
                    //  If the buffer is full and another report arrives, the host drops the 
                    //  oldest report.
                    
                    //  Requires:
                    //  A handle to a HID
                    //  An integer to hold the number of buffers. 
                    
                    //  Returns: true on success, false on failure.
                    //  ***
                    
                    HidD_SetNumInputBuffers( hidDeviceObject, numberBuffers );
                    return true;                    
                } 
                else 
                { 
                    //  Not supported under Windows 98 Gold.
                    
                    return false; 
                }
                            } 
            catch ( Exception ex ) 
            { 
                DisplayException( MODULE_NAME, ex ); 
                throw ; 
            }            
        } 
                
        ///  <summary>
        ///  Find out if the current operating system is Windows XP or later.
        ///  (Windows XP or later is required for HidD_GetInputReport and HidD_SetInputReport.)
        ///  </summary>
        
        internal Boolean IsWindowsXpOrLater() 
        {        
            try 
            { 
                OperatingSystem myEnvironment = Environment.OSVersion; 
                
                //  Windows XP is version 5.1.
                
                System.Version versionXP = new System.Version( 5, 1 );

                if (myEnvironment.Version >= versionXP)                 
                { 
                    Debug.Write( "The OS is Windows XP or later." ); 
                    return true; 
                } 
                else 
                { 
                    Debug.Write( "The OS is earlier than Windows XP." ); 
                    return false; 
                }                 
            } 
            catch ( Exception ex ) 
            { 
                DisplayException( MODULE_NAME, ex ); 
                throw ; 
            }          
        }         
        
        ///  <summary>
        ///  Find out if the current operating system is Windows 98 Gold (original version).
        ///  Windows 98 Gold does not support the following:
        ///  Interrupt OUT transfers (WriteFile uses control transfers and Set_Report).
        ///  HidD_GetNumInputBuffers and HidD_SetNumInputBuffers
        ///  (Not yet tested on a Windows 98 Gold system.)
        ///  </summary>
        
        internal Boolean IsWindows98Gold() 
        {
            Boolean result = false;
            try 
            { 
                OperatingSystem myEnvironment = Environment.OSVersion; 
                
                //  Windows 98 Gold is version 4.10 with a build number less than 2183.
                
                System.Version version98SE = new System.Version( 4, 10, 2183 );

                if (myEnvironment.Version < version98SE)                
                { 
                    Debug.Write( "The OS is Windows 98 Gold." );
                    result = true; 
                } 
                else 
                { 
                    Debug.Write( "The OS is more recent than Windows 98 Gold." );
                    result = false; 
                }
                return result;
            } 
            catch ( Exception ex ) 
            { 
                DisplayException( MODULE_NAME, ex ); 
                throw ;                 
            }                     
        }         
        
        ///  <summary>
        ///  Provides a central mechanism for exception handling.
        ///  Displays a message box that describes the exception.
        ///  </summary>
        ///  
        ///  <param name="moduleName">  the module where the exception occurred. </param>
        ///  <param name="e"> the exception </param>
        
        internal static void DisplayException( String moduleName, Exception e ) 
        {             
            String message = null; 
            String caption = null; 
            
            //  Create an error message.
            
            message = "Exception: " + e.Message + ControlChars.CrLf + "Module: " + moduleName + ControlChars.CrLf + "Method: " + e.TargetSite.Name; 
            
            caption = "Unexpected Exception"; 
            
            MessageBox.Show( message, caption, MessageBoxButtons.OK ); 
            Debug.Write( message );             
        }         
    } 
} 
