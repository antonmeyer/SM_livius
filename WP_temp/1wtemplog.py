#! /usr/bin/python
import ow
import time
from time import gmtime, strftime
import csv

filename = "project" + time.strftime( "_%Y%m%d", time.gmtime()) + ".csv"
tmp_file = open(filename, mode='w')
temp_writer = csv.writer(tmp_file, delimiter=';', quotechar='"', quoting=csv.QUOTE_MINIMAL)

# has to match with ow settings
ow.init('localhost:4304')
sensorlist = ow.Sensor("/uncached/").sensorList()

def getAlias(Sensor): return Sensor.alias

aliaslist= map(getAlias, sensorlist)

headline = ["timestamp"]
headline.extend(aliaslist)

print headline
temp_writer.writerow(headline)

#ToDo recover from a lost sensor;
# rescan, new file, detect a new or lost
# try ...exception
#might be a good reason to change files every hour

tempvallist = [0.0] * len(sensorlist)

period = 1 # in seconds
nexttime = time.time()
while True:
    for idx in range(len(sensorlist)):

        try:
            temp = float(sensorlist[idx].temperature)
        #it happens from time to time that negative values are reported
        # root cause is unknown ToDo
            while (temp < -25.0):
#	        time.sleep(1)
	            temp = float(sensorlist[idx].temperature)
        except ow.exUnknownSensor:
            print "error reading:", idx
            temp = "na"

        tempvallist[idx] = temp

    tempstr = ';'.join(map(str,tempvallist))

    print time.strftime( "%d.%m.%Y %H:%M:%S", time.gmtime()), ";", tempstr

    rowstr = [time.strftime( "%d.%m.%Y %H:%M:%S", time.gmtime())]
    rowstr.extend(tempvallist)

    temp_writer.writerow(rowstr)

    nexttime += period # a bit uggly if there is an overrun by time
    time.sleep(nexttime - time.time())
