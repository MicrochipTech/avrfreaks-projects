/*!\file NeoRS232.java
 ** This class implements an Idle RQ Stop & Wait protocol.  This is a
 * complicated manner to say that a transmitter must wait for an answer
 * of the receiver before it can send something again.
 * This protocol isn't the most speed-efficient way of communicating, 
 * but I'm using it to communicate it with a microcontroller.  To prevent
 * slowing down the microcontroller too much, I have to use a simple 
 * protocol.
 * The protocol also includes a CRC16-error detection.
 * \n
 * For a user of this library, the above information is of no importance.
 * The only thing you need to know is how to implement the following
 * functions.
 * Oh, remember: to use this package, you must have some version of 
 * javacomm installed.  It's available from the 
 * <a href="http://java.sun.com">Java</a> website.
 *
 * \image html LieBtrau_anim.gif
 * \author 	LieBtrau
 * \author	locomotieff@tiscali.be
 * \version	version 1.1
 *
 */
package LieBTrau.comm;

import java.io.*;
import java.util.*;
import javax.comm.*;
import javax.swing.Timer;
import LieBtrau.util.CRC16;
import java.awt.event.ActionListener;
import java.awt.event.ActionEvent;

/*! Main class of the RS232-protcol.
 */
public class NeoRS232 implements Protocol{

    private Enumeration portList;
    private CommPortIdentifier portId;
    private SerialPort serialPort;
    private SimpleWrite sw;
    private SimpleRead sr;
	private byte[] data;
	private TijdKlok tk;
	private boolean rxBusyFlag=false;
	
	//set debug to "true" when you want messages in the command window
	private boolean debug=false;
	
	//om te laten weten dat er data ontvangen is
	private ActionListener actionListener;
		
	//zal de ontvangen data bevatten
	private byte[] receivedData=null;
	
	//baudrate of communication
	private int baudRate;

/*	public static void main(String[] args){
		SerieData sd=new SerieData("COM1");
		byte[] a=new byte[10];
		sd.schrijfData(a);
		//sd.sluitPoort();
	}//main*/
	
	/** This is a constructor 
	 *	@param poort This is a String indicating the type of port
	 *	you want.  e.g. "COM1"
	 *  @param baudRate This is the baudRate used for communication
	 *  	(e.g. 115200)
	 *  @param debug Make this boolean true if you want debug info,
	 *  otherwise make this boolean false;
	 */ 
	 public NeoRS232(String poort, int baudRate, boolean debug){
	 	this(poort,baudRate);	 //call other constructor	
	 	this.debug=debug;
	 	sr.setDebug(debug);
	 }//constructor1
	 
	/** This is a constructor 
	 *	@param poort This is a String indicating the type of port
	 *  @param baudRate This is the baudRate used for communication
	 *  	(e.g. 115200)
	 *	you want.  e.g. "COM1"
	 */ 	 
	public NeoRS232(String poort, int baudRate){
		this.baudRate=baudRate;
		tk=new TijdKlok();

		boolean poortOpen=false;
        portList = CommPortIdentifier.getPortIdentifiers();
        if(!portList.hasMoreElements())System.err.println("No ports found!");
        while (portList.hasMoreElements()) {
            portId = (CommPortIdentifier) portList.nextElement();
            if (portId.getPortType() == CommPortIdentifier.PORT_SERIAL)
                if (portId.getName().equals(poort))poortOpen=openPoort(poort);
		}
	}//constructor
	
	//!Returns true when debug info enabled
	boolean getDebug(){
		return debug;
	}//getDebug
	
	//!Stop the timer (packet has arrived before timeout)
	void stopTimer(){
		tk.timerStoppen();
	}//stopTimer
	
	/** Use this function to write data to the port
	 *	@param data A byte-array containing the bytes to be sent.
	 *	Don't use large arrays, because you will flood the receiver.
	 */
	public void schrijfData(byte[] data){
		this.data=data;
		data=appendCRC(data);
		/*start timer before data is sent because ACK sometimes
		 *comes in before timer is started
		 */
		tk.timerStarten();
		sw.schrijfString(data,INFO_FRAME);
		if(debug)System.out.println("Data geschreven");
	}//schrijfData
	
