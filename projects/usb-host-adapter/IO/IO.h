///////////////////////////////////////////////////////////
//  IO.h
//  Implementation of the Class IO
//  Created on:      16-Jun-2009 10:29:44
//  Original author: rockelp
///////////////////////////////////////////////////////////

#if !defined(EA_A42FAA51_9498_441e_8627_E3A4662FD3E4__INCLUDED_)
#define EA_A42FAA51_9498_441e_8627_E3A4662FD3E4__INCLUDED_

#include "Request.h"

class IO
{

public:
	enum StateType
	{
		SearchExternalMedia_ReadSerialPorts,
		SearchExternalMedia_Final,
		SearchExternalMedia_ReadUSBPorts,
		ST_NOSTATE
	};

	class USBDevice
	{

	public:
		USBDevice();
		virtual ~USBDevice();
		void CLEAR_FEATURE();
		unsigned char GET_CONFIGURATION(int iNoOfBytes);
		unsigned char GET_DESCRIPTOR(int iNoOfBytes);
		void GET_STATUS();
		void RESET();
		void SET_ADDRESS(double dAddress);
		void SET_CONFIGURATION(unsigned long uDescriptor);
		void SET_DESCRIPTOR(unsigned long uDescriptor);
		void SET_FEATURE(int iFeatureNo);

	};

	enum CommandType
	{
		Do,
		Entry,
		Exit
	};

	IO();
	virtual ~IO();
	void SetLastError(int iErrNo);
	void SetNoOfBytesReceived(int iVal);

private:
	bool bDataRead;
	bool bValid;
	StateType currState;
	TransitionType currTransition;
	int iBytesReceived;
	StateType nextState;
	StateType SearchExternalMedia_history;
	Request selection;
	bool transcend;

	void createPort(string sFileName);
	void doReadSerialPort();
	void doReadUSB();
	void initializeStateMachine();
	void readPort(int iNoOfBytes);
	void runStateMachine();
	void searchExternalMedia_ReadSerialPorts(CommandType command);
	void searchExternalMedia_ReadUSBPorts(CommandType command);
	void setupUSB(bool bValidDevice);
	void StatesProc(StateType currState, CommandType command);
	void TransitionsProc(TransitionType transition);

	/* Begin - EA generated code for Activities and Interactions */

	void createPort(string sFileName);

	void doReadSerialPort();

	void readPort(int iNoOfBytes);

	void setupUSB(bool bValidDevice);

	void doReadUSB();


	/* End - EA generated code for Activities and Interactions */

	/* Begin - EA generated code for StateMachine */


	enum StateType
	{
		SearchExternalMedia_ReadSerialPorts,
		SearchExternalMedia_Final,
		SearchExternalMedia_ReadUSBPorts,
		ST_NOSTATE
	};
	enum TransitionType
	{
		TT_NOTRANSITION
	};
	enum CommandType
	{
		Do, 
		Entry, 
		Exit 
	};
private:
	StateType currState;
	StateType nextState;
	TransitionType currTransition;
	bool transcend;
	StateType SearchExternalMedia_history;
	void searchExternalMedia_ReadSerialPorts(CommandType command);

	void searchExternalMedia_ReadUSBPorts(CommandType command);
	void StatesProc(StateType currState, CommandType command);
	void TransitionsProc(TransitionType transition);
	void initializeStateMachine();
	void runStateMachine();


	/* End - EA generated code for StateMachine */

};
#endif // !defined(EA_A42FAA51_9498_441e_8627_E3A4662FD3E4__INCLUDED_)
