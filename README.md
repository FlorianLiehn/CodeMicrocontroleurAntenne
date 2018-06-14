*****************************************************************************
** Microcontroler Antenna Interface ( on ARM-Cortex-M4 STM32F407 )         **
*****************************************************************************

** Installation **

Somewhere in your file system create a directory for CHIBIOS projects

#mkdir ChibiOS_projects && cd ChibiOS_projects

Install ChibiOS

#git clone https://github.com/ChibiOS/ChibiOS.git

Switch to a stable version (18.2 for exemple)

#cd ChibiOS
#git pull
#git checkout stable_18.2.x
#cd ..

Now you can clone this project :

#git clone https://github.com/FlorianLiehn/CodeMicrocontroleurAntenne.git
#cd CodeMicrocontroleurAntenne

** Build Procedure **

For building you just have to install correct compilator !
#TODO which one ?

And in the project folder tape make
#cd CodeMicrocontroleurAntenne
#make

The Elf file and PcHandler are generated in build directory

