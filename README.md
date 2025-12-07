### Building

```
cd garage-door-opener
idf.py build
```

### Flashing

```
cd garage-door-opener/build
python3 -m esptool --chip esp32c6 -b 460800 --before default_reset --after hard_reset write_flash "@flash_args" && python3 -m serial /dev/cu.usbmodem14101 115200
```
