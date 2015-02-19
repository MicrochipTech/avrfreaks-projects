// ***********************************************************************
// USBASP_tty.c
// USBASP_tty is a compainion program for the modified version of
//            Thomas Fischl's USBASP AVR Programmer http://www.fischl.de
// This program provides a simple serial terminal that accepts data from the
// target device through the programmers UART via the USB connection.
// The standard USBASP programmer must be modified in such a way that the
// RX and TX lines are exposed and availibale to the TX and RX lines of the
// target device. The USBASP programmer must also have the updated firmware
// included in this package. (USBASP+)
// ***********************************************************************
#define _WIN32_IE 0X0400
#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <memory.h>

#include <lusb0_usb.h>  /* this is libusb NOTE: May require files to be copied to LIB & INCLUDE folders*/
#include "opendevice.h" /* common code moved to separate module */

// ** The 2 files listed below are copies of the SAME files as in usbasp+\firmware folder **
#include "usbconfig.h"  /* device's VID/PID and names  */
#include "usbasp.h"  /* USB function call identifiers ** MODIFIED WITH CUSTOM REQUEST IDs*/

#include "resource.h"

#define TIMER_NO 100
#define OUTBUF_SIZE 3000
#define INBUF_SIZE (OUTBUF_SIZE / 2)
#define GET_ALL_ATTEMPTS 3
#define MAX_OPEN_ATTEMPTS 20

