from threading import Thread
from queue import Queue
import time

import serial
import json

ser = serial.Serial('/dev/ttyUSB0', 9600)

q = Queue()
Data = [True, True, True, True, True, True]

table_map = {(0, 2): 0, (0, 3): 2, (0, 4): 4, (1, 2): 1, (1, 3): 3, (1, 4): 5}


def fetch_uart():
    data = [False]*6
    new_data = [False]*6
    memory = 6
    counters = [memory]*6;
    flag = False
    while True:
        for i in range(6):
            if counters[i] <= 0:
                data[i] = False
            counters[i] = counters[i]-1
        flag = True
        ser_inp = ser.readline()
        values = [int(el) for el in ser_inp.split()]
        for i in [0, 1]:
            for j in [2, 3, 4]:
                if (values[i] + values[j])/2 < 0.3 * 1000:
                    new_data[table_map[(i, j)]] = True
                else:
                    new_data[table_map[(i, j)]] = False
        for i in range(6):
            if new_data[i]:
                counters[i] = memory 
                data[i] = True
                flag = True
        q.put(data)
        print(values)
        print(data)
        logger = open("log.data", "a")
        logger.write(json.dumps({"ts": time.time(), "val": values})+"\n")
        logger.close()


t = Thread(target=fetch_uart)
#t.daemon = True
t.start()


def getData():
    global Data
    if q.empty():
        return {'tables': Data}
    else:
        Data = q.get()
        return {'tables': Data}
