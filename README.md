kalmon-fw
=========

Firmware for a modular, configurable Arduino sensor hub

## Features

Currently implemented features:

* Config loading/saving/setting/getting
* Power saving/sleep mode with alternating wake/sleep periods
* Serial input handling
* Command handler with callback registration
* Decent logging output
* Dynamic module/peripheral/sensor registration support (you can add up to n
  modules by specificing a type and parameters)

## Documentation

See [doc/main.md](doc/main.md)

## TODO

* (LOW) Add `cfg_list` and `cfg_clear` commands
* (MED) Submit proper battery voltage
* (LOW) ADXL345: (double) tap, freefall, freefall impact detection support
* (MED) Write more documentation
* (LOW) Generate a UUID for the device on first boot, and save it
* (MED) Allow for commands to be executed over the air
* (LOW) Move serial stuff into separate file
* (LOW) Move power management stuff into separate file

## License

```
The MIT License (MIT)

Copyright (c) 2014-2015 Kalman Olah

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
```
