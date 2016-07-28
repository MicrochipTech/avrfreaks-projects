# SAMV71 XPlained Ultra -- QSPI MEMORY MODE -- DMA

Uploaded by dujiarui_001 on 2016-07-28 06:51:00 (rating 0 out of 5)

## Summary

您好：


     我使用 ATMEL STUDIO 7，SAMV71 XPlained Ultra 评估板。使用 QUARD SPI memory mode，发现用复制语句可以产生 QSPI 波形，但是用 DMA 操作（从内部 SRAM 向目标地址 0x80000000 开始的内存块复制32个字  ）无法产生 QSPI 波形。


    QUARD SPI memory mode 可以使用 DMA 吗？该如何设置？



```

static void configure\_qspi(void)
{
 struct qspi\_config\_t qspi\_config ;
 
 qspi\_config.serial\_memory\_mode = 1; // QSPI\_MR. qspi memory mode
 qspi\_config.loopback\_en = false; // QSPI\_MR.
 qspi\_config.wait\_data\_for\_transfer = false; // QSPI\_MR. 
 qspi\_config.csmode = 1;    // QSPI\_MR. CS -- LASTXFER
 qspi\_config.bits\_per\_transfer = QSPI\_MR\_NBBITS\_8\_BIT; // QSPI\_MR.
 qspi\_config.min\_delay\_qcs = 0;  // QSPI\_MR. DLYCS = 0 : delay between the deactivation and the activation of QCS 
 qspi\_config.delay\_between\_ct = 0; // QSPI\_MR. DLYBCT = 0 : delay between two consecutive transfers
 qspi\_config.clock\_polarity = 1;  // QSPI\_SCR. Figure 39-2
 qspi\_config.clock\_phase = 1;  // QSPI\_SCR. Figure 39-2
 qspi\_config.baudrate = 1000000;  // QSPI\_SCR. 
 qspi\_config.transfer\_delay = 0x40; // QSPI\_SCR. delay from QCS valid to the first valid QSCK transition 
 qspi\_config.scrambling\_en = false; // QSPI\_SMR
 qspi\_config.scrambling\_random\_value\_dis = false; // 
 qspi\_config.scrambling\_user\_key = 0; // QSPI\_SKR

 sysclk\_enable\_peripheral\_clock(ID\_QSPI);
 qspi\_initialize(QSPI,&qspi\_config) ;
 
 if (1)
 {
  uint32\_t mask = 0 ;
  mask |= ( QSPI\_IFR\_WIDTH\_QUAD\_CMD | QSPI\_IFR\_DATAEN | QSPI\_IFR\_TFRTYP\_TRSFR\_WRITE ) ;
  // QSPI\_IFR\_INSTEN, QSPI\_IFR\_ADDREN, QSPI\_IFR\_OPTEN, OPTL, ADDRL, CRM, NBDUM 均为 0。

  QSPI->QSPI\_IFR = mask;
 }
 
 printf("QSPI Init OK!\r\n") ;
}

void config\_xdmac (void)
{
 /* Initialize and enable DMA controller */
 pmc\_enable\_periph\_clk(ID\_XDMAC);

 /*Enable XDMA interrupt */
 NVIC\_ClearPendingIRQ(XDMAC\_IRQn);
 NVIC\_SetPriority( XDMAC\_IRQn ,1);
 NVIC\_EnableIRQ(XDMAC\_IRQn);
}


#pragma GCC push\_options
#pragma GCC optimize("O0")
void xdmac\_transfer (uint32\_t xdma\_channel, uint32\_t* pSrc, uint32\_t* pDst, uint32\_t sizeW)
{
 xdmac\_channel\_disable(XDMAC, xdma\_channel);
 
 // Initialize channel config
 xdmac\_channel\_cfg.mbr\_ubc = sizeW ; 
 xdmac\_channel\_cfg.mbr\_sa = (uint32\_t)pSrc;
 xdmac\_channel\_cfg.mbr\_da = (uint32\_t)pDst;
 
 xdmac\_channel\_cfg.mbr\_cfg = XDMAC\_CC\_TYPE\_MEM\_TRAN | // 0<<0
 XDMAC\_CC\_MEMSET\_NORMAL\_MODE |      // 0<<7
 XDMAC\_CC\_MBSIZE\_SINGLE |       // 0<<1
 XDMAC\_CC\_DWIDTH\_WORD |        // 0<<11
 XDMAC\_CC\_SIF\_AHB\_IF0 |        // 0<<13
 XDMAC\_CC\_DIF\_AHB\_IF0 |        // 0<<14
 XDMAC\_CC\_SAM\_INCREMENTED\_AM |      // 1<<16
 XDMAC\_CC\_DAM\_INCREMENTED\_AM;      // 1<<18
 xdmac\_channel\_cfg.mbr\_bc = BLOCK\_LEN - 1;    // 15
 xdmac\_channel\_cfg.mbr\_ds =  0;
 xdmac\_channel\_cfg.mbr\_sus = 0;
 xdmac\_channel\_cfg.mbr\_dus = 0;

 xdmac\_configure\_transfer(XDMAC, xdma\_channel, &xdmac\_channel\_cfg);

 // Initialize linked list descriptor
 lld[0].mbr\_nda = 0 ;//(uint32\_t)(&lld[1]);
 
 lld[0].mbr\_ubc = XDMAC\_UBC\_NVIEW\_NDV0 |     // 0x0u << XDMAC\_UBC\_NVIEW\_Pos
 XDMAC\_UBC\_NDE\_FETCH\_EN |      // 1<<24
 XDMAC\_UBC\_NSEN\_UPDATED |      // 1<<25
 XDMAC\_UBC\_NDEN\_UPDATED |      // 1<<26
 XDMAC\_UBC\_UBLEN(sizeW);    // (XDMAC\_UBC\_UBLEN\_Msk & ((value) << XDMAC\_UBC\_UBLEN\_Pos))
 lld[0].mbr\_da = (uint32\_t)pDst;

 xdmac\_channel\_set\_descriptor\_control(XDMAC, xdma\_channel, XDMAC\_CNDC\_NDVIEW\_NDV0 | // 0<<3
 XDMAC\_CNDC\_NDE\_DSCR\_FETCH\_EN |           // 1<<0
 XDMAC\_CNDC\_NDSUP\_SRC\_PARAMS\_UPDATED |         // 1<<1
 XDMAC\_CNDC\_NDDUP\_DST\_PARAMS\_UPDATED);         // 1<<2
 xdmac\_channel\_set\_descriptor\_addr(XDMAC, xdma\_channel, (uint32\_t)(&lld[0]), 0);

 xdmac\_enable\_interrupt(XDMAC, xdma\_channel);
 xdmac\_channel\_enable\_interrupt(XDMAC, xdma\_channel, XDMAC\_CIE\_BIE);
 xdmac\_channel\_enable(XDMAC, xdma\_channel);
}
#pragma GCC pop\_options

/**
 * \brief XDMAC interrupt handler.
 */
void XDMAC\_Handler(void)
{
 volatile uint32\_t dma\_status;

 dma\_status = xdmac\_channel\_get\_interrupt\_status(XDMAC, XDMA\_MEM\_CH);

 if (dma\_status & XDMAC\_CIS\_LIS) // XDMAC\_CIS\_BIS 
 {
  g\_xfer\_done = 1;
 }
 
 dma\_status = xdmac\_channel\_get\_interrupt\_status(XDMAC, XDMA\_QSPI\_CH);
 if (dma\_status & XDMAC\_CIS\_LIS) // XDMAC\_CIS\_BIS
 {
  g\_xdma\_qspi\_done = 1;
 }
 
 dmaint\_num++;
}

int main (void)
{
// Initialize the SAM system. 
 sysclk\_init();
 board\_init();

 // Configure debug UART 
 configure\_console();

 // Print example information. 
 puts(STRING\_HEADER);
 
 // check reset source
 check\_rst() ;

 // Bring up the ethernet interface & initialize timer0, channel0. 
 init\_ethernet();

 // Bring up the web server. 
 httpd\_init();
 
 //action\_test() ; 
 
 configure\_qspi() ;
 
 config\_xdmac() ;config\_xdmac() ;

xdmac\_transfer(XDMA\_MEM\_CH, src\_buf, (uint32\_t *)QSPIMEM\_ADDR, 32) ;
   while (!g\_xfer\_done)
   {
   }
}
```

## Compilers

- Atmel Studio 6

## Tags

- Atmel Studio 6