LRESULT CALLBACK WindowProcedure (HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK DialogSetBaudRateProc (HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK newTxDataOutProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

int GetPrivateProfileValue(LPCTSTR lpszSection, LPCTSTR lpszKey, int iDefault, LPCTSTR lpszFile);
void SetStatusMessage(HWND hWnd, LPSTR str);
void AddSystemMessage(HWND hWnd, LPSTR str, char bNewLine);
void AddCharToConsole(HWND hCtl, unsigned char c, char bLastChar);
void AddStringToConsole(HWND hCtl, LPSTR str, char bNewLine);
void AddTextToConsole(HWND hCtl, LPSTR str);
void ConvertNewlinesInBuf(LPSTR inBuf, LPSTR outBuf);
void ProcessBackspace(HWND hCtl);
void GetCursorRowCol(HWND hCtl, unsigned int *pRow, unsigned int *pCol);
void UpdateCursorDisplay(void);
int ProcessSendCommand(char *buf, int szBuf);

int OpenUsb(void);
int SubmitRequestCmd(usb_dev_handle *hUSB, unsigned char cmd);
int GetAvailableByteCount(usb_dev_handle *hUSB);
int SubmitGeneralCmd(usb_dev_handle *hUSB, unsigned char cmd);
int GetAllAvailableBytesFromDevice(usb_dev_handle *hUSB, unsigned char *buf, int size);
int SendBytesToDevice(usb_dev_handle *hUSB, unsigned char *buf, int size);

int ConvertASCIIBufferToHEX(char *buf);
int SendCustomString(char *name);

char szClassName[ ] = "USBASP_tty";  //Also set this as CLASS in resource (*.rc) file
char msg[200];
char sfName[80];
char dataBuf[100];
int  bPolling=1, bWarnBeforeExit=1;
int bEcho=0;
int err_no=0, timer_interval = 100, timerId, ctTimer0=0, ctTimer1=0;
int dataBufPtr=0, dataBufCode=0, dataBufIndex=0;
int slPos[3];


char *pTxBuf;
static HWND hWnd;
static HFONT hfCourierFont;
DWORD lCtBytes;
DWORD dwBaud;
unsigned char baudBuf[sizeof(DWORD)+1];
unsigned int cursor_row, cursor_col;

HINSTANCE hInst;
HBRUSH brush;
HMENU hResMenu;
HMENU hMenu;

DWORD dwTxDataOutLen=0;
HWND hTxDataOut;         // Global Handle for TXT_DATA Edit Control
WNDPROC  txDataOutProc;  // Original TxDataOut Window Procedure

// ********** USB Variables *******************************
 usb_dev_handle      *handle = NULL;
 unsigned char rawVid[2] = {USB_CFG_VENDOR_ID}, rawPid[2] = {USB_CFG_DEVICE_ID};
 char          vendor[40] = {USB_CFG_VENDOR_NAME, 0}, product[40] = {USB_CFG_DEVICE_NAME, 0};
 char          buffer[4];
 unsigned char txtBuf[INBUF_SIZE+2];
 int           cnt, vid, pid, x, usb_open_fail_count=0;
 unsigned char cmd=0, usbBusy=0, gen_cmds[3]= {0,0,0};
// ********************************************************
 unsigned char workBuf[OUTBUF_SIZE];

int WINAPI WinMain (HINSTANCE hThisInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR lpszArgument,
                     int nCmdShow)
{
    HWND hwnd;               /* This is the handle for our window */
    MSG messages;            /* Here messages to the application are saved */
    WNDCLASSEX wincl;        /* Data structure for the windowclass */
    LOGFONT LogFont;         // Font Info


    /* The Window structure */
    wincl.hInstance = hThisInstance;
    wincl.lpszClassName = szClassName;
    wincl.lpfnWndProc = WindowProcedure;      /* This function is called by windows */
    wincl.style = CS_DBLCLKS;                 /* Catch double-clicks */
    wincl.cbSize = sizeof (WNDCLASSEX);

    /* Use default icon and mouse-pointer */
    wincl.hIcon = LoadIcon (hThisInstance, (LPCTSTR)IDI_ICON1);
    wincl.hIconSm = LoadIcon (hThisInstance, (LPCTSTR)IDI_ICON1);
    wincl.hCursor = LoadCursor (NULL, IDC_ARROW);
    wincl.lpszMenuName = (LPSTR)MAKEINTRESOURCE(IDR_MENU1);  /* menu from resource file*/
    wincl.cbClsExtra = 0;                      /* No extra bytes after the window class */
    wincl.cbWndExtra = DLGWINDOWEXTRA;         /* Reserve dialog box extra data. */
    /* Use Windows's default colour as the background of the window */
    wincl.hbrBackground = GetSysColorBrush(COLOR_BTNFACE);


    /* Register the window class, and if it fails quit the program */
    if (!RegisterClassEx (&wincl))
      {
       MessageBox(NULL, "RegisterClassEx FAILED!", "Error", MB_OK);
       return -1;  //Just exit if this fails
      }

    // Create a main window for this application instance.
    // We will use the DialogBox resource created in the resource editor
    // as the template for the main window.
    hwnd = CreateDialog(hThisInstance,   /* handle of application instance	*/
                    (LPSTR)MAKEINTRESOURCE(DLG_MAIN),   /* Dialog ID */
                    (HWND)NULL,                            /* No Owner  */
                    (DLGPROC)(FARPROC)WindowProcedure);    /* Window Proc. */

   hWnd = hwnd; //Set a global copy of hwnd

   if(!hwnd)
     {
      MessageBox(NULL, "CreateDialog FAILED!", "Error", MB_OK);
      return -2;  //Just exit if this fails
     }

    hInst = hThisInstance;

	//** Create  the Courier font and set it as the selected window font for **
	//** the status message windows **
	memset(&LogFont,0,sizeof(LogFont));
	LogFont.lfStrikeOut = 0;
	LogFont.lfUnderline = 0;
	LogFont.lfHeight = 15;
	LogFont.lfEscapement = 0;
	LogFont.lfItalic = FALSE;
	LogFont.lfWeight = FW_NORMAL;
    strcpy((char *)&LogFont.lfFaceName, (LPSTR)"Courier New");

	hfCourierFont = CreateFontIndirect(&LogFont);
	hTxDataOut = GetDlgItem(hWnd, TXT_DATAOUT);
    txDataOutProc =(WNDPROC)GetWindowLong(hTxDataOut, GWL_WNDPROC);//get the default procedure
    SetWindowLong(hTxDataOut, GWL_WNDPROC, (LONG_PTR)newTxDataOutProc);//set your custom procedure :)
    SetFocus(hTxDataOut);

	SendDlgItemMessage(hwnd, TXT_DATAOUT, WM_SETFONT, (WPARAM)hfCourierFont, 0L);



    /* Make the window visible on the screen */
   ShowWindow (hwnd, nCmdShow);

   // ***************************************************************************
   // Get Configuration Settings from {app-name}.ini in current working directory
   // ***************************************************************************
   GetCurrentDirectory(79, sfName);
   strcat(sfName, "\\");
   strcat(sfName, szClassName);
   strcat(sfName, ".ini");

   // Append configuration name (if any) to the title bar text
   GetPrivateProfileString("OPTIONS", "CFGNAME", "", (char *)txtBuf, OUTBUF_SIZE, sfName);
   if(strlen((char *)txtBuf)>0)
     {
       GetWindowText(hWnd, msg, sizeof(msg));
       strcat(msg, (char *)txtBuf);
       SetWindowText(hWnd,msg);
     }

   // Get USB Device ID value from .ini file ** DEFAULTS to values from usbconfig.h
   GetPrivateProfileString("USB", "DEVICE_NAME", product, product, 40, sfName);
   GetPrivateProfileString("USB", "VENDOR_NAME", vendor, vendor, 40, sfName);

   rawVid[0] = (DWORD)GetPrivateProfileValue("USB", "VENDOR_ID0", rawVid[0], sfName);
   rawVid[1] = (DWORD)GetPrivateProfileValue("USB", "VENDOR_ID1", rawVid[1], sfName);

   rawPid[0] = (DWORD)GetPrivateProfileValue("USB", "DEVICE_ID0", rawPid[0], sfName);
   rawPid[1] = (DWORD)GetPrivateProfileValue("USB", "DEVICE_ID1", rawPid[1], sfName);

   bWarnBeforeExit = (DWORD)GetPrivateProfileInt("OPTIONS", "WARN_BEFORE_EXIT", 1, sfName);

   timer_interval = GetPrivateProfileInt("OPTIONS", "TIMER_INTERVAL", 100, sfName);

   // Send WM_TIMER message every TIMER_INTERVAL milliseconds
   timerId = SetTimer(hwnd, TIMER_NO, timer_interval, (TIMERPROC)NULL);

   bPolling = (DWORD)GetPrivateProfileInt("OPTIONS", "POLLING", 1, sfName);
   if(bPolling)
     {
      CheckMenuItem(GetMenu(hwnd), IDM_POLLING, MF_BYCOMMAND | MF_CHECKED );
     }
   else
     {
      CheckMenuItem(GetMenu(hwnd), IDM_POLLING, MF_BYCOMMAND | MF_UNCHECKED );
      CheckDlgButton(hwnd, RB_POLLING, 0);
     }

   hResMenu = GetMenu(hWnd);
   hMenu = GetSubMenu(hResMenu,1);
   gen_cmds[0] = GetPrivateProfileInt("OPTIONS", "USB_CMD1", 0, sfName);
   if(gen_cmds[0]==0)
      EnableMenuItem(hMenu, IDM_SEND_CMD1, MF_BYCOMMAND | MF_GRAYED);

   gen_cmds[1] = GetPrivateProfileInt("OPTIONS", "USB_CMD2", 0, sfName);
   if(gen_cmds[1]==0)
      EnableMenuItem(hMenu, IDM_SEND_CMD2, MF_BYCOMMAND | MF_GRAYED);

   gen_cmds[2] = GetPrivateProfileInt("OPTIONS", "USB_CMD3", 0, sfName);
   if(gen_cmds[2]==0)
      EnableMenuItem(hMenu, IDM_SEND_CMD3, MF_BYCOMMAND | MF_GRAYED);

   GetPrivateProfileString("OPTIONS", "STR1_VAL", "", (char *)txtBuf, OUTBUF_SIZE, sfName);
   if(strlen((char *)txtBuf)==0)
      EnableMenuItem(hMenu, IDM_SEND_STR1, MF_BYCOMMAND | MF_GRAYED);

   GetPrivateProfileString("OPTIONS", "STR2_VAL", "", (char *)txtBuf, OUTBUF_SIZE, sfName);
   if(strlen((char *)txtBuf)==0)
      EnableMenuItem(hMenu, IDM_SEND_STR2, MF_BYCOMMAND | MF_GRAYED);

   GetPrivateProfileString("OPTIONS", "STR3_VAL", "", (char *)txtBuf, OUTBUF_SIZE, sfName);
   if(strlen((char *)txtBuf)==0)
     EnableMenuItem(hMenu, IDM_SEND_STR3, MF_BYCOMMAND | MF_GRAYED);

   // ************** USB SETUP **********************************************
   usb_init();
   usbBusy=0;

   /* compute VID/PID from usbconfig.h or .ini file settings */
   vid = rawVid[1] * 256 + rawVid[0];
   pid = rawPid[1] * 256 + rawPid[0];

   if(OpenUsb())
     {
      sprintf(msg, "Opened USB device \"%s\" with vid=0x%x pid=0x%x\n", product, vid, pid);
      SetStatusMessage(hwnd, (LPSTR)msg);
      usb_close(handle);
      handle=NULL;
     }
   else
    {
     //Open Device FAILED - DO NOT Start Polling
     CheckMenuItem(GetMenu(hwnd), IDM_POLLING, MF_BYCOMMAND | MF_UNCHECKED );
     bPolling=0;
     CheckDlgButton(hwnd, RB_POLLING, 0);
    }

   // ***********************************************************************
   // ****************** Main Window Message Loop ***************************
   // ***********************************************************************

    /* Run the message loop. It will run until GetMessage() returns 0 */
    while (GetMessage (&messages, NULL, 0, 0))
    {

     if(messages.hwnd==hTxDataOut)  // Need to trap WM_KEY messages from dialog controls here
       {
        if((messages.message == WM_CHAR))
          {
           SendMessage(hwnd, messages.message, messages.wParam, messages.lParam);
          }
       }

     if(!IsDialogMessage(hwnd, &messages))
        {
         /* Translate virtual-key messages into character messages */
         TranslateMessage(&messages);
         /* Send message to WindowProcedure */
         DispatchMessage(&messages);
        }
    }

    /* The program return-value is 0 - The value that PostQuitMessage() gave */
    return messages.wParam;
}

//**************************************************************************
// Subclassed window procedure for TXT_DATAOUT Edit Control
//
//**************************************************************************
LRESULT CALLBACK newTxDataOutProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
 int szTxt;
 HWND hTextBuf;
 char *pCBData, bDoPosUpdate=0, bDoDefaultProcess=1;
 unsigned char keystr[5];

 switch(msg)
    {
        case WM_KEYDOWN:
            {
             switch (wParam)
                {
                 case VK_F1:  // F1= Clear Screen
                     SetWindowText(hwnd, (LPSTR)"");
                     dwTxDataOutLen=0;
                     AddCharToConsole(hwnd, '\r', 0);
                     bDoPosUpdate=1;
                     bDoDefaultProcess=0;
                 break;
                 case VK_DELETE:  // Translate Delete Key to BS
                     SendMessage(hwnd, WM_CHAR, VK_BACK, (LPARAM)NULL);
                 break;
                }
            }
        break;
        case WM_CHAR:  // Process characters typed into Edit Control TXT_DATAOUT
            keystr[0] = (unsigned char)wParam;
            keystr[1] =0;

            if(bEcho)
              {
               SendMessage(hwnd, EM_SETSEL, -1, 0); // Position Cursor to End of Text
               AddCharToConsole(hwnd, (unsigned char) wParam, 1);
               bDoPosUpdate=1;
              }

            ProcessSendCommand((char *)keystr, 1);

            if(wParam == VK_RETURN) //Do Newline
              {
               keystr[0]=0;
               ProcessSendCommand((char *)keystr, 0);
              }
        break;

        case WM_PASTE:
          if(OpenClipboard(hwnd))
            {
             hTextBuf=GetClipboardData(CF_TEXT);
             if(hTextBuf)
               {
                pCBData=(char*)GlobalLock(hTextBuf);
                dataBuf[0]=0;
                lstrcpy((LPSTR)dataBuf, (LPSTR)pCBData);
                GlobalUnlock(hTextBuf);
                szTxt=strlen(dataBuf);
                ProcessSendCommand((char *)dataBuf, szTxt);
                bDoPosUpdate=1;
               }
            }
        break;
    }

  if(bDoPosUpdate)
     UpdateCursorDisplay();

  if(bDoDefaultProcess==0)
     return 0;
  else
     return CallWindowProc(txDataOutProc, hwnd, msg, wParam, lParam);
}


//**************************************************************************
// *** This function is called by the Windows function DispatchMessage() ***
//**************************************************************************
LRESULT CALLBACK WindowProcedure (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    unsigned char baudBuf[sizeof(DWORD)+1];
    int szTxt;
    DWORD dwBaud;

    switch (message)
    {
        case WM_DESTROY:
            PostQuitMessage (0);  // send a WM_QUIT to the message queue
            if(handle)
              usb_close(handle);  // clean up USB connection
        break;

        case WM_COMMAND:  // message: command from application menu or buttons
          switch(wParam)
           {
             case BTN_EXIT:  //User Clicked the Cancel Button
             case IDM_EXIT1:  //User selected Exit from main menu
              if(bWarnBeforeExit)
               {
                x = MessageBox(hwnd, "Exit USBASPconsole?", "Exit?", MB_YESNO);
                if(x!=IDYES)
                  return(0L);
               }

              PostQuitMessage(0);
              if(handle)
                 usb_close(handle);
             break;

             case IDM_ABOUT:  //User selected Help/About from menu
                MessageBox(hwnd, "Version 1.6  02/19/2015           \n By: Eric Klaus", "USBASP Terminal Application", MB_OK);
             break;

             case IDM_FROM_DEVICE_CLEAR:  // User selected the "From Device: Clear" menu option
                SetWindowText(hTxDataOut, (LPSTR)"");
                dwTxDataOutLen=0;
                UpdateCursorDisplay();
             break;

             case IDM_POLLING:  // Toggle the "Polling On/Off" menu option
               if(bPolling)
                 {
                  CheckMenuItem(GetMenu(hwnd), IDM_POLLING, MF_BYCOMMAND | MF_UNCHECKED );
                  bPolling=0;
                  CheckDlgButton(hwnd, RB_POLLING, 0);
                 }
               else
                 {
                  CheckMenuItem(GetMenu(hwnd), IDM_POLLING, MF_BYCOMMAND | MF_CHECKED );
                  bPolling=1;
                 }
             break;

             case IDM_ECHO: // Toggle the "Echo" menu option
               if(bEcho)
                 {
                  CheckMenuItem(GetMenu(hwnd), IDM_ECHO, MF_BYCOMMAND | MF_UNCHECKED );
                  bEcho=0;
                 }
               else
                 {
                  CheckMenuItem(GetMenu(hwnd), IDM_ECHO, MF_BYCOMMAND | MF_CHECKED );
                  bEcho=1;
                 }
             break;

             // ** "Get Byte Count" menu option selected ***
             case IDM_GET_BYTE_COUNT:
               usbBusy=1;
               if(OpenUsb())  // Open and connect to the device
                 {
                  cnt = GetAvailableByteCount(handle);   //Send the request
                  if(cnt < 0)  // Error - show error message
                     {
                      AddSystemMessage(hWnd, (LPSTR)"GET_DATA_COUNT USB error:",0);
                      AddSystemMessage(hWnd, (LPSTR)usb_strerror(),1);
                     }
                   else  // Success - show the count in the status bar
                     {
                      szTxt = cnt;
                      sprintf(msg,"Available: %d", szTxt);
                      SetDlgItemText(hwnd, LBL_STATUS, (LPSTR)msg);
                     }

                  usb_close(handle);
                  handle=NULL;
                 }

               usbBusy=0;
             break;

             // ** Get Available Bytes menu option was selected ***
             case IDM_GET_AVAILABLE_BYTES:
               usbBusy=1;
               if(OpenUsb())   // Open and connect to device
                 {
                  cnt = GetAllAvailableBytesFromDevice(handle, txtBuf, OUTBUF_SIZE);
                  if(cnt < 0)  // Error - show error message
                     {
                      AddSystemMessage(hWnd, (LPSTR)"GET_DATA_BYTE USB error:",0);
                      AddSystemMessage(hWnd, (LPSTR)usb_strerror(),1);
                     }
                  else     // Success - report how many bytes in status bar and
                     {     //  append the text to the "From Device" text box
                      sprintf(msg,"Got %d Bytes", cnt);
                      SetDlgItemText(hwnd, LBL_STATUS, (LPSTR)msg);

                      pTxBuf = (char *)txtBuf;

                      // Append to the text in the "From Device" text box
                      AddTextToConsole(hTxDataOut, (LPSTR)pTxBuf);
                      UpdateCursorDisplay();
                     }

                  usb_close(handle);
                  handle=NULL;
                 }

               usbBusy=0;

             break;

             // ** One of the STR menu options was selected **
             // ** Send a string from the .ini file
             case IDM_SEND_STR1:
                 SendCustomString("STR1_");
             break;

             case IDM_SEND_STR2:
                 SendCustomString("STR2_");
             break;

             case IDM_SEND_STR3:
                 SendCustomString("STR3_");
             break;

             // ** One of the CMD menu options was selected **
             case IDM_SEND_CMD1:
             case IDM_SEND_CMD2:
             case IDM_SEND_CMD3:

               cmd = (unsigned char)(wParam - IDM_SEND_CMD1);  // Calculate command# offset(0-2)

               if(OpenUsb())   // Open and connect to device
                 {
                  usbBusy=1;
                  cnt = SubmitGeneralCmd(handle, gen_cmds[cmd]);  // Send the RequestID
                  if(cnt < 0)  // ERROR - display error message
                    {
                     sprintf(msg, "SEND CMD%d error:",cmd+1);
                     AddSystemMessage(hWnd, (LPSTR)msg,0);
                     AddSystemMessage(hWnd, (LPSTR)usb_strerror(),1);
                    }
                  else     // Success - tell us the response value in status bar
                    {
                     sprintf(msg,"SEND CMD%d RETURNED:%02Xx", cmd+1, cnt);
                     SetDlgItemText(hwnd, LBL_STATUS, (LPSTR)msg);
                    }
                  usb_close(handle);
                  handle=NULL;
                 }

               usbBusy=0;
             break;

             case IDM_SET_BAUD_RATE: // ** Set Device Baud Rate menu item was selected
               // *** Show the Set Baud Rate Dialog - Returns the value entered or zero if canceled ***
               dwBaud = (DWORD)DialogBox(hInst,(LPSTR)MAKEINTRESOURCE(IDD_GET_BAUD_RATE), hWnd, (DLGPROC)DialogSetBaudRateProc );

               if(dwBaud > 0)
                 {
                  if(OpenUsb())   // Open and connect to device
                    {
                     usbBusy=1;
                     memcpy(baudBuf,(unsigned char *)&dwBaud,sizeof(DWORD));

                     cnt = usb_control_msg(handle, USB_TYPE_VENDOR | USB_RECIP_DEVICE | USB_ENDPOINT_IN, USBASP_FUNC_UART_SETBAUDRATE, (baudBuf[1] << 8) | baudBuf[0],(baudBuf[3] << 8) | baudBuf[2], (char *)dataBuf, sizeof(dataBuf), 5000);
                     if(cnt < 0)  // ERROR - display error message
                       {
                        sprintf(msg, "SET BAUD RATE CMD error:");
                        AddSystemMessage(hWnd, (LPSTR)msg,0);
                        AddSystemMessage(hWnd, (LPSTR)usb_strerror(),1);
                       }
                     else     // Success - tell us the response value in status bar
                       {
                        sprintf(msg,"SET BAUD RATE TO %d    RETURN CODE:%02Xx", (int)dwBaud, dataBuf[0]);
                        SetDlgItemText(hwnd, LBL_STATUS, (LPSTR)msg);
                       }
                       // NOTE: 0x53 is the appropriate return code.
                       // If anything else is returned your device doesn't support this option

                     usb_close(handle);
                     handle=NULL;
                     usbBusy=0;
                    }
                 }
             break;

             break;

             default:
                return (DefWindowProc(hwnd, message, wParam, lParam));
           }
        break;

        // ********************** POLLING ROUTINES ******************************
        // * Occurs every TIMER_INTERVAL milliseconds - used to poll USB device *
        // **********************************************************************
        case WM_TIMER:
          if((bPolling)&&(wParam == (DWORD)timerId))
             {
              ctTimer0+=timer_interval;
              if(ctTimer0 >= 1000)
                 { // UPdate the Seconds Counter ctTimer1
                  ctTimer0=0;
                  ctTimer1++;
                  // ** Toggle a radio button (looks like a blinking light)
                  // ** every second to show we are polling ***
                  x = IsDlgButtonChecked(hwnd, RB_POLLING);
                  CheckDlgButton(hwnd, RB_POLLING, !x);

                  if(ctTimer1 > 98)  //Reset every 100 sec.
                    ctTimer1=0;
                 }

              szTxt =0;

              if(usbBusy==0)           //  If not processing another command
                {
                 if(!OpenUsb())        // Open & connect to device
                   {
                    // Too many failed open attempts = turn polling off
                    if(++usb_open_fail_count > MAX_OPEN_ATTEMPTS)
                      {
                       CheckMenuItem(GetMenu(hwnd), IDM_POLLING, MF_BYCOMMAND | MF_UNCHECKED );
                       bPolling=0;
                       CheckDlgButton(hwnd, RB_POLLING, 0);
                       strcpy(msg, "\r\n !!! TOO MANY FAILED OPEN ATTEMPTS !!!");
                       AddSystemMessage(hwnd, (LPSTR)msg, 1);
                      }

                    return 0;
                   }
                 else
                   usb_open_fail_count=0;    // Open success - reset fail count

                 // Get any bytes waiting from the device
                 usbBusy=1;
                 cnt = GetAllAvailableBytesFromDevice(handle, txtBuf, OUTBUF_SIZE);
                 if(cnt < 0)  //** negative return = error ***
                   {
                    sprintf(msg, "GET_DATA_BYTE USB error: %s\n", usb_strerror());
                    SetDlgItemText(hwnd, LBL_STATUS, (LPSTR)msg);
                   }
                  else
                   {
                    if(cnt > 0)  // Got somthing
                      {
                       // Show us how many we got in status bar
                       sprintf(msg,"Got %d Bytes", cnt);
                       SetDlgItemText(hwnd, LBL_STATUS, (LPSTR)msg);

                       // Append to the text in the "From Device" text box
                       ConvertNewlinesInBuf((LPSTR) txtBuf, (LPSTR)workBuf);
                       AddTextToConsole(hTxDataOut, (LPSTR)workBuf);
                      }
                     else
                       UpdateCursorDisplay();
                  }

                 usbBusy=0;
                 usb_close(handle);
                 handle=NULL;
                }

             }
        break;

        default:     /* for messages that we don't deal with */
            return DefWindowProc (hwnd, message, wParam, lParam);
    }

    return 0;
}

//**************************************************************************
// *** Process Messages for the Set Baud Rate Dialog Box ***
//**************************************************************************
LRESULT CALLBACK DialogSetBaudRateProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
  unsigned int iBaud;

    switch (message)
    {
        case WM_CLOSE:      // user clicked X on title bar
        case WM_DESTROY:
            EndDialog(hwnd, 0);  // Return zero
            return TRUE;
        break;

        case WM_COMMAND:  // message: command from application menu or buttons
          switch(wParam)
           {
             case BTN_OK:  //User Clicked the OK Button or pressed [Enter]
               iBaud = GetDlgItemInt(hwnd, TXT_BAUD_RATE, NULL, FALSE);
               EndDialog(hwnd, iBaud);
               return TRUE;
             break;

             case BTN_CANCEL:  //User Clicked the Cancel Button
               EndDialog(hwnd, 0);  // Return zero
               return TRUE;
             break;
             }
        break;

        default:  // messages that we don't process
            return FALSE;
    }

    return 0;
}


