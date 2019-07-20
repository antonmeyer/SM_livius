import time
import datetime
from w1thermsensor import W1ThermSensor
sensor = W1ThermSensor()

while True:
	for sensor in W1ThermSensor.get_available_sensors([W1ThermSensor.THERM_SENSOR_DS18B20]):
    		print("%s;%.2f" % (sensor.id, sensor.get_temperature()))

	time.sleep (1)

