import matplotlib.pyplot as plt
import numpy as np
import re

def capacitor_value(width, length, mm=True):
    e_r = 2.25
    e_0 = 8.854*10**(-12)
    d = 0.005*10**(-2)

    if mm:
        width = width*10**(-3)
        length = length*10**(-3)
    A = length*width

    return e_0*e_r * A/d

def capacitors_in_series(C1, C2):
    return 1/(1/C1 + 1/C2)

def get_data(filename):
    x = []
    y = []
    file = open(filename, 'r+')
    data = file.read()
    lines = data.split('\n')

    for line in lines:
        x_res = re.findall('\((.*),', line)
        y_res = re.findall('.*,(.*)\)', line)
        if len(x_res) >= 1:
            x_res = x_res[0]
            y_res = y_res[0]
            if not (y_res == '0'):
                x.append(x_res)
                y.append(y_res)
    x = np.array(x).astype(np.float)
    y = np.array(y).astype(np.float)
    return x, y


############################### MEASURED WITH TEENSY ############################################
# For this file
x, y = get_data('./LivePlotting/data/sensitivity/new/stream_data_raw_t15_t20')
intervals = [[15, 15.5], [15.8, 16], [16.2, 16.3], [16.5, 16.6], [17, 17.5], [17, 17.5],
           [17.8, 17.9], [18.1, 18.2], [19, 19.5]]

steps = [0, 1, 2, 3, 4, 5, 6, 7, 8, 9]
centimeters = [0, 1, 2, 3, 4, 3, 2, 1, 0]
# centimeters = [0, -1, -2, -3, -4, -3, -2, -1, 0]
step = []

for inter in intervals:
    step_index = np.where((x > inter[0]) & (x < inter[1]))[0]
    step_value = np.mean(y[step_index])
    step.append(step_value)

step = np.array(step)*1/50
################################ MEASURED BY HAND MEASUREMENTS ###################################
ground = capacitor_value(7.56, 9.51)

print(f'Ground C: {ground}')

length_signal = 8.48
width = [3.23, 3.36, 3.81, 4.02, 4.38, 4.36, 4.18, 3.65, 3.22]
signal_arr = []
for wid in width:
    signal_arr.append(capacitor_value(wid, length_signal))

total_capcitor_arr = []

for cap in signal_arr:
    total_capcitor_arr.append(capacitors_in_series(cap, ground))

add = total_capcitor_arr[:-1]
add.reverse()
# print(add)
# [total_capcitor_arr.append(x) for x in add]
# print(len(total_capcitor_arr))
total_capcitor_arr = np.array(total_capcitor_arr)/(1e-12)
steps = [0, 1, 2, 3, 4, 3, 2, 1, 0]
cm = [0, 1, 2, 3, 4, 5, 6, 7, 8]
negative = [0, -1.0, -2.0, -3.0, -4.0]
positive = [0, 1.0, 2.0, 3.0, 4.0]

plt.plot(steps, total_capcitor_arr, '-o')
# plt.plot(step, '-o')
# plt.xticks(np.arange(9), [0, 1, 2, 3, 4, 3, 2, 1, 0])
plt.ylabel('Target Capacitance [pF]')
plt.xlabel('Offset [cm]')

plt.figure()
plt.plot(steps, step, '-o')
# plt.xticks(np.arange(9), [0, 1, 2, 3, 4, 3, 2, 1, 0])
plt.ylabel('Target Capactiance [pF]')
plt.xlabel('Offset [cm]')


plt.show()




