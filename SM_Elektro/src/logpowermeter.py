import requests
import time
from time import gmtime, strftime
import csv

intervall =10 # intervall in seconds
urlgetstr = "http://192.168.1.111/getdata" # IP Address needs to be static in ESP32

sumVal = 0
idx = 0

#prepare the file
filename = "smE" + time.strftime( "_%Y%m%d", time.gmtime()) + ".csv"
sme_file = open(filename, mode='w')
sme_writer = csv.writer(sme_file, delimiter=';', quotechar='"', quoting=csv.QUOTE_MINIMAL)

#write headline
#ToDo write serial to file or to filename
sme_writer.writerow({["timestamp", "m1_sumVal", "m1_actVal", "m2_sumCnt"]})

while 1:
    time.sleep(intervall) #we might put that at the end of the while loop

    try: 
        r = requests.get(urlgetstr, timeout=3)
        jr = r.json()
        m1 = jr['meters']['meter1']
        m2 = jr['meters']['meter2']
        #log2file (m1['sumVal'], m1['actVal'], m2['sumCnt'])
        timestr = time.strftime( "%d.%m.%Y %H:%M:%S", time.gmtime())
        sme_writer.writerow({timestr, m1['sumVal'], m1['actVal'],m2['sumCnt']})

    except:
        print "we have a problem"