// ***********************************************************************
// GetPrivateProfileValue(lpszSection, lpszKey, iDefault, lpszFile)
// Read a setting from the .ini file and convert it to an integer
// if the setting value contains "X" assume a hex value and convert.
// Return the resulting setting value
// ***********************************************************************
int GetPrivateProfileValue(LPCTSTR lpszSection, LPCTSTR lpszKey, int iDefault, LPCTSTR lpszFile)
{
  int iRetVal;
  DWORD dwLen;
  char sValue[10];

  dwLen = GetPrivateProfileString(lpszSection, lpszKey, "", sValue, sizeof(sValue), lpszFile);
  if(dwLen < 1)
    return iDefault;

  strupr(sValue);

  if(strchr(sValue, 'X'))
    sscanf(sValue, "%x", (int *)&iRetVal);
  else
    iRetVal = atoi(sValue);

  return iRetVal;
}

// ***********************************************************************
// SetStatusMessage(hWnd, str)
// Set the text in the status label control (LBL_STATUS).
// ***********************************************************************
void SetStatusMessage(HWND hWnd, LPSTR str)
{
 SetDlgItemText(hWnd, LBL_STATUS, (LPSTR)str);
}

// ***********************************************************************
// AddSystemMessage(hWnd, str, bNewLine)
// Append a system message (str) to the Data Out edit control
// add CR&LF if bNewline is True
// ***********************************************************************
void AddSystemMessage(HWND hWnd, LPSTR str, char bNewLine)
{
 HWND hCTL;
 unsigned char * outBuf;

 hCTL = GetDlgItem(hWnd, TXT_DATAOUT);

 outBuf = malloc(strlen((char *)str));
 if(outBuf+3)
   {
    ConvertNewlinesInBuf(str, (LPSTR)workBuf);

    if(bNewLine)
      strcat((char *)workBuf, "\r\n");
   }
  else // this should never happen, but...
    outBuf = (unsigned char *)str;

 AddTextToConsole(hCTL, (LPSTR)outBuf);

 if(outBuf != (unsigned char *)str)
    free(outBuf);
}

