import java.util.Arrays;

/*! Class that determines the lower OSI-level structure of the Kryptoknight
 *messages.  This class also checks the validity of those packages.
 */
class AuthPacket{
	private boolean debug=true;//!<Set to true to see more run-time info
	
	//! Size of the message to authenticate
	private static int 
		MESSAGE_SIZE=Kryptoknight.MESSAGE_SIZE,
	//! Number of bytes needed to form the address
		ADDRESS_SIZE=Kryptoknight.ADDRESS_SIZE,
	//! Number of bytes needed for a nonce (=random number)
		NONCE_SIZE=Kryptoknight.NONCE_SIZE,
	//! Number of bytes in a MAC
		MAC_SIZE=Kryptoknight.MAC_SIZE;

	private byte[] 
		fromP=null, 	//!< Source address contained in package
		toP=null, 		//!< Destination address contained in package
		naP=null, 		//!< Nonce A in a package
		nbP=null,		//!< Nonce B in a package
		hmacP=null,		//!< hmac contained in a package
		messageP=null;	//!< Message contained in a package
	private byte flowP=0;		//!< Current flow (FLOW1, FLOW2 or FLOW3)
	
	/*! Constructor for FLOW1 and FLOW2 packages.
	 *\param from Source address
	 *\param to	Destination address
	 *\param nonce NonceA for FLOW1, NonceB for FLOW2
	 *\param x Message in FLOW1, Hmac in FLOW2
	 */
	AuthPacket(byte[] from, byte[] to, byte[] nonce, byte[] x){
		//Constructor for flow1 & 2
		if(from.length==ADDRESS_SIZE && to.length==ADDRESS_SIZE){
			fromP=new byte[from.length];
			System.arraycopy(from,0,fromP,0,from.length);
			toP=new byte[to.length];
			System.arraycopy(to,0,toP,0,to.length);
		}else if(debug)System.out.println("Size is wrong");
		if(x.length==MESSAGE_SIZE){
			naP=new byte[nonce.length];
			System.arraycopy(nonce,0,naP,0,nonce.length);
			messageP=new byte[x.length];
			System.arraycopy(x,0,messageP,0,x.length);
			flowP=FlowState.FLOW1;
		}
		if(x.length==MAC_SIZE){
			nbP=new byte[nonce.length];
			System.arraycopy(nonce,0,nbP,0,nonce.length);
			hmacP=new byte[x.length];
			System.arraycopy(x,0,hmacP,0,x.length);
			flowP=FlowState.FLOW2;	
		}
		if(MAC_SIZE==MESSAGE_SIZE)System.err.println("Choose"+
		" message size different from mac size otherwise packets"+
		" will not correctly be generated");
	}//constructor1

	/*! Constructor for FLOW3 packages.
	 *\param from Source address
	 *\param to Destination address
	 *\param hmac Hmac
	 */
	AuthPacket(byte[] from, byte[] to, byte[] hmac){
		//Constructor for flow3
		if(from.length==ADDRESS_SIZE && to.length==ADDRESS_SIZE &&
			hmac.length==MAC_SIZE){
			fromP=new byte[from.length];
			System.arraycopy(from,0,fromP,0,from.length);
			toP=new byte[to.length];
			System.arraycopy(to,0,toP,0,to.length);
			hmacP=new byte[hmac.length];
			System.arraycopy(hmac,0,hmacP,0,hmac.length);
			flowP=FlowState.FLOW3;
		}else if(debug)System.out.println("Size is wrong");
	}//constructor3
	
