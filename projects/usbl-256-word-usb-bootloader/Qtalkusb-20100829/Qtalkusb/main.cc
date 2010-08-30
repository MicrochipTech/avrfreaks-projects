/*
 * main.cc
 */
#include "Qtalkusb.hh"

int
main( int argc, char *argv[] )
{
	QApplication         app( argc, argv );
    QTalkusbMainWindow   mw;

	mw.show( );

    return app.exec( );
}

/*
 * Prepare a hexdump buffer showing all of data[], then set it into the
 * hexdump widget.
 */
void
QTalkusbMainWindow::show_dump( const FlashImage &data, size_t len )
{
    unsigned loga;
    size_t   addr;
    unsigned llen;

    for (loga = 1, addr = len >> 4; addr; addr >>= 4) ++loga;

    llen = loga + 1 + 16 * 3 + 16 + 1;
    
    char *buf = new char[((len + 15) / 16) * llen + 1];

    for (addr = 0; addr < len; addr += 16) {
        sprintf( buf + (addr / 16) * llen, "%0*lX ", loga, addr );
        for (unsigned i = 0; i < 16; ++i)
            sprintf( buf + (addr / 16) * llen + loga + 1 + i * 3, "%02X ", data[addr+i] );
        for (unsigned i = 0; i < 16; ++i)
            buf[(addr / 16) * llen + loga + 1 + 16 * 3 + i] = (isprint( data[addr+i] )
                                                               ? data[addr+i]
                                                               : '.' );
        buf[(addr / 16 + 1) * llen - 1] = '\n';
    }

    buf[((len + 15) / 16) * llen] = '\0';

    pUI.hexdump->setPlainText( buf );

    delete buf;

    std::ofstream bin( "/var/tmp/flash.bin" );

    if (bin.good( )) {
        for (addr = 0; addr < len; ++addr)
            bin.put( data[addr] );
        bin.close( );

        FILE *disout = popen( "avr-elf-objdump -m avr -b binary -D /var/tmp/flash.bin | "
                              "uniq -f 1 |"
                              "tr -s '[ \t]' ' '",
                              "r" );
        if (disout) {
            std::ostringstream   disbuf;
            char                 line[512];

            while (fgets( line, sizeof(line), disout ))
                disbuf << line;
            pUI.disassembly->setPlainText( disbuf.str( ).c_str( ) );
            fclose( disout );
        }
        unlink( "/var/tmp/flash.bin" );
    } else {
        pUI.disassembly->setPlainText( "Failed to disassemble Flash" );
    }
}

void
QTalkusbMainWindow::on_actionOpen_Intel_HEX_triggered( void )
{
    static QString   lastName;
    QString          fileName = QFileDialog::getOpenFileName(
        this, tr("Open Intel HEX File"), lastName, tr("Hex Files (*.hex *.ihex)"));

    set_image( 0 );

    if (fileName.length( )) {
        FlashImage *img = new FlashImage( fileName );
        if (img->size( )) {
            char     buf[32];

            sprintf( buf, "Loaded %lu bytes.", img->size( ) );
            pUI.statusbar->showMessage( buf );
            lastName = fileName;
            set_image( img );
        } else {
            pUI.statusbar->showMessage( "Couldn't load image." );
            delete img;
        }
    } else {
        pUI.statusbar->showMessage( "No file selected." );
    }
}

void
QTalkusbMainWindow::on_actionQuit_triggered( void )
{
    exit( 0 );
}

void
QTalkusbMainWindow::on_attachDetach_clicked( void )
{
    if (pUI.attachDetach->text( ) == "Attach" ) {
        if ((pTarget = MCUTarget::find( pUSB, 0x16c0, 0x0479 )) != 0) {
            char    buf[8];
            pUI.attachDetach->setText( "Detach" );
            pUI.MCU->setCurrentWidget( pUI.AT90USB128x );
            sprintf( buf, "0x%02X", pTarget->fusel( ) );
            pUI.fuselHEX->setText( buf );
            on_fuselHEX_editingFinished( );
            sprintf( buf, "0x%02X", pTarget->fuseh( ) );
            pUI.fusehHEX->setText( buf );
            on_fusehHEX_editingFinished( );
            sprintf( buf, "0x%02X", pTarget->fusex( ) );
            pUI.fusexHEX->setText( buf );
            on_fusexHEX_editingFinished( );
            sprintf( buf, "0x%02X", pTarget->lock( ) );
            pUI.lockHEX->setText( buf );
            on_lockHEX_editingFinished( );
            pTarget->load( );
            show_dump( *pTarget, pTarget->flashSize( ) );
        } else {
            pUI.statusbar->showMessage( "No suitable device found." );
        }
    } else {
        if (pTarget) {
            delete pTarget;
            pTarget = 0;
        }
        pUI.attachDetach->setText( "Attach" );
        pUI.MCU->setCurrentWidget( pUI.NoMCU );
        pUI.hexdump->setPlainText( "(not connected)" );
        pUI.disassembly->setPlainText( "(not connected)" );
    }
}

void
QTalkusbMainWindow::on_rebootButton_clicked( void )
{
    if (pTarget) {
        pTarget->reboot( );
        on_attachDetach_clicked( );
    } else {
        pUI.statusbar->showMessage( "No MCU attached." );
    }
}

