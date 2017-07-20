#!/bin/bash

SCRIPTPATH=$(pwd)

set -e

/usr/bin/expect <<SCRIPT
set timeout -1;
spawn ftp 192.168.2.1;
expect "(192.167.2.1:$USER):";
send "root\r";
expect "ftp>";
send "put /usr/local/MIT_MatlabToolbox/bin/firmware/dragon-prog dragon-prog\r"
expect "ftp>";
send "exit\r";
expect eof
SCRIPT

echo "FINISHED!!"
