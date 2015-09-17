### Fuse settings

```
avrdude -p m328p -c usbasp -P usb -U lfuse:w:0xe2:m -U hfuse:w:0xd9:m -U efuse:w:0xfe:m
```

### Flashing FW

```
avrdude -p m328p -c usbasp -P usb -U flash:w:./.build/atmega328/firmware.hex
```
