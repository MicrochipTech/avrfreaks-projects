/*! \mainpage Java-Kryptoknight
 *
 * \section intro_sec Introduction
 * \subsection what_in What's in this package
 * In this package you can find the Java implementation of the IBM-Kryptoknight
 * protocol.  IBM developed this protocol for authentication especially for
 * devices with limited calculation abilities such as small microcontrollers.
 * Here the implementation is made in Java.  This will make it possible to 
 * authenticate several PC to eachother or to authenticate a microcontroller to
 * a PC.
 * In contrast with the AVR-Kryptoknight package for the Atmel ATMEGA8535 
 * microcontroller, this package can function as a station waiting for an 
 * authentication request.  This is the so called BOB-mode.  The other available
 * mode is the ALICE-mode.  Alice always starts the authentication procedure.
 *
 * \section References
 * There is a lot information available on the internet about authentication.
 * I focused on the Kryptoknight-protocol because it's a lightweight protocol
 * that suited my needs best.
 *
 * \subsection Authentication
 * \li BIRD R., et al., Systematic Design of a Family of Attack-Resistant 
 *	Authentication Protocols
 * \li BIRD R., et al., The Kryptoknight Family of Light-weight Protocols for 
 *	Authentication and Key Distribution, December 1993
 * \li CLARK J., JACOB J. A Survey of Authentication Protocol Literature: 
 *	Version 1.0
 * \li COMPTECHDOC, Authentication protocols http://www.comptechdoc.org/
 *		independent/networking/protocol/protauthen.html, December 3, 2004
 * \li CRISPO B., et al., Symmetric Key Authentication Services Revisited 
 * \li INFOSYSSEC, The Security Portal for Information System Security 
 *	Professionals
 * 		http://www.infosyssec.com/infosyssec/secauthen1.htm 
 * \li JANSON P., et al. Scalability and Flexibility in Authentication Services:
 *		The Kryptoknight approach, 1997
 * \li MOLVA K., et al., Kryptoknight Authentication and Key Distribution System
 * \li MOLVA K., et al., Authentication of Mobile Users, August 20, 1993
 * \li RFC1994 - PPP Challenge Handshake Authentication Protocol (CHAP)
 * \li TSUDIK G. VAN HERREWEGHEN E., On Simple and Secure Key Distribution
 * 
 * \subsection Hmac
 * \li BELLARE M., et al., Keying Hash Functions for Message Authentication
 * \li FIPS PUB 198 The Keyed-Hash Message Authentication Code (HMAC)
 * \li IETF, RFC2104 HMAC: Keyed-Hashing for Message Authentication, February
 *		1997
 *
 * \subsection Hashing
 * \li FIPS PUB 180-2 Announcing the Secure Hash Standard, August 1, 2002
 * \li HUSNI A. Implementation Secure Hash Standard with AVR Microcontroller
 * \li IETF, RFC3174 US Secure Hash Algorithm 1 (SHA1)
 *
 * \subsection secRand Random Number Generators
 * \li BERNSTEIN G.M., et al., Method and Apparatus for generating Secure Random
 *		Numbers using Chaos, US Patent US5007087, April 9, 1991.
 * \li DAVIES R., Hardware Random Number Generators, 
 *		http://www.robertnz.net/hwrng.htm
 * \li GILLEY J.E., Method and Apparatus for generating truly random numbers,
 *		US Patent US5781458, July 14, 1998
 * \li GOLBECK E. C., Random Bit Stream Generator and Method, US Patent 
 *		US5239494, August 24, 1993.
 * \li HOFFMAN E.J., Random Number Generator, US Patent US6061702, May 9 2000
 * \li INTEL, http://web.archive.org/web/20040229005150/http://developer.intel.com/design/security/rng/rngppr.htm
 * \li JAKOBSSON M., et al.  A Physical Secure Random Bit Generator
 * \li JUN B.,et al., The Intel Random Number Generator, April 22, 1999
 * \li LOGUE A., Hardware Random Number Generator, May 2002
 *		http://www.cryogenius.com/hardware/rng/
 * \li MESSINA M. et al., Random Bit Sequence Generator, US Patent 
 *		US2003/0093455 A1, May 15, 2003
 * \li RSA, Hardware-Based Random Number Generation, An RSA Data Security White
 *		Paper
 * \li SKLAVOS N., Random Number Generator Architecture And Vlsi Implementation,
 *		May 2002
 * \li UNER E., Generating Random Numbers, Embedded.com
 *		http://www.embedded.com/showArticle.jhtml?articleID=20900500
 * \li WELLS S.E., Programmable Random Bit Source, US Patent US6795837,
 *		September 21, 2004
 * \li WELLS S.E., et al.Secure Handware Random Number Generator, 
 *		US Patent US6792438, 14 sept 2004
 * \li WIKIPEDIA, Hardware Random Number Generator,
 *		http://en.wikipedia.org/wiki/Hardware_random_number_generator
 * \li WILLWARE, Hardware Random Bit Generator, 
 *		http://willware.net:8080/hw-rng.html
 *
 * \subsection secNoise Random Noise Sources
 * \li ELECTRONIC DESIGN, Wide-Band Analog White-Noise Generator, November 3,
 *		1997, http://www.elecdesign.com/Articles/Index.cfm?AD=1&ArticleID=6356
 * \li SHUPE C.D., Random Voltage Source With Subtstantially Uniform 
 *		Distribution, US Patent US4578649
 */