// ***********************************************************************
// AddCharToConsole(HWND hCtl, unsigned char c, char bLastChar)
// Output 1 byte to the specified text control (hCtl)
// test for and process backspace LF & CR
// ***********************************************************************
void AddCharToConsole(HWND hCtl, unsigned char c, char bLastChar)
{
 unsigned char buf[2];
 if(c==0x08) //Backspace
  {
   ProcessBackspace(hCtl);
  }
 else
  {
   buf[0]=c;    //Convert 1 byte into a null terminated string
   buf[1]=0;
   dwTxDataOutLen = GetWindowTextLength(hCtl);       //Get Cursor Offset
   SendMessage(hCtl, EM_SETSEL, -1, dwTxDataOutLen); // Position Cursor to End of Text
   SendMessage(hCtl, EM_REPLACESEL, 0, (LPARAM)buf); // Add 1 Character
   dwTxDataOutLen++;

   if((c=='\r')||(c=='\n')||(c=='\v'))
     {
      SendMessage(hCtl, EM_SETSEL, -1, dwTxDataOutLen); // Position Cursor to End of Text
      SendMessage(hCtl, EM_GETSEL, (WPARAM) (LPDWORD)NULL, (LPARAM)&dwTxDataOutLen); // Read The New Text Length
     }

   if(bLastChar)
      SendMessage(hCtl, WM_VSCROLL, SB_BOTTOM, (LPARAM)NULL); // Scroll into view

  }
}
// ***********************************************************************
// AddStringToConsole(hCtl, str, bNewLine)
// !!! NO LONGER USED !!!
// ***********************************************************************
void AddStringToConsole(HWND hCtl, LPSTR str, char bNewLine)
{
 unsigned int p=0, pLastPos=0;
 char bLastChar;

 pLastPos=strlen(str);
 if(pLastPos>0)
    pLastPos--;

 while(str[p])
  {
   bLastChar = (p == pLastPos);
   AddCharToConsole(hCtl, (unsigned char)str[p++], bLastChar);
  }

 if(bNewLine)
   {
    AddCharToConsole(hCtl, '\r',0);
    AddCharToConsole(hCtl, '\n',1);
   }
}

