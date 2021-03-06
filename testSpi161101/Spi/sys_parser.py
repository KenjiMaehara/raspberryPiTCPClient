# -*- coding: utf-8 -*-

import spidev
import RPi.GPIO as GPIO
import time
import sys
import argparse
#import spi.max_speed_hz = 50000000

#GPIO.cleanup()

def commSpi(data03):
	spi = spidev.SpiDev()
	spi.open(1,0)
	counter = 0
	#spi.max_speed_hz = 4000000
	spi.max_speed_hz = 1000000
	
	GPIO.setmode(GPIO.BCM)
	# GPIOを稼動させます。
	GPIO.setup(16, GPIO.OUT)
	# GPIO 16番ピンを出力専用に設定します。
	
	GPIO.output(16, GPIO.HIGH)
	time.sleep(0.1)
	GPIO.output(16, GPIO.LOW)

	list0203 = list()
	list0203 = []

	i = 0
	for x in data03:
		list0203.insert(i,ord(x))
		i += 1


	##################   for debug 20170203   ###################################################
	#print "list0203 :  ",
	#i = 0
	#while i < len(list0203):
	#	print hex(list0203[i]),
	#	i += 1

	
	#print "\n"

	#data = [0x00,0x00,0x4d,0x45,0x2c,0x72,0x64,0x2c,0x30,0x31,0x2c,0x61,0x62,0x63,0x2c,0x0d,0x00,0x00]
	
	#print "data :      ",
	#i=0
	#while i < len(list0203):
	#	print hex(list0203[i]),
	#	i += 1


	#print "\n"
	##################   for debug 20170203   ###################################################

	str(spi.xfer2(list0203))
	
	time.sleep(0.3)


	list01 = list()
	list01 = []
	i=0
	while i < 30:
		data = [0x00]
		list01.insert(i,spi.xfer2(data)[0])
		i += 1

	spi.close()
	
	GPIO.cleanup()
	return list01

def parser():

	parser = argparse.ArgumentParser()
	parser.add_argument('--inputAll')
	parser.add_argument('--relayCtl', nargs=2)
	parser.add_argument('--wSensor', nargs=5)
	parser.add_argument('--foo', nargs=2)
	parser.add_argument('--bar', nargs=1)
	args = parser.parse_args()

	if args.inputAll:
		data03 = "ME,rd,inputall,abc,\r"
		list01 = commSpi(data03)

		mojiretsu = ''.join(chr(x) for x in list01)
		print mojiretsu
		listRcvData = mojiretsu.split(",")
		print type(listRcvData)


		i=0
		while i < len(listRcvData):
			print listRcvData[i]
			i += 1


		return 'testtest0201!!!'
	
	if args.relayCtl:
		data03 = "ME,wr,relayCtl," + args.relayCtl[0] + "," + args.relayCtl[1] + ",\r"
		#############################################
		#	option 2:	ch1 --- ch5					#
		#	option 3:	on or off					#
		#############################################

		list01 = commSpi(data03)

		mojiretsu = ''.join(chr(x) for x in list01)
		print mojiretsu
		listRcvData = mojiretsu.split(",")
		print type(listRcvData)


		i=0
		while i < len(listRcvData):
			print listRcvData[i]
			i += 1
		

		return "test020106!"

	if args.wSensor:
		data03 = "ME,wr,wSensor," + args.wSensor[0] + "," + args.wSensor[1] + "," + args.wSensor[2] + "," + args.wSensor[3] + "," + args.wSensor[4] + ",\r"
		print data03
		list01 = commSpi(data03)

		return "test0207"

if __name__ == '__main__':
    result = parser()
    print(result)


