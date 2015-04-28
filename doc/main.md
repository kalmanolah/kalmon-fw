Kalmon
======

Kalmon is a configurable sensor hub package. It was created to be reusable,
configurable and well-integrated with a fair amount of systems.

Since Kalmon is built ontop of [MySensors][1], it should work with [a handful][2]
of open source domotics systems.

## Agreements and Specifications

* The serial baud rate is set to `115200`.

## MySensor Serial Protocol Additions

This section describes additions to the MySensor Serial Protocol, for which
documentation can be found [here][3].

Users and Integrators are encouraged to read the MySensors Serial Protocol
documentation thoroughly before reading any further.

### Custom Value Types

The MySensors Serial Protocol reserves one unsigned 8-bit short for value types
in the message header, of which about `40` are in use.

Since some information would be lost if we were to solely use these value
types, we have started defining custom value types starting from `128`.

As opposed to the MySensors value type constants, the constants used in the
code are prefixed with `C` for `CUSTOM`.

Currently, the custom value types in use are the following:

| Name | Value |
|------|-------|
| CV_AVAILABLE_MEMORY | 128 |

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

[1]: http://www.mysensors.org/
[2]: http://www.mysensors.org/controller/
[3]: http://www.mysensors.org/download/serial_api_14
