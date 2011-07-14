CMD_LINE_WC		- command line library
DLE_CMD			- build folder	
DLE_CMD_LIB		- device commands library 
DLE_CMD_PROC		- User Interface
DLE_IO_PROC		- Device Interface

Start up:
Startup USER_PACK\SW\DLE_CMD\DLE_CMD_PROC.exe,
IO proc (DLE_IO_PROC.exe) should start up in another new console window as well. 
Connect the device to USB port, wait until device detected by IO_proc

For measure start types the following in command line of user interface window:
scan_start -MEAS_APD 0x9030 -CALIB_APD 0x9010
or use command from the history list using UP/down arrows