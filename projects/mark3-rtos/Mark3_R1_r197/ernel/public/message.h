/*===========================================================================
     _____        _____        _____        _____
 ___|    _|__  __|_    |__  __|__   |__  __| __  |__  ______
|    \  /  | ||    \      ||     |     ||  |/ /     ||___   |
|     \/   | ||     \     ||     \     ||     \     ||___   |
|__/\__/|__|_||__|\__\  __||__|\__\  __||__|\__\  __||______|
    |_____|      |_____|      |_____|      |_____|

--[Mark3 Realtime Platform]--------------------------------------------------

Copyright (c) 2012-2015 Funkenstein Software Consulting, all rights reserved.
See license.txt for more information
===========================================================================*/
/*!

    \file   message.h    

    \brief  Inter-thread communication via message-passing
    
    Embedded systems guru Jack Ganssle once said that without a robust form of
    interprocess communications (IPC), an RTOS is just a toy.  Mark3 implements
    a form of IPC to provide safe and flexible messaging between threads.

    Using kernel-managed IPC offers significant benefits over other forms of
    data sharing (i.e. Global variables) in that it avoids synchronization
    issues and race conditions common to the practice.  Using IPC also enforces
    a more disciplined coding style that keeps threads decoupled from one
    another and minimizes global data, preventing careless and hard-to-debug
    errors.

    \section MBCreate Using Messages, Queues, and the Global Message Pool 

    \code
    
        // Declare a message queue shared between two threads
        MessageQueue my_queue;
        
        int main()
        {
            ...
            // Initialize the message queue
            my_queue.init();
            ...
        }
        
        void Thread1()
        {
            // Example TX thread - sends a message every 10ms
            while(1)
            {
                // Grab a message from the global message pool
                Message *tx_message = GlobalMessagePool::Pop();
                
                // Set the message data/parameters
                tx_message->SetCode( 1234 );
                tx_message->SetData( NULL );
                
                // Send the message on the queue.
                my_queue.Send( tx_message );
                Thread::Sleep(10);
            }
        }
        
        void Thread2()
        {
            while()
            {
                // Blocking receive - wait until we have messages to process
                Message *rx_message = my_queue.Recv();
                
                // Do something with the message data...
                
                // Return back into the pool when done
                GlobalMessagePool::Push(rx_message);                
            }
        }
    \endcode
*/

#ifndef __MESSAGE_H__
#define __MESSAGE_H__

#include "kerneltypes.h"
#include "mark3cfg.h"

#include "ll.h"
#include "ksemaphore.h"

#if KERNEL_USE_MESSAGE

#if KERNEL_USE_TIMEOUTS
	#include "timerlist.h"
#endif

//---------------------------------------------------------------------------
/*!
    Class to provide message-based IPC services in the kernel.
*/
class Message : public LinkListNode
{
public:
    /*!
        \fn void Init();
        
        Initialize the data and code in the message.
    */
    void Init() { ClearNode(); m_pvData = NULL; m_usCode = 0; }
    
    /*!
        \fn void SetData( void *pvData_ )
        
        Set the data pointer for the message before transmission.
        
        \param pvData_ Pointer to the data object to send in the message
    */
	void SetData( void *pvData_ ) { m_pvData = pvData_; }
    
    /*!
        \fn void *GetData()
        
        Get the data pointer stored in the message upon receipt
        
        \return Pointer to the data set in the message object
    */
	void *GetData() { return m_pvData; }
	
    /*!
        \fn SetCode( K_USHORT usCode_ )
        
        Set the code in the message before transmission
        
        \param usCode_ Data code to set in the object
    */
    void SetCode( K_USHORT usCode_ ) { m_usCode = usCode_; }
	
    /*!
        \fn K_USHORT GetCode()
        
        Return the code set in the message upon receipt
        
        \return User code set in the object
    */
    K_USHORT GetCode() { return m_usCode; }
private:

    //! Pointer to the message data
	void *m_pvData;
    
    //! Message code, providing context for the message
	K_USHORT m_usCode;
};

//---------------------------------------------------------------------------
/*!
    Implements a list of message objects shared between all threads.
*/
class GlobalMessagePool
{
public:
    /*!
        \fn void Init()
        
        Initialize the message queue prior to use
    */
	static void Init();	
    
    /*!
        \fn void Push( Message *pclMessage_ )
        
        Return a previously-claimed message object back to the global queue.
        Used once the message has been processed by a receiver.
        
        \param pclMessage_ Pointer to the Message object to return back to 
               the global queue
    */
	static void Push( Message *pclMessage_ );
    
    /*!
        \fn Message *Pop()
        
        Pop a message from the global queue, returning it to the user to be 
        populated before sending by a transmitter.
        
        \return Pointer to a Message object
    */
	static Message *Pop();

private:
    //! Array of message objects that make up the message pool
    static Message m_aclMessagePool[GLOBAL_MESSAGE_POOL_SIZE];
    
    //! Linked list used to manage the Message objects
	static DoubleLinkList m_clList;
};

//---------------------------------------------------------------------------
/*!
    List of messages, used as the channel for sending and receiving messages
    between threads.
*/
class MessageQueue
{
public:
    /*!
        \fn void Init()
        
        Initialize the message queue prior to use.
    */
	void Init();		
    
    /*!        
        \fn Message *Receive()
        
        Receive a message from the message queue.  If the message queue
        is empty, the thread will block until a message is available.
        
        \return Pointer to a message object at the head of the queue
    */
	Message *Receive();
    
#if KERNEL_USE_TIMEOUTS
	/*!        
        \fn Message *Receive( K_ULONG ulWaitTimeMS_ )
        
        Receive a message from the message queue.  If the message queue
        is empty, the thread will block until a message is available for
		the duration specified.  If no message arrives within that 
		duration, the call will return with NULL.
        
		\param ulWaitTimeMS_ The amount of time in ms to wait for a 
				message before timing out and unblocking the waiting thread.
		
        \return Pointer to a message object at the head of the queue or
				NULL on timeout.
    */
	Message *Receive( K_ULONG ulTimeWaitMS_ );
#endif	
	
    /*!
        \fn void Send( Message *pclSrc_ )
        
        Send a message object into this message queue.  Will un-block the 
        first waiting thread blocked on this queue if that occurs.
        
        \param pclSrc_ Pointer to the message object to add to the queue
    */
	void Send( Message *pclSrc_ );
    
	
	/*!
		\fn K_USHORT GetCount()
		
		Return the number of messages pending in the "receive" queue.
		
		\return Count of pending messages in the queue.
	*/
	K_USHORT GetCount();
private:

#if KERNEL_USE_TIMEOUTS
    Message *Receive_i( K_ULONG ulTimeWaitMS_ );
#else
    Message *Receive_i( void );
#endif

    //! Counting semaphore used to manage thread blocking
	Semaphore m_clSemaphore;
    
    //! List object used to store messages
	DoubleLinkList m_clLinkList;
};

#endif //KERNEL_USE_MESSAGE

#endif
