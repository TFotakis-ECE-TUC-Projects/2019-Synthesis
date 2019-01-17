from threading import Thread
from queue import Queue

import serial

ser = serial.Serial('/dev/ttyUSB0', 9600)

def fetch_uart():
    while True:
        ser.write(b'1');
        print(ser.readline())


t = Thread(target=fetch_uart)
t.start()
