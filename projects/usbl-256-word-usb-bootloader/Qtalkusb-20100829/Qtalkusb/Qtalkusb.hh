/*
 * Qtalkusb.hh
 */
#ifndef __Qtalkusb_hh__
#define __Qtalkusb_hh__

extern "C" {
#   include <stdio.h>
#   include <sys/types.h>
#   include <sys/stat.h>
#   include <fcntl.h>
#   include <errno.h>
#   include <ctype.h>
#   include <stdlib.h>
#   include <linux/usbdevice_fs.h>
#   include <asm/byteorder.h>
#   include <errno.h>
#   include <string.h>
#   include <libusb-1.0/libusb.h>
}

#include "ui_Qtalkusb.h"
#include <QtGui/QFileDialog>
#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>

class FlashImage : public std::vector<unsigned char> {
    /**/                 FlashImage( const FlashImage & );
    FlashImage          &operator=( const FlashImage & );

  public:
    /**/                 FlashImage( void );
    /**/                 FlashImage( const QString &filename );
    /**/                ~FlashImage( void );

    void                 setLoaded( bool l ) { pLoaded = l; }
    bool                 isLoaded( void ) const { return pLoaded; }
    void                 setModified( bool m = true ) { pModified = m; }
    bool                 isModified( void ) const { return pModified; }

  private:
    bool                 pLoaded;
    bool                 pModified;
} ;

class MCUTarget : public FlashImage {
    /**/                 MCUTarget( void );
    /**/                 MCUTarget( const MCUTarget & );
    MCUTarget           &operator=( const MCUTarget & );
    
  public:
    /**/                 MCUTarget( libusb_device_handle *d );
    /**/                ~MCUTarget( void );

    unsigned long        signature( void ) const { return pSig; }
    unsigned char        cal( void ) const { return pRCCal; }
    unsigned char        fusel( void ) const { return pFuseL; }
    unsigned char        fuseh( void ) const { return pFuseH; }
    unsigned char        fusex( void ) const { return pFuseX; }
    unsigned char        lock( void ) const { return pLock; }

    void                 setBootSize( size_t s ) { pBootSize = s; }

    size_t               flashSize( void ) const { return pFlashSize; }
    size_t               pageSize( void ) const { return pPageSize; }
    size_t               bootSize( void ) const { return pBootSize; }

    bool                 load( void );
    bool                 erase( void );
    bool                 write( void );
    bool                 reboot( void );
    
    static MCUTarget    *find( libusb_context *ctx, uint16_t vendor, uint16_t product );

  private:
    libusb_device_handle*pDev;
    unsigned long        pSig;
    unsigned char        pRCCal;
    unsigned char        pFuseL;
    unsigned char        pFuseH;
    unsigned char        pFuseX;
    unsigned char        pLock;
    size_t               pPageSize;
    size_t               pFlashSize;
    size_t               pBootSize;
} ;

class QTalkusbMainWindow : public QMainWindow {
    Q_OBJECT

  public:
    /**/                         QTalkusbMainWindow( void )
                                   : pUI( )
                                   , pUSB( 0 )
                                   , pTarget( 0 )
                                   , pImage( 0 )
                                   {
                                       pUI.setupUi( this );
                                       
                                       if (libusb_init( &pUSB ) < 0) {
                                           std::cerr << "Failed to initialize libusb\n";
                                           exit( 0 );
                                       }
                                       libusb_set_debug( pUSB, 3 );
                                   }

    void                         show_dump( const FlashImage &img, size_t len );
    void                         set_image( FlashImage *i )
                                   {
                                       if (pImage) delete pImage;
                                       pImage = i;
                                   }

  public slots:
    void                         on_actionOpen_Intel_HEX_triggered( void );
    void                         on_actionQuit_triggered( void );
    void                         on_attachDetach_clicked( void );
    void                         on_eraseButton_clicked( void );
    void                         on_mergeButton_clicked( void );
    void                         on_writeButton_clicked( void );
    void                         on_reloadButton_clicked( void );
    void                         on_rebootButton_clicked( void );

    void                         on_fuselHEX_editingFinished( void );
    void                         on_fusehHEX_editingFinished( void );
    void                         on_fusexHEX_editingFinished( void );
    void                         on_lockHEX_editingFinished( void );

    void                         on_fuselCLKDIV_currentIndexChanged( int );
    void                         on_fusehBOOTSZ_currentIndexChanged( int );

  private:
    Ui_MainWindow            pUI;
    libusb_context          *pUSB;
    MCUTarget               *pTarget;
    FlashImage              *pImage;
} ;

#endif  /* !defined(__Qtalkusb_hh__) */
