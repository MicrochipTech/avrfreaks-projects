/*
 * FlashImage
 */
#include "Qtalkusb.hh"

FlashImage::FlashImage( void )
  : std::vector<unsigned char>( )
  , pLoaded( false )
  , pModified( false )
{

}

FlashImage::FlashImage( const QString &filename )
  : std::vector<unsigned char>( )
  , pLoaded( false )
  , pModified( false )
{
    std::ifstream    f( filename.toStdString( ).c_str( ) );
    std::string      line;
    
    unsigned long    ea = 0;

    while (f.good( ) && f >> line) {
        if (line[0] != ':' || (line.length( ) & 1) != 1 || line.length( ) < 11) {
            std::cerr << "Invalid line \"" << line << "\"\n";
            continue;
        }

        unsigned char cs = 0;

        for (unsigned i = 1; i < line.length( ); i += 2)
            cs += strtoul( line.substr( i, 2 ).c_str( ), 0, 16 );

        if (cs) {
            std::cerr << "Bad checksum: " << (unsigned int)cs << " line \"" << line << "\"\n";
            continue;
        }

        unsigned nbytes = strtoul( line.substr( 1, 2 ).c_str( ), 0, 16 );
        unsigned addr = strtoul( line.substr( 3, 4 ).c_str( ), 0, 16 );
        unsigned type = strtoul( line.substr( 7, 2 ).c_str( ), 0, 16 );

        switch (type) {
          case 0:               // Data
              if (size( ) < ea + addr + nbytes + 1)
                  resize( ea + addr + nbytes + 1, ~0 );
              for (unsigned i = 0; i < nbytes; ++i)
                  (*this)[ ea + addr + i] =
                      strtoul( line.substr( 9 + i * 2, 2 ).c_str( ), 0, 16 );
              pLoaded = true;
              break;
          case 1:               // EOF
              break;
          case 2:               // Extended Segment Address
              if (addr != 0 || nbytes != 2) {
                  std::cerr << "Invalid Extended Segment Address record\n";
              } else {
                  ea = addr << 4;
              }
              break;
          case 3:               // Start Address Segment
              break;
          case 4:               // Extended Linear Address
              if (addr != 0 || nbytes != 2) {
                  std::cerr << "Invalid Extended Linear Address record\n";
              } else {
                  ea = strtoul( line.substr( 9, 4 ).c_str( ), 0, 16 ) << 16;
              }
              break;
          case 5:               // Start Linear Address
              break;
          default:
              std::cerr << "Unknown Intel HEX record " << type << std::endl;
              break;
        }
    }
}

FlashImage::~FlashImage( void )
{

}