import LieBTrau.comm.NeoRS232;
import java.awt.event.*;
import javax.swing.Timer;


/*! Interface that groups some constants.  These constants are used for 
 *defining in which state ALICE is when waiting for a flow3 package.
 */
interface WaitingState{
	byte NOT_WAITING=0,//!<ALICE is not waiting
		WAITING=1,//!< ALICE is waiting for a response of BOB
		NO_RESPONSE=2;//!< BOB doesn't respond
}//interface WaitingState



/*! Interface grouping the possible flows
 */
interface FlowState{
		//!Flow1, first flow (generated by ALICE) of the Kryptoknight protocol. 
	byte FLOW1=1,
		//!Flow2, second flow (generated by BOB) of the Krypoknight protocol
		FLOW2=2,
		//!Flow3, third flow (generated by ALICE) of the Kryptoknight protocol
		FLOW3=3;
}//FlowState



/*! Interface grouping the possible communicating states
 */
interface ModeState{
	/*! Two modes are possible.  BOB_MODE is the mode in which this program 
	 *acts as BOB in the Kryptoknight protocol. i.e. The program waits for a
	 *flow2 package to come in.  It will send a flow3 package when the flow2
	 *package was found correct.
	 */
	boolean BOB_MODE=false;
	/*! This program now works in ALICE_MODE.  This is the other possible mode.
	 *In ALICE_MODE, this program generates the flow1 messages and then waits
	 *for a flow3 message to come in.
	 */
	boolean ALICE_MODE=true;

}//ModeState



/*! This class implements the Kryptoknight protocol. It
 *specifies which action to undertake when a certain packet arrives.
 */
public class Kryptoknight implements ActionListener{
	
	static final int 
	//! A variable specifying the length of the message
		MESSAGE_SIZE=19,
	//! Address contained in the packets consists of 4 bytes
		ADDRESS_SIZE=4,
	//! The random number (=nonce) consists of 16bytes = 128bit
		NONCE_SIZE=16,
	//! HMAC with SHA-1 is used, so length of the hash is 20bytes = 160bit
		MAC_SIZE=20;
			
	private boolean 
	/* A value specifiying in which mode the program works
	 * in debug mode more comment is shown on the command line.
	 */
		debug=true,
	//!	mode contains the current mode that is used for communication.
		mode=ModeState.BOB_MODE;
		
	private byte[] 
		na=null, //!< NonceA, generated by ALICE
		nb=null,//!< NonceB, generated by BOB
		message=null,//!< Message that must be authenticated
		me=null,//!< Address of this program (4bytes)
		other=null,//!< Address of the peer (4bytes)
		secretKey=null;//!< Shared secret key between ALICE & BOB
		