	/*! Constructor that can make a package out of a byte array.  Packages for 
	 *all flows can be constructed with this method.
	 *\param array Array containing an incoming package
	 *\param flow Flow number (FLOW1, FLOW2 or FLOW3)
	 */
	AuthPacket(byte[] array, byte flow){
		if(debug)System.out.println(
			"Size of received packet: "+array.length+" flow"+flow);
		fromP=new byte[ADDRESS_SIZE];
		System.arraycopy(array,0,fromP,0,ADDRESS_SIZE);
		toP=new byte[ADDRESS_SIZE];
		System.arraycopy(array,ADDRESS_SIZE,toP,0,ADDRESS_SIZE);
		flowP=flow;

		switch(flow){
			case FlowState.FLOW1:
				if(array.length!=2*ADDRESS_SIZE+NONCE_SIZE+MESSAGE_SIZE){
					if(debug)System.err.println("Wrong packet size in flow1");
					return;
				}
				naP=new byte[NONCE_SIZE];
				System.arraycopy(array,2*ADDRESS_SIZE,naP,0,NONCE_SIZE);
				messageP=new byte[MESSAGE_SIZE];
				System.arraycopy(
					array,2*ADDRESS_SIZE+NONCE_SIZE,messageP,0,MESSAGE_SIZE);
				break;
			case FlowState.FLOW2:
				if(array.length!=2*ADDRESS_SIZE+NONCE_SIZE+MAC_SIZE){
					if(debug)System.err.println("Wrong packet size in flow2");
					return;
				}
				nbP=new byte[NONCE_SIZE];
				System.arraycopy(array,2*ADDRESS_SIZE,nbP,0,NONCE_SIZE);
				hmacP=new byte[MAC_SIZE];
				System.arraycopy(
					array,2*ADDRESS_SIZE+NONCE_SIZE,hmacP,0,MAC_SIZE);
				break;
			case FlowState.FLOW3:
				if(array.length!=2*ADDRESS_SIZE+MAC_SIZE){
					if(debug)System.err.println("Wrong packet size in flow3");
					return;
				}
				hmacP=new byte[MAC_SIZE];
				System.arraycopy(array,2*ADDRESS_SIZE,hmacP,0,MAC_SIZE);
				break;
			default:
				System.err.println("Invalid flow");
		}
	}//constructor4
	
	/*! Get source address of a package
	 *\return bytearray containing source address
	 */
	byte[] getFrom(){
		if(fromP==null)if(debug)System.err.println("From is null");
		byte[] ret=new byte[fromP.length];
		System.arraycopy(fromP,0,ret,0,fromP.length);
		return ret;
	}//getFrom
	
	/*! Get nonceA of a package
	 *\return bytearray containing nonceA.
	 */
	byte[] getNa(){
		if(naP==null)System.err.println("Na is null");
		byte[] ret=new byte[naP.length];
		System.arraycopy(naP,0,ret,0,naP.length);
		return ret;
	}//getNa
	
	/*! Get nonceB of a package
	 *\return bytearray containing nonceB
	 */
	byte[] getNb(){
		if(nbP==null)System.err.println("Nb is null");
		byte[] ret=new byte[nbP.length];
		System.arraycopy(nbP,0,ret,0,nbP.length);
		return ret;
	}//getNb
	
	/*! Get message out of a package
	 *\return bytearray containing message
	 */
	byte[] getMessage(){
		if(messageP==null)if(debug)System.err.println("Message is null");
		byte[] ret=new byte[messageP.length];
		System.arraycopy(messageP,0,ret,0,messageP.length);
		return ret;
	}//getMessage
	
	/*! Converts a package to a bytearray.
	 *\param flow Flow number (FLOW1, FLOW2 or FLOW3)
	 *\return bytearray containing the package
	 */
	byte[] toBytes(byte flow){
		ByteArray b=new ByteArray();
		b.add(fromP);
		b.add(toP);
		switch(flow){
			case FlowState.FLOW1:
				b.add(naP);
				b.add(messageP);
				break;
			case FlowState.FLOW2:
				b.add(nbP);
				b.add(hmacP);
				break;
			case FlowState.FLOW3:
				b.add(hmacP);
				break;
			default:
				System.err.println("Invalid flow");
				b=null;
		}
		return b.toByte();
	}//toBytes
	
