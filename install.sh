#!/bin/bash

# In order to install the program you have to run the install.sh.
# If there isn't any first argument, which is <pathname>, there will be an echo 
# containing the information to how install the program

if [ $# -eq 0 ]

then
    echo "To install correctly insert this line: source ./install.sh <pathname> ";
    exit;
fi

# If there isn't any folder inside the path, we have to create it. We can check if there is a 
# folder thanks to this if statement ($1 is the folder we have to create).

if [ ! -d $1 ]

then
 echo "There is an ERROR: $1 directory does NOT exist!!!");
 while true; 
 do
  read -p "Do you want to create $1 directory? [y/n] " yn
  case $yn in
   [y]* ) mkdir $1; break;;
   [n]* ) exit;;
   * ) "y = yes & n = no";;
  esac
 done
fi

# Thanks to the following lines we will unzip the surces.script. 
# We export the path of the folder in order to use it in a
# file bash.

echo "Program installation on $1 ... ";
export X = $1;
cp info.txt $1;
unzip sources.zip -d $1;

# Compile of all the .c files and we put the executables inside a new 
# Putting the executables inside a new folder.

echo "Compiling files on sources ...";
cd $1

cd fcommand
gcc -o command command.c
mv command ..
cd ..

cd finspection
gcc -o inspection inspection.c
mv inspection ..
cd ..

cd fmotor_x
gcc -o m_x m_x.c
mv m_x ..
cd ..

cd fmotor_z
gcc -o m_z m_z.c
mv m_z ..
cd ..

cd fmaster
gcc -o master master.c
mv master ..
cd ..

cd fwatchdog
gcc -o watchdog wd.c
mv watchdog ..
cd ..

mkdir executables

mv command executables
mv inspection executables
mv motor_x executables
mv motor_z executables
mv master executables
mv watchdog executables
cd ..

echo "Run the program with ./run.sh ";


