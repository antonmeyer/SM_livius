import requests
import time

sumVal = 0
idx = 0
while idx < 120000:
    try: 
        r = requests.get('http://192.168.1.111/getdata')

        jr = r.json()

        m1 = jr['meters']['meter1']

        sumVal = m1['sumVal']
        print sumVal, idx
        idx +=1
    except:
        print "we have a problem"
        time.sleep(1)

