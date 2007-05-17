/****************************************************************************
  Function definitions
****************************************************************************/
void		resetState(unsigned char s);
void		ansFault(unsigned char s);
void		ansSuccess(void);
void		initCPU(void);
void		timer(void);
void		connectTest(void);
void		usbWork(void);
void		dialog(void);
void 		cryptInit(void);
void		recordRec(void);
void		recordView(void);
void		pageWriting(void);
void		setLock(void);
void		boot_program_page(uint32_t page, uint8_t *buf);
