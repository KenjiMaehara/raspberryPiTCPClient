#!/bin/sh
while true
do
#isAlive=`ps -ef | grep " sudo python /home/pi/raspberryPiTCPClient/tcp_client1_02.py " | \grep -v grep | wc -l`
#if [ $isAlive = 1 ]:then
#if ps axww | grep "[s]udo python /home/pi/raspberryPiTCPClient/tcp_client1_02.py" >/dev/null
if ps axww | grep "[p]ython /home/pi/raspberryPiTCPClient/tcp_client1_02.py" >/dev/null
then
echo "プロセスは生きています"
else
echo "プロセスは死んでいます"
sudo sh /home/pi/raspberryPiTCPClient/autorun_script
fi
sleep 3
done
