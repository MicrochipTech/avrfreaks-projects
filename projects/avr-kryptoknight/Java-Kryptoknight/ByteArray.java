import java.util.ArrayList;

/*! A helper class that makes it possible to have an array with variable
 *length.
 */
public class ByteArray{
	private ArrayList a;//!< Array containing the bytes
	private boolean debug=true;//!< Set to true to see more run-time messages
	
	/*!Constructor.  Makes a new ByteArray-object with an empty array.
	 */
	ByteArray(){
		a=new ArrayList();
	}//constructor
	
	/*! Method to add an array to the current object.
	 *\param b Array to add to the ByteArray object
	 */
	void add(byte[] b){
		if(b==null && debug==true)
			System.err.println("ByteArray.add(): Argument is null");
		for(int i=0;i<b.length;i++)a.add(new Byte(b[i]));
	}//add
	
	/*! Method that converts the array of this object to an array of bytes
	 *\return the array of bytes.
	 */
	byte[] toByte(){
		byte[] c=new byte[a.size()];
		for(int i=0;i<c.length;i++)c[i]=((Byte)(a.get(i))).byteValue();
		return c;
	}//toByte
	
}//class