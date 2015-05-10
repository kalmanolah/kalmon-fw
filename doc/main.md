Kalmon
======

## Table of Contents

<!-- MarkdownTOC depth=4 autoanchor=true bracket=round autolink=true -->

- [Introduction](#introduction)
- [Agreements and Specifications](#agreements-and-specifications)
- [MySensor Serial Protocol Additions](#mysensor-serial-protocol-additions)
    - [Custom Sensor Types](#custom-sensor-types)
    - [Custom Value Types](#custom-value-types)
    - [Node Information & Stats](#node-information--stats)
- [Commands](#commands)
- [Configuration](#configuration)
- [Modules](#modules)
    - [DHT11](#dht11)
        - [Configuration](#configuration-1)
        - [Parameters](#parameters)
    - [HCSR04](#hcsr04)
        - [Configuration](#configuration-2)
        - [Parameters](#parameters-1)
    - [KY038](#ky038)
        - [Configuration](#configuration-3)
        - [Parameters](#parameters-2)
        - [Notes](#notes)
    - [MNEBPTCMN](#mnebptcmn)
        - [Configuration](#configuration-4)
        - [Parameters](#parameters-3)
        - [Notes](#notes-1)
    - [ADXL345](#adxl345)
        - [Configuration](#configuration-5)
        - [Parameters](#parameters-4)

<!-- /MarkdownTOC -->

<a name="introduction"></a>
## Introduction

Kalmon is a configurable sensor hub package. It was created to be reusable,
configurable and well-integrated with a fair amount of systems.

Since Kalmon is built ontop of [MySensors][1], it should work with [a handful][2]
of open source domotics systems.

<a name="agreements-and-specifications"></a>
## Agreements and Specifications

* The serial baud rate is set to `115200`.

<a name="mysensor-serial-protocol-additions"></a>
## MySensor Serial Protocol Additions

This section describes additions to the MySensor Serial Protocol, for which
documentation can be found [here][3].

Users and Integrators are encouraged to read the MySensors Serial Protocol
documentation thoroughly before reading any further.

<a name="custom-sensor-types"></a>
### Custom Sensor Types

The MySensors Serial Protocol supports about thirty sensor types. These are
sent to the gateway when presenting a new sensor in the form of an unsigned
eight-bit char. We have started defining custom sensor types starting from
`128`. The constant names are prefixed with `C` for `CUSTOM`.

| Name | Value | Description |
|------|-------|-------------|
| CS_ACCELEROMETER | 128 | Accelerometer. Added to support the ADXL-345 module. |

<a name="custom-value-types"></a>
### Custom Value Types

The MySensors Serial Protocol supports about fourty value types. These are
sent to the gateway when submitting a sensor value in the form of an unsigned
eight-bit char. We have started defining custom value types starting from
`128`. The constant names are prefixed with `C` for `CUSTOM`.

| Name | Value | Description |
|------|-------|-------------|
| CV_AVAILABLE_MEMORY | 128 | Available memory in bytes. |
| CV_ACCELERATION_X | 129 | Acceleration X value. |
| CV_ACCELERATION_Y | 130 | Acceleration Y value. |
| CV_ACCELERATION_Z | 131 | Acceleration Z value. |

<a name="node-information--stats"></a>
### Node Information & Stats

A MySensors node has support for sending the battery level to the gateway.
When this is done a `child_sensor_id` of `255` is used by default.

As the public API is limited to using the `MySensor::send()` method, we are
forced to use the `C_SET` message types. As such, we have chosen to submit any
and all custom node-related info & stats using the message type `C_SET` and
child sensor id `NODE_SENSOR_ID` or `255`.

Currently, the following node information and statistics are sent:

* Available memory:

    Example message:

    ```
    5;255;1;0;128;1064
    ```

    Parsing this message would yield:

    ```
    Node ID         => 5
    Child Sensor ID => 255  # NODE_SENSOR_ID
    Message Type    => 1    # C_SET
    Ack Required    => 0    # false
    Value Type      => 128  # CV_AVAILABLE_MEMORY
    Value           => 1064 # Available memory in bytes
    ```

<a name="commands"></a>
## Commands

A few commands can be executed, mostly related to debugging and configuration.
These commands can currently only be executed over a serial connection.
Wireless command execution is in the works.

Command arguments are space-delimited, and input ends when a newline character
is encountered.

The following commands are currently defined:

| Command | Format | Description |
|---------|--------|-------------|
| 21 | `$cmd\n` | Print device stats |
| 22 | `$cmd\n` | Perform a soft reset |
| 41 | `$cmd\n` | Load configuration from EEPROM |
| 42 | `$cmd\n` | Save configuration to EEPROM |
| 43 | `$cmd $key\n` | Get the value of a configuration variable |
| 44 | `$cmd $key $value\n` | Set the value of a configuration variable |

<a name="configuration"></a>
## Configuration

A variety of different configuration options are supported which can be read
and written using certain commands.

Please keep in mind that not saving the configuration after you have made
changes will cause those changes to be lost upon the next reboot.

The following configuration options are currently defined:

| Name | Key | Type | Default | Description |
|------|-----|------|---------|-------------|
| DEBUG | 0 | bool | true | The global debug flag. |
| LOOP_DELAY | 8 | uint16_t | 250 | The time the device should be idle per loop, in milliseconds. |
| SERIAL_BAUD_RATE | 9 | uint16_t | 9600 | Serial baud rate. Deprecated. |
| SERIAL_INPUT_BUFFER_SIZE | 10 | uint16_t | 32 | The buffer size for serial input, in bytes. |
| SENSOR_UPDATE_INTERVAL | 11 | uint16_t | 15 | Interval between sensor updates, in seconds. If set to `0`, disables sensor updates. If the device is woken from sleep, the sensor update timer is reset, meaning the interval time has to pass every time the device wakes up before a sensor update is sent. |
| AWAKE_DURATION | 12 | uint16_t | 25 | How long the device should stay awake, in seconds. This setting only matters if `SLEEP_DURATION` is also set. |
| SLEEP_DURATION | 13 | uint16_t | 1800 | How long the device should remain asleep, in seconds. If set to `0` disables sleeping. |
| MODULE_1_CONFIGURATION | 24 | char[n] | NULL | Configuration for module #1. For more information, see the modules section. |
| MODULE_2_CONFIGURATION | 25 | char[n] | NULL | Configuration for module #2. For more information, see the modules section. |
| MODULE_3_CONFIGURATION | 26 | char[n] | NULL | Configuration for module #3. For more information, see the modules section. |
| MODULE_4_CONFIGURATION | 27 | char[n] | NULL | Configuration for module #4. For more information, see the modules section. |
| MODULE_5_CONFIGURATION | 28 | char[n] | NULL | Configuration for module #5. For more information, see the modules section. |
| MODULE_6_CONFIGURATION | 29 | char[n] | NULL | Configuration for module #6. For more information, see the modules section. |
| MODULE_7_CONFIGURATION | 30 | char[n] | NULL | Configuration for module #7. For more information, see the modules section. |
| MODULE_8_CONFIGURATION | 31 | char[n] | NULL | Configuration for module #8. For more information, see the modules section. |

<a name="modules"></a>
## Modules

External modules can be attached, mixed and matched to suit your needs.
Examples of such modules are a sonar, accelerometer, humidity sensor, and so
on. To configure modules, simply use the configuration commands to insert
module configuration into one of the available slots.

The following modules are currently defined:

<a name="dht11"></a>
### DHT11

[DHT11 Digital Temperature Humidity Sensor Module](http://www.dx.com/p/arduino-digital-temperature-humidity-sensor-module-121350)

Presented as humidity sensor `S_HUM` and temperature sensor `S_TEMP`,
values sent as humidity value `V_HUM` and temperature value `V_TEMP`.

<a name="configuration-1"></a>
#### Configuration

```
1,${pin}
```

<a name="parameters"></a>
#### Parameters

* *pin*:

    * digital input pin

<a name="hcsr04"></a>
### HCSR04

[HC-SR04 Ultrasonic Sensor Distance Measuring Module](http://www.dx.com/p/hc-sr04-ultrasonic-sensor-distance-measuring-module-133696)

Presented as distance sensor `S_DISTANCE`, values sent as distance value
`V_DISTANCE`.

<a name="configuration-2"></a>
#### Configuration

```
2,${trig_pin},${echo_pin}
```

<a name="parameters-1"></a>
#### Parameters

* *trig_pin*:

    * trigger pin

* *echo_pin*:

    *echo pin

<a name="ky038"></a>
### KY038

[Keyes Microphone Sound Detection Sensor Module](http://www.dx.com/p/arduino-microphone-sound-detection-sensor-module-red-135533)

Presented as custom sensor `S_CUSTOM`, values sent as var1 value `V_VAR1`.

<a name="configuration-3"></a>
#### Configuration

```
3,${pin}
```

<a name="parameters-2"></a>
#### Parameters

* *pin*:

    * analog input pin

<a name="notes"></a>
#### Notes

Not accurate at all. If I could go back in time, I wouldn't have bought 4.

<a name="mnebptcmn"></a>
### MNEBPTCMN

[Meeeno MN-EB-PTCMN Photosensitive Sensor Module](http://www.dx.com/p/meeeno-mn-eb-ptcmn-photosensitive-sensor-module-orange-202511)

Presented as light level sensor `S_LIGHT_LEVEL`, values sent as light level
value `V_LIGHT_LEVEL`.

<a name="configuration-4"></a>
#### Configuration

```
4,${pin}
```

<a name="parameters-3"></a>
#### Parameters

* *pin*:

    * analog input pin

<a name="notes-1"></a>
#### Notes

It's easy to get a reading, you just don't know how to scale that reading
unless you perform some assisted calibration.

<a name="adxl345"></a>
### ADXL345

[ADXL345 Digital 3-Axis Gravity Acceleration Sensor Module](http://www.dx.com/p/adxl345-digital-3-axis-gravity-acceleration-sensor-module-blue-149476)

Additional documentation: [here](http://www.analog.com/media/en/technical-documentation/data-sheets/ADXL345.pdf)

Presented as accelerometer sensor `CS_ACCELEROMETER`, values sent as
acceleration values `CS_ACCELERATION_X`, `CS_ACCELERATION_Y` and
`CS_ACCELERATION_Z`.

If activity or inactivity detection are enabled, also presents a motion sensor
`S_MOTION`, with values sent as tripped status `V_TRIPPED`.

Supports link mode, where activity interrupts are prevented until inactivity
is detected, and vice versa.

Supports an auto-sleep mode, where power consumption is reduced by a huge
margin. Highly recommended for low-power projects.

<a name="configuration-5"></a>
#### Configuration

```
5,${activity_threshold},${inactivity_threshold},${inactivity_time},${sensitivity_range},${data_rate},${power_mode}
```

<a name="parameters-4"></a>
#### Parameters

* *activity_threshold*:

    * threshold (in G) for activity detection
    * enables activity detection if not set to `0`

* *inactivity_threshold*:

    * threshold (in G) for inactivity detection
    * enables inactivity detection if not set to `0`

* *inactivity_time*:

    * inactivity time
    * required for inactivity detection
    * defaults to `5` if set to `0`

* *sensitivity_range*:

    * configures the sensitivity
    * contains the value sent to the `DATA_FORMAT` register
    * highly recommended to read the additional documentation linked above
    * currently not used

* *data_rate*:

    * configures the data rate
    * contains the value sent to the `BW_RATE` register
    * highly recommended to read the additional documentation linked above
    * currently not used

* *power_mode*:

    * configures the power mode
    * contains the value sent to the `POWER_CTL` register
    * highly recommended to read the additional documentation linked above
    * defaults to `0x08` / `8` (measurement mode)
    * suggested values:

        | Dec | Hex | Description |
        |-----|-----|-------------|
        | 8 | 0x08 | Measurement mode |
        | 40 | 0x28 | Measurement mode + link mode |
        | 56 | 0x38 | Measurement mode + link mode + auto-sleep mode |

[1]: http://www.mysensors.org/
[2]: http://www.mysensors.org/controller/
[3]: http://www.mysensors.org/download/serial_api_14
