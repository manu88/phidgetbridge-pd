# PhidgetBridge Pure Data object

This project is a fork from https://github.com/pure-data/helloworld.


## Usage

Clone and build this example via:

    git clone --recursive https://github.com/manu88/phidgetbridge-pd
    cd phidgetbridge-pd
    make

Make sure you use the `--recursive` flag when checking out the repository so that the pd-lib-builder dependency is also checked out. If you forgot to add it, you can also manually fetch pd-lib-builder via:

    cd helloworld
    git submodule init
    git submodule update

## Build

### Pure Data
You should have a copy of the pure-data source code - the following build command assumes it is in the `../pure-data` directory. [This page explains how you can get the pure-data source code](https://puredata.info/docs/developer/GettingPdSource).

By default, pd-lib-builder will attempt to auto-locate an install of Pure Data. The following command will build the external and install the distributable files into a subdirectory called `build/helloworld`.

    make install pdincludepath=../pure-data/src/ objectsdir=./build

See `make help` for more details.

### Phidget library

#### macOs
get the development libraries [here](https://www.phidgets.com/docs/OS_-_macOS)

#### Linux
More doc [here](https://www.phidgets.com/docs/OS_-_Linux).

```bash
curl -fsSL https://www.phidgets.com/downloads/setup_linux | sudo -E bash -
sudo apt install -y libphidget22
sudo apt install -y libphidget22-dev
``` 