// ***********************************************************************
// ConvertNewlinesInBuf(LPSTR inBuf, LPSTR outBuf)
// Scan inBuf and copy to outBuf while translating LF to CR&LF
// NOTE: outBuf MUST be twice the size of inBuf
// ***********************************************************************
void ConvertNewlinesInBuf(LPSTR inBuf, LPSTR outBuf)
  {
   unsigned char *cpIn, *cpOut;
   static unsigned char cb, pb;

   cpIn = (unsigned char *)inBuf;
   cpOut = (unsigned char *)outBuf;
   cb = *cpIn;

   while(cb)
     {
      if((cb=='\n') && (pb != 'r'))
        {
          *cpOut='\r';
          cpOut++;
        }

      *cpOut=cb;
      cpOut++;
      pb=cb;
      cpIn++;
      cb=*cpIn;
     }

   *cpOut=0;
  }

// ***********************************************************************
//  AddTextToConsole(hCtl, str)
// ***********************************************************************
void AddTextToConsole(HWND hCtl, LPSTR str)
{
  dwTxDataOutLen = GetWindowTextLength(hCtl);
  SendMessage(hCtl, EM_SETSEL, dwTxDataOutLen, dwTxDataOutLen);
  SendMessage(hCtl, EM_REPLACESEL, 0, (LPARAM)str);
  SendMessage(hCtl, WM_VSCROLL, SB_BOTTOM, (LPARAM)NULL);
}

