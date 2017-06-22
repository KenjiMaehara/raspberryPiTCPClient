import spidev
import datetime
import locale
import time

i=0	#繰り返しを制御する変数
d0=0	#MCP3002から出力されたAD変換結果
v0=0	#MCP3002に入力されたアナログ電圧
t0=0	#室内の温度
d1=0	#日時,時刻を記憶する。


spi = spidev.SpiDev()
spi.open(0,0)
f=open("/home/pi/share/tmp1.txt","w")
f.close()

while i		# read SPI data from MCP3002 chip

def get_adc(channel):

	if ((channel > 1) or (channel < 0)):	#channelは0 or 1のみをとります。
		return -1


	r = spi.xfer2([1,(2+channel)<<6,0])	#(*1)を参照  

	ret = ((r[1]&31) << 6) + (r[2] >> 2)	#(*2)　を参照  
	return ret
	i=i+1

	d0=get_adc(0)				#AD変換値です。 
	v0=get_adc(0)*3.3/1024			#MCP3002に入力されたアナログ電圧です。
	t0=(get_adc(0)*3.3/1024-0.5)/0.01	#室内の温度を計算しています。
	d1=datetime.datetime.today()		#データが取得された時刻を表示します。

	print 'ADC=%s'  % (d0)
	print 'VOL=%s'  % (v0)				#input voltage
	print 'TEMP=%s' % (t0)				#temperature
	print 'd == %s : %s\n' % (d1, type(d1))		#time now
	f=open("/home/pi/share/tmp1.txt","a")		#データを書き込むファイルを追加モードで開きます。
	f.write('ADC=%s   '  % (d0))
	f.write('VOL=%s   '  % (v0))
	f.write('TEMP=%s   '  % (t0))
	f.write('d == %s : %s\n   ' % (d1, type(d1)))
	f.close()					#ファイルを閉じます。
	time.sleep(300)					#5分ほど待ちます。