/*
 * MCUTarget
 */
#include "Qtalkusb.hh"

MCUTarget::MCUTarget( libusb_device_handle *d )
  : FlashImage( )
  , pDev( d )
  , pSig( 0 )
  , pFuseL( 0 )
  , pFuseH( 0 )
  , pFuseX( 0 )
  , pLock( 0 )
  , pPageSize( 64 )
  , pFlashSize( 0 )
  , pBootSize( 256 )
{
    unsigned char    sigbytes[64];
    unsigned char    lockbytes[64];

    if (pDev) {
        if (libusb_control_transfer( pDev,
                                     0xc0, // Vendor IN
                                     (1 << 5) | (1 << 0), // SIGRD | SPMEN
                                     0,
                                     0,
                                     (unsigned char *)sigbytes,
                                     5,
                                     1000 ) == 5) {

            pSig = ((((unsigned long)sigbytes[0]) << 16) |
                    (((unsigned long)sigbytes[2]) <<  8) |
                    (((unsigned long)sigbytes[4]) <<  0));

            if (pSig == 0x1E9488) { // ATmega16U4
                pPageSize = 256;
                pFlashSize = 16 * 1024;
            } else
            if (pSig == 0x1E9587) { // ATmega32U4
                pPageSize = 256;
                pFlashSize = 32 * 1024;
            } else
            if (pSig == 0x1E9682) { // AT90USB64x
                pPageSize = 256;
                pFlashSize = 64 * 1024;
            } else
            if (pSig == 0x1E9782) { // AT90USB128x
                pPageSize = 256;
                pFlashSize = 128 * 1024;
            }

            if (pFlashSize) reserve( pFlashSize );

            pRCCal = sigbytes[1];
        }
        if (libusb_control_transfer( pDev,
                                     0xc0, // Vendor IN
                                     (1 << 3) | (1 << 0), // BLBSET | SPMEN
                                     0,
                                     0,
                                     (unsigned char *)lockbytes,
                                     4,
                                     1000 ) == 4) {
            pFuseL = lockbytes[0];
            pFuseH = lockbytes[3];
            pFuseX = lockbytes[2];
            pLock = lockbytes[1];
        }
    }
}

MCUTarget::~MCUTarget( void )
{
    if (pDev) libusb_close( pDev );
}

bool
MCUTarget::load( void )
{
    if (!pFlashSize) {
        std::cerr << "Unknown Flash size (unknown MCU).  Can't load.\n";
        return false;
    } else {
        clear( );
        resize( pFlashSize, ~0 );
    }

    for (size_t addr = 0; addr < pFlashSize; addr += pPageSize) {
        unsigned char    buf[1024];
        int              r;
        
        if ((r = libusb_control_transfer( pDev,
                                          0xc0,
                                          0,
                                          addr >> 16,
                                          addr & 0xffff,
                                          buf,
                                          pPageSize,
                                          1000 )) != int(pPageSize)) {
            fprintf( stderr, "Flash read failed at 0x%08lX: %d\n", addr, r );
            return false;
        }
        for (size_t i = 0; i < pPageSize; ++i)
            (*this)[addr + i] = buf[i];
    }
    setLoaded( true );
    return true;
}

bool
MCUTarget::erase( void )
{
    unsigned char    buf[1024];

    for (size_t addr = 0; addr < pFlashSize - pBootSize - pPageSize; addr += pPageSize) {
        if (libusb_control_transfer( pDev,
                                     0x40,
                                     0,
                                     addr >> 16,
                                     addr & 0xffff,
                                     buf,
                                     0,
                                     1000 ) != 0)
            return false;
    }
    
    return true;
}

bool
MCUTarget::write( void )
{
    unsigned char    buf[1024];
    
    for (size_t addr = 0; addr < pFlashSize - pBootSize; addr += pPageSize) {
        bool nonFF = false;
        for (size_t i = 0; i < pPageSize; ++i)
            if ((buf[i] = at( addr + i )) != (unsigned char)(~0)) nonFF = true;

        if (nonFF &&
            libusb_control_transfer( pDev,
                                     0x40,
                                     0,
                                     addr >> 16,
                                     addr & 0xffff,
                                     buf,
                                     pPageSize,
                                     1000 ) != (int)pPageSize) {
            std::cerr << "Failed to write page at " << addr << std::endl;
            return false;
        }
    }
    
    return true;
}

bool
MCUTarget::reboot( void )
{
    /* Reboot by reading 0 bytes */
    unsigned char   buf[1];
    
    return libusb_control_transfer( pDev,
                                    0xc0, // Vendor IN
                                    0,
                                    0,
                                    0,
                                    buf,
                                    0,
                                    1000 ) == 0;
}

MCUTarget *
MCUTarget::find( libusb_context *ctx, uint16_t vendor, uint16_t product )
{
    libusb_device_handle *d = libusb_open_device_with_vid_pid( ctx, vendor, product );

    return d ? new MCUTarget( d ) : 0;
}
