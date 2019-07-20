#! /usr/bin/python
import ow
import time
ow.init('localhost:4304')
mysensors = ow.Sensor("/uncached/").sensorList( )
while True:
   for sensor in mysensors[:]:
       thisID = sensor.address[2:12]
       print "ID = ", thisID, " : ", sensor.temperature
   time.sleep(10)
