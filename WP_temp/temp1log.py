import time
import datetime
from w1thermsensor import W1ThermSensor
sensor = W1ThermSensor()

while True:
	temp1 = sensor.get_temperature()
	now = datetime.datetime.now()
	print now.strftime("%y-%m-%d %H:%M:%S") +";" +str(temp1)
	time.sleep (10)

