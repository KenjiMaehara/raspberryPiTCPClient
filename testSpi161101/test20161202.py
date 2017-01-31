# -*- coding: utf-8 -*-

import spidev
import RPi.GPIO as GPIO
import time
#import spi.max_speed_hz = 50000000


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
data = b"abcdefg"
str(spi.xfer2(data))
time.sleep(0.1)
#except(keyboardInterrupt, SystemExit):
spi.close()
#raise
#spi.close()



print "done"

