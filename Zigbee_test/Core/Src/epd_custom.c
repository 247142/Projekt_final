#include "epd_custom.h"
#include "EPD_1in9.h"
#include <string.h>

extern const unsigned char DSPNUM_off[];
extern const unsigned char DSPNUM_W0[];
extern const unsigned char DSPNUM_W1[];
extern const unsigned char DSPNUM_W2[];
extern const unsigned char DSPNUM_W3[];
extern const unsigned char DSPNUM_W4[];
extern const unsigned char DSPNUM_W5[];
extern const unsigned char DSPNUM_W6[];
extern const unsigned char DSPNUM_W7[];
extern const unsigned char DSPNUM_W8[];
extern const unsigned char DSPNUM_W9[];

static const unsigned char* NUM_MAP[10] = {
    DSPNUM_W0, DSPNUM_W1, DSPNUM_W2, DSPNUM_W3, DSPNUM_W4,
    DSPNUM_W5, DSPNUM_W6, DSPNUM_W7, DSPNUM_W8, DSPNUM_W9
};

static unsigned char epd_buffer[15];

// Masky pro bajt 13
#define BIT_C_BOTTOM   0x01
#define BIT_DEGREE_C   0x04
#define BIT_BLUETOOTH  0x08
#define BIT_BATTERY    0x10

void EPD_Custom_Init(void) {
    EPD_1in9_init();
    EPD_Custom_Clear();
    EPD_Custom_Update();
}

void EPD_Custom_Clear(void) {
    memcpy(epd_buffer, DSPNUM_off, 15);
}

void EPD_Custom_Update(void) {
    EPD_1in9_init();
    EPD_1in9_lut_5S();
    EPD_1in9_Write_Screen(epd_buffer);
    EPD_1in9_sleep();
}

static void EPD_Custom_WriteDigit(uint8_t pos, uint8_t number) {
    if (number > 9 || pos > 13) return;
    epd_buffer[pos]     = NUM_MAP[number][pos];
    epd_buffer[pos + 1] = NUM_MAP[number][pos + 1];
}

void EPD_Zobraz_Aplikaci(int16_t teplota_x10, uint8_t stav_dveri, bool bluetooth_on) {
    EPD_Custom_Clear();

    // --- 1. TEPLOTA NAHOŘE (např. 254 -> 25.4) ---
    uint8_t desitky  = (teplota_x10 / 100) % 10;
    uint8_t jednotky = (teplota_x10 / 10) % 10;
    uint8_t desetiny = teplota_x10 % 10;

    // Velké číslice nahoře
    if (desitky > 0) EPD_Custom_WriteDigit(1, desitky);
    EPD_Custom_WriteDigit(3, jednotky);
    EPD_Custom_WriteDigit(11, desetiny);

    // Desetinná tečka nahoře (Bit 5 na indexu 4)
    epd_buffer[4] |= 0x20;

    // --- 2. DVEŘE DOLE (Uprostřed na indexu 7) ---
    // Vyčistíme okolí
    epd_buffer[5] = 0x00; epd_buffer[6] = 0x00;
    epd_buffer[9] = 0x00; epd_buffer[10] = 0x00;

    if (stav_dveri == 0) {
        // ZAVŘENO [ ]
        epd_buffer[7] = 0xBF;
        epd_buffer[8] = 0x1F;
    } else {
        // OTEVŘENO [
        epd_buffer[7] = 0xBF;
        epd_buffer[8] = 0x00;
    }

    // --- 3. SYMBOLY (°C a Bluetooth) ---
    // 0x04 | 0x01 je symbol stupně a Céčka
    uint8_t symboly = 0x05;

    // Pokud chceš Bluetooth, přidáme bit 3 (0x08)
    if (bluetooth_on) {
        symboly |= 0x08;
    }

    epd_buffer[13] = symboly;

    // --- FINÁLNÍ ZÁPIS ---
    EPD_Custom_Update();
}
void EPD_Custom_Show_Boot_Screen(void) {
    // 1. Smažeme buffer dočista
    EPD_Custom_Clear();

    // 2. Ručně tam napereme "00.0" nahoře
    // Indexy 1 a 3 jsou první dvě číslice, index 11 je ta za tečkou
    EPD_Custom_WriteDigit(1, 0);
    EPD_Custom_WriteDigit(3, 0);
    EPD_Custom_WriteDigit(11, 0);

    // Zapneme desetinnou tečku nahoře (Bit 5 na indexu 4)
    epd_buffer[4] |= 0x20;

    // 3. Nastavíme symboly: Stupně (0x04 | 0x01) a Baterku (0x10)
    // Bluetooth (0x08) necháme zatím zhasnutý
    epd_buffer[13] = 0x04 | 0x01 | 0x10;

    // 4. Pošleme na displej
    EPD_Custom_Update();
}
