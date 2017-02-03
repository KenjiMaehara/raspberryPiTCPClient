# -*- coding: utf-8 -*-

import spidev
import RPi.GPIO as GPIO
import time
import sys
#import spi.max_speed_hz = 50000000

#GPIO.cleanup()

def commSpi():
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

	data03 = "ME,rd,01,abc,\r"
	print data03 

	i = 0

	for x in data03:
		list0203.insert(i,ord(x))
		i += 1	


	print "list0203 :  ",
	i = 0
	while i < len(list0203):
		print hex(list0203[i]),
		i += 1

	
	print "\n"

	data = [0x00,0x00,0x4d,0x45,0x2c,0x72,0x64,0x2c,0x30,0x31,0x2c,0x61,0x62,0x63,0x2c,0x0d,0x00,0x00]
	
	print "data :      ",
	i=0
	while i < len(list0203):
		print hex(list0203[i]),
		i += 1


	print "\n"

	str(spi.xfer2(data))
	
	time.sleep(0.3)


	list01 = list()
	list01 = []
	i=0
	while i < 10:
		data = [0x00]
		list01.insert(i,spi.xfer2(data)[0])
		i += 1




	i=0
	while i < len(list01):
		print hex(list01[i]),
		i += 1


	#i=0
	#while i < 10:
	#	a = chr(list01[i])
	#	print a
	#	i += 1


	i=0
	while i < len(list01):
		if list01[i] == 0x4d and list01[i+1] == 0x45:
			print "\n address:",
			print chr(list01[i+3]),
			print chr(list01[i+4])     ,
			print "ch8～1 : ",
			print format(list01[i+6], 'b'),
			print "   ch16～9 : ",
			print format(list01[i+7], 'b')
		i += 1

	#print hex(list01[0])
	#print(list01.find('ME'))
	#print '%s' % list01

	spi.close()
	print "done"
	
	GPIO.cleanup()

def parser():
	usage = 'Usage: python {} FILE [--verbose] [--cat <file>] [--help]'\
	        .format(__file__)
	arguments = sys.argv
	if len(arguments) == 1:
	    return usage
	# ファイル自身を指す最初の引数を除去
	arguments.pop(0)
	# 引数として与えられたfile名
	fname = arguments[0]
	#if fname.startswith('-'):
	#    return usage
	# - で始まるoption
	options = [option for option in arguments if option.startswith('-')]
	
	if '-h' in options or '--help' in options:
	    return usage
	if '-v' in options or '--verbose' in options:
	    return 'your input is {}!!!'.format(fname)
	if '-c' in options or '--cat' in options:
	    cat_position = arguments.index('-c') \
	            if '-c' in options else arguments.index('--cat')
	    another_file = arguments[cat_position + 1]
	    return 'concatnated: {}{}'.format(fname, another_file)
	if '-ia' in options or '--inputall' in options:
		commSpi()
		return 'testtest0201!!!'
	return 'input is {}'.format(fname)

if __name__ == '__main__':
    result = parser()
    print(result)


