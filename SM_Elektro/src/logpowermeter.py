import csv
import signal
import sys
import time
from time import gmtime, strftime

import requests

period =10 # intervall in seconds
urlgetstr = "http://192.168.100.200/getdata" # IP Address needs to be static in ESP32

sumVal = 0
idx = 0

#prepare the file
filename = "smE" + time.strftime( "_%Y%m%d", time.gmtime()) + ".csv"
sme_file = open(filename, mode='w')
sme_writer = csv.writer(sme_file, delimiter=';', quotechar='"', quoting=csv.QUOTE_MINIMAL)

#write headline
#ToDo write serial to file or to filename
sme_writer.writerow(["timestamp", "m1_sumVal", "m1_actVal", "m2_sumCnt"])

def signal_handler(sig, frame):
        print('You pressed Ctrl+C!')
        sme_file.flush()
        sme_file.close()
       # sys.exit(0)

signal.signal(signal.SIGINT, signal_handler)
signal.signal(signal.SIGTERM, signal_handler)

nexttime = time.time()
while 1:
    try: 
        r = requests.get("http://192.168.100.200/getdata", timeout=3)
        jr = r.json()
        m1 = jr['meters']['meter1']
        m2 = jr['meters']['meter2']
        #log2file (m1['sumVal'], m1['actVal'], m2['sumCnt'])
        timestr = time.strftime( "%d.%m.%Y %H:%M:%S", time.gmtime())
        sme_writer.writerow([timestr, m1['sumVal'], m1['actVal'],m2['sumCnt']])
        sme_file.flush()
    except:
        e = sys.exc_info()[0]
        print ("we have a problem")
        print str(e)

    nexttime += period # a bit uggly if there is an overrun by time
    time.sleep(nexttime - time.time())