void
QTalkusbMainWindow::on_eraseButton_clicked( void )
{
    if (pTarget) {
        pUI.statusbar->showMessage( pTarget->erase( ) ? "Erased." : "Erase failed." );
    } else {
        pUI.statusbar->showMessage( "No MCU attached." );
    }
}

void
QTalkusbMainWindow::on_mergeButton_clicked( void )
{
    if (!pTarget) {
        pUI.statusbar->showMessage( "No MCU attached." );
    } else
    if (!pImage) {
        pUI.statusbar->showMessage( "No image loaded." );
    } else
        if (pImage->size( ) > pTarget->size( ) - pTarget->bootSize( )) {
        pUI.statusbar->showMessage( "Image exceeds Flash size." );
    } else {
        for (size_t a = 0; a < pImage->size( ); ++a) {
            if (pTarget->at( a ) != pImage->at( a )) {
                pTarget->setModified( true );
                pTarget->at( a ) = pImage->at( a );
            }
        }
        if (pTarget->isModified( )) {
            show_dump( *pTarget, pTarget->flashSize( ) );
            pUI.statusbar->showMessage( "Ready to write." );
        } else {
            pUI.statusbar->showMessage( "(no change)" );
        }
    }
}

void
QTalkusbMainWindow::on_writeButton_clicked( void )
{
    if (!pTarget) {
        pUI.statusbar->showMessage( "No MCU attached." );
    } else
    if (!pTarget->isModified( )) {
        pUI.statusbar->showMessage( "No changes to write." );
    } else {
        pUI.statusbar->showMessage( pTarget->write( ) ? "Written." : "Write failed." );
    }
}

void
QTalkusbMainWindow::on_reloadButton_clicked( void )
{
    if (pTarget) {
        pUI.statusbar->showMessage( "Save temp file" );
        std::ofstream f( "flash.bin" );
        if (f.good( )) {
            for (size_t i = 0; i < pTarget->flashSize( ); ++i)
                f.put( (*pTarget)[i] );
        }
    } else {
        pUI.statusbar->showMessage( "No MCU attached." );
    }
}

void
QTalkusbMainWindow::on_fuselHEX_editingFinished( void )
{
    unsigned char byte = pUI.fuselHEX->text( ).toUInt( 0, 0 );

    pUI.fuselCLKDIV->setCurrentIndex( (byte >> 7) & 1 );
    pUI.fuselCKOUT->setCurrentIndex( (byte >> 6) & 1 );
    pUI.fuselSUT->setCurrentIndex( (byte >> 4) & 3 );
    pUI.fuselCKSEL->setCurrentIndex( (byte >> 0) & 0x0f );
}

void
QTalkusbMainWindow::on_fuselCLKDIV_currentIndexChanged( int i )
{
    unsigned char byte = pUI.fuselHEX->text( ).toUInt( 0, 0 );
    char     buf[8];

    switch (i) {
      case 0:   byte |= 1; break;
      case 1:   byte &= ~1; break;
    }
    
    sprintf( buf, "0x%02X", byte );
    
    pUI.fuselHEX->setText( QString( buf ) );
}

void
QTalkusbMainWindow::on_fusehHEX_editingFinished( void )
{
    unsigned char byte = pUI.fusehHEX->text( ).toUInt( 0, 0 );

    pUI.fusehOCDEN->setCurrentIndex( (byte >> 7) & 1 );
    pUI.fusehJTAGEN->setCurrentIndex( (byte >> 6) & 1 );
    pUI.fusehSPIEN->setCurrentIndex( (byte >> 5) & 1 );
    pUI.fusehWDTON->setCurrentIndex( (byte >> 4) & 1 );
    pUI.fusehEESAVE->setCurrentIndex( (byte >> 3) & 1 );
    pUI.fusehBOOTSZ->setCurrentIndex( (byte >> 1) & 3 );
    pUI.fusehBOOTRST->setCurrentIndex( (byte >> 0) & 1 );

    if (pTarget) {
        pTarget->setBootSize( pUI.fusehBOOTSZ->currentText( ).toUInt( 0, 0 ) * 2 );
    }
}

void
QTalkusbMainWindow::on_fusehBOOTSZ_currentIndexChanged( int i )
{
    unsigned char byte = pUI.fusehHEX->text( ).toUInt( 0, 0 );
    char     buf[8];

    byte = (byte & ~(3 << 1)) | (i << 1);

    sprintf( buf, "0x%02X", byte );
    
    pUI.fusehHEX->setText( QString( buf ) );
}

void
QTalkusbMainWindow::on_fusexHEX_editingFinished( void )
{
    unsigned char byte = pUI.fusexHEX->text( ).toUInt( 0, 0 );

    pUI.fusexHWBE->setCurrentIndex( (byte >> 3) & 1 );
    pUI.fusexBODLEVEL->setCurrentIndex( (byte >> 0) & 7 );
}

void
QTalkusbMainWindow::on_lockHEX_editingFinished( void )
{
    unsigned char byte = pUI.lockHEX->text( ).toUInt( 0, 0 );

    pUI.lockBLB0->setCurrentIndex( (byte >> 4) & 3 );
    pUI.lockBLB1->setCurrentIndex( (byte >> 2) & 3 );
    pUI.lockLB->setCurrentIndex( (byte >> 0) & 3 );
}

