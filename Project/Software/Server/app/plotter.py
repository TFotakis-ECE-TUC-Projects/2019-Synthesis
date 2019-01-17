import pylab
import json

data = []
x = []
vals = [[], [], [], [], []]

f = open("log_16_1_2019_afternoon.data", "r")
for l in f.readlines():
    data.append(json.loads(l))
flag = False
start = 0;
for d in data:
    if flag==False:
        x.append(0)
        start = d["ts"]
        flag=True
    else:
        x.append(d["ts"]-start)
    for i in range(5):
        vals[i].append(d["val"][i])

pylab.plot(x, vals[0], x, vals[1], x, vals[2], x, vals[3], x, vals[4])
pylab.legend(["1", "2", "3", "4", "5"])
pylab.show()
