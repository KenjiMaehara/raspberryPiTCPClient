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
	spi.max_speed_hz = 4000000
	
	GPIO.setmode(GPIO.BCM)
	# GPIOを稼動させます。
	GPIO.setup(16, GPIO.OUT)
	# GPIO 16番ピンを出力専用に設定します。
	
	GPIO.output(16, GPIO.HIGH)
	time.sleep(0.1)
	GPIO.output(16, GPIO.LOW)
	
	#while True:
	#try:
	print "writing data"
	#hello spi (ASCII)
	
	#data = [0x61,0x62,0x63,0x64,0x65,0x66,0x67]
	#data = b"abcdefg"
	data = [0x4d,0x45,0x2c,0x72,0x64,0x2c,0x01,0x0d]
	
	str(spi.xfer2(data))
	
	time.sleep(0.1)
	#data = [0x16]
	
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


