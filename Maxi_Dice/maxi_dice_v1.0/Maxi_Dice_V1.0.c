
/************************************************************************************************************
*											Maxi Dice														*
*								   ____________________________  											*
*																											*
*								- Würfel auf einer 8 x 8 LED Matrix											*
*																											*
* 						Maxi Dice Schaltet sich nach ca. einer halben Sekunde aus 							*
*						und sorgt dafür das der Akku nicht so schnell entladen wird.						*
*						Dies sorgt für eine menge Spielspaß. Der Akku braucht ca. 3 Stunden					*
*						bis er Voll geladen ist. Dies wird mit den Status LED´s (nähe der USB				*
*						Buchse optisch dargestellt.															*	
*																											*
*						Der Würfel (Maxi Dice) ist für eine Spannung von max 5 VDC ausgelegt.				*
*																											*
*						Entwicklung der Software : Jan Homann / Daniel Welther (Zeichensatz / Grafiken)		*
*						Entwicklung der Hardware : Hardo Grubitz / Jan Homann								*
************************************************************************************************************/


/***********************************************************************************************************/
												/*** SETTINGS ***/
						
									//#define CONTINUOUS_ACCU_LOADING_ANIMATION
			
			

/************************************************************************************************************/


/* Definiert die Frequenz */
#define F_CPU 16000000UL

/* Umbennung der Eingabe ( Taster ) */
#define SPIELER_3	(!(PIND & (1<<PIND3)))
#define SPIELER_1	(!(PIND & (1<<PIND5)))
#define SPIELER_2	(!(PIND & (1<<PIND6)))
#define SPIELER_4	(!(PIND & (1<<PIND4)))
	
/* USB Ladeerkennung */
#define USB_CONNECTED PINB & (1<<PINB4)

/* Akku muss geladen werden */	
#define ACCU_MUST_LOADING (!(PIND & (1<<PIND7)))
	
/* Summer */
#define SUMMER_AUS	  PORTC |=  (1<<PC2)
#define SUMMER_AN     PORTC &= ~(1<<PC2)
#define SUMMER_TOGGLE PORTC ^=  (1<<PC2)
	
/* Richtung zum schieben der Anzeige */
#define SHIFT_LEFT  0
#define SHIFT_RIGHT 1
#define STAND_STILL 2
	
/* Schwelle für AutoOFF ( 6000 = 1 min. ) 12000 */ 
#define AUTO_OFF 12000
	
/* Verhindert ein drücken während des Betriebes */
#define INTERRUPT_INT0_DISABLE GICR &= ~(1<<INT0)
#define INTERRUPT_INT0_ENABLE  GICR |=  (1<<INT0)
	
	
/* alias OutputCompareMatch1 Interrupt */
#define AUTO_OFF_ISR_ENABLE		TIMSK  |=  (1<<OCIE1A);
#define AUTO_OFF_ISR_DISABLE	TIMSK  &= ~(1<<OCIE1A);
		
/* Benötigte Bibliotheken */									
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <avr/pgmspace.h>
#include <string.h>
#include <stdlib.h>
#include "Font_8x8.h"
#include "build_info.h"

/* Benötigte Variablen */
uint16_t VRAM[16] = {};
uint8_t USB_STATE = 0;
uint16_t AUTO_OFF_Counter = 0;
uint16_t CharacterIndexValue = 0;
uint8_t Gewuerfelte_Zahl;
uint8_t Werbung = 0;
uint8_t PWM_Summer = 0;

/* Benötigte Unterprogramme (Warum so viele ? Damit es übersichtlicher bleibt!) */
void clear_screen(void);
void fill_screen(void);
void check_usb(void);
void Display_all_Symbols();
void ShiftVRAM();
void StringtoCharacter(const char *s);
void check_ACCU(void);
void check_AUTO_OFF();
void DisplayString( const char *s );
void ScrollLeft(void);
void DisplayString( const char *s );
void StartUP_Text_Maxi_Dice(void);
void Wuerfeln(void);
void Blink_Display_with_acutall_Number(uint8_t Player, uint8_t Anzahl_Blinken, uint16_t Time_On, uint16_t Time_Off);
void Matrix_Test_Mode(void);
uint8_t Show_Wuerfel_Effekte(void);
uint8_t GetCharacterIndex(char a);
void Show_Werbung(void);
void Summer_PWM(void);
void sleep(void);

void scrollMessage( char *msg )
{
	uint8_t z1 = 0 , z2 = 0;
	while ( *msg )
	{
		for ( uint8_t x = 0 ; x < 16 ; x++ )
		{
			for( uint8_t i = 0 ; i < 8 ; i++ )
			{
				z1 = ~pgm_read_byte(&charMap[ GetCharacterIndex( 'a' )  ] [ i ] );
				z2 = ~pgm_read_byte(&charMap[ GetCharacterIndex( 'b' )  ] [ i ] );
				VRAM[ i + 8 ] = z1 << x ;
			}	
			_delay_ms(250);					
		}
	}
}
	
uint8_t ScrollPrint ( char *msg )
{
	union
	{
		unsigned int V16Bit;
		unsigned char V8Bit[2];
	} tmp;

	uint8_t scrollPos;
	uint8_t leng;
	for ( leng = 0 ; *msg != '\0' ; leng++ )
	{
		for ( scrollPos = 0 ; scrollPos < 16 ; scrollPos++ )
		{
			for( uint8_t i = 0 ; i < 8 ; i++ )
			{
				tmp.V16Bit	=  0xFF & ~pgm_read_byte(&charMap[ GetCharacterIndex( *msg ) ][i]);
				tmp.V16Bit	= tmp.V16Bit << scrollPos; // nach links verschieben
				VRAM[i]		= tmp.V8Bit[1]; // rechte Seite ausgeben
			}
			_delay_ms(250);		
		}
		msg++;		
	}
	
	return leng;
}
	
int main(void)
{
	
	/* Benötigte I/O´s definieren */
	DDRA |= ((1<<PA0) | (1<<PA1) | (1<<PA2) | (1<<PA3) | (1<<PA4) | (1<<PA5) | (1<<PA6) | (1<<PA7));
	DDRB |= ((1<<PB0) | (1<<PB1) | (1<<PB2) | (1<<PB3) | (1<<PB5) | (1<<PB6) | (1<<PB7));
	DDRC |= ((1<<PC2) | (1<<PC3) | (1<<PC4) | (1<<PC5) | (1<<PC6) | (1<<PC7));
	
	/* TIMER0 CompareMatch (Multiplexen der Anzeige) */
	TCCR0 |= (1<<WGM01) | (1<<WGM00) | (1<<CS01) | (1<<CS00);
	TIMSK |= (1<<OCIE0);
		
	/* Timer 1 Initalisieren */
	TCCR1B |= ((1<<CS12) | (1<<CS10));   // Teiler auf 1028
	TCCR1B |= (1<<WGM12);				// CTC = ClearTimerCompare
	OCR1A   = 77;						// ca. jede 10 ms. ein Überlauf
	TIMSK  |= (1<<OCIE1A);				// OutputCompareMatch 1 Enable
	
	/* Interrupt "0" aktivieren */
	GICR |= (1<<INT0);
	
	/* Muss ausgeschaltet werden, weil der Summer sonst dauernd an ist */
	SUMMER_AUS;
	
	/* Interrupts global erlauben */
    sei(); 	
	
	/* Sorgt dafür das während des Lauftextes kein Taster (die an INT0 angeschlossen sind) gedrückt wird */
	INTERRUPT_INT0_DISABLE;
	
// 	while(1)
// 	{
// 		ScrollPrint( "HHHHHHH");
// 	}
	
	
	while (1)
	{
		/* Start Lauftext (wird nur einmal nach Flashen oder einlegen eines neuen Akkus dargestellt (Warum? Damit nicht nur Werbung läuft :-P) */
		DisplayString( "Hallo" );	
	}

	  
	/* Stellt einen Smiley da */
	for (uint8_t d= 0 ; d < 8 ; d++)
	{
	  	VRAM[d] = ~(pgm_read_byte(&charMap[64][d]));
	}
		_delay_ms(2000);
	 
	/* Interrupt "INT0" wieder aktivieren / erlauben */
	INTERRUPT_INT0_ENABLE;
	

	
    while(1)
    {	
		
		
		INTERRUPT_INT0_DISABLE;		
		check_ACCU();
		check_usb();
		check_AUTO_OFF();		
		
		/* Sorgt dafür, dass wenn via. USB geladen wird, nichts anderes ausgeführt wird */
		if (USB_STATE == 0)
		{
			Matrix_Test_Mode();
			Wuerfeln();	
		}	
	
	

	
	}// Ende While 

}// Ende Main



