package com.yarin.android.Examples_08_09;

import java.util.ArrayList;
import java.util.List;

import android.app.ListActivity;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.DialogInterface.OnDismissListener;
import android.os.Bundle;
import android.os.Handler;
import android.util.Log;
import android.view.View;
import android.view.WindowManager;
import android.widget.ArrayAdapter;
import android.widget.ListView;

public class DiscoveryActivity  extends ListActivity
{
	private Handler _handler = new Handler();
	/* Get Default Adapter */
	private BluetoothAdapter _bluetooth = BluetoothAdapter.getDefaultAdapter();
	/* Storage the BT devices */
	private List<BluetoothDevice> _devices = new ArrayList<BluetoothDevice>();
	/* Discovery is Finished */
	private volatile boolean _discoveryFinished;
	
	private Runnable _discoveryWorkder = new Runnable() {
		public void run() 
		{
			/* Start search device */
			_bluetooth.startDiscovery();
			Log.d("EF-BTBee", ">>Starting Discovery");
			for (;;) 
			{
				if (_discoveryFinished) 
				{
					Log.d("EF-BTBee", ">>Finished");
					break;
				}
				try 
				{
					Thread.sleep(100);
				} 
				catch (InterruptedException e){}
			}
		}
	};
	
	/**
	 * Receiver
	 * When the discovery finished be called.
	 */
	private BroadcastReceiver _foundReceiver = new BroadcastReceiver() {
		public void onReceive(Context context, Intent intent) {
			/* get the search results */
			BluetoothDevice device = intent.getParcelableExtra(BluetoothDevice.EXTRA_DEVICE);
			/* add to list */
			_devices.add(device);
			/* show the devices list */
			showDevices();
		}
	};
	private BroadcastReceiver _discoveryReceiver = new BroadcastReceiver() {

		@Override
		public void onReceive(Context context, Intent intent)  
		{
			/* unRegister Receiver */
			Log.d("EF-BTBee", ">>unregisterReceiver");
			unregisterReceiver(_foundReceiver);
			unregisterReceiver(this);
			_discoveryFinished = true;
		}
	};
	
	protected void onCreate(Bundle savedInstanceState)
	{
		super.onCreate(savedInstanceState);
		getWindow().setFlags(WindowManager.LayoutParams.FLAG_BLUR_BEHIND, WindowManager.LayoutParams.FLAG_BLUR_BEHIND);
		setContentView(R.layout.discovery);
		
		/* BT isEnable */
		if (!_bluetooth.isEnabled())
		{
			Log.w("EF-BTBee", ">>BTBee is disable!");
			finish();
			return;
		}
		/* Register Receiver*/
		IntentFilter discoveryFilter = new IntentFilter(BluetoothAdapter.ACTION_DISCOVERY_FINISHED);
		registerReceiver(_discoveryReceiver, discoveryFilter);
		IntentFilter foundFilter = new IntentFilter(BluetoothDevice.ACTION_FOUND);
		registerReceiver(_foundReceiver, foundFilter);
		
		/* show a dialog "Scanning..." */
		SamplesUtils.indeterminate(DiscoveryActivity.this, _handler, "Scanning...", _discoveryWorkder, new OnDismissListener() {
			public void onDismiss(DialogInterface dialog)
			{

				for (; _bluetooth.isDiscovering();)
				{

					_bluetooth.cancelDiscovery();
				}

				_discoveryFinished = true;
			}
		}, true);
	}

	/* Show devices list */
	protected void showDevices()
	{
		List<String> list = new ArrayList<String>();
		for (int i = 0, size = _devices.size(); i < size; ++i)
		{
			StringBuilder b = new StringBuilder();
			BluetoothDevice d = _devices.get(i);
			b.append(d.getAddress());
			b.append('\n');
			b.append(d.getName());
			String s = b.toString();
			list.add(s);
		}
		Log.d("EF-BTBee", ">>showDevices");
		final ArrayAdapter<String> adapter = new ArrayAdapter<String>(this, android.R.layout.simple_list_item_1, list);
		_handler.post(new Runnable() {
			public void run()
			{
				setListAdapter(adapter);
			}
		});
	}
	
	/* Select device */
	protected void onListItemClick(ListView l, View v, int position, long id)
	{
		Log.d("EF-BTBee", ">>Click device");
		Intent result = new Intent();
		result.putExtra(BluetoothDevice.EXTRA_DEVICE, _devices.get(position));
		setResult(RESULT_OK, result);
		finish();
	}
}