// ***********************************************************************
// ProcessBackspace(HWND hCtl)
// ***********************************************************************
void ProcessBackspace(HWND hCtl)
{
 SendMessage(hCtl, EM_SETSEL, -1, 0); // Position Cursor to End of Text
 dwTxDataOutLen = GetWindowTextLength(hCtl); //Get Cursor Offset
 SendMessage(hCtl, EM_SETSEL, dwTxDataOutLen-1, dwTxDataOutLen); // Select Last Character
 SendMessage(hCtl, EM_REPLACESEL, 0, 0); // Delete it
 dwTxDataOutLen--;
}

// ***********************************************************************
//  GetCursorRowCol(HWND hCtl, unsigned int *pRow, unsigned int *pCol)
// ***********************************************************************
void GetCursorRowCol(HWND hCtl, unsigned int *pRow, unsigned int *pCol)
{
 DWORD dwLineOffset, dwLine, dwCol;
 dwTxDataOutLen = GetWindowTextLength(hCtl); //Get Cursor Offset
 SendMessage(hCtl, EM_SETSEL, -1, dwTxDataOutLen); // Position Cursor to End of Text
 dwLine = SendMessage(hCtl, EM_LINEFROMCHAR, -1, 0); // Get Line#
 dwLineOffset = SendMessage(hCtl, EM_LINEINDEX, dwLine, 0); // Get Line#
 dwCol = dwTxDataOutLen-dwLineOffset;
 *pRow=(unsigned int)dwLine+1;

 if(!dwCol)
   dwCol++;

 *pCol=(unsigned int)dwCol;
}

