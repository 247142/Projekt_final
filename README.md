# 🛡️ Zigbee Smart Monitor: Dveřní senzor a teploměr
*Tento projekt byl vytvořen v rámci předmětu **MPC-SSY**.*
## 👥 Autoři
* **Bc. Oldřich Hána (247113)** – *Kordinátor*
* **Bc. Matěj Matoušek (247142)** – *Router*

Projekt demonstruje komplexní komunikaci v bezdrátové síti **Zigbee 3.0** mezi dvěma uzly. Zaměřuje se na implementaci zabezpečeného clusteru **IAS Zone** (bezpečnostní dveřní senzor simulovaný tlačítkem), **Temperature Measurement** (měření teploty) a obousměrné ovládání stavu přes **OnOff** cluster (Toggle).

## 📱 Hardware
* **MCU:** 2x Nucleo-WBA55CG 
* **Displej:** E-Paper Display (EPD) 1.9" připojený ke koordinátorovi.
* **Senzory:** Tlačítko simulující dveřní kontakt  a senzor teploty BMP180.

## 📡 Síťové parametry
* **Zigbee kanál:** 13
* **Role 1:** Zigbee Coordinator (CIE - Control Indicator Equipment).
* **Role 2:** Zigbee Router / End Device (IAS Zone Server).
* **Topologie:** Centralizovaná síť.

## 🏗️ Implementované Clustery

### Koordinátor 
* **IAS Zone Client (EP 1):** Spravuje registraci senzorů a přijímá poplachy o otevření/zavření dveří.
* **Temperature Client (EP 1):** Přijímá reporty o teplotě ze senzoru.
* **OnOff Server (EP 1):** Umožňuje vzdálené ovládání palubní LED.
* **UI:** Grafické zobrazení stavu na E-Paper displeji a textový dashboard přes UART.

### Router 
* **IAS Zone Server (EP 3):** Detekuje stav tlačítka. 
* **Temperature Server (EP 2):** Měří teplotu a odesílá ji pomocí konfigurovatelného reportingu.
* **OnOff Client (EP 1):** Odesílá Toggle příkazy pro ovládání LED na koordinátorovi.
* **UI:** Textový dashboard přes UART.

## 💻 Vývoj a ladění
* **Prostředí:** STM32CubeIDE.
* **Stack:** STM32_WPAN.
* **Ladící výstup:** UART @ 115200 baud (Putty).





## 🧩 Softwarové řešení (Architektura)

Popis jednotlivých souftwarových řešní.

### 🧠 Koordinátor
**Kordinátor bla bla bla**

### 🧠 Router
**SEM TO DEJ** 
