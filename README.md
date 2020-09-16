# CFAL32128A0-0171B-WC
Crystalfontz CFAL32128A0-0171B-WC - Tiny OLED with Capacitive Touch

Example Seeeduino (Arduino Clone) Software.

This product can be found here:
https://www.crystalfontz.com/product/cfal32128a00171bwc

## Connection Details
### Display using SPI interface
| OLED / Touch Module Pin | Seeeduino Pin | Connection Description |
|-------------------------|---------------|------------------------|
| 17 (DCS)                | 10            | Display CS             |
| 18 (TCS)                | 9             | Touch CS               |
| 19 (RES)                | A0            | Display Reset          |
| 20 (TRES)               | A1            | Touch Reset            |
| 21 (D/C)                | 8             | Display D/C            |
| 22 (IRQ)                | 2             | Touch IRQ              |
| 23 (TD0)                | SCL           | Touch I2C SCL          |
| 24 (TD1)                | SDA           | Touch I2C SDA          |
| 25 (TD2)                | SDA           | Touch I2C SDA          |
| 26 (D0)                 | 13 / SCK      | Display SPI SCK        |
| 27 (D1)                 | 11 / MOSI     | Display SPI MOSI       |
| 28 (D2)                 | 11 / MOSI     | Display SPI MOSI       |
| 29 (BS1)                | GND           | Display SPI/I2C Select |

### Display using I2C interface
| OLED / Touch Module Pin | Seeeduino Pin | Connection Description |
|-------------------------|---------------|------------------------|
| 17 (DCS)                | 3.3V          | Display CS (tied high) |
| 18 (TCS)                | 9             | Touch CS               |
| 19 (RES)                | A0            | Display Reset          |
| 20 (TRES)               | A1            | Touch Reset            |
| 21 (D/C)                | GND           | Display D/C (tied low) |
| 22 (IRQ)                | 2             | Touch IRQ              |
| 23 (TD0)                | SCL           | Touch I2C SCL          |
| 24 (TD1)                | SDA           | Touch I2C SDA          |
| 25 (TD2)                | SDA           | Touch I2C SDA          |
| 26 (D0)                 | SCL           | Display I2C SCL        |
| 27 (D1)                 | SDA           | Display I2C SDA        |
| 29 (D2)                 | SDA           | Display I2C SDA        |
| 29 (BS1)                | 3.3V          | Display SPI/I2C Select |
