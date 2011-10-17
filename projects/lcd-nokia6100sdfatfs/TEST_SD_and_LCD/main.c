
#include <avr/io.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include "ff.h"
#include "lcd_driver.h"
#include "UART_routines.h"
///////////////////////////////////////////////////////////////////

char Buff[256];				// Read-write buffer
		DWORD	*dir_;
		DWORD	*file;

WORD acc_files, acc_dirs;
uint8_t bg=255, fg=0, x=0, y=0;

uint8_t dir_counter=0;
uint8_t file_counter=0;


//FRESULT scan_files (char* path);
FRESULT scan_files (char* path,int count_);
//FRESULT scan_files (void);
////FILINFO Finfo;
//WORD acc_files, acc_dirs;
//DWORD acc_size;


///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
/////////////////////    MAIN!!!              /////////////////////
///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////


int main (void) {
//static unsigned char byte ;

	uart1_init(); 
	init_lcd();
	clrscr(0,0,131,131,255); // ОЧИСТКА ЭКРАНА

//putch(50, 50, 'Ю', 0, 255);

//	putstr_pgm(20,3, hello,0,255);// ЗАПУСК ПРОШЕЛ

//drawLine(10, 10, 100, 100, fg, 2);
//while(1){}


// x=10, y=10;
// x = putstr_ram(x, y, "Нихренасе",fg,bg); // распечатать и вернуть текущее положение X
/// где fg - цвет текста, bg - цвет фона (задается глобально или потом модифицируется)

//while(1){}


//////////////////////////////////////////////////////////////
/////////////////////  FATFS & LCD !!!          //////////////
//////////////////////////////////////////////////////////////
//	waitms(10000);
 

	//fcc = "TRI";
	dir_ = "0:/";
	//dir_ ="TRI";





 FRESULT f_err_code;
 static FATFS FATFS_Obj;
//  FIL fil_obj;

	x=2;y=120;

 disk_initialize(0);		// Init drive	
 f_err_code=f_mount(0, &FATFS_Obj);	//Mount Fat Fs
 x = putstr_ram(x,y,"mounting FAT-",fg,bg);
 if(f_err_code==0) { 
					x = putstr_ram(x,y,"OK",fg,bg);
 				

  				//f_err_code=f_chdir ("0:/SKINS");	// Check current directore
 				//	x = putstr_ram(x=2,y-=8,"move to DIR SKINS-",fg,bg);
 				//	if(f_err_code==0) x = putstr_ram(x,y,"OK",fg,bg);
              	//				else  x = putstr_ram(x,y,"FAIL",fg,bg);


  				x = putstr_ram(x=2, y-=8, "scaning DIR-",fg,bg);
				x = putstr_ram(x, y, dir_,fg,bg);
 				//scan_files ("0:/SKINS");   // Scan DIR  SKINS

				scan_files (dir_,1); // 2 - номер папки, название которой надо сбуфер

 				x = putstr_ram(x=2, y-=8, "scaning ended",fg,bg);
///////////////////////////////////////////////////////////////////////////////	
  				x = putstr_ram(x=2, y-=8, "scaning DIR-",fg,bg);
				x = putstr_ram(x, y, dir_,fg,bg);

//	printDecimal(20, 15, dir_counter, fg, bg);
//	printDecimal(20, 3, file_counter, fg, bg);

				scan_files (dir_,0);

				x = putstr_ram(x=2, y-=8, "scaning ended",fg,bg);

			//	acc_dirs
///////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////
/*			
 					f_err_code=f_open(&fil_obj, "BMP0.BMP",FA_READ);//Open BMP0.BMP for reading
 					x=putstr_ram(x=2,y-=8,"open BMP0.BMP-",fg,bg);
 					if(f_err_code==0) x=putstr_ram(x,y,"OK",fg,bg);
              					else  x=putstr_ram(x,y,"FAIL",fg,bg);
 					x= putstr_ram(x=2,y-=8,"data in BMP0.BMP: ",fg,bg);
///////////////////////////////////////////////////////////////
 

print_bmp_sd(&fil_obj); // Вывести картинку на LCD

					//x=10; y=5;
 					 f_err_code=f_close(&fil_obj);
 					x = putstr_ram(x=10,y=5,"closing BMP0.BMP-",fg,bg);//Close newfile
 					if(f_err_code==0) x = putstr_ram(x,y,"OK",fg,bg);
              					else  x = putstr_ram(x,y,"FAIL",fg,bg);
*/
///////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////
					}
								
							

} // END MAIN
/////////////////////////////////////////////
       // res = f_write(&fdst, buffer, br, &bw);               /* Write it to the dst file */
       // if (res || bw < br) break; /* error or disk full */
	   //////////////////////////////////////////////////////////
//while(1){ // test UART & LCD
 //  bmp_rcv_uart();
//}

///////////////////////////////////
void waitms(int ms) {
  int i;
  for (i=0;i<ms;i++) _delay_ms(1);
}
///////////////////////////////////


FRESULT scan_files (char* path,int count_)		//Scan files and directories on path
{
	int flag = 0;

 FRESULT res;
 FILINFO fno;
 DIR dir;
 DWORD *fn;
	int i=1;
    res = f_opendir(&dir, path);
    if (res == FR_OK) {
						//i = strlen(path);
                       for (;;) 
					    {
                         res = f_readdir(&dir, &fno);		//Read dir items

                         if (res != FR_OK || fno.fname[0] == 0) break;
                         if (fno.fname[0] == '.') continue;
                         fn = fno.fname;
                         if (fno.fattrib & AM_DIR) {		// Check item type, DIR or FILE
										
																							
												    x =putstr_ram(x=2,y-=8,"DIR: ",fg,bg);
                                                    x =putstr_ram(x,y,fn,fg,bg);
													
													

													if(i==count_) strcpy(dir_, fn);
													
													

													dir_counter++;
	
                                                 	i++;
												   } 
											  else {
											  		
													x=putstr_ram(x=2,y-=8,"FILE: ",fg,bg);
                                                    x=putstr_ram(x,y,fn,fg,bg);
													file_counter++;
                                                   }
                       }
                     }

    return res;

}


