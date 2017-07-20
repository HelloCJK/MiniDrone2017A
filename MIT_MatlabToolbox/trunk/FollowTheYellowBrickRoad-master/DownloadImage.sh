#!/bin/bash

# 3. Telnet into drone, and set ad-hoc network IP variable in param file
echo "Taking picture!"
/usr/bin/expect <<SCRIPT
spawn telnet 192.168.1.1;
expect "RS.edu] \$ ";
send "rm /data/edu/testy.mjpeg \r";
send "rm /data/edu/image.uyvy \r";
expect "RS.edu] \$ ";
send "yavta  --file=/data/edu/testy.mjpeg  -fMJPEG  /dev/video0 --capture=4 --skip 3 \r";
expect "RS.edu] \$ ";
send "yavta  --file=/data/edu/image.uyvy  -fYUYV -s 160x120  /dev/video0 --capture=4 --skip 3 \r";
expect "RS.edu] \$ ";
send "exit\r";
expect eof
SCRIPT
echo "Drone: Drone initialized!"

# 2. ftp into the drone and get the data
echo "Drone: Trying to download images from drone to DroneExchange..."
echo "> Drone: FTP into the drone"
/usr/bin/expect <<SCRIPT
spawn ftp 192.168.1.1;
expect "(192.168.1.1:$USER):";
send "\r";
expect "ftp>";
send "get testy.mjpeg \r";
expect "ftp>";
send "get image.uyvy \r";
expect "ftp>";
send "exit\r";
expect eof
SCRIPT

python ~/images/FollowTheYellowBrickRoad/simple_processing.py

# 3. Move the data file to the correct location

#mv img* ../DroneExchange/imgs

echo "Drone: imgs downloaded from drone to DroneExchange/imgs!\n"