/* Multiplexen der Anzeige */
ISR(TIMER0_COMP_vect)
{
    static volatile uint8_t spalte = 0;

    PORTC &= ~((1<<PC7) | (1<<PC6) | (1<<PC5) | (1<<PC4) | (1<<PC3));
    PORTB &= ~((1<<PB2) | (1<<PB1) | (1<<PB0));
    
    // neue Zeilendaten ausgeben
  	
	PORTA = ~(VRAM[spalte]);
		
    // Spalte aktivieren	
    switch (spalte) {
	    case 0: PORTC |= (1<<PC3);
	    break;
	    case 1: PORTC |= (1<<PC4);
	    break;
	    case 2: PORTC |= (1<<PC5);
	    break;
	    case 3: PORTC |= (1<<PC6);
	    break;
	    case 4: PORTC |= (1<<PC7);
	    break;
	    case 5: PORTB |= (1<<PB2);
	    break;
	    case 6: PORTB |= (1<<PB1);
	    break;
	    case 7: PORTB |= (1<<PB0);
	    break;
	    default: break;
    }

    spalte++; // nächste Spalte
    if (spalte >= 8) spalte = 0; // Spalten 0..7		
}

/* Jede 10 ms wird hier die Variable "Save_Value_Counter" um + 1 erhöht */
ISR (TIMER1_COMPA_vect)
{
	AUTO_OFF_Counter = AUTO_OFF_Counter + 1;		
}

/* Aufwecken des Gerätes */
ISR (INT0_vect)
{
	AUTO_OFF_Counter = 0;
}

/* Löschen des aktuellen Displays */
void clear_screen(void) 
{
	for(uint8_t i = 0 ; i < 8 ; i++)
	{
		VRAM[i] = 0x00;
	}	
}

/* Display komplett anschalten */
void fill_screen(void)
{
	for (uint8_t i = 0 ; i < 8 ; i++)
	{
		VRAM[i] = 0xFF;
	}
}

/* Checkt ob Maxi Dice mit USB Verbunden ist */
void check_usb(void)
{
	Matrix_Test_Mode();

	if (USB_CONNECTED)
	{
		
	#ifdef CONTINUOUS_ACCU_LOADING_ANIMATION
		
		AUTO_OFF_ISR_DISABLE;
		
	#endif
		
		USB_STATE = 1;
		
		for (uint8_t i = 65 ; i < 70 ; i++)
		{
			for (uint8_t d= 0 ; d < 8 ; d++)
			{
					VRAM[d] = ~(pgm_read_byte(&charMap[i][d]));			
			}
			 _delay_ms(500);
		}
	   
	 }
		else
	{
		USB_STATE = 0;
		
	#ifdef CONTINUOUS_ACCU_LOADING_ANIMATION
	
		AUTO_OFF_ISR_ENABLE;
		
	#endif
	
	}
		

		
}

/* Routine um das gesamte Grafikaray da zu stellen */
void Display_all_Symbols()
{
	if (USB_STATE == 0)
	{
		for (uint8_t i = 0 ; i < 72 ; i++)
		{

			for (uint8_t d= 0 ; d < 8 ; d++)
			{
				VRAM[d] = ~(pgm_read_byte(&charMap[i][d]));
			}
			check_usb();
			
			_delay_ms(150);
				}// Ende for
				
				
			for (uint8_t i = 3 ; i < 9 ; i++)
			{

				for (uint8_t d= 0 ; d < 8 ; d++)
				{
					VRAM[d] = ~(pgm_read_byte(&charMap[i][d]));
				}
							
					_delay_ms(500);
			}
			
				
			
			for (uint8_t i = 0 ; i < 6 ; i++)
			{

				for (uint8_t d= 0 ; d < 8 ; d++)
				{
					VRAM[d] = ~(pgm_read_byte(&charSet_Spieler_2[i][d]));
				}
						
					_delay_ms(500);
			}


			for (uint8_t i = 0 ; i < 6 ; i++)
			{

					for (uint8_t d= 0 ; d < 8 ; d++)
					{
						VRAM[d] = ~(pgm_read_byte(&charSet_Spieler_3[i][d]));
					}
														
						_delay_ms(500);
			}
													

			for (uint8_t i = 0 ; i < 6 ; i++)
			{
			
					for (uint8_t d= 0 ; d < 8 ; d++)
					{
						VRAM[d] = ~(pgm_read_byte(&charSet_Spieler_4[i][d]));
					}
								
						_delay_ms(500);
			}
							

	}
}