	private byte 
		//! flow contains the value of the current flow
		flow=FlowState.FLOW1,
		//! A flag indicating whether ALICE is waiting for an answer yes or no.
		waitingFlag=WaitingState.NOT_WAITING;
		
	//! An object that is used for communication on the lower OSI-layers
	private NeoRS232 neoRS232=null;
	//! A clock indicating how long ALICE must wait for a FLOW3 package
	private TijdKlok t;
	
	/*! The main function initiates two new objects of the Kryptoknight class
	 *and lets them authenticate eachother.
	 *\param args Command line parameters
	 */
	public static void main(String[] args){
		Kryptoknight k1,k2;
		byte[] me={(byte)0,(byte)1,(byte)2,(byte)3};
		byte[] other={(byte)0,(byte)1,(byte)2,(byte)2};
		byte[] sk={
			(byte)0x02,(byte)0xe4,(byte)0x80,(byte)0xb2,
			(byte)0x05,(byte)0x5c,(byte)0x96,(byte)0x31,
			(byte)0x85,(byte)0x87,(byte)0xff,(byte)0x8a,
			(byte)0x37,(byte)0x49,(byte)0x4f,(byte)0x6d,
			(byte)0xf2,(byte)0x22,(byte)0x6c,(byte)0x65,
			(byte)0xd5,(byte)0x16,(byte)0x0d,(byte)0x7c,
			(byte)0x4e,(byte)0x82,(byte)0x8c,(byte)0xc5,
			(byte)0x1d,(byte)0x32,(byte)0xc3,(byte)0xfd,
			(byte)0xbd,(byte)0x5d,(byte)0xe9,(byte)0xdb,
			(byte)0xfc,(byte)0x57,(byte)0x5e,(byte)0x41,
			(byte)0x50,(byte)0x41,(byte)0x3b,(byte)0xd4,
			(byte)0xd9,(byte)0xee,(byte)0x61,(byte)0xee,
			(byte)0xf7,(byte)0xa8,(byte)0xfe,(byte)0x15,
			(byte)0x03,(byte)0x02,(byte)0x1c,(byte)0xf6,
			(byte)0x07,(byte)0x9f,(byte)0xd4,(byte)0x37,
			(byte)0x40,(byte)0x96,(byte)0x5b,(byte)0x2f
		};
		k1=new Kryptoknight(me, sk,"COM2");
		//k2=new Kryptoknight(other, sk,"COM2");
		/*byte[] message= new byte[MESSAGE_SIZE];
		if(k1.authenticate(other, message))
			System.out.println("Client authenticated by host");
		else System.out.println("Not authenticated");*/
		//threading();
	}//main
	
	/*! The constructor for the Kryptoknight class
	 *\param me Address of this object (4 bytes)
	 *\param sk 64-byte secret key
	 *\param port COM-port that must be used for authentication (COM1 or COM2)
	 */
	public Kryptoknight(byte[] me, byte[] sk, String port){
		if(me.length==ADDRESS_SIZE){
			this.me=new byte[me.length];
			System.arraycopy(me,0,this.me,0,me.length);
			secretKey=new byte[sk.length];
			System.arraycopy(sk,0,secretKey,0,sk.length);
			neoRS232=new NeoRS232(port,true);
			neoRS232.addActionListener(this);
			t=new TijdKlok(30000);
		}
		else if(debug)System.err.println("Size of address is incorrect");
	}//constructor
	
	/*! This command closes the communication port.  This must be called before
	 *closing the program.
	 */
	void close(){
		neoRS232.sluitPoort();
	}//close
	