	/** This function closes the communications port.  Call this
	 * function before closing your application.  Otherwise the port
	 * remains open and you won't get the message: "Press any key to
	 * continue..."
	 */	
	public void sluitPoort(){
		/*Only close port when no data is being transferred.
		 *Check this first!
		 */
		 if(debug)System.out.println("Waiting for closure"+
		 (tk.isRunning()?"tkTrue":"tkFalse"));
		while(tk.isRunning());
		try{
			sw.close();
			sr.close();
		}
		catch(IOException e){
			System.err.println("Not possible to close port: "+e);
		}
		serialPort.close();
		if(debug)System.out.println("All is closed");
	}//sluitPoort

	/** If you want that your class does something when
	 *  data comes in, then you have to add an ActionListener
	 *  using this method.  The implementing 
	 *  ActionListener-class should contain a method 
	 *  actionPerformed.  This method will be called when
	 *  data comes in.
	 *  @see java.awt.event.ActionListener
	 *  @param actionListener an object of a class that 
	 *  implements the ActionListener class.
	 */
	public void addActionListener(ActionListener actionListener){
		this.actionListener=actionListener;
	}//addActionListener

	/** Call this function to get the received data
	 */
	public byte[] getData(){
		if(debug)System.out.println("receivedData: "+receivedData.length);
		/*byte[] ret=new byte[receivedData.length];
		for(int i=0;i<receivedData.length;i++){
			ret[i]=receivedData[i];
		}
		return ret;*/
		return receivedData;
	}//getData
//------------------------------------------------------------------------------	

	//! Low level command to open the comport.
	boolean openPoort(String poortNaam){
		if(debug)System.out.print("Opening "+poortNaam+"...");
		try {
			serialPort = (SerialPort)portId.open("SimpleWriteApp", 2000);
			/*Wacht 2000ms op het openen van de poort
			 *Indien de poort niet opent, wordt een PortInUseException gegenereerd
			 */
		} catch (PortInUseException e) {
			System.err.println("\n"+poortNaam+" "+e);
			return false;
		}
		try {
			serialPort.setSerialPortParams(
				baudRate,
				SerialPort.DATABITS_8,
				SerialPort.STOPBITS_1,
				SerialPort.PARITY_NONE
			);
		} catch (UnsupportedCommOperationException e) {
			System.err.println("\n"+poortNaam+" "+e);
			return false;
		}
		
		sw=new SimpleWrite(serialPort);
		sr=new SimpleRead(serialPort,this);
						
		if(debug)System.out.println("Success");
		return true;
	}//openPoort


	private byte[] appendCRC(byte[] ab){
		/*Maak data klaar voor verzenden (append CRC)
		 *parameter = array of bytes to be sent.
		 *return-value = array of bytes to be sent with CRC appended.
		 *
		 *The CRC is appended with the LSB first (little endian order)
		 */
		 
		int grootte=ab.length;
		byte[] result=new byte[grootte+2];
		CRC16 crc16=new CRC16();
		crc16.update(ab);
		short crcValue=crc16.getValue();
		if(debug)System.out.println("CRC-Value: 0x"+Integer.toHexString(crcValue));
		//vul returnarray met de te zenden data
		for(int i=0;i<grootte;i++)result[i]=ab[i];
		//voeg er de CRC-bytes aan toe - little endian
		for(int i=0;i<2;i++){
			result[i+grootte]=(byte)((crcValue>>8*i)&0xFF);
		}
		
		return result;
	}//appendCRC
	
	private byte[] stripCRC(byte[] buffer){
		/*Calculate the checksum
		 *parameter = received array with CRC at the end
		 *
		 *return: 	null  when wrong checksum
		 *			received array without checksum when correct checksum	 
		 */
		if(debug){
			System.out.println("Buffer length(data+CRC): "+buffer.length);
			for(int i=0;i<buffer.length;i++){
				String s=Integer.toHexString(buffer[i]);
				if(s.length()>2)s=s.substring(s.length()-2);
				System.out.print(s+" ");
			}
		}
		CRC16 crc16=new CRC16();
		short crc16Ontvangen=0;
		int grootte=buffer.length-2;
		if(grootte>0){
			byte[] result=new byte[grootte];
			for(int i=0;i<grootte;i++)result[i]=buffer[i];
			crc16.update(result);
			
			//zoek ontvangen CRC op aan het einde van de ontvangen data
			for(int i=0;i<2;i++)crc16Ontvangen|=(buffer[grootte+i]&0xFF)<<(8*i);
			if(debug)System.out.println(Integer.toHexString(crc16.getValue()));		
			if(crc16.getValue()!=crc16Ontvangen){
				System.err.println("\nFoute Checksum");
				return null;
			}
			else{
				if(debug)System.out.println("\nJuiste checksum");
				return result;
			}
		}else{
			System.err.println("Invalid data");
			return null;
		}
	}//stripCRC
	
