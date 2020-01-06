import numpy as np
import re
import matplotlib.pyplot as plt
import os

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

# x, y = get_data('./data/sensitivity/new/stream_data_raw_t15_t20')
# x, y = get_data('./data/sensitivity/oppositive_direction_raw_1cm_step')
#intervals = [[10, 10.5], [10.9, 11.1], [11.4, 11.45], [11.7, 11.9], [12.1, 12.2], [12.5, 12.6],
#            [12.8, 13], [13.2, 13.4], [14, 15]]

# For this file
# x, y = get_data('./data/sensitivity/new/stream_data_raw_t15_t20')
# intervals = [[15, 15.5], [15.8, 16], [16.2, 16.3], [16.5, 16.6], [17, 17.5], [17, 17.5],
#            [17.8, 17.9], [18.1, 18.2], [19, 19.5]]

# Distance
# x, y = get_data('./data/distance/stream_data_raw_t5_t10')
# intervals = [[5, 5.4], [5.6, 5.7], [6, 6.1], [6.4, 6.5], [7, 7.5], [7, 7.5], [7, 7.5],
#              [7, 7.5], [7, 7.5], [8.2, 8.3], [8.5, 8.6], [8.8, 8.9], [9.3, 9.6]]
# print(len(intervals))

# New sens
# x, y = get_data('./data/new_sens/stream_data_raw_t10_t15')
# intervals = [[10, 10.5], [11.5, 11.7], [12.1, 12.3], [12.5, 12.6], [12.8, 13], [13.2, 13.3],
#              [13.7, 13.8], [14.1, 14.2], [14.6, 14.9]]


# For this file
x, y = get_data('./data/sensitivity/new/stream_data_raw_t45_t50')
intervals = [[45, 45.6], [45, 45.6], [46.4, 46.5], [46.8, 47], [47.3, 47.4], [47.6, 47.7],
           [48, 48.2], [48.6, 48.7], [49.5, 50]]


# intervals = [[15, 15.5], [15.8, 16], [16.2, 16.3], [16.5, 16.6], [17, 17.5], [17, 17.5],
#            [17.8, 17.9], [18.1, 18.2], [19, 19.5]]
# intervals = [[25.3, 25.4], [25.5, 25.65], [26, 26.2], [26.5, 26.7], [26.9, 27.1], [27.4, 27.55],
#              [27.9, 28], [28.175, 28.2],  [29, 30]]
distance = False
step = []
if distance:
    steps = [7, 6, 5, 4, 3, 2, 1, 2, 3, 4, 5, 6, 7]
    positive = [8.0, 7.0, 6.0, 5.0, 4.0, 3.0, 2.0, 1.0]
else:
    steps = [0, 1, 2, 3, 4, 3, 2, 1, 0]
    negative = [0, -1.0, -2.0, -3.0, -4.0]
    positive = [0, 1.0, 2.0, 3.0, 4.0]
    centimeters = [0, 1, 2, 3, 4, 3, 2, 1, 0]
    # centimeters = [0, -1, -2, -3, -4, -3, -2, -1, 0]


for inter in intervals:
    step_index = np.where((x > inter[0]) & (x < inter[1]))[0]
    step_value = np.mean(y[step_index])
    step.append(step_value)

# step = np.array(step)*1/50
print(len(step))
print(len(steps))

# change = step[4] - step[3]
# change_rate_cm = change*1/50
# print(change_rate_cm)
# centimeters = centimeters[:len(step)]
plt.plot(steps, step, '->')
# plt.xticks(np.arange(9), centimeters)
# plt.xticks(np.arange(5), negative)
# plt.xticks(np.arange(8), positive)
plt.xlabel("Offset distance [cm]")
plt.ylabel("Target Capacitance [1/640 pF]")
# plt.title("Offset measure in capacitor value through steps of 1cm")
plt.show()


# second_step = np.where(x)
# print(np.where(x >= 11)[0][0])