	/*! Command used for authenticating another party.
	 *\param other Address of the other party (4bytes)
	 *\param message Message to authenticate
	 *\return true when authentication passed, otherwise false
	 */
	boolean authenticate(byte[] other, byte[] message){
		mode=ModeState.ALICE_MODE;
		boolean ret=authenticateLowLevel(other, message);
		//toString();
		mode=ModeState.BOB_MODE;
		flow=FlowState.FLOW1;
		na=null;
		nb=null;
		other=null;
		return ret;
	}//authenticate
	
	/*! This function takes care of the message flow in the Kryptoknight
	 *protocol.  This is by far the most important function.
	 *\param other Address of the other to authenticate
	 *\param message Message that must be authenticated
	 *\return true when authentication passed, otherwise false.
	 */
	boolean authenticateLowLevel(byte[] other, 
		byte[] message){
		
		if(other.length==ADDRESS_SIZE && message.length==MESSAGE_SIZE){
			//FLOW1
			na=(new Nonce()).generateNonce(NONCE_SIZE);
			if(debug){
				showArray(na,"NonceA");
				showArray(me,"Me");
				showArray(other,"Other");
				showArray(message,"Message");
			}
			AuthPacket ap=new AuthPacket(me, other, na, message);
			if(debug)showArray(ap.toBytes(FlowState.FLOW1),"FLOW1 Packet");
			neoRS232.schrijfData(ap.toBytes(FlowState.FLOW1));
			//FLOW2
			flow=FlowState.FLOW2;
			waitingFlag=WaitingState.WAITING;
			t.timerStarten();
			//wait for an answer of the client
			while(waitingFlag==WaitingState.WAITING);
			t.timerStoppen();
			//was it a timeout or a correct answer?
			if(waitingFlag==WaitingState.NO_RESPONSE){
				if(debug)System.out.println("No Response");
			}else{
				//reconstruct packet that client sent
				ap=new AuthPacket(neoRS232.getData(),FlowState.FLOW2);
				//is this package correct?
				if(ap.checkFlow2(me,other,na,message,secretKey)){
					//if yes, then send final package
					//FLOW3
					//get Nb from Packet
					nb=ap.getNb();
					ByteArray b=new ByteArray();
					b.add(na);
					b.add(nb);
					/*microcontroller only calculates hashes of 55-byte data
					 *so we add another 23 bytes to the 32 bytes comprising the
					 *nonces
					 */
					b.add(new byte[23]);
					ap=new AuthPacket(me, other, Hmac.selfHash(secretKey,b.toByte()));
					byte[] c=ap.toBytes(FlowState.FLOW3);
					if(debug)showArray(c,"Flow3 package");
					neoRS232.schrijfData(c);
					return true;
				}
			}
		}else{
			if(debug)System.err.println("Size of arguments is wrong");
		}
		return false;
	}//authenticate
	
	/*! Routine that will be called by the lower level OSI-routines when data
	 *comes in.  It calls routines that check the incoming packets.  When the
	 *packets are correct a response can be sent.
	 *\param e The ActionEvent
	 */
	public void actionPerformed(ActionEvent e){
		if(debug)System.out.println("Packet received in mode "+
			(mode==ModeState.ALICE_MODE?"ALICE":"BOB")+
			"\tFlow"+flow);
		if(mode==ModeState.ALICE_MODE){
			waitingFlag=WaitingState.NOT_WAITING;
		}
		else{
			AuthPacket ap;
			switch((byte)flow){
				case FlowState.FLOW1:
					ap=new AuthPacket(neoRS232.getData(),FlowState.FLOW1);
					if(ap.checkFlow1(me)){
						if(debug)System.out.println("Correct Flow1 package");
						//Save A, Na, Message (& Nb)
						other=ap.getFrom();
						na=ap.getNa();
						message=ap.getMessage();
						nb=(new Nonce()).generateNonce(NONCE_SIZE);
						ByteArray b=new ByteArray();
						b.add(na);
						b.add(message);
						b.add(nb);
						b.add(me);
						if(debug){
							showArray(na, "Na IN");
							showArray(message, "Message IN");
							showArray(nb, "Nb IN");
							showArray(me,"FROM IN");
						}
						ap=new AuthPacket(me, other, nb, Hmac.selfHash(secretKey,b.toByte()));
						byte[] c=ap.toBytes(FlowState.FLOW2);
						if(debug)showArray(c,"Flow2 package");
						t.timerStarten();
						neoRS232.schrijfData(c);
						flow=FlowState.FLOW3;
					}else{
						if(debug)System.out.println("Invalid Flow1 packet");
					};
					break;
				case FlowState.FLOW2:
					if(debug)System.err.println("Impossible state");
					break;
				case FlowState.FLOW3:
					ap=new AuthPacket(neoRS232.getData(),FlowState.FLOW3);
					if(ap.checkFlow3(me,other,na,nb,secretKey)){
						System.out.println("Host authenticated by client");
						t.timerStoppen();
					}else{
						if(debug)System.out.println("Invalid Flow3 packet");
						System.out.println("Host not authenticated by client");
					}
					flow=FlowState.FLOW1;
					break;
				default:
					if(debug)System.err.println("Impossible flow");
			}
		}
	}//actionPerformed
	
