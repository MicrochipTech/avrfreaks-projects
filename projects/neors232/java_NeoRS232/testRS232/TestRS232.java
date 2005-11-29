import LieBTrau.comm.NeoRS232;
import java.awt.event.*;

class TestRS232 implements ActionListener{

	private NeoRS232 neoRS232;
	
	public static void main(String[] args){
		TestRS232 t1=new TestRS232("COM1",115200);
		//TestRS232 t2=new TestRS232("COM2");
		byte[] a={
			(byte)0x50,(byte)0x51,(byte)0x52,(byte)0x53,(byte)0xAA,
			(byte)0x54,(byte)0x55,(byte)0x56,(byte)0x57,(byte)0x10,
			(byte)0x58,(byte)0x59,(byte)0x5A,(byte)0x5B,(byte)0x30,
			(byte)0x5C,(byte)0x5D,(byte)0x5E,(byte)0x5F,(byte)0xFF,
			(byte)0x60,(byte)0x61,(byte)0x62,(byte)0x63,(byte)0x64
		};
		for(int i=0;i<a.length;i++){
			String s=Integer.toHexString(a[i]);
			if(s.length()>2)s=s.substring(s.length()-2);
			System.out.print(s+" ");
			if((i+1)%5==0)System.out.println();
		}
		t1.schrijf(a);
	}//main
	
	public TestRS232(String port, int baud){
		neoRS232=new NeoRS232(port, baud, true);
		neoRS232.addActionListener(this);
	}//constructor
	
	void schrijf(byte[] buffer){
		neoRS232.schrijfData(buffer);
	}//schrijf
	
	void close(){
		System.out.println("Calling for closing");
		neoRS232.sluitPoort();
	}//close
	
	public void actionPerformed(ActionEvent e){
		System.out.println("Data ready");
		byte[] buffer=neoRS232.getData();
		for(int i=0;i<buffer.length;i++)
			System.out.print(Integer.toHexString(buffer[i])+ " ");
		//ThreadGroup sys=Thread.currentThread().getThreadGroup();
		//sys.list();
	}//actionPerformed
	
}//class