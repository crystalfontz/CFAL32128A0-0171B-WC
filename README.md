# CFAL32128A0-0171B-WC
Crystalfontz CFAL32128A0-0171B-WC - Tiny OLED with Capacitive Touch

Example Seeeduino (Arduino Clone) Software.

This product can be found here:
https://www.crystalfontz.com/product/cfal32128a00171bwc

## Display Connection Details
### Display using SPI interface
| OLED / Touch Module Pin | Seeeduino Pin | Connection Description |
|-------------------------|---------------|------------------------|
| 17 (DCS)                | 10            | Display CS             |
| 19 (RES)                | 3             | Display Reset          |
| 21 (D/C)                | 8             | Display D/C            |
| 26 (D0)                 | 13 / SCK      | Display SPI SCK        |
| 27 (D1)                 | 11 / MOSI     | Display SPI MOSI       |
| 28 (D2)                 | 11 / MOSI     | Display SPI MOSI       |
| 29 (BS1)                | GND           | Display SPI Select     |

### Display using I2C interface
| OLED / Touch Module Pin | Seeeduino Pin | Connection Description |
|-------------------------|---------------|------------------------|
| 17 (DCS)                | 3.3V          | Display CS (tied high) |
| 19 (RES)                | 3             | Display Reset          |
| 21 (D/C)                | GND           | Display D/C (tied low) |
| 26 (D0)                 | SCL           | Display I2C SCL        |
| 27 (D1)                 | SDA           | Display I2C SDA        |
| 29 (D2)                 | SDA           | Display I2C SDA        |
| 29 (BS1)                | 3.3V          | Display I2C Select     |

## Touch Connection Details
### Touch using SPI interface
| OLED / Touch Module Pin | Seeeduino Pin | Connection Description |
|-------------------------|---------------|------------------------|
| 18 (TCS)                | 9             | Touch CS               |
| 20 (TRES)               | 4             | Touch Reset            |
| 22 (IRQ)                | 2             | Touch IRQ              |
| 23 (TD0)                | 13 / SCK      | Touch SPI SCK          |
| 24 (TD1)                | 11 / MOSI     | Touch SPI MOSI         |
| 25 (TD2)                | 12 / MISO     | Touch SPI MISO         |
| 30 (BS3)                | GND           | Touch SPI Select       |

### Touch using I2C interface
| OLED / Touch Module Pin | Seeeduino Pin | Connection Description |
|-------------------------|---------------|------------------------|
| 18 (TCS)                | 9             | Touch CS               |
| 20 (TRES)               | 4             | Touch Reset            |
| 22 (IRQ)                | 2             | Touch IRQ              |
| 23 (TD0)                | SCL           | Touch I2C SCL          |
| 24 (TD1)                | SDA           | Touch I2C SDA          |
| 25 (TD2)                | SDA           | Touch I2C SDA          |
| 30 (BS3)                | 3.3V          | Touch I2C Select       |
