#! /usr/bin/python
import ow
import time
ow.init('localhost:4304')
mysensors = ow.Sensor("/uncached/").sensorList( )
while True:
    for sensor in mysensors[:]:
        thisID = sensor.address[2:12]
	temp = float(sensor.temperature)
	while (temp < 0):
	    print "ID = ", thisID, ": error: ", temp
#	    time.sleep(1)
	    temp = float(sensor.temperature)

 	print "ID = ", thisID, " : ", temp
    time.sleep(10)