	/*! Checks a flow1 package
	 *\param me source address
	 *\return true when package is correct
	 */
	boolean checkFlow1(byte[] me){
		//Can't send a message to my own
		if(compare(me,fromP)){
			System.out.println("Source address is me!");
			return false;
		}
		//Make sure it's meant for me
		if(!compare(me,toP)){
			System.out.println("Destination address is not me");
			return false;
		}
		return true;
	}//checkFlow1
	
	/*! Checks a flow2 package
	 *\param me source address
	 *\param other destination address
	 *\param na nonceA
	 *\param message message of flow1
	 *\param secretKey bytearray containing secret key
	 *\return true when package is a correct flow2 package
	 */
	boolean checkFlow2(
		byte[] me,
		byte[] other, 
		byte[] na,
		byte[] message,
		byte[] secretKey){
			
		if(!compare(me, toP)){
			if(debug)System.err.println("Destination address wrong");
			showArray(me,"me");
			showArray(toP,"toP");
			return false;
		}
		if(!compare(other, fromP)){
			if(debug)System.err.println("Source address wrong");
			showArray(other,"other");
			showArray(fromP,"fromP");
			return false;
		}
		ByteArray b;
		b=new ByteArray();
		b.add(na);
		b.add(message);
		//Nb not yet available to Kryptoknight
		b.add(nbP);
		b.add(other);
		byte[] c=Hmac.selfHash(secretKey,b.toByte());
		/*if(debug){
			showArray(other,"FROM OUT");
			showArray(nbP,"Nb OUT");
			showArray(message, "message OUT");
			showArray(na, "Na OUT");
			showArray(c,"HMAC OUT");
			showArray(secretKey,"Secret key");
		}*/
		if(!compare(hmacP,c)){
			if(debug)System.err.println("Hash calculation wrong");
			return false;
		}
		return true;
	}//checkFlow2

	/*! Checks a flow3 package
	 *\param me Expected source address
	 *\param other Expected destination address
	 *\param na Expected nonceA
	 *\param nb Expected nonceB
	 *\param secretKey secretKey that will be used to calculate HMAC
	 *\return true when package is a correct flow3 package
	 */
	boolean checkFlow3(
		byte[] me,
		byte[] other, 
		byte[] na,
		byte[] nb,
		byte[] secretKey){
			
		if(!compare(me, toP)){
			if(debug)System.out.println("Destination address wrong.");
			showArray(me,"me");
			showArray(toP,"toP");
			return false;
		}
		if(!compare(other, fromP)){
			if(debug)System.out.println("Source address wrong.");
			showArray(other,"other");
			showArray(fromP,"fromP");
			return false;
		}
		ByteArray b;
		b=new ByteArray();
		b.add(na);
		b.add(nb);
		/*microcontroller only calculates hashes of 55-byte data
		 *so we add another 23 bytes to the 32 bytes comprising the
		 *nonces
		 */
		b.add(new byte[23]);
		if(!compare(hmacP,Hmac.selfHash(secretKey,b.toByte()))){
			if(debug)System.out.println("Hash calculation wrong.");
			showArray(hmacP,"Received hash");
			showArray(Hmac.selfHash(secretKey,b.toByte()),"Calculated hash");
			showArray(b.toByte(),"55-byte data");
			return false;
		}
		return true;
	}//checkFlow3
	
	/*! Compares two arrays for equality
	 *\param a first array
	 *\param b second array
	 *\return true when arrays are equal
	 */
	boolean compare(byte[] a, byte [] b){
		if(a==null||b==null)return false;
		return Arrays.equals(a,b);
	}//compare
	
	/*! Prints the contents of an array.  Only used for debugging purposes.
	 *\param array Array to show
	 *\param name Name of the array to show
	 */
	void showArray(byte[] array, String name){
		Kryptoknight.showArray(array,name);
	}//showArray

}//class
