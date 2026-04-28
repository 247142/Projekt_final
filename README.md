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
Koordinátor po spuštění formuje Zigbee síť a naslouchá novým zařízením. 
* **Registrace IAS senzoru:** Jakmile se připojí Router, Koordinátor automaticky zapíše svou IEEE adresu do atributu `IasCieAddress` senzoru na Endpointu 3. Následně zachytí `Zone Enroll Request` a schválí jej, čímž vznikne zabezpečená vazba.
* **Zpracování poplachů:** Při změně stavu zachytí `Zone Status Change Notification`, vyhodnotí nultý bit (Alarm 1) a vztyčí vlajku pro překreslení uživatelského rozhraní.
* **E-Paper displej:** V hlavní smyčce se na základě zachycených událostí překresluje E-Paper displej. Zobrazuje aktuální teplotu a grafický symbol dveří (otevřeno/zavřeno).
* **Reporting teploty:** Koordinátor zasílá do Routeru žádost o konfiguraci reportingu (`ZbZclAttrReportConfigReq`), čímž definuje minimální a maximální interval zasílání hodnot.

### 🧠 Router
Router po spuštění vyhledá síť na určeném kanálu a připojí se do ní.
* **Měření teploty (BMP180):** Zařízení pravidelně komunikuje přes I2C s čidlem BMP180. Naměřenou teplotu přepočítává do 16bitového formátu specifikovaného standardem ZCL (hodnota ve stupních Celsia vynásobená 100) a ukládá ji do lokálního atributu na Endpointu 2. O odesílání se stará nastavený reporting stacku.
* **Dveřní senzor (IAS Zone):** Stav externího tlačítka je mapován na atribut `ZoneStatus`. Při stisku/uvolnění se změní nultý bit atributu, což ihned vyvolá odeslání notifikace do Koordinátora.
* **Ovládání LED (OnOff):** Další tlačítko na desce je nastaveno na Endpoint 1. Při jeho stisku se sestaví zpráva typu `Toggle` a přes `ZbZclOnOffClientToggleReq` je odeslána Koordinátorovi, kterému tímto přepne stav interní LED diody.
