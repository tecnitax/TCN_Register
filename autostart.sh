 #! /bin/sh

 ### BEGIN INIT INFO
 # Provides:          noip
 # Required-Start:    $remote_fs $syslog
 # Required-Stop:     $remote_fs $syslog
 # Default-Start:     2 3 4 5
 # Default-Stop:      0 1 6
 # Short-Description: Simple script to start a program at boot
 ### END INIT INFO

cd  /home/pi/Desktop/registrador14
./hora > out.txt
./registrador &
./tcn &


 #change YourProgramExactName to Exact name of your program that you want to auto start
 exit 0 
