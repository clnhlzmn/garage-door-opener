### Building

```
cd garage-door-opener
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

### Flashing

```
cd garage-door-opener/build
python3 -m esptool --chip esp32c6 -b 460800 --before default_reset --after hard_reset write_flash "@flash_args" && python3 -m serial /dev/cu.usbmodem14101 115200
```

### Monitoring Serial Output

```
python3 -m serial /dev/cu.usbmodem14101 115200
```
