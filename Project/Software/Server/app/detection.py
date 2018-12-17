from threading import Thread
from queue import Queue

import serial

ser = serial.Serial('COM6', 9600)

q = Queue()
Data = [True, True, True, True, True, True]

table_map = {(0, 2): 0, (0, 3): 2, (0, 4): 4, (1, 2): 1, (1, 3): 3, (1, 4): 5}


def fetch_uart():
	data = [None, None, None, None, None, None]
	while True:
		ser_inp = ser.readline()
		values = [int(el) for el in ser_inp.split()]
		for i in [0, 1]:
			for j in [2, 3, 4]:
				if (values[i] + values[j])/2 < 0.8 * 1000:
					data[table_map[(i, j)]] = True
				else:
					data[table_map[(i, j)]] = False
		q.put(data)


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
