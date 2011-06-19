package com.yarin.android.Examples_08_09;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.UUID;

import android.app.Activity;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothSocket;
import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;
import android.util.Log;
import android.view.WindowManager;
import android.widget.Toast;

public class ClientSocketActivity  extends Activity
{
	private static final String TAG = ClientSocketActivity.class.getSimpleName();
	private static final int REQUEST_DISCOVERY = 0x1;;
	private Handler _handler = new Handler();
	private BluetoothAdapter _bluetooth = BluetoothAdapter.getDefaultAdapter();
	
	private BluetoothSocket socket = null;
	
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		getWindow().setFlags(WindowManager.LayoutParams.FLAG_BLUR_BEHIND,
		WindowManager.LayoutParams.FLAG_BLUR_BEHIND);
		setContentView(R.layout.client_socket);
		if (!_bluetooth.isEnabled()) {
			finish();
			return;
		}
		Intent intent = new Intent(this, DiscoveryActivity.class);
		
		/* Prompted to select a server to connect */
		Toast.makeText(this, "select device to connect", Toast.LENGTH_SHORT).show();
		
		/* Select device for list */
		startActivityForResult(intent, REQUEST_DISCOVERY);
	}
	
	/* after select, connect to device */
	protected void onActivityResult(int requestCode, int resultCode, Intent data) {
		if (requestCode != REQUEST_DISCOVERY) {
			return;
		}
		if (resultCode != RESULT_OK) {
			return;
		}
		final BluetoothDevice device = data.getParcelableExtra(BluetoothDevice.EXTRA_DEVICE);
		new Thread() {
			public void run() {
				connect(device);
			};
		}.start();
	}
	
	
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
			
//			new Thread() {
//				public void run() {
//						while(true)
//						{	
//						try {
//							Log.d("EF-BTBee", ">>Send data thread!");
//							OutputStream outputStream = socket.getOutputStream();
//							outputStream.write(new byte[] { (byte) 0xa2, 0, 7, 16, 0, 4, 0 });
//						} catch (IOException e) {
//							Log.e("EF-BTBee", "", e);
//						}
//						}
//				};
//			}.start();
			
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
		}
	}
}