	/*! A useful routine for debugging purposes.  It shows an array on the
	 *output capture window.
	 *\param array Array to show
	 *\param name Name of the array that will be shown
	 */
	static void showArray(byte[] array, String name){
		if(array!=null){
			System.out.println("\n"+name+" (length="+array.length+"bytes)");
			for(int i=0;i<array.length;i++){
				int a=(array[i]<0?array[i]+256:array[i]);
				System.out.print("0x"+(a<0x10?"0":"")+
				Integer.toHexString(a)+",");
				if((i+1)%8==0)System.out.println();
			}
		}else System.out.print(name+" null");
		System.out.println();
	}//showArray

	/*! A debug routine that shows which threads are currently running.
	 */
	static void threading(){
		//All non-daemons must close before the application can close;
		ThreadGroup sys=Thread.currentThread().getThreadGroup();
		sys.list();
		Thread[] all=new Thread[sys.activeCount()];
		sys.enumerate(all);
		for(int i=0;i<all.length;i++){
			System.out.println(
				"Name: "+all[i].getName()
				+"\tDeamon: "+(all[i].isDaemon()?"true":"false"));
		}		
	}//threading
	
	/*! The toString method that shows the elements of an object of the
	 *Kryptoknight class.
	 */
	public String toString(){
		showArray(me,"Me");
		showArray(other,"Other");
		showArray(na,"Na");
		showArray(nb,"Nb");
		showArray(message,"Message");
		showArray(secretKey,"SecretKey");
		return null;		
	}//toString
	
	/*! Inner class for keeping time.
	 */
	class TijdKlok implements ActionListener{
		/*Timer class added because a transmitter must know how long
		 *it has to wait for an acknowledge of the receiver.
		 */
		 
    	Timer timer;//!< The timer object holding the time
    	int waittime;//!< Time in seconds that must be waited
    	
    	/*! Initialize the timer with a waiting interval "waittime" in seconds.
    	 *\param waittime Time that must pass before an interrupt is called
    	 */
		TijdKlok(int waittime){
			//Timer zal na TIMEDELAY de actionListener van deze klasse oproepen
			timer=new Timer(waittime,this);
			//Timer zal stoppen als TIMEDELAY bereikt is
			timer.setRepeats(false);
			this.waittime=waittime;
		}//constructor
		
		/*! Method that will be called when the "waittime" interval has passed.
		 *\param e The ActionEvent
		 */
		public void actionPerformed(ActionEvent e){
			if(debug)System.err.println("No response received in "+
			waittime/1000+"s."+" Going back to flow1");
			waitingFlag=WaitingState.NO_RESPONSE;
			flow=FlowState.FLOW1;
			timerStoppen();
		}//actionPerformed
		
		/*! Start the timer
		 */
		void timerStarten(){
			timer.start();
		}//timerStarten
		
		/*! Stop the timer
		 */
		void timerStoppen(){
			timer.stop();
		}//timerStoppen
	
	}//class TijdKlok
}//class