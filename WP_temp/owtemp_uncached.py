#! /usr/bin/python
import ow
import time
ow.init('localhost:4304')
while True:
   mysensors = ow.Sensor("/uncached").sensorList( )
   for sensor in mysensors[:]:
       thisID = sensor.address[2:12]
       print "ID = ", thisID, " : ", sensor.temperature
   time.sleep(10)
