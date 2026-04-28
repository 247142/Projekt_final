#ifndef EPD_CUSTOM_H
#define EPD_CUSTOM_H

#include <stdint.h>
#include <stdbool.h>

void EPD_Custom_Init(void);
void EPD_Custom_Clear(void);
void EPD_Custom_Update(void);

// Hlavní master funkce, kterou budeš volat!
// teplota: 0-199
// stav_dveri: 0 = Zavřeno [ ], 1 = Otevřeno [
void EPD_Zobraz_Aplikaci(int16_t teplota_x10, uint8_t stav_dveri, bool bluetooth_on);

#endif
