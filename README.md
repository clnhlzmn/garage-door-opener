### What is it?

It's a single channel contact closure that is controllable using matter over thread. I made this to be a garage door opener for my house (the contact closure is connected to the opener button input) but it could be used for anything you need a relay for.

The code here is tailored specifically for being a garage door opener, though, meaning it always turns the relay off after 1 second. That can easily be changed by removing the timer stuff in relay/relay_driver.cpp.

### Hardware

This is based on Seeed Studio's [XIAO ESP32-C6](https://www.seeedstudio.com/Seeed-Studio-XIAO-ESP32C6-p-5884.html?qid=EUBP4U_y436nzzh_1765296052518) combined with their [1 channel relay](https://www.seeedstudio.com/Relay-add-on-module-for-XIAO-p-6310.html).

### Building

```
cd garage-door-opener
idf.py set-target esp32c6
idf.py build
```

### Generate Factory Partition

```
cd garage-door-opener
rm -rf build/mfg && esp-matter-mfg-tool --discriminator 4069 --passcode $RANDOM --vendor-id 0xFFF2 --product-id 0x8001 --outdir build/mfg
```

### Flash the Factory Partition

Note: this command depends on having produced exactly one factory partition binary using the command above.

```
cd garage-door-opener/build
python3 -m esptool --chip esp32c6 write_flash 0x10000 mfg/fff2_8001/*/*partition.bin
```

After having flashed the factory partition the device can be paired using the corresponding QR code found at `mfg/fff2_8001/*/*qrcode.png`.

### Flashing the App

```
cd garage-door-opener/build
python3 -m esptool --chip esp32c6 -b 460800 --before default_reset --after hard_reset write_flash "@flash_args" && python3 -m serial /dev/cu.usbmodem14101 115200
```

### Monitoring Serial Output

```
python3 -m serial /dev/cu.usbmodem14101 115200
```
