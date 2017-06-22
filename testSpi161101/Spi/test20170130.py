# -*- coding: utf-8 -*-

import spidev
import RPi.GPIO as GPIO
import time
#import spi.max_speed_hz = 50000000

#GPIO.cleanup()

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
data = "abcdefg"

list0215 = list()
list0215 = []

i=0
for x in data:
	list0215.insert(i,ord(x))
	i += 1


str(spi.xfer2(list0215))
#str(spi.xfer2(data))

time.sleep(0.1)
#data = [0x16]

spi.close()
print "done"

GPIO.cleanup()


