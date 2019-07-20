#! /usr/bin/python
import ow

ow.init('localhost:4304')
sensorlist = ow.Sensor("/uncached/").sensorList( )

str1 = ""
for sensor in sensorlist:
    print sensor.address