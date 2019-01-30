from threading import Thread
from queue import Queue
import time

import serial
import json

plotting = False

if plotting:
    import pylab

import random
import subprocess
import numpy


ser = serial.Serial('/dev/ttyUSB0', 9600)
TRAINING = False

if TRAINING:
	from pynput import keyboard

if plotting:
    xq = Queue()
    vq = Queue()

Data = [True]*5
Keyboard = [False]*5
Done = False


def playdatshit():
    subprocess.call(["ffplay", "-nodisp", "-autoexit", "-loglevel", "panic", "./app/fire.mp3"])

def fetch_uart():
    global Data
    x = []
    vals = [[], [], [], [], []]
    fft_vals = [[], [], [], [], []]

    data = [False]*5
    plot_mem = 120
    fft_mem = 16
    flag = False
    while True:
        ser_inp = ser.readline()
        t = time.time()
        x.append(t)
        if len(x) > plot_mem:
            x.pop(0)
            for i in range(5):
                vals[i].pop(0)
        if plotting:
            xq.put(x)
            vq.put(vals)
        values = [int(el) for el in ser_inp.split()]
        for i in range(5):
            if values[i] > 1023:
                values[i] = 1023
            elif values[i] < 0:
                values[i] = 0
        for i in range(5):
            vals[i].append(values[i])
            if len(vals[i]) < fft_mem:
                fft_vals[i] = vals[i]
            else:
                fft_vals[i] = vals[i][len(vals[i])-fft_mem:]
        data = fft_detection(fft_vals)
        #if flag:
        #    t4 = Thread(target=playdatshit)
        #    t4.start()
        Data = data
        print(values)
        print(data)
        logger = open("log.data", "a")
        logger.write(json.dumps({"ts": t, "val": values, "smk": data})+"\n")
        logger.close()

def fetch_uart_training():
    global Keyboard
    global Done
    x = []
    vals = [[], [], [], [], []]

    data = [False]*5
    new_data = [False]*5
    plot_mem = 120
    flag = False
    while not Done:
        ser_inp = ser.readline()
        t = time.time()
        x.append(t)
        if len(x) > plot_mem:
            x.pop(0)
            for i in range(5):
                vals[i].pop(0)
        if plotting:
            xq.put(x)
            vq.put(vals)
        values = [int(el) for el in ser_inp.split()]
        for i in range(5):
            if values[i] > 1023:
                values[i] = 1023
            elif values[i] < 0:
                values[i] = 0
        for i in range(5):
            vals[i].append(values[i])
        print(values)
        print(Keyboard)
        logger = open("log.data", "a")
        logger.write(json.dumps({"ts": t, "val": values, "smk": Keyboard})+"\n")
        logger.close()

def plotter():
    global Done
    while not Done:
        x = xq.get()
        vals = vq.get()
        pylab.clf()
        pylab.plot(x, vals[0], x, vals[1], x, vals[2], x, vals[3], x, vals[4])
        pylab.legend(["1", "2", "3", "4", "5"])
        pylab.pause(0.0001)
    pylab.close()

def fft_detection(vals):
    N = len(vals[0])
    FW = N/4 - 1
    data = []
    for i in range(5):
        #print(vals[i])
        fft_t = (numpy.fft.fftshift(numpy.fft.fft(vals[i])))
        fft_t = sum(abs(fft_t[int(N/2+1):]))/N/FW
        print(fft_t)
        if fft_t > 1:
            data.append(True)
        else:
            data.append(False)
    #print(data)
    return data


def randomshit():
    global Data
    while(True):
        Data = [random.random()<0.05 for _ in range(5)]
        flag = False
        for i in range(6):
            if Data[i]:
                flag = True
        if flag:
            t4 = Thread(target=playdatshit)
            t4.start()
        time.sleep(0.5)

def getData():
    global Data
    return {'tables': Data}

def on_press(key):
    global Keyboard
    try:
        if key.char in ["1", "2", "3", "4", "5"]:
            Keyboard[int(key.char)-1] = True
    except:
        pass

def on_release(key):
    global Keyboard
    global Done
    try:
        if key.char in ["1", "2", "3", "4", "5"]:
            Keyboard[int(key.char)-1] = False
    except:
        if key==keyboard.Key.esc:
            Done = True
            return False

if __name__=="__main__":
    cmd = fetch_uart_training

    t1 = Thread(target=cmd)
    t1.start()

    if plotting:
        t2 = Thread(target=plotter)
        t2.start()

    if TRAINING:
        with keyboard.Listener(on_press=on_press, on_release=on_release) as listener:
            listener.join()
            t1.join()
            t2.join()
    #t3 = Thread(target=randomshit)
    #t3.start()
else:
    cmd = fetch_uart

    t1 = Thread(target=cmd)
    t1.start()

    if plotting:
        t2 = Thread(target=plotter)
        t2.start()

