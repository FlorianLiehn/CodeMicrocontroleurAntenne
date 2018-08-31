Microcontroler Antenna Interface ( on ARM-Cortex-M4 STM32F407 )
===============


### Installation

Somewhere in your file system create a directory for CHIBIOS projects
```bash
mkdir ChibiOS_projects && cd ChibiOS_projects
```
Install ChibiOS
```bash
git clone https://github.com/ChibiOS/ChibiOS.git
```
Switch to a stable version (18.2 for exemple)
```bash
cd ChibiOS
git pull
git checkout stable_18.2.x
cd ..
```
Now you can clone this project :
```bash
git clone https://github.com/FlorianLiehn/CodeMicrocontroleurAntenne.git
cd CodeMicrocontroleurAntenne
```
### Build Procedure

For building you need the correct compilator: arm-none-eabi-gcc
```bash
sudo apt-get install make
sudo apt-get install gcc #C compilator
sudo apt-get install software-properties-common #install add-apt-repository
sudo add-apt-repository ppa:team-gcc-arm-embedded/ppa #add arm gcc rep
sudo apt-get update
sudo apt-get install gcc-arm-none-eabi #install compilator
sudo apt-get install openocd #install flash tools
sudo apt-get install gdb-multiarch
```

And in the project folder tape make
```bash
cd CodeMicrocontroleurAntenne
make
```
The Elf file and PcHandler are generated in build directory

### Execution
just run the flash.sh script