// ***********************************************************************
// UpdateCursorDisplay(void)
// Update the status bar with the current cursor row & col
// ***********************************************************************
void UpdateCursorDisplay(void)
{
 char str[25];
 GetCursorRowCol(hTxDataOut, &cursor_row, &cursor_col);
 sprintf(str, "%03d - %03d", cursor_row, cursor_col);
 SetDlgItemText(hWnd, LBL_ROWCOL, (LPSTR)str);
}

// ***********************************************************************
// ProcessSendCommand(buf, szBuf)
// Send (buf) to the device. Convert to HEX if requested.
// Update the status message and return the number of bytes sent.
// ***********************************************************************
int ProcessSendCommand(char *buf, int szBuf)
{
  int x;
  char rptBuf[20];

  if(szBuf < 1)    // If set to append LF&CR OR sending an empty
    {                         // buffer  (e.g. enter pressed on blank line)
     strcat(buf, "\n\r");     // Add LF & CR to buf and increase size by 2
     szBuf+=2;
    }


  while(usbBusy)     // Wait for usb
       Sleep(1);

  OpenUsb();  // Open and connect to the device

  usbBusy=1;
  x = SendBytesToDevice(handle, (unsigned char *)buf, szBuf);  // Send the bytes to the device

  usb_close(handle);
  handle=NULL;
  usbBusy=0;

  if(x > 0)   // Positive value = success - report it!
    {
     sprintf(rptBuf,"[%d Bytes Sent]",x);
     SetDlgItemText(hWnd, LBL_STATUS, (LPSTR)rptBuf);
    }

  buf[0] =0;

  return x;   // Return number of bytes sent or error code.
}

// ***********************************************************************
// OpenUsb()
// Open the configured USB HID device and set the global (handle) value.
// Returns 0 if error 1 if success.
// ***********************************************************************
int OpenUsb(void)
  {
   /* The following function is in opendevice.c: */
   if(usbOpenDevice(&handle, vid, vendor, pid, product, NULL, NULL, NULL) != 0)
     {
      sprintf(msg, " ERROR: Could not find USB device \"%s\" [%s] with vid=0x%x pid=0x%x", product, vendor, vid, pid);
      AddSystemMessage(hWnd, msg, 1);
      handle=0;
      return 0;
     }

    return 1;
  }

// ***********************************************************************
// SubmitRequestCmd(hUSB, cmd)
// Send Request ID =(cmd) to the device and return the response byte.
// NOTE:timeout is fixed at 5 sec. (5000ms)
// ***********************************************************************
int SubmitRequestCmd(usb_dev_handle *hUSB, unsigned char cmd)
{
 char data[4];
 int ct=0;

 if(! hUSB)
   return -1;

 ct = usb_control_msg(hUSB, USB_TYPE_VENDOR | USB_RECIP_DEVICE | USB_ENDPOINT_IN, cmd, cmd, 0, data, sizeof(data), 5000);

 if(ct < 1)
    return -1;

 return (int)((unsigned char)data[0]);
}


