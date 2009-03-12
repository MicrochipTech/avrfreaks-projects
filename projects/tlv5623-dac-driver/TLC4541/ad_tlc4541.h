/***************************************************************************/
/* File        : ads7816p.h                                                */
/* Descrizione : A/D Converter ADS7816 driver                              */
/* Autore      : S. Favillini                                              */
/* Data        : 28/05/2008                                                */
/* Versione    : 1.0                                                       */
/***************************************************************************/
#ifndef __ADS_H
#define __ADS_H



                          
void Ads_init(void);
void Ads_reset(void);
WORD Ads_getsample(void);  
WORD Ads_getsamplemean(WORD nsample);   







#endif 
