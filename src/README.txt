
This code monitors MAC addresses found with ESP8266 on wemos d1 board
and then send the status to IFTTT MAKER - if there is a change.

Needs IFTTT to work, create IFTTT webhook events:
<devicename>_<deviceState>
for each of your events and states to connect the MAC status events to IFTTT services

to use, copy config.org to config.h and update with your own data

Thanks for the idea to:
  https://github.com/RicardoOliveira/FriendDetector
  https://github.com/mylob/ESP-To-IFTTT


Change  history:
  19.1.2020 - v 1.0 PetriKarj
