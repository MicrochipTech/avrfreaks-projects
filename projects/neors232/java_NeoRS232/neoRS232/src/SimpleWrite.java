/*!\file SimpleWrite.java
 * Contains low-level functions for writing data to the 
 * serial port.
 */
 
package LieBTrau.comm;

import java.io.*;
import java.util.*;
import javax.comm.*;

/*!Class that will write the data to the serial port
 */
class SimpleWrite implements Protocol {
	
    private OutputStream outputStream;
	
	/*!Constructor
	 *\param serialPort Handler to the serialPort
	 */
	SimpleWrite(SerialPort serialPort){
    	try {
        	outputStream = serialPort.getOutputStream();
        } catch (IOException e) {
        	System.err.println(e);
        }
	}//constructor
	
	/*!Check if the received character is a flag character.
	 *\param c The received data byte.
	 */
	boolean vlagKarakterInData(byte c){
		if((c==LEADING_FLAG||
			c==TRAILING_FLAG)||
			(c==DLE_FLAG||
			c==ACK0_FLAG))return true;
		else return false;
	}//vlagKarakter
	
	/*!Write a dataframe to the serial port
	 *\param buffer Buffer containing the data
	 *\param typeFrame Type of the frame (ordinary or ack).
	 */
	void schrijfString(byte[] buffer, int typeFrame){
		/*Er is nog geen beperking in de buffergrootte, een grote
		 *hoeveelheid data kan de ontvangstbuffer van de ontvanger 
		 *overspoelen
		 */
		try {
			switch(typeFrame){
				
				case INFO_FRAME:
					//Leading flag
					outputStream.write(LEADING_FLAG);
					//Data	
					for(int i=0;i<buffer.length;i++){		
        				if(vlagKarakterInData(buffer[i]))outputStream.write(DLE_FLAG);
        				outputStream.write(buffer[i]);
        			}
					//Trailing flag
		        	outputStream.write(TRAILING_FLAG);
		        	break;
		        	
		        case ACK_FRAME:
		        	outputStream.write(ACK0_FLAG);
		        	break;
		        	
		        default:
		        	System.err.println("Geen type gegeven");
		        	
		   	}    	
        	outputStream.flush();
        } catch (IOException e) {
        	System.err.println("k1:" +e);
        }
    }//SchrijfString
    
    /*!Close outputstream, so no more data can be sent.
     */
    void close() throws IOException{
    	outputStream.flush();
    	outputStream.close();
    }//close
    
}//class
