from threading import Thread
from queue import Queue
import time

import serial
import json
import pylab

ser = serial.Serial('/dev/ttyUSB0', 9600)

q = Queue()
xq = Queue()
vq = Queue()
Data = [True, True, True, True, True, True]

table_map = {(0, 2): 0, (0, 3): 2, (0, 4): 4, (1, 2): 1, (1, 3): 3, (1, 4): 5}


def fetch_uart():
    x = []
    vals = [[], [], [], [], []]

    data = [False]*6
    new_data = [False]*6
    memory = 6
    plot_mem = 120
    counters = [memory]*6;
    flag = False
    while True:
        for i in range(6):
            if counters[i] <= 0:
                data[i] = False
            counters[i] = counters[i]-1
        flag = True
        ser_inp = ser.readline()
        t = time.time()
        x.append(t)
        if len(x) > plot_mem:
            x.pop(0)
            for i in range(5):
                vals[i].pop(0)
        xq.put(x)
        vq.put(vals)
        values = [int(el) for el in ser_inp.split()]
        for i in range(5):
            vals[i].append(values[i])
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
        logger.write(json.dumps({"ts": t, "val": values})+"\n")
        logger.close()

def plotter():
    while(True):
        x = xq.get()
        vals = vq.get()
        pylab.clf()
        pylab.plot(x, vals[0], x, vals[1], x, vals[2], x, vals[3], x, vals[4])
        pylab.legend(["1", "2", "3", "4", "5"])
        pylab.pause(0.0001)

t = Thread(target=fetch_uart)
t.start()

t2 = Thread(target=plotter)
t2.start()


def getData():
    global Data
    if q.empty():
        return {'tables': Data}
    else:
        Data = q.get()
        return {'tables': Data}
