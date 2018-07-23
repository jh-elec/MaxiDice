/**********************************************************************************
* C Library for the display modules
* XV-GDM128X64 and XV-GDM128X64-RGB from XV-Display.de
*
* Section: Display_Interface_Xmega.h.h
*
* Revision 1
* 2013 Feb. 07
*
* AVR and AVR Xmega microcontrollers,
* SoftwareSPI and HardwareSPI supported.
*
*
* Recommended development environment:
*
* Atmel Studio 6
* ---> http://www.atmel.com/Microsite/atmel_studio6/default.aspx
* Developmentboard Nano from dev-tools.de.
* ---> http://www.dev-tools.de/index.php/development-boards/developmentboard-nano
*
* We invests time and resources providing this open source code,
* please support www.avr-praxis.de and open-source hardware
* by purchasing products from www.mikrocontroller-praxis.de!
*
* Written by Dirk Rodenhausen (dirk at avr-praxis.de)
*
* Our webpages:
* http://www.avr-praxis.de
* German discussion forum related to AVR microcontroller family.
* http://www.mikrocontroller-praxis.de
* Onlineshop where you can purchase the XV-Display display modules and
* the Dev-Tools Nano development board.
* http://www.dev-tools.de
* Further resources of products available from www.mikrocontroller-praxis.de.
*
*
* BSD license, all text above must be included in any redistribution.
* Please read the License.txt file.
* AVR is a registered trademark of Atmel corporation.
**********************************************************************************/

#ifndef __XVDISPLAY_XMEGA_H__
#define __XVDISPLAY_XMEGA_H__



    #define DISPLAY_SCK_LOW       DISPLAY_SPI_PORT.OUTCLR = 1<<DISPLAY_SPI_SCK;
    #define DISPLAY_SCK_HIGH      DISPLAY_SPI_PORT.OUTSET = 1<<DISPLAY_SPI_SCK;
    
    #define DISPLAY_MOSI_LOW      DISPLAY_SPI_PORT.OUTCLR = 1<<DISPLAY_SPI_MOSI;
    #define DISPLAY_MOSI_HIGH     DISPLAY_SPI_PORT.OUTSET = 1<<DISPLAY_SPI_MOSI;

    #define DISPLAY_CS_LOW        DISPLAY_CONTROL_PORT.OUTCLR = 1<<DISPLAY_SIGNAL_CS;
    #define DISPLAY_CS_HIGH       DISPLAY_CONTROL_PORT.OUTSET = 1<<DISPLAY_SIGNAL_CS;

    #define DISPLAY_A0_LOW        DISPLAY_CONTROL_PORT.OUTCLR = 1<<DISPLAY_SIGNAL_A0;
    #define DISPLAY_A0_HIGH       DISPLAY_CONTROL_PORT.OUTSET = 1<<DISPLAY_SIGNAL_A0;
    
    #define DISPLAY_RESET_LOW     DISPLAY_CONTROL_PORT.OUTCLR = 1<<DISPLAY_SIGNAL_RESET;
    #define DISPLAY_RESET_HIGH    DISPLAY_CONTROL_PORT.OUTSET = 1<<DISPLAY_SIGNAL_RESET;

static inline void _Display_InitInterface(void)
{

  /***
  DISPLAY CONTROL SIGNALS
  ***/

  DISPLAY_CONTROL_PORT.OUTSET = (1<<DISPLAY_SIGNAL_CS) | (1<<DISPLAY_SIGNAL_A0) | (1<<DISPLAY_SIGNAL_RESET);
  DISPLAY_CONTROL_PORT.DIRSET = (1<<DISPLAY_SIGNAL_CS) | (1<<DISPLAY_SIGNAL_A0) | (1<<DISPLAY_SIGNAL_RESET);  // CS, A0, RESET: Output, high


  /***
  SPI software interface
  ***/
  DISPLAY_SPI_PORT.OUTSET = (1<<DISPLAY_SPI_MOSI) | (1<<DISPLAY_SPI_SCK);  // MOSI, SCK: Output, high
  DISPLAY_SPI_PORT.DIRSET = (1<<DISPLAY_SPI_MOSI) | (1<<DISPLAY_SPI_SCK);
  

  #if (DISPLAY_SPI_MODE_HARDWARESPI == true)
    /***
    Hardware SPI
    ***/

    #if ((DISPLAY_SPI_SPEED_FCPU_DIV != 2) && (DISPLAY_SPI_SPEED_FCPU_DIV != 4) && (DISPLAY_SPI_SPEED_FCPU_DIV != 8) \
	    && (DISPLAY_SPI_SPEED_FCPU_DIV != 16) && (DISPLAY_SPI_SPEED_FCPU_DIV != 32) && (DISPLAY_SPI_SPEED_FCPU_DIV != 64) \
	    && (DISPLAY_SPI_SPEED_FCPU_DIV != 128))

        #warning No valid definition of DISPLAY_SPI_SPEED_FCPU_DIV. Using F_CPU/128 for SPI clock! \
                 DISPLAY_SPI_SPEED_FCPU_DIV will be found in config.h
    #endif

    switch (DISPLAY_SPI_SPEED_FCPU_DIV)
	{
      case 2:    DISPLAY_SPI_MODULE.CTRL = SPI_CLK2X_bm;
	             break;
      case 4:    DISPLAY_SPI_MODULE.CTRL = 0;
	             break;
	  case 8:    DISPLAY_SPI_MODULE.CTRL = SPI_CLK2X_bm | (1 << SPI_PRESCALER_gp);
				 break;
      case 16:   DISPLAY_SPI_MODULE.CTRL = 1 << SPI_PRESCALER_gp;
                 break;
      case 32:   DISPLAY_SPI_MODULE.CTRL = SPI_CLK2X_bm | (2 << SPI_PRESCALER_gp);
				 break;
      case 64:   DISPLAY_SPI_MODULE.CTRL = 2 << SPI_PRESCALER_gp;
				 break;
      default:   DISPLAY_SPI_MODULE.CTRL = 3 << SPI_PRESCALER_gp;

    }
    
	DISPLAY_SPI_MODULE.CTRL |= SPI_ENABLE_bm | SPI_MODE_3_gc | SPI_MASTER_bm;        // SPI Enable, Slave, SPI Mode 3 
	
  #endif



}  