/* Errechnet den Index des aktuellen Zeichens */
uint8_t GetCharacterIndex(char a)
{
		uint8_t index = 1;    // alles aus

		if ((a >= '0') && (a <= '9'))
		index = a+2-48;  // erste Zahl ist Index in der Tabelle des ersten Zeichens, zweite Zahl ist Index in Ascii Code des ersten Zeichens
		else if ((a >= 'A') && (a <= 'Z'))
		index = a+12-65;
		else if ((a >= 'a') && (a <= 'z'))
		index = a+38-97;
		else if ((a == '.'))
		index = a+70-46;	
		else if ((a >= '/'))
		index = a+71-47;
			
		return index;
}

/* Überprüft ob Akku leer ist */
void check_ACCU(void)
{
	clear_screen();
	
	if ((ACCU_MUST_LOADING))
	{
		for (uint8_t i = 65 ; i < 66 ; i++)
			{
	
				for (uint8_t d= 0 ; d < 8 ; d++)
				{
					VRAM[d] = ~(pgm_read_byte(&charMap[i][d]));
				}
				check_usb();
				_delay_ms(500);
						
			}// Ende for
	
		}
}

/* Funktion für AutoOff */
void check_AUTO_OFF(void)
{
	set_sleep_mode(SLEEP_MODE_PWR_DOWN);
			
	if (AUTO_OFF_Counter >= AUTO_OFF)
	{
													
		INTERRUPT_INT0_DISABLE;

			
		AUTO_OFF_Counter = 0;
			
		SUMMER_AN;
		_delay_ms(4);
		SUMMER_AUS;
				
		sleep();
			
		TIMSK &= ~(1<<OCIE0);
		
		PORTA = 0x00;
		PORTB = 0x00;
		PORTC = 0x00;
		PORTD = 0x00;
				
		DDRA = 0x00;
		DDRB = 0x00;
		DDRC = 0x00;
		DDRD = 0x00;
		
		INTERRUPT_INT0_ENABLE;
				
		clear_screen();
		sleep_enable();
		sleep_cpu();
		sleep_disable();
		INTERRUPT_INT0_DISABLE;
					
		for (uint8_t x = 0 ; x < 250 ; x++)

	
		TIMSK |= (1<<OCIE0);
		DDRA |= ((1<<PA0) | (1<<PA1) | (1<<PA2) | (1<<PA3) | (1<<PA4) | (1<<PA5) | (1<<PA6) | (1<<PA7));
		DDRB |= ((1<<PB0) | (1<<PB1) | (1<<PB2) | (1<<PB3) | (1<<PB5) | (1<<PB6) | (1<<PB7));
		DDRC |= ((1<<PC2) | (1<<PC3) | (1<<PC4) | (1<<PC5) | (1<<PC6) | (1<<PC7));		
					
		SUMMER_AN;
		_delay_ms(4);
		SUMMER_AUS;
						
		StartUP_Text_Maxi_Dice();
		
		Show_Werbung();
				
	}
}

/* Schreibt den aktuellen String auf die Matrix */
void DisplayString( const char *s)
{
	uint16_t len;
	uint8_t i, d, u1, u2;
	
	len = strlen(s)*8;
	
	for (uint16_t x = 0 ; x < len ; x++ )
	{
		i = x >> 3; // Index vom ersten Zeichen
		u1 = GetCharacterIndex(s[i++]); // erstes Zeichen
		u2 = GetCharacterIndex(s[i]);  // zweites Zeichen
		for (d=0 ; d<16 ; d++)
		{
			VRAM[d] = 0xFFFF;
			VRAM[d] &= ~(pgm_read_byte(&charMap[u1][d]) << (( x & 7 )) ); // erstes Zeichen verschoben nach links
			VRAM[d] &= ~(pgm_read_byte(&charMap[u2][d]) >> ( 8 - ( x & 7 )) );  // zweites Zeichen verschoben nach rechts
		}	
		_delay_ms(80);	
	}
}

