import java.security.*;

/*! A helper class to generate the nonces.  Nonces are cryptographically
 *strong random numbers so pseudorandom numbers are not suited.
 */
class Nonce{
	
	private SecureRandom sr=null;//!< Member holding the random number
	
	/*! Constructor initiating the random number generator
	 */
	Nonce(){
		try{
			sr=SecureRandom.getInstance("SHA1PRNG");
		}catch(NoSuchAlgorithmException nsae){
			System.err.println(nsae);
		}
	}//constructor
	
	/*! Method that generates a nonce of the given length.
	 *\param length Length of the nonce in bytes.
	 *\return an array of bytes containing the random number.
	 */
	byte[] generateNonce(int length){
    	byte bytes[] = new byte[length];
    	sr.nextBytes(bytes);
    	return bytes;
	}//generateNonce
 
}//class