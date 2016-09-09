#!/bin/sh
#ログの保存ディレクトリを指定
#LOGDIR=ログ保存ディレクトリ
LOGDIR=/home/pi/python

#while true
#do
#isAlive=`ps -ef | grep " sudo python /home/pi/Desktop/tcp_client1_02.py " | \grep -v grep | wc -l`
#if [ $isAlive = 1 ]:then

#日付かわったらログファイル名かわります
FNAME=`date +%Y%m%d`

#if ps axww | grep "[s]udo python /home/pi/Desktop/tcp_client1_02.py" >/dev/null
if ps axww | grep "[p]ython /home/pi/raspberryPiTCPClient/tcp_client1_02.py" >/dev/null
then
echo "`date +%Y%m%d-%H%M%S` :プロセスは生きています" | tee -a $LOGDIR/${FNAME}.log
else
echo "`date +%Y%m%d-%H%M%S` :プロセスは死んでいます" | tee -a $LOGDIR/${FNAME}.log
sh /home/pi/python/autorun_script
fi
:
#sleep 60
#done