/* Hier wird die Zahl gewürfelt, die später im VRAM steht */
void Wuerfeln(void)
{
	
		if (SPIELER_1)
		{	
			Gewuerfelte_Zahl = Show_Wuerfel_Effekte();
			_delay_ms(500);		
			AUTO_OFF_Counter = 0;
			for (uint8_t d= 0 ; d < 8 ; d++)
			{
				VRAM[d] = ~(pgm_read_byte(&charSet_Spieler_1[Gewuerfelte_Zahl][d]));
			}
			Blink_Display_with_acutall_Number(1 , 5 , 300 , 200);
			Matrix_Test_Mode();
			
		}
				
		if (SPIELER_2)
		{
			Gewuerfelte_Zahl = Show_Wuerfel_Effekte();
			_delay_ms(500);
			AUTO_OFF_Counter = 0;
			for (uint8_t d= 0 ; d < 8 ; d++)
			{
				VRAM[d] = ~(pgm_read_byte(&charSet_Spieler_2[Gewuerfelte_Zahl][d]));
			}
			Blink_Display_with_acutall_Number(2 , 5 , 300 , 200);
			Matrix_Test_Mode();
		}
				
		if (SPIELER_3)
		{
			Gewuerfelte_Zahl = Show_Wuerfel_Effekte();
			_delay_ms(500);
			AUTO_OFF_Counter = 0;
			for (uint8_t d= 0 ; d < 8 ; d++)
			{
				VRAM[d] = ~(pgm_read_byte(&charSet_Spieler_3[Gewuerfelte_Zahl][d]));
			}
			Blink_Display_with_acutall_Number(3 , 5 , 300 , 200);
			Matrix_Test_Mode();
		}

		if (SPIELER_4)
		{
			Gewuerfelte_Zahl = Show_Wuerfel_Effekte();
			_delay_ms(500);
			AUTO_OFF_Counter = 0;
			for (uint8_t d= 0 ; d < 8 ; d++)
			{
				VRAM[d] = ~(pgm_read_byte(&charSet_Spieler_4[Gewuerfelte_Zahl][d]));
			}
			Blink_Display_with_acutall_Number(4 , 5 , 300 , 200);
			Matrix_Test_Mode();
		}
}

/* Hier entscheidet der Zufallsgenerator wer am weitesten kommt :-P */
uint8_t Show_Wuerfel_Effekte(void)
{
	uint8_t Ergebniss;

	
	for (uint8_t i = 0 ; i < 15 ; i++)
	{	
		Ergebniss = (rand() % 6);	
			
		for (uint8_t d= 0 ; d < 8 ; d++)
		{
			
			VRAM[d] = ~(pgm_read_byte(&Wuerfel_Effekte[Ergebniss][d]));
		}
		_delay_ms(100);
	}
	
	return Ergebniss;
}

/* Durch blinken wird die gewürfelte Zahl dargestellt */
void Blink_Display_with_acutall_Number(uint8_t Player, uint8_t Anzahl_Blinken, uint16_t Time_On, uint16_t Time_Off)
{
	
	switch (Player)
	{
		case 1 :
		{
			for (uint16_t x = 0; x < Anzahl_Blinken; x++)
			{
				for (uint16_t d= 0 ; d < 8 ; d++)
				{
					VRAM[d] = ~(pgm_read_byte(&charSet_Spieler_1[Gewuerfelte_Zahl][d]));
				}
	
				for (uint16_t x = 0 ; x < Time_On ; x++)
				{
					_delay_ms(1);
				}
	
				clear_screen();
				
				for (uint16_t x = 0 ; x < Time_Off ; x++)
				{
					_delay_ms(1);
				}
	
				
			}
		}
			break;
		
		
		case 2 :
		{
			for (uint16_t x = 0; x < Anzahl_Blinken ; x++)
			{
				for (uint16_t d= 0 ; d < 8 ; d++)
				{
					VRAM[d] = ~(pgm_read_byte(&charSet_Spieler_2[Gewuerfelte_Zahl][d]));
				}
				
				for (uint16_t x = 0 ; x < Time_On ; x++)
				{
					_delay_ms(1);
				}
				
				clear_screen();
				
				for (uint16_t x = 0 ; x < Time_Off ; x++)
				{
					_delay_ms(1);
				}
			}
		}
			break;
				
		case 3 :
		{
			for (uint16_t x = 0; x < Anzahl_Blinken ; x++)
			{
				for (uint16_t d= 0 ; d < 8 ; d++)
				{
					VRAM[d] = ~(pgm_read_byte(&charSet_Spieler_3[Gewuerfelte_Zahl][d]));
				}
				
				for (uint16_t x = 0 ; x < Time_On ; x++)
				{
					_delay_ms(1);
				}
				
				clear_screen();
				
				for (uint16_t x = 0 ; x < Time_Off ; x++)
				{
					_delay_ms(1);
				}
			}
		}
			break;

		case 4 :
		{
			for (uint16_t x = 0; x < Anzahl_Blinken ; x++)
			{
				for (uint16_t d= 0 ; d < 8 ; d++)
				{
					VRAM[d] = ~(pgm_read_byte(&charSet_Spieler_4[Gewuerfelte_Zahl][d]));
				}
				
				for (uint16_t x = 0 ; x < Time_On ; x++)
				{
					_delay_ms(1);
				}
				
				clear_screen();
				
				for (uint16_t x = 0 ; x < Time_Off ; x++)
				{
					_delay_ms(1);
				}
			}
		}
			break;
	
	}
}