#if (DISPLAY_SPI_MODE_HARDWARESPI == true)


  /***
  Hardware SPI
  ***/

  static inline void _Display_SPI_Write (uint8_t output)
  {

    DISPLAY_SPI_MODULE.DATA = output;

    while(!(DISPLAY_SPI_MODULE.STATUS & SPI_IF_bm)) { }
					
  }		

#else



  /***
  Software SPI
  ***/

  static inline void _Display_SPI_Write (uint8_t output)
  {
    uint8_t input=0;
    uint8_t i;
	
    for (i=0; i<8; i++)
    {
      DISPLAY_SCK_LOW    // SCK low

      if (output & 0x80)
        DISPLAY_MOSI_HIGH // MOSI high
      else
        DISPLAY_MOSI_LOW // MOSI low

      DISPLAY_SCK_HIGH    // SCK high

      output <<= 1;

    }
	
  }		



#endif


#if DISPLAY_BACKLIGHT_CONTROL == true
static inline void _Display_InitBacklight(void)
{
	
	#if DISPLAY_BACKLIGHT_INVERTED == true
	  DISPLAY_BACKLIGHT_PORT.OUTSET = (1<<DISPLAY_BACKLIGHT_RED) | (1<<DISPLAY_BACKLIGHT_GREEN) | (1<<DISPLAY_BACKLIGHT_BLUE); // high
	#else
	  DISPLAY_BACKLIGHT_PORT.OUTCLR= (1<<DISPLAY_BACKLIGHT_RED) | (1<<DISPLAY_BACKLIGHT_GREEN) | (1<<DISPLAY_BACKLIGHT_BLUE);  // low
	#endif
	
	DISPLAY_BACKLIGHT_PORT.DIRSET =  (1<<DISPLAY_BACKLIGHT_RED) | (1<<DISPLAY_BACKLIGHT_GREEN) | (1<<DISPLAY_BACKLIGHT_BLUE); // RED, GREEN, BLUE: Output
	
}

#if DISPLAY_BACKLIGHT_INVERTED == false

#define DISPLAY_BACKLIGHT_RED_OFF		DISPLAY_BACKLIGHT_PORT.OUTCLR =  (1<<DISPLAY_BACKLIGHT_RED)
#define DISPLAY_BACKLIGHT_RED_ON		DISPLAY_BACKLIGHT_PORT.OUTSET =  (1<<DISPLAY_BACKLIGHT_RED)

#define DISPLAY_BACKLIGHT_GREEN_OFF		DISPLAY_BACKLIGHT_PORT.OUTCLR =  (1<<DISPLAY_BACKLIGHT_GREEN)
#define DISPLAY_BACKLIGHT_GREEN_ON		DISPLAY_BACKLIGHT_PORT.OUTSET =  (1<<DISPLAY_BACKLIGHT_GREEN)

#define DISPLAY_BACKLIGHT_BLUE_OFF		DISPLAY_BACKLIGHT_PORT.OUTCLR =  (1<<DISPLAY_BACKLIGHT_BLUE)
#define DISPLAY_BACKLIGHT_BLUE_ON		DISPLAY_BACKLIGHT_PORT.OUTSET =  (1<<DISPLAY_BACKLIGHT_BLUE)

#else
#define DISPLAY_BACKLIGHT_RED_ON		DISPLAY_BACKLIGHT_PORT.OUTCLR =  (1<<DISPLAY_BACKLIGHT_RED)
#define DISPLAY_BACKLIGHT_RED_OFF		DISPLAY_BACKLIGHT_PORT.OUTSET =  (1<<DISPLAY_BACKLIGHT_RED)

#define DISPLAY_BACKLIGHT_GREEN_ON		DISPLAY_BACKLIGHT_PORT.OUTCLR =  (1<<DISPLAY_BACKLIGHT_GREEN)
#define DISPLAY_BACKLIGHT_GREEN_OFF		DISPLAY_BACKLIGHT_PORT.OUTSET =  (1<<DISPLAY_BACKLIGHT_GREEN)

#define DISPLAY_BACKLIGHT_BLUE_ON		DISPLAY_BACKLIGHT_PORT.OUTCLR =  (1<<DISPLAY_BACKLIGHT_BLUE)
#define DISPLAY_BACKLIGHT_BLUE_OFF		DISPLAY_BACKLIGHT_PORT.OUTSET =  (1<<DISPLAY_BACKLIGHT_BLUE)

#endif

#endif



#endif

