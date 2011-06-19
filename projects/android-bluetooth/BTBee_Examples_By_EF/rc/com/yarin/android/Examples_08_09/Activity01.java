package com.yarin.android.Examples_08_09;

import android.app.Activity;
import android.bluetooth.BluetoothAdapter;
import android.content.Intent;
import android.os.Bundle;
import android.view.View;

public class Activity01 extends Activity
{
	/* Get Default Adapter */
	private BluetoothAdapter	_bluetooth				= BluetoothAdapter.getDefaultAdapter();

	/* request BT enable */
	private static final int	REQUEST_ENABLE			= 0x1;
	/* request BT discover */
	private static final int	REQUEST_DISCOVERABLE	= 0x2;


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
		//Intent enabler = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);
		//startActivityForResult(enabler, REQUEST_ENABLE);
		
		//enable
		_bluetooth.enable();
		
		Intent enabler = new Intent(BluetoothAdapter.ACTION_REQUEST_DISCOVERABLE);
		startActivityForResult(enabler, REQUEST_DISCOVERABLE);
	}


	/* Close BT */
	public void onDisableButtonClicked(View view)
	{
		_bluetooth.disable();
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
