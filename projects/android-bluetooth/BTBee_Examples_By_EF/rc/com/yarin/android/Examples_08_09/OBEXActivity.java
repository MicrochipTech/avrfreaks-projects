package com.yarin.android.Examples_08_09;

import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.Arrays;

import android.app.Activity;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothServerSocket;
import android.bluetooth.BluetoothSocket;
import android.content.Intent;
import android.database.Cursor;
import android.net.Uri;
import android.os.Bundle;
import android.os.Handler;
import android.provider.ContactsContract;
import android.util.Log;

public class OBEXActivity extends Activity
{
	private static final String		TAG						= "@MainActivity";
	private Handler					_handler				= new Handler();
	private BluetoothServerSocket	_server;
	private BluetoothSocket			_socket;
	private static final int		OBEX_CONNECT			= 0x80;
	private static final int		OBEX_DISCONNECT			= 0x81;
	private static final int		OBEX_PUT				= 0x02;
	private static final int		OBEX_PUT_END			= 0x82;
	private static final int		OBEX_RESPONSE_OK		= 0xa0;
	private static final int		OBEX_RESPONSE_CONTINUE	= 0x90;
	private static final int		BIT_MASK				= 0x000000ff;
	Thread t = new Thread() 
	{
		public void run()
		{
			try														
			{														
				_server = BluetoothAdapter.getDefaultAdapter().listenUsingRfcommWithServiceRecord("OBEX", null);														
				new Thread() 
				{														
					public void run()														
					{																			
						Log.d("@Rfcom", "begin close");													
						try													
						{												
							_socket.close();													
						}													
						catch (IOException e)														
						{														
							Log.e(TAG, "", e);													
						}											
						Log.d("@Rfcom", "end close");													
					};												
				}.start();														
				_socket = _server.accept();														
				reader.start();															
				Log.d(TAG, "shutdown thread");														
			}														
			catch (IOException e)														
			{														
				e.printStackTrace();													
			}													
		};											
	};

	Thread reader = new Thread() 
	{																										
		public void run()														
		{														
			try													
			{
				Log.d(TAG, "getting inputstream");														
				InputStream inputStream = _socket.getInputStream();														
				OutputStream outputStream = _socket.getOutputStream();														
				Log.d(TAG, "got inputstream");														
				int read = -1;												
				byte[] bytes = new byte[2048];
				ByteArrayOutputStream baos = new ByteArrayOutputStream(bytes.length);
				while ((read = inputStream.read(bytes)) != -1)
				{
					baos.write(bytes, 0, read);
					byte[] req = baos.toByteArray();
					int op = req[0] & BIT_MASK;
					Log.d(TAG, "read:" + Arrays.toString(req));
					Log.d(TAG, "op:" + Integer.toHexString(op));
					switch (op)
					{
						case OBEX_CONNECT:
							outputStream.write(new byte[] { (byte) OBEX_RESPONSE_OK, 0, 7, 16, 0, 4, 0 });
							break;
						case OBEX_DISCONNECT:
							outputStream.write(new byte[] { (byte) OBEX_RESPONSE_OK, 0, 3, 0 });
							break;

						case OBEX_PUT:
							outputStream.write(new byte[] { (byte) OBEX_RESPONSE_CONTINUE, 0, 3, 0 });
							break;
						case OBEX_PUT_END:

							outputStream.write(new byte[] { (byte) OBEX_RESPONSE_OK, 0, 3, 0 });
							break;

						default:
							outputStream.write(new byte[] { (byte) OBEX_RESPONSE_OK, 0, 3, 0 });
					}
					Log.d(TAG, new String(baos.toByteArray(), "utf-8"));
					baos = new ByteArrayOutputStream(bytes.length);
				}
				Log.d(TAG, new String(baos.toByteArray(), "utf-8"));
			}
			catch (IOException e)
			{
				e.printStackTrace();
			}
		};
	};

	private Thread put = new Thread() {
		public void run()														
		{													
		};												
	};
	public void onCreate(Bundle savedInstanceState)
	{
		super.onCreate(savedInstanceState);
		setContentView(R.layout.obex_server_socket);
		t.start();
	}
	protected void onActivityResult(int requestCode, int resultCode, Intent data)
	{
		Log.d(TAG, data.getData().toString());
		switch (requestCode)
		{
			case (1):
				if (resultCode == Activity.RESULT_OK)
				{
					Uri contactData = data.getData();
					Cursor c = managedQuery(contactData, null, null, null, null);
					for (; c.moveToNext();)
					{
						Log.d(TAG, "c1---------------------------------------");
						dump(c);
						Uri uri = Uri.withAppendedPath(data.getData(), ContactsContract.Contacts.Photo.CONTENT_DIRECTORY);
						Cursor c2 = managedQuery(uri, null, null, null, null);
						for (; c2.moveToNext();)
						{
							Log.d(TAG, "c2---------------------------------------");
							dump(c2);
						}
					}
				}
				break;
		}

	}
	private void dump(Cursor c)
	{
		for (int i = 0, size = c.getColumnCount(); i < size; ++i)
		{
			String col = c.getColumnName(i);
			String s = c.getString(i);
			Log.d(TAG, col + "=" + s);
		}
	}
}
