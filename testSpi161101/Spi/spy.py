import spidev
import datetime
import locale
import time

i=0	#�J��Ԃ��𐧌䂷��ϐ�
d0=0	#MCP3002����o�͂��ꂽAD�ϊ�����
v0=0	#MCP3002�ɓ��͂��ꂽ�A�i���O�d��
t0=0	#�����̉��x
d1=0	#����,�������L������B


spi = spidev.SpiDev()
spi.open(0,0)
f=open("/home/pi/share/tmp1.txt","w")
f.close()

while i		# read SPI data from MCP3002 chip

def get_adc(channel):

	if ((channel > 1) or (channel < 0)):	#channel��0 or 1�݂̂��Ƃ�܂��B
		return -1


	r = spi.xfer2([1,(2+channel)<<6,0])	#(*1)���Q��  

	ret = ((r[1]&31) << 6) + (r[2] >> 2)	#(*2)�@���Q��  
	return ret
	i=i+1

	d0=get_adc(0)				#AD�ϊ��l�ł��B 
	v0=get_adc(0)*3.3/1024			#MCP3002�ɓ��͂��ꂽ�A�i���O�d���ł��B
	t0=(get_adc(0)*3.3/1024-0.5)/0.01	#�����̉��x���v�Z���Ă��܂��B
	d1=datetime.datetime.today()		#�f�[�^���擾���ꂽ������\�����܂��B

	print 'ADC=%s'  % (d0)
	print 'VOL=%s'  % (v0)				#input voltage
	print 'TEMP=%s' % (t0)				#temperature
	print 'd == %s : %s\n' % (d1, type(d1))		#time now
	f=open("/home/pi/share/tmp1.txt","a")		#�f�[�^���������ރt�@�C����ǉ����[�h�ŊJ���܂��B
	f.write('ADC=%s   '  % (d0))
	f.write('VOL=%s   '  % (v0))
	f.write('TEMP=%s   '  % (t0))
	f.write('d == %s : %s\n   ' % (d1, type(d1)))
	f.close()					#�t�@�C������܂��B
	time.sleep(300)					#5���قǑ҂��܂��B