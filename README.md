# CFAL32128A0-0171B-WC
Crystalfontz CFAL32128A0-0171B-WC - Tiny OLED with Capacitive Touch

Example Seeeduino (Arduino Clone) Software.

This product can be found here:
https://www.crystalfontz.com/product/cfal32128a00171bwc

## Connection Details
### Display using SPI interface
| OLED / Touch Module Pin | Seeeduino Pin | Connection Description |
|-------------------------|---------------|------------------------|
| 16 (DCS)                | 10            | Display CS             |
| 17 (TCS)                | 9             | Touch CS               |
| 18 (RES)                | A0            | Display Reset          |
| 19 (TRES)               | A1            | Touch Reset            |
| 20 (D/C)                | 8             | Display D/C            |
| 21 (IRQ)                | 2             | Touch IRQ              |
| 22 (TD0)                | SCL           | Touch I2C SCL          |
| 23 (TD1)                | SDA           | Touch I2C SDA          |
| 24 (TD2)                | SDA           | Touch I2C SDA          |
| 25 (D0)                 | 13 / SCK      | Display SPI SCK        |
| 26 (D1)                 | 11 / MOSI     | Display SPI MOSI       |
| 27 (D2)                 | 11 / MOSI     | Display SPI MOSI       |
| 33 (BS1)                | GND           | Display SPI/I2C Select |

### Display using I2C interface
| OLED / Touch Module Pin | Seeeduino Pin | Connection Description |
|-------------------------|---------------|------------------------|
| 16 (DCS)                | 3.3V          | Display CS (tied high) |
| 17 (TCS)                | 9             | Touch CS               |
| 18 (RES)                | A0            | Display Reset          |
| 19 (TRES)               | A1            | Touch Reset            |
| 20 (D/C)                | GND           | Display D/C (tied low) |
| 21 (IRQ)                | 2             | Touch IRQ              |
| 22 (TD0)                | SCL           | Touch I2C SCL          |
| 23 (TD1)                | SDA           | Touch I2C SDA          |
| 24 (TD2)                | SDA           | Touch I2C SDA          |
| 25 (D0)                 | SCL           | Display I2C SCL        |
| 26 (D1)                 | SDA           | Display I2C SDA        |
| 27 (D2)                 | SDA           | Display I2C SDA        |
| 33 (BS1)                | 3.3V          | Display SPI/I2C Select |