// ***********************************************************************
// GetAvailableByteCount(hUSB)
// Return the nuber of bytes waiting to be sent from the device
// ***********************************************************************
int GetAvailableByteCount(usb_dev_handle *hUSB)
{
 return SubmitRequestCmd(hUSB, USBASP_FUNC_UART_GETBYTECOUNT);
}

// ***********************************************************************
// SubmitGeneralCmd(hUSB, cmd)
// Send Request ID =(cmd) to the device and return the response byte.
// NOTE: cmd values are set in the .INI file
// ***********************************************************************
int SubmitGeneralCmd(usb_dev_handle *hUSB, unsigned char cmd)
{
 return SubmitRequestCmd(hUSB, cmd);
}


// ***********************************************************************
// SendBytesToDevice(hUSB, buf, size)
// Send (size) byte from (buf) to the device
// Returns: Number of bytes send or negative error code
// ***********************************************************************
int SendBytesToDevice(usb_dev_handle *hUSB, unsigned char *buf, int size)
{
  int cnt=0, x=0;
  char tempBuf[100];

  if(! hUSB)
    return -1;


  while(x < size)
  {
   // Send 1 byte to device
   cnt = usb_control_msg(hUSB, USB_TYPE_VENDOR | USB_RECIP_DEVICE | USB_ENDPOINT_OUT, USBASP_FUNC_UART_PUTBYTE, buf[x], 0, tempBuf, 0, 5000);

   if(cnt < 0)  // If error - report it and exit
     {
      AddSystemMessage(hWnd, (LPSTR)"PUT_DATA_BYTE USB error:",0);
      AddSystemMessage(hWnd, (LPSTR)usb_strerror(),1);
      x = cnt;
      break;
     }

    x++;  // inc. buffer index.
  }

 return x;  // Return number of bytes read.
}

// ***********************************************************************
// GetAllAvailableBytesFromDevice(hUSB, buf, size)
// Request bytes from device until none are returned or (size) bytes reached.
// store the results in (buf)
// Returns: the number of bytes recieved or -1 if error.
// ***********************************************************************
int GetAllAvailableBytesFromDevice(usb_dev_handle *hUSB, unsigned char *buf, int size)
{
  int x=0, i=0, szData=0, data;

  if(! hUSB)
    return -1;

  szData=SubmitRequestCmd(hUSB, USBASP_FUNC_UART_GETBYTECOUNT);
  if(szData == -1)
    {
     buf[x]=0;
     return -1;
    }

  while((szData)&&(x < size))
    {
     for(i=0; i<szData; i++)   // for all available bytes - Request 1 byte from device
       {
         data=SubmitRequestCmd(hUSB, USBASP_FUNC_UART_GETBYTE);
         if(data != -1)                       // Got a Byte To Read
          {
           buf[x]=(unsigned char)data &0xFF;  // Save it
           if(x++ > size)                     // Inc. buffer index and check for overflow.
             break;
          }
       }

      szData=SubmitRequestCmd(hUSB, USBASP_FUNC_UART_GETBYTECOUNT);
      if(szData == -1)
        break;
     }

 buf[x]=0;
 return x;
}

// ***********************************************************************
//  ConvertASCIIBufferToHEX(buf)
//  Search (buf) for pairs of characters in the range of '0'-'9' or 'A'-'F'
//  and convert them to hex values stored in (buf)
//  Return: the number of converted bytes in (buf)
// ***********************************************************************
int ConvertASCIIBufferToHEX(char *buf)
  {
   int pVal=0, pOut=0, pIn=0, inLen;
   char c, valBuf[3]={0,0,0};
   unsigned char val;

   inLen=strlen(buf);
   while(pIn < inLen)
     {
      c=toupper(buf[pIn++]);
      if(((c>='0')&&(c<='9'))||((c>='A')&&(c<='F')))
        {
         valBuf[pVal++]=c;
         if(pVal>1)
           {
            if(valBuf[0]>'9')
               val=((valBuf[0]-'A')+10)*16;
            else
               val=((valBuf[0]-'0'))*16;

            if(valBuf[1]>'9')
               val+=(valBuf[1]-'A')+10;
            else
               val+=(valBuf[1]-'0');

            buf[pOut++]=val;
            pVal=0;
           }
        }
     }

   buf[pOut]=0;
   return pOut;
  }


// ***********************************************************************
// SendCustomString(name)
// Read the specified custom string "STR1", "STR2" or "STR3" from  the .ini
// file and send it's configured value to the host.(see USBASPconsole.ini file)
// ***********************************************************************
int SendCustomString(char *name)
{
 int bHex=0, szBuf=0, ret;
 char sItem[10];
 strcpy(sItem, name);
 strcat(sItem, "VAL");
 GetPrivateProfileString("OPTIONS", sItem, "", (char *)txtBuf, 40, sfName);
 strcpy((char *)&sItem[5], "HEX");
 bHex = GetPrivateProfileInt("OPTIONS", sItem, 0, sfName);
 if(bHex==1)
    szBuf = ConvertASCIIBufferToHEX((char *)txtBuf);
 else
    szBuf = strlen((char *)txtBuf);

 while(usbBusy)
   Sleep(1);

 OpenUsb();  // Open and connect to the device

 usbBusy=1;
 ret = SendBytesToDevice(handle, txtBuf, szBuf);

 usb_close(handle);
 handle=NULL;

 usbBusy=0;
 if(ret > 0)
   {
     sprintf(msg,"[%d Bytes Sent]",ret);
     SetStatusMessage(hWnd, (LPSTR)msg);
   }

 return ret;
}
// ***************************************************************************
//
//
// ***************************************************************************