/* Wenn alle 4 Taster solange gedrückt werden bis 4 mal gewürfelt wurde, kommt man in den Testmode! (LED´s werden zyklisch 254 mal an / aus gemacht) */
void Matrix_Test_Mode()
{
	if ((SPIELER_1) && (SPIELER_2) && (SPIELER_3) && (SPIELER_4))
	{	
		char s[] = " TEST MODE ";
		DisplayString(s);

	while (1)
	{	
		
              /* from right to left */
              for (uint16_t x = 1 ; x <= 255 ; x = x * 2)
              {
	              VRAM[0] = x;
	              _delay_ms(100);
	              clear_screen();  	
              }
              
              /* from left to right */
              for (uint16_t x = 128 ; x >= 1 ; x = x / 2)
              {
	              VRAM[1] = x;
	              _delay_ms(100);
	              clear_screen();  
              }
              
              /* from right to left */
              for (uint16_t x = 1 ; x <= 255 ; x = x * 2)
              {
	              VRAM[2] = x;
	              _delay_ms(100);
	              clear_screen();  
              }
              
              /* from left to right */
              for (uint16_t x = 128 ; x >= 1 ; x = x / 2)
              {
	              VRAM[3] = x;
	              _delay_ms(100);
	              clear_screen();  
              }
              
              /* from right to left */
              for (uint16_t x = 1 ; x <= 255 ; x = x * 2)
              {
	              VRAM[4] = x;
	              _delay_ms(100);
	              clear_screen();  
              }
              
              /* from left to right */
              for (uint16_t x = 128 ; x >= 1 ; x = x / 2)
              {
	              VRAM[5] = x;
	              _delay_ms(100);
	              clear_screen();  
              }
              
              /* from right to left */
              for (uint16_t x = 1 ; x <= 255 ; x = x * 2)
              {
	              VRAM[6] = x;
	              _delay_ms(100);
	              clear_screen();  
              }
              
              /* from left to right */
              for (uint16_t x = 128 ; x >= 1 ; x = x / 2)
              {
	              VRAM[7] = x;
	              _delay_ms(100);
	              clear_screen(); 
              }		
			
			fill_screen();
			_delay_ms(1500);
			clear_screen();

			if ((SPIELER_1) || (SPIELER_2) || (SPIELER_3) || (SPIELER_4))
			{
				break;
			}	
		}
	}	
}

/* Bisschen Werbung muss auch sein */
void Show_Werbung(void)
{
	Werbung ++;
									
	if (Werbung >= 3)
	{
		char s[] = " www.hesch.de ";
		DisplayString(s);
	}								
	Werbung = 0;									
}

/* PWM für den Summer */
void Summer_PWM(void)
{
		static uint8_t counter = 0;
		counter ++;
		
		if (counter >= 100)
		{
			counter = 0;
			
			if (PWM_Summer > 0)
			{
				SUMMER_AN;
			}
			else
			{
				SUMMER_AUS;
			}
				
		}
		else
		{

			
			if (counter ==  PWM_Summer)
			{
				SUMMER_AUS;
			}
			
		}
}

/* Text bevor "Maxi Dice" schclafen geht */
void sleep(void)
{
	char s[] = " Sleep";
	DisplayString( s );
}

/* Beim jeden "aufwecken" von Maxi Dice, wird der StartUP_Text... dargestellt */
void StartUP_Text_Maxi_Dice(void)
{
	char s[] = " Wake Up";
	DisplayString(s);
}