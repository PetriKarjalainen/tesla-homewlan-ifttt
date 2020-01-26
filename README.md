
Wemos D1 - Detect when tesla or any other wlan device like mobile phone is home and
set status <device>_home or <device>_away to IFTTT webhook

This code monitors MAC addresses found with ESP8266 on wemos d1 board
and then send the status to IFTTT MAKER - if there is a change.

Needs IFTTT to work, create IFTTT webhook events:
<devicename>_<deviceState>
for each of your events and states to connect the MAC status events to IFTTT services

to use, copy config.org to config.h and update with your own data

Thanks for the idea to:
  https://github.com/RicardoOliveira/FriendDetector
  https://github.com/mylob/ESP-To-IFTTT

This package is for Atom editor with platformIO extension to have Arduino framework compatibility. The code may work also with normal Arduino.

Remember to set Platform.io for right board before compiling. To do this use platformIO Home page to create a new project. Then delete src/main.cpp and .gitignore files.

to set up git in plaform io first using Atom initialize the directory by rightclick on top of your main directory, select git/initiatlize.

Then navitage using command promp to that directory and do the following commands in that directory.

git remote add origin
From https://github.com/PetriKarjalainen/tesla-homewlan-ifttt

git pull origin master

After these commands restart atom and youre good to go.
