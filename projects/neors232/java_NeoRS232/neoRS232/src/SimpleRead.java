/*!\file SimpleRead.java
 * Contains low level functions for reading data from the
 * serial port.
 */
 
package LieBTrau.comm;

import java.io.*;
import java.util.*;
import javax.comm.*;
import java.util.zip.CRC32;

/*!Class that listens for incoming data on the serial port
 */
class SimpleRead implements SerialPortEventListener, Protocol {
 
	private InputStream inputStream=null;
	private boolean dle_received=false;
	private ArrayList inputBuffer=new ArrayList();
	private NeoRS232 sd=null;
	private boolean debug=false;
	
	/*!Constructor
	 *\param serialPort Handler to this serial port
	 *\param sd NeoRS232-object that will read the data
	 */
    public SimpleRead(SerialPort serialPort, NeoRS232 sd) {
        try {
            inputStream = serialPort.getInputStream();
        } catch (IOException e) {
        	System.err.println(e);
		}
		try {
            serialPort.addEventListener(this);
		} catch (TooManyListenersException e) {
			System.err.println(e);
		}
		serialPort.notifyOnDataAvailable(true);
		
		this.sd=sd;
		debug=sd.getDebug();
    }//constructor

	/*!Function that will automatically be called when an
	 *event occurs on the serial port
	 *\param event event on serial port that will trigger
	 *	this function.
	 */
    public void serialEvent(SerialPortEvent event) {
	    switch(event.getEventType()) {
	        case SerialPortEvent.BI:
	        	System.err.println("Break interrupt");
	        	break;
	        case SerialPortEvent.OE:
	        	System.err.println("Overrun Error");
	        	break;
	        case SerialPortEvent.FE:
	        	System.err.println("Framing error");
	        	break;
	        case SerialPortEvent.PE:
	        	System.err.println("Parity error");
	        	break;
	        case SerialPortEvent.CD:
	        	System.err.println("Carrier detect");
	        	break;
	        case SerialPortEvent.CTS:
	        	System.err.println("Clear to send");
	        	break;
	        case SerialPortEvent.DSR:
	        	System.err.println("Data set ready");
	        	break;
	        case SerialPortEvent.RI:
	        	System.err.println("Ring indicator");
	        	break;
	        case SerialPortEvent.OUTPUT_BUFFER_EMPTY:
	        	System.err.println("JAVA has written data.  Buffer is now empty.");
	        	break;
	        case SerialPortEvent.DATA_AVAILABLE:
	 			/*Lees meerdere bytes per keer dat we in dit blok komen.
	 			 *De processor voert dit blok ongeveer om de tien databytes uit
	 			 */
	            try {
	            	int c=0;
	            	/*must read ints instead of bytes, because out trailing
	            	 *flag=0xFF=-1 as byte, which means reading will be stopped
	            	 */
	            	while((c=inputStream.read())!=-1){
						byte character=(byte)c;
	 	 				switch(character){
		 					case LEADING_FLAG:
		 						if(!dle_received){
		 							inputBuffer.clear();
		 							if(debug)System.out.println("\nLeading flag");
		 						}
		 						else {
		 							inputBuffer.add(new Byte(character));
		 							dle_received=false;
		 						}
		 						break;
		 					case TRAILING_FLAG:
		 						if(!dle_received){
		 							if(debug)System.out.println("\nTrailing flag");
		 							sd.bewerkDatagram(inputBuffer);
		 						}
		 						else {
		 							inputBuffer.add(new Byte(character));
		 							dle_received=false;
		 						}
		 						break;
		 					case ACK0_FLAG:
		 						if(!dle_received){
		 							if(debug)System.out.println("Ack ontvangen");
		 							sd.stopTimer();
		 						}
		 						else {
		 							inputBuffer.add(new Byte(character));
		 							dle_received=false;
		 						}
		 						break;
		 					case DLE_FLAG:
		 						if(!dle_received){
		 							if(debug)System.out.println("DLE received");
		 							dle_received=true;
		 						}
		 						else {
		 							inputBuffer.add(new Byte(character));
		 							dle_received=false;
		 						}
		 						break;
		 					default:
		 						inputBuffer.add(new Byte(character));
		 						dle_received=false;
		 				}//switch
		 			}//while
	            } catch (IOException e) {
	            	System.err.println(e);
				}
	            break;
	        }//switch
   }//serialEvent

	/*!Close inputStream so no more data will be read.
	 */
	void close() throws IOException{
		inputStream.close();
	}//close

	/*!Set to true if you want to receive debug data
	 *\param d Set to true to receive debug info.
	 */
	void setDebug(boolean d){
		debug=d;
	}//setDebug
	
		
}//class