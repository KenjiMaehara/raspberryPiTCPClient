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
#hello spi (ASCII)
#data = [0x4d,0x45,0x2c,0x72,0x64,0x2c,0x01,0x0d]
data = b"ME,rd,1\n"
#data = "abcdefg"
#data = [0x20]
#resp = spi.xfer2(data)
str(spi.xfer2(data))


#data = [0x72]
#str(spi.xfer2(data))
#time.sleep(0.1)
#data = [0x64]
#str(spi.xfer2(data))
#time.sleep(0.1)
#data = [0x2c]
#str(spi.xfer2(data))
#time.sleep(0.1)
#data = [0x01]
#str(spi.xfer2(data))
#time.sleep(0.1)
#data = [0x0d]
#str(spi.xfer2(data))
#time.sleep(0.1)

#str(spi.xfer2(data))
#spi.xfer2(data)
#spi.xfer2(data)
time.sleep(0.1)
#data = [0x16]
#str(spi.xfer2(data))
#time.sleep(0.1)
#data = [0x00]
#data = [0x0d]
#str(spi.xfer2(data))
#data = [0x00]
#print "CH18  >>>" + str(spi.xfer2(data))
#data = [0x00]
        #print ">>>" + str(spi.xfer2([0x00]))

        #time.sleep(0.1)
        #n = 0
        # while n < 10:
        #         print ">>>" + str(spi.xfer2([0x00]))
        #         n += 1
        #else:
        #         print "END"

        #time.sleep(1)
        #counter += 1
        #if counter > 4:
            #break
        time.sleep(0.1)
    #except(keyboardInterrupt, SystemExit):
        #spi.close()
        #raise
#spi.close()
print "done"

GPIO.cleanup()


