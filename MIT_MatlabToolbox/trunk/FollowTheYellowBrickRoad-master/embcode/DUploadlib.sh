set -e

/usr/bin/expect <<SCRIPT
set timeout -1;
spawn ftp 192.168.1.1;
expect "(192.168.1.1:$USER):";
send "\r";
expect "ftp>";
send "put ./librsedu.so librsedu.so\r";
expect "ftp>";
send "exit\r";
expect eof
SCRIPT

echo "END!!"