	/*!This function gets called when a complete frame has been
	 *received.
	 *It sends the transmitter an acknowledge when data is 
	 *received correctly.  It also passes the data on to a 
	 *higher OSI-layer.
	 *\param inputBuffer Buffer containing data to send.
	 */
	void bewerkDatagram(ArrayList inputBuffer){
		receivedData=stripCRC(toByteArray(inputBuffer));
	 	//toonBuffer(receivedData);
	 	if(receivedData!=null){
	 		//Frame goed ontvangen, stuur dan een acknowledge
	 		if(debug)System.out.println("ACK sent");
	 		sw.schrijfString(null,ACK_FRAME);
	 		//Laat aan de actionListener weten dat we iets ontvangen hebben
	 		if(actionListener!=null)
				actionListener.actionPerformed(
					new ActionEvent(this,0,"Data ready"));			
	 		else 
	 			System.err.println("No ActionListener attached.");
	 	}
	}//bewerkDatagram
	
	//!Print contents of the buffer on the screen
	void toonBuffer(ArrayList buffer){
		System.out.println(buffer.size());
		for(int i=0;i<buffer.size();i++)System.out.print((Byte)buffer.get(i));
	}//toonBuffer
	
	//!Print contents of the buffer on the screen
	void toonBuffer(byte[] buffer){
		System.out.println(buffer.length);
		for(int i=0;i<buffer.length;i++)System.out.print(Byte.toString(buffer[i]));
	}//toonBuffer
	
	//!Convert an arraylist to a buffer of bytes
	byte[] toByteArray(ArrayList buffer){
		byte[] result=new byte[buffer.size()];
		for(int i=0;i<buffer.size();i++)result[i]=((Byte)buffer.get(i)).byteValue();
		return result;
	}//toByteArray
	
///////////////////////////////////////////////////////////	
	/*!Timer class added because a transmitter must know how long
	 *it has to wait for an acknowledge of the receiver.
	 */		
	class TijdKlok implements ActionListener{
		 
    	Timer timer;//!< Timer object keeping track of time
		/*!Variable that counts how many times we tried to
		 *resend data.
		 */
		int poging;
    	
    	final int TIMEDELAY=5000;//!< Timeout (in ms.)
    	//!Maximum number of times to try resending data
    	final int MAX_POGING=5;
    	
		TijdKlok(){
			//Timer zal na TIMEDELAY de actionListener van deze klasse oproepen
			timer=new Timer(TIMEDELAY,this);
			//Timer zal stoppen als TIMEDELAY bereikt is
			timer.setRepeats(false);
		}//constructor
		
		/*!Timeout reached.  Try five times to send the 
		 * data again.
		 * \param e timer event that triggers this function.
		 */
		public void actionPerformed(ActionEvent e){
			System.err.println("Het antwoord is niet aangekomen binnen de tijd");
			if(poging<MAX_POGING){
				poging++;
				System.err.println("Poging "+poging+" probeer opnieuw te zenden");
				//timer.setInitialDelay(TIMEDELAY);
				timer.start();
				sw.schrijfString(data,INFO_FRAME);
			}
			else{
				System.err.println("Pogingen gestaakt. Verbinding verbroken");
				timerStoppen();
				sluitPoort();
			}
		}//actionPerformed
		
		//!Return true when timer is running.
		boolean isRunning(){
			return timer.isRunning();
		}//isRunning
		
		//!Start the timer
		void timerStarten(){
			timer.start();
		}//timerStarten
		
		//!Stop the timer
		void timerStoppen(){
			poging=0;
			timer.stop();
		}//timerStoppen
	
	}//class TijdKlok

	
}//class