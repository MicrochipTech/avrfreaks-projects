# Android + BlueTooth

Uploaded by leolegend on 2011-06-19 14:02:00 (rating 0 out of 5)

## Summary

We all know Android is very popular now, especially ADK (Android Open Accessory Development Kit), which allows external 0pen source hardware to connect with Android system by USB and interact with an Android-powered device in a special "accessory" mode.
We keep working on it for weeks, and try to assemble some bata prototype and make a library for the communication between Android and Arduino by bluetooth.
With the Google providing source files, we are able to provide this board from now on. We produce a batch, you can purchase them **[here](http://www.elecfreaks.com/store/bare-boardgoogle-android-open-accessory-toolki-adk-p-169.html)**.We will provide the Bata version as soon as possible.
To make the communication between Android and Arduino easy, we would like show you a new way that android interact with Arduino and other similar boards. Bluetooth for example.
Step one: Make a APP to Android which could communicate with other devices by bluetooth.
Step two: Android APP connect to Arduino by **[Bluetooth Bee](http://www.elecfreaks.com/store/bluetooth-bee-masterslave-p-171.html)**.
For step one(Part1), we have just made a little APP for Android, achieve simple bluetooth connection with Android.

This APP allows Android connect to each other by bluetooth, so you need at least two Android devices for this demo. Here we used two Android phones which are SAMSUNG I7680 and Lenovo O3. Then, there should be a complete development environment like Eclipse on your PC. You can find many of them in the Internet.
I guess you all get ready, let's see more details.
Â·
**Bluetooth**
The Android platform includes support for the Bluetooth network stack, which allows a device to wirelessly exchange data with other Bluetooth devices. The application framework provides access to the Bluetooth functionality through the Android Bluetooth APIs. These APIs let applications connect wirelessly to other Bluetooth devices, enabling point-to-point and multipoint wireless features.
Â·
**The Basics**
All of the Bluetooth APIs are available in the Android bluetooth package. Here is a summary of the classes you will need to create as below. 
* BluetoothAdapter: Represents the local Bluetooth adapter (Bluetooth radio)
* BluetoothDevice: Represents a remote Bluetooth device, query information such as its name, address, class, and bonding state.
* BluetoothSocket: Represents the interface for a Bluetooth socket (similar to a TCP Socket).
* BluetoothServerSocket: Represents an open server socket that listens for incoming requests (similar to a TCP ServerSocket).
* BluetoothClass: Describes the general characteristics and capabilities of a Bluetooth device.

Â·
**Bluetooth Permissions**
In order to use Bluetooth features in your application, you need to declare at least one of two Bluetooth permissions: BLUETOOTH and BLUETOOTH\_ADMIN.
Declare the Bluetooth permission(s) in your application's AndroidManifest.xml as below.


```
[cce\_js]
<manifest ... >
  <uses-permission android:name="android.permission.BLUETOOTH" />
	<uses-permission android:name="android.permission.BLUETOOTH\_ADMIN" />
  ...
</manifest>
[/cce\_js]
```

Oh, Is it a little boring in front of this? OK, let us see some code, really start from here.
In order to connect by bluetooth, there should be four steps as below: 
* **Setting Up Bluetooth**
* **Finding Device**
* **Connecting Device**
* **Managing Device(server/client)**

Â·



**Setting Up Bluetooth**

![](http://www.elecfreaks.com/wp-content/uploads/2011/06/11.jpg "1")
We made the APP include six java files. For the Bata, we only need to use Activity01, DiscoveryActivity, ClientSocketActivity and ServerSocketActivity.
Before your application can communicate with Bluetooth, you should verify Bluetooth is supported on the device and make sure that it is enabled. If Bluetooth is not supported, you should gracefully disable any Bluetooth features. If Bluetooth is supported, but disabled, you are able to request the user enable Bluetooth without leaving your application.
So, at Activity01 we get five buttons, as below:
![](http://www.elecfreaks.com/wp-content/uploads/2011/06/21.jpg "2")
Select â€œOpen BTâ€ button make the device's own Bluetooth adapter (the Bluetooth radio) working. There is one Bluetooth adapter for the entire system, and your application can interact with it when it is open.


```
[cce\_js]private BluetoothAdapter \_bluetooth = BluetoothAdapter.getDefaultAdapter()[/cce\_js]
```

There are two ways to enable bluetooth:


```
[cce\_js]
    //Intent enabler = new Intent(BluetoothAdapter.ACTION\_REQUEST\_ENABLE);
		//startActivityForResult(enabler, REQUEST\_ENABLE);

		//enable
		\_bluetooth.enable();
[/cce\_js]
```

The frist way, create a new Intent as "Intent enabler = new Intent(BluetoothAdapter.ACTION\_REQUEST\_ENABLE)", a dialog box will appear and request user permission to enable Bluetooth. Select "Yes" and the system will enable Bluetooth and focus will return to your application once the process completes (or fails). "Yes" returns RESULT\_OK and "No" returns RESULT\_CANCELED.
![](http://www.elecfreaks.com/wp-content/uploads/2011/06/3.png "3")
The other way is force enable Bluetooth instead of dialog, we used this way here.
Next, you need to ensure that Bluetooth is enabled and allowed other devices could discover it. Add the below code, and a dialog box will appear also, you should click "Yes".
![](http://www.elecfreaks.com/wp-content/uploads/2011/06/4.png "4")


```
[cce\_js]
    Intent enabler = new Intent(BluetoothAdapter.ACTION\_REQUEST\_DISCOVERABLE);
		startActivityForResult(enabler, REQUEST\_DISCOVERABLE);
[/cce\_js]
```

OK, Setting up device is completed. The full code as below:


```
[cce\_js]
ackage com.yarin.android.Examples\_08\_09;

import android.app.Activity;
import android.bluetooth.BluetoothAdapter;
import android.content.Intent;
import android.os.Bundle;
import android.view.View;

public class Activity01 extends Activity
{
	/* Get Default Adapter */
	private BluetoothAdapter	\_bluetooth				= BluetoothAdapter.getDefaultAdapter();

	/* request BT enable */
	private static final int	REQUEST\_ENABLE			= 0x1;
	/* request BT discover */
	private static final int	REQUEST\_DISCOVERABLE	= 0x2;

	/** Called when the activity is first created. */
	@Override
	public void onCreate(Bundle savedInstanceState)
	{
		super.onCreate(savedInstanceState);
		setContentView(R.layout.main);
	}

	/* Enable BT */
	public void onEnableButtonClicked(View view)
	{
		//Intent enabler = new Intent(BluetoothAdapter.ACTION\_REQUEST\_ENABLE);
		//startActivityForResult(enabler, REQUEST\_ENABLE);

		//enable
		\_bluetooth.enable();

		Intent enabler = new Intent(BluetoothAdapter.ACTION\_REQUEST\_DISCOVERABLE);
		startActivityForResult(enabler, REQUEST\_DISCOVERABLE);
	}

	/* Close BT */
	public void onDisableButtonClicked(View view)
	{
		\_bluetooth.disable();
	}

	/* Start search */
	public void onStartDiscoveryButtonClicked(View view)
	{
		Intent enabler = new Intent(this, DiscoveryActivity.class);
		startActivity(enabler);
	}

	/* Client */
	public void onOpenClientSocketButtonClicked(View view)
	{

		Intent enabler = new Intent(this, ClientSocketActivity.class);
		startActivity(enabler);
	}

	/* Server */
	public void onOpenServerSocketButtonClicked(View view)
	{

		Intent enabler = new Intent(this, ServerSocketActivity.class);
		startActivity(enabler);
	}

}
[/cce\_js]
```

Â·
**Finding Device**
With Discovery Activity.java file, We will find around device. The process is asynchronous and the method will immediately return with a boolean indicating whether discovery has started successfully. The discovery process usually involves an inquiry scan of about 12 seconds, followed by a page scan of each found device to retrieve its Bluetooth name. Your application must register a BroadcastReceiver for the ACTION\_FOND. Intent in order to receive information about each device discovered.


```
[cce\_js]
/* Register Receiver*/
		IntentFilter discoveryFilter = new IntentFilter(BluetoothAdapter.ACTION\_DISCOVERY\_FINISHED);
		registerReceiver(\_discoveryReceiver, discoveryFilter);
		IntentFilter foundFilter = new IntentFilter(BluetoothDevice.ACTION\_FOUND);
		registerReceiver(\_foundReceiver, foundFilter);

/**
	 * Receiver
	 * When the discovery finished be called.
	 */
	private BroadcastReceiver \_foundReceiver = new BroadcastReceiver() {
		public void onReceive(Context context, Intent intent) {
			/* get the search results */
			BluetoothDevice device = intent.getParcelableExtra(BluetoothDevice.EXTRA\_DEVICE);
			/* add to list */
			\_devices.add(device);
			/* show the devices list */
			showDevices();
		}
	};
	private BroadcastReceiver \_discoveryReceiver = new BroadcastReceiver() {

		@Override
		public void onReceive(Context context, Intent intent)
		{
			/* unRegister Receiver */
			Log.d("EF-BTBee", ">>unregisterReceiver");
			unregisterReceiver(\_foundReceiver);
			unregisterReceiver(this);
			\_discoveryFinished = true;
		}
	};
[/cce\_js]
```
After discovery will return remote device's MAC address, device name and class. Using this information, the device performing discovery can then choose to initiate a connection with the discovered device. The following figure is click the "Search Device" button.



![](http://www.elecfreaks.com/wp-content/uploads/2011/06/51-261x300.jpg "5")
Â·
**Connecting Device and Managing Device**
In order to create a connection between your application and two devices, you must implement the both server-side and client-side mechanisms, because one device must open a server socket and the other one must initiate the connection (use the server device's MAC address to initiate a connection). So you must used a same UUID, I used one find from internet and you can get the UUID here: [the UUID Generator](http://www.uuidgenerator.com/).
One implementation technique is to automatically prepare each device as a server, so that each one has a server socket open and listening for connections. Then either device can initiate a connection with the other one as the client. Alternatively, one device can explicitly "host" the connection and open a server socket on demand and the other device can simply initiate the connection.
As Server


```
[cce\_js]
\_serverSocket = \_bluetooth.listenUsingRfcommWithServiceRecord(PROTOCOL\_SCHEME\_RFCOMM,
					UUID.fromString("a60f35f0-b93a-11de-8a39-08002009c666"));
[/cce\_js]
```

As Client


```
[cce\_js]socket = device.createRfcommSocketToServiceRecord(UUID.fromString("a60f35f0-b93a-11de-8a39-08002009c666"));[/cce\_js]
```

It must select a server and a client, we has made two buttons which are "AS Client" and "AS Server". Then one phone will be server and the other will be client.
**Server**
Frist, this connection used Socket way, so there are some usually functions such as connect, close, listen and so on. and there are others stream control such as getInputStream, getOutputStream ...
select "AS Server" button, then you will see the following show.
![](http://www.elecfreaks.com/wp-content/uploads/2011/06/61-261x300.jpg "6")
The sample code:


```
[cce\_js]
/* Client devices*/
			final List<String> lines = new ArrayList<String>();
			\_handler.post(new Runnable() {
				public void run() {
					lines.add("Rfcomm server started...");
					ArrayAdapter<String> adapter = new ArrayAdapter<String>(
							ServerSocketActivity.this,
							android.R.layout.simple\_list\_item\_1, lines);
					setListAdapter(adapter);
				}
			});
			/* accept client request */
			BluetoothSocket socket = \_serverSocket.accept();
			Log.d("EF-BTBee", ">>Accept Client Request");

			/* Processing the request content*/
			if (socket != null) {
				InputStream inputStream = socket.getInputStream();
				int read = -1;
				final byte[] bytes = new byte[2048];
				for (; (read = inputStream.read(bytes)) > -1;) {
					final int count = read;
					\_handler.post(new Runnable() {
						public void run() {
							StringBuilder b = new StringBuilder();
							for (int i = 0; i < count; ++i) {
								if (i > 0) {
									b.append(' ');
								}
								String s = Integer.toHexString(bytes[i] & 0xFF);
								if (s.length() < 2) {

									b.append('0');
								}
								b.append(s);
							}
							String s = b.toString();
							lines.add(s);
							ArrayAdapter<String> adapter = new ArrayAdapter<String>(
									ServerSocketActivity.this,
									android.R.layout.simple\_list\_item\_1, lines);
							setListAdapter(adapter);
						}
					});
				}
[/cce\_js]
```
**Client**


Click the "AS Client" button, it will discover remote devices and show the devices list. It will call DiscoveryActivity class and show the devices list at last, then click one device to connect to server, send socket.connect() to server and connect to RFCOMM Channel. We send a buffer to server as below.





```
[cce\_js]
protected void connect(BluetoothDevice device) {
		//BluetoothSocket socket = null;
		try {
			//Create a Socket connection: need the server's UUID number of registered
			socket = device.createRfcommSocketToServiceRecord(UUID.fromString("a60f35f0-b93a-11de-8a39-08002009c666"));

			socket.connect();
			Log.d("EF-BTBee", ">>Client connectted");

			InputStream inputStream = socket.getInputStream();
			OutputStream outputStream = socket.getOutputStream();
			outputStream.write(new byte[] { (byte) 0xa0, 0, 7, 16, 0, 4, 0 });

		} catch (IOException e) {
			Log.e("EF-BTBee", "", e);
		} finally {
			if (socket != null) {
				try {
					Log.d("EF-BTBee", ">>Client Close");
					socket.close();
					finish();
					return ;
				} catch (IOException e) {
					Log.e("EF-BTBee", "", e);
				}
			}
[/cce\_js]
```

Once the Server received the data, it will show on the screen.
![](http://www.elecfreaks.com/wp-content/uploads/2011/06/71-254x300.jpg "7")
At last, of course is release resources, click "Stop Server" and return to main menu, click "Close BT" button.


```
[cce\_js]
/* Stop server */
	private void shutdownServer() {
		new Thread() {
			public void run() {
				\_serverWorker.interrupt();
				if (\_serverSocket != null) {
					try {
						/* close sever */
						\_serverSocket.close();
					} catch (IOException e) {
						Log.e("EF-BTBee", "", e);
					}
					\_serverSocket = null;
				}
			};
		}.start();
	}
[/cce\_js]
```

```
[cce\_js]
/* Close BT */
	public void onDisableButtonClicked(View view)
	{
		\_bluetooth.disable();
	}
[/cce\_js]
```

OK, This is where the fun begins, you can connect the device and send data to each other. There are many things you can do, right?
Let us see this APP operational processes totally again:
Open BT (enable bluetooth and discoverable) -> AS Server(listen)/AS Clent(search devices and show devices list) -> Select Server from the devices list(client sends data and server receives) -> Show the data on the Server screen -> Stop Server -> Stop BT.
The demo is a Bata version, just a sample code for your reference. We will gradually enrich with it and you are appreciated to redistribute and/or modify it, and please share with us.
Â·
**Download full code here:**
[BTBee\_Examples\_By\_EF.zip](http://elecfreaks.com/store/download/datasheet/Bluetooth/BTBee_Examples_By_EF.zip)
Â·
**Reference**:
[UUID Generator](http://www.uuidgenerator.com/)
[Android Bluetooth Chat](http://developer.android.com/resources/samples/BluetoothChat/src/com/example/android/BluetoothChat/BluetoothChat.html)
[Android Bluetooth API](http://innovator.samsungmobile.com/cms/cnts/knowledge.detail.view.do?platformId=3&cntsId=4262&listReturnUrl=http%3A%2F%2Finnovator.samsungmobile.com%3A80%2Fcms%2Fcnts%2Fknowledge.list.do%3FplatformId%3D3%26cateId%3D3%26cntsId%3D%26imgType%3D%26searchText%3Dbluetooth%26sortType%3D0%26codeType%3DAll%26indexDirection%3D1%26indexType%3D1%26listLines%3D10%26tabNum%3D1&linkType=0)
[Android 3.1 Platform](http://developer.android.com/sdk/android-3.1-highlights.html)
[Wireless Application Programming with J2ME and Bluetooth](http://developers.sun.com/mobility/midp/articles/bluetooth1/)

## Tags

- Complete code
