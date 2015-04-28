# Preparations

* Get a Raspberry PI
* Install Raspbian
* Configure your networking
* Connect the wires of the rf24 transceiver (see
  https://github.com/mysensors/Raspberry/blob/master/README)
* Get rid of useless stuff with these commands:

    ```
    sudo apt-get remove cups*
    sudo apt-get remove gnome*
    sudo apt-get remove x11-common*
    sudo apt-get autoremove
    ```

* Update your system with these commands:

    ```
    sudo apt-get update
    sudo apt-get upgrade
    ```

* Get a few extra packages with these commands:

    ```
    sudo apt-get install -y git-core subversion screen htop vim
    ```

* Update your firmware with these commands:

    ```
    sudo apt-get install ca-certificates
    sudo wget http://goo.gl/1BOfJ -O /usr/bin/rpi-update && sudo chmod +x /usr/bin/rpi-update
    sudo rpi-update
    sudo reboot
    ```

# Gateway

* Undo your overclock
* Enable SPI (`raspi-config > Advanced Options -> SPI`)
* Run these commands:

    ```
    sudo su -
    apt-get update && apt-get upgrade -y
    mkdir /opt/mysensors && cd /opt/mysensors
    git clone https://github.com/mysensors/Raspberry.git
    cd Raspberry/librf24-bcm
    make all && make install
    cd ../
    make all && make install
    make enable-gwserial # if you want to start the gateway on boot
    ```

# Domoticz

Please keep in mind that this will install the beta branch.

* Run these commands:

    ```
    sudo apt-get install cmake make gcc g++ libboost-dev libboost-thread-dev libboost-system-dev libboost-python-dev libsqlite3-dev subversion curl libcurl4-openssl-dev libusb-dev
    sudo mkdir /opt/domoticz && sudo chown pi:pi /opt/domoticz && cd /opt/domoticz
    wget http://domoticz.sourceforge.net/beta/domoticz_linux_armv6l.tgz
    tar xvfz domoticz_linux_armv6l.tgz
    rm domoticz_linux_armv6l.tgz
    sudo cp domoticz.sh /etc/init.d
    sudo chmod +x /etc/init.d/domoticz.sh
    sudo update-rc.d domoticz.sh defaults
    ```

* Modify `/etc/init.d/domoticz.sh` to set `USERNAME`, `DAEMON` and such
* Start the `domoticz.sh` service

## Updating Domoticz

* Run these commands:

    ```
    cd /opt/domoticz
    wget http://domoticz.sourceforge.net/beta/domoticz_linux_armv6l.tgz
    tar xvfz domoticz_linux_armv6l.tgz
    rm domoticz_linux_armv6l.tgz
    ```
