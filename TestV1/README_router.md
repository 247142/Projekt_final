# Zigbee Router: Multi-Sensor Node (TestV1)

Tato složka obsahuje firmware pro vývojovou desku **Nucleo-WBA55CG**. Zařízení v Zigbee síti funguje v roli **Routeru** a sdružuje tři logické funkce:
1. **Chytrý vypínač** (Toggle funkce - odesílání příkazů pro On/Off zařízení).
2. **Teplotní senzor** (Čtení reálné teploty z I2C senzoru BMP180).
3. **Senzor dveří/oken** (Kontakt simulovaný tlačítkem využívající IAS Zone cluster).

## 🛠️ Konfigurace v STM32CubeMX

Pro správný běh Zigbee stacku a periférií je projekt v CubeMX nastaven následovně:

### 1. Základní periferie
* **TIM2:** Nastaven jako zdroj přerušení s periodou 2 sekundy. Slouží pro pravidelné vyčítání teploty ze senzoru bez blokování hlavní smyčky.
* **I2C3:** Slouží ke komunikaci se senzorem BMP180. Rychlost je ponechána na Standard Mode (100 kHz). Fyzická adresa senzoru je `0x77` (v kódu posunuta o 1 bit na `0xEE`).
* **USART1:** Použit pro výpis ladících informací (Log) rychlostí 115200 bd.
    * **Kritické nastavení (DMA):** Aby logování přes UART nenarušovalo přesné časování rádiového stacku, je nutné v záložce *DMA Settings* u USART1 přidat **USART1_TX kanál (GPDMA1)**. Bez tohoto nastavení by systém při pokusu o první logový výpis spadl do `HardFault`. Zároveň je nutné mít zapnuté příslušné globální přerušení v NVIC.

### 2. Middleware: STM32_WPAN (Zigbee)
Konfigurace rádiového stacku se nachází v sekci *Middleware and Software Packs* -> *STM32_WPAN*:
* Role nastavena na **Router**.
* **Endpoint Vypínač:** Obsahuje `OnOff Client` cluster. Deska zařízení sama nestavuje, pouze posílá `Toggle` příkazy na adresu `0x0000` (Coordinator).
* **Endpoint Teplota:** Obsahuje `Temperature Measurement Server` cluster. Zde lokálně zapisujeme naměřenou hodnotu ve formátu ZCL (stupně Celsia * 100).
* **Endpoint Dveřní kontakt:** Obsahuje `IAS Zone Server` cluster. Zde logujeme stavy Alarm (otevřeno) a Clear (zavřeno).

### 3. Zpráva paměti (Linker a Heap)
Rádiový stack WBA55 alokuje buffery dynamicky (`malloc`). Vzhledem k použitým endpointům bylo nutné razantně navýšit paměť, jinak by aplikace havarovala s nedostatkem paměti (`zb_msg_tick` error).
* V Linker skriptu (`STM32WBA55CGUX_FLASH.ld`) byla paměť sjednocena do jednoho 128KB bloku `RAM`.
* **Minimum Heap Size** byl navýšen na `0x10000` (64 KB).
* **Minimum Stack Size** byl navýšen na `0x1000` (4 KB).
