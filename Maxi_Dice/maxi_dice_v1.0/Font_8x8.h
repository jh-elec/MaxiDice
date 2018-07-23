/* Grafiken */

const char charMap[72][8] PROGMEM ={
	 {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}, // an
	 {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}, // aus
	 {0xC3,0x99,0x99,0x99,0x99,0x99,0xC3,0xFF}, // null
	 {0xE7,0xC7,0xE7,0xE7,0xE7,0xE7,0xE7,0xFF}, // eins
	 {0xC3,0x99,0xF9,0xF3,0xE7,0xCF,0x81,0xFF}, // zwei
	 {0xC3,0x99,0xF9,0xE3,0xF9,0x99,0xC3,0xFF}, // drei
	 {0xF3,0xE3,0xC3,0x93,0x81,0xF3,0xF3,0xFF}, // vier
	 {0x81,0x9F,0x83,0xF9,0xF9,0x99,0xC3,0xFF}, // fünf
	 {0xE3,0xCF,0x9F,0x83,0x99,0x99,0xC3,0xFF}, // sechs
	 {0x81,0xF9,0xF9,0xF3,0xE7,0xE7,0xE7,0xFF}, // sieben
	 {0xC3,0x99,0x99,0xC3,0x99,0x99,0xC3,0xFF}, // acht
	 {0xC3,0x99,0x99,0xC1,0xF9,0xF3,0xC7,0xFF}, // neun
	 {0xe7,0xdb,0xdb,0xdb,0xdb,0xc3,0xdb,0xdb}, // A
	 {0x87,0xbb,0xbb,0xbb,0x87,0xbb,0xbb,0x87}, // B
	 {0xC3,0x99,0x3F,0x3F,0x3F,0x99,0xC3,0xFF}, // C
	 {0x87,0x93,0x99,0x99,0x99,0x93,0x87,0xFF}, // D
	 {0x81,0x9F,0x9F,0x87,0x9F,0x9F,0x81,0xFF}, // E
	 {0x81,0x9F,0x9F,0x87,0x9F,0x9F,0x9F,0xFF}, // F
	 {0xC3,0x99,0x3F,0x3F,0x31,0x99,0xC1,0xFF}, // G
	 {0x99,0x99,0x99,0x81,0x99,0x99,0x99,0xFF}, // H
	 {0xE7,0xE7,0xE7,0xE7,0xE7,0xE7,0xE7,0xFF}, // I
	 {0xF9,0xF9,0xF9,0xF9,0x99,0x99,0xC3,0xFF}, // J
	 {0x99,0x99,0x93,0x87,0x93,0x99,0x99,0xFF}, // K
	 {0x9f,0x9f,0x9f,0x9f,0x9f,0x9f,0x81,0x81}, // L
	 {0x39,0x11,0x01,0x01,0x29,0x39,0x39,0xFF}, // M
	 {0x39,0x19,0x09,0x21,0x31,0x39,0x39,0xFF}, // N
	 {0xC3,0x99,0x99,0x99,0x99,0x99,0xC3,0xFF}, // O
	 {0x83,0x99,0x99,0x83,0x9F,0x9F,0x9F,0xFF}, // P
	 {0xC3,0x99,0x99,0x99,0x91,0xC3,0xF1,0xFF}, // Q
	 {0x83,0x99,0x99,0x83,0x93,0x99,0x99,0xFF}, // R
	 {0xC3,0x99,0x8F,0xC7,0xF1,0x99,0xC3,0xFF}, // S
	 {0x81,0xE7,0xE7,0xE7,0xE7,0xE7,0xE7,0xFF}, // T
	 {0x99,0x99,0x99,0x99,0x99,0x99,0xC1,0xFF}, // U
	 {0x99,0x99,0x99,0x99,0x99,0xC3,0xE7,0xFF}, // V
	 {0x39,0x39,0x39,0x29,0x01,0x11,0x39,0xFF}, // W
	 {0x99,0x99,0xC3,0xE7,0xC3,0x99,0x99,0xFF}, // X
	 {0x99,0x99,0x99,0xC3,0xE7,0xE7,0xE7,0xFF}, // Y
	 {0x01,0xF9,0xF3,0xE7,0xCF,0x9F,0x01,0xFF}, // Z	 
	 {0xFF,0xFF,0xC3,0xF9,0xC1,0x99,0xC5,0xFF}, // a
	 {0x9F,0x9F,0x9F,0x83,0x99,0x99,0xA3,0xFF}, // b
	 {0xFF,0xFF,0xC3,0x99,0x9F,0x99,0xC3,0xFF}, // c
	 {0xF9,0xF9,0xF9,0xC1,0x99,0x99,0xC5,0xFF}, // d
	 {0xFF,0xFF,0xC3,0x99,0x81,0x9F,0xC3,0xFF}, // e
	 {0xE3,0xC9,0xCF,0x87,0xCF,0xCF,0xCF,0xFF}, // f
	 {0xFF,0xFF,0xC5,0x99,0x99,0xC1,0xF9,0xC3}, // g
	 {0x9F,0x9F,0x93,0x89,0x99,0x99,0x99,0xFF}, // h
	 {0xC3,0xFF,0xC3,0xC3,0xC3,0xC3,0xC3,0xFF}, // i
	 {0xF3,0xFF,0xF3,0xF3,0xF3,0x33,0x33,0x87}, // j
	 {0x9F,0x9F,0x99,0x93,0x87,0x93,0x99,0xFF}, // k
	 {0xE7,0xE7,0xE7,0xE7,0xE7,0xE7,0xE7,0xFF}, // l
	 {0xFF,0xFF,0x39,0x11,0x01,0x29,0x39,0xFF}, // m
	 {0xFF,0xFF,0x83,0x99,0x99,0x99,0x99,0xFF}, // n
	 {0xFF,0xFF,0xC3,0x99,0x99,0x99,0xC3,0xFF}, // o
	 {0xFF,0xFF,0xA3,0x99,0x99,0x83,0x9F,0x9F}, // p
	 {0xFF,0xFF,0xC5,0x99,0x99,0xC1,0xF9,0xF9}, // q
	 {0xFF,0xFF,0xA3,0x89,0x9F,0x9F,0x9F,0xFF}, // r
	 {0xFF,0xFF,0xC1,0x9F,0xC3,0xF9,0x83,0xFF}, // s
	 {0xCF,0xCF,0x83,0xCF,0xCF,0xCB,0xE7,0xFF}, // t
	 {0xFF,0xFF,0x99,0x99,0x99,0x99,0xC5,0xFF}, // u
	 {0xFF,0xFF,0x99,0x99,0x99,0xC3,0xE7,0xFF}, // v
	 {0xFF,0xFF,0x39,0x29,0x01,0x01,0x93,0xFF}, // w
	 {0xFF,0xFF,0x39,0x93,0xC7,0x93,0x39,0xFF}, // x
	 {0xFF,0xFF,0x99,0x99,0x99,0xC1,0xF9,0xC3}, // y
	 {0xFF,0xFF,0x81,0xF3,0xE7,0xCF,0x81,0xFF}, // z		 
	 {0x81,0x7E,0x5A,0x7E,0x42,0x66,0x7E,0x81}, // Smiley
	 {0xFF,0xE7,0xC3,0xDB,0xDB,0xDB,0xDB,0xC3}, // Battery (leer)
	 {0xFF,0xE7,0xC3,0xDB,0xDB,0xDB,0xC3,0xC3}, // Battery ( mit einem Strich  )
	 {0xFF,0xE7,0xC3,0xDB,0xDB,0xC3,0xC3,0xC3}, // Battery ( mit zwei Strichen )
	 {0xFF,0xE7,0xC3,0xDB,0xC3,0xC3,0xC3,0xC3}, // Battery ( mit drei Strichen )
	 {0xFF,0xE7,0xC3,0xC3,0xC3,0xC3,0xC3,0xC3}, // Battery ( mit vier Strichen )
	 {0xFF,0xFF,0xFF,0xE7,0xE7,0xFF,0xFF,0xFF}, // Punkt für´s www(.)
	 {0xFC,0xF8,0xF1,0xE3,0xC7,0x8F,0x1F,0x3F}  // Backslash

	 };


const char charSet_Spieler_1[7][8] PROGMEM ={
	{0xE7,0xC7,0xE7,0xE7,0xE7,0xE7,0xE7,0xFF}, 
	{0xC3,0x99,0xF9,0xF3,0xE7,0xCF,0x81,0xFF}, 
	{0xC3,0x99,0xF9,0xE3,0xF9,0x99,0xC3,0xFF}, 
	{0xF3,0xE3,0xC3,0x93,0x81,0xF3,0xF3,0xFF}, 
	{0x81,0x9F,0x83,0xF9,0xF9,0x99,0xC3,0xFF}, 
	{0xE3,0xCF,0x9F,0x83,0x99,0x99,0xC3,0xFF}
	};

const char charSet_Spieler_2[7][8] PROGMEM ={

	{0xFF,0xE7,0xE7,0xE7,0xE7,0xE7,0xE3,0xE7},
	{0xFF,0x81,0xF3,0xE7,0xCF,0x9F,0x99,0xC3},
	{0xFF,0xC3,0x99,0x9F,0xC7,0x9F,0x99,0xC3},
	{0xFF,0xCF,0xCF,0x81,0xC9,0xC3,0xC7,0xCF},
	{0xFF,0xC3,0x99,0x9F,0x9F,0xC1,0xF9,0x81},
	{0xFF,0xC3,0x99,0x99,0xC1,0xF9,0xF3,0xC7}
	};

const char charSet_Spieler_3[7][8] PROGMEM ={

	{0xFF,0xFF,0xFF,0x01,0x01,0xBF,0xFF,0xFF},
	{0xFF,0x9D,0x0D,0x6D,0x65,0x31,0xB9,0xFF},
	{0xFF,0x93,0x01,0x6D,0x6D,0x39,0xBB,0xFF},
	{0xFF,0xF7,0x01,0x01,0x97,0xC7,0xE7,0xFF},
	{0xFF,0x63,0x41,0x5D,0x5D,0x19,0x1B,0xFF},
	{0xFF,0xF3,0x61,0x6D,0x2D,0x81,0xC3,0xFF}
	};
	
const char charSet_Spieler_4[7][8] PROGMEM ={

	{0xFF,0xFF,0xFD,0x80,0x80,0xFF,0xFF,0xFF}, // eins
	{0xFF,0xBD,0x9C,0x8E,0xA6,0xB0,0xB9,0xFF}, // zwei
	{0xFF,0xDD,0x9C,0xB6,0xB6,0x80,0xC9,0xFF}, // drei
	{0xFF,0xE7,0xE3,0xE9,0x80,0x80,0xEF,0xFF}, // vier
	{0xFF,0xD8,0x98,0xBA,0xBA,0x82,0xC6,0xFF}, // füns
	{0xFF,0xC3,0x81,0xB4,0xB6,0x86,0xCF,0xFF} // sechs
	};
	

const char Wuerfel_Effekte[7][8] PROGMEM ={
	
	{0xFF,0xFF,0xFF,0xE7,0xE7,0xFF,0xFF,0xFF}, // eins
	{0xFC,0xFC,0xFF,0xFF,0xFF,0xFF,0x3F,0x3F}, // zwei
	{0xFC,0xFC,0xFF,0xE7,0xE7,0xFF,0x3F,0x3F}, // drei	
	{0x3C,0x3C,0xFF,0xFF,0xFF,0xFF,0x3C,0x3C}, // vier
	{0x3C,0x3C,0xFF,0xE7,0xE7,0xFF,0x3C,0x3C}, // fünf
	{0x99,0x99,0xFF,0x99,0x99,0xFF,0x99,0x99}, // sechs
	};




