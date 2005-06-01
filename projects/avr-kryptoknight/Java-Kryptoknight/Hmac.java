import java.security.*;
import javax.crypto.*;

/*! A class that implements the HMAC-algorithm described in RFC2104
 */
class Hmac{
	
	private static SecretKey sk=null;//!< Shared secret key
	//! Set to true to see more run-time info
	private static boolean debug=false;
	
	/*! Method that generates a secret key for use in HMAC
	 *\return The secret key
	 */
	static SecretKey generateKey(){
        // Generate secret key for HMAC-SHA1
        try{
        	KeyGenerator kg = KeyGenerator.getInstance("HmacSHA1");
        	sk = kg.generateKey();
		}catch(NoSuchAlgorithmException nsae){
			System.err.println(nsae);
		}
		return sk;
	}//generateKey
       
    /*! Calculate the HMAC. Java also implements this function 
     *"javax.crypto.Mac"  The disadvantage of the java implementation is that
     *it's impossible to pass the secret key as a byte-array.
     *\param key Secret key (64-bytes)
     *\param text Message to authenticate
     *\return the 20-byte (160-bit) HMAC
     */
    static byte[] selfHash(byte[] key, byte[] text){
		
		if(debug)showArray(key,"Key");
		
		byte[] dummy=new byte[key.length];
		byte[] dummy2=new byte[key.length];
		for(int i=0;i<key.length;i++)dummy[i]=(byte)(key[i]^0x36);
		
		if(debug)showArray(dummy,"Key XOR ipad");
		
		MessageDigest md=null;
		try{
			md=MessageDigest.getInstance("SHA");
		}catch(NoSuchAlgorithmException nsae){
			System.err.println(nsae);
		}
		md.update(dummy);
		md.update(text);
		dummy=md.digest();
		
		if(debug)showArray(dummy,"First message digest");
		md.reset();
		for(int i=0;i<key.length;i++)dummy2[i]=(byte)(key[i]^0x5C);
		
		if(debug)showArray(dummy2,"Key XOR opad");
		
		md.update(dummy2);
		md.update(dummy);
		
		byte[] result=md.digest();
		if(debug)showArray(result,"Final digest");
		return result;
	}//selfHash
	
	/*! Shows an array
	 *\param array Array to show
	 *\param name Name of the array to show
	 */
	static void showArray(byte[] array, String name){
		Kryptoknight.showArray(array,name);
	}//showArray

}//class