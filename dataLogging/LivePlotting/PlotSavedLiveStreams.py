import os
import re
import numpy as np
import matplotlib.pyplot as plt

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

plotall = False

files = []

if plotall:
    files_list = os.listdir(os.getcwd())

    for file in files_list:
        if 'stream_data' in file:
            files.append(file)
else:
    # Write the files you wante plottet
    # files.append("./data/sensitivity/oppositive_direction_raw_1cm_step")
    # files.append("./data/sensitivity/stream_data_offset_t65_t70")

    # Testing
    files.append('./data/new_sens/stream_data_raw_t10_t15')

    # files.append('./stream_data_raw_t15_t20')

    # Distance measure
    # files.append('./data/distance/stream_data_raw_t5_t10')

    # Signal Processing
    # files.append('./stream_data_raw_t10_t15')
    # files.append('./stream_cusum_data_t30_t35')

    # Compare raw signal to GLR
    # files.append('./data/GLR_forget/raw_signal')
    # files.append('./data/GLR_forget/glr_gk')

    # Compare mechanics
    # files.append('./data/compare_flat_angle/angle_data_raw')
    # files.append('./data/compare_flat_angle/flat_raw')

    # Sensitivity chapter 2
    # files.append("./data/sensitivity/new/stream_data_raw_t45_t50")
    # files.append("./data/sensitivity/new/stream_data_raw_t15_t20")
    # files.append("./data/sensitivity/new/stream_data_offset_t45_t50")
    # files.append("./data/sensitivity/new/stream_data_offset_t15_t20")

    # Sensitivity chapter
    # files.append("./data/sensitivity/stream_data_raw_t5_t10")
    # files.append("./data/sensitivity/stream_data_offset_t5_t10")
    # files.append("./data/sensitivity/stream_data_raw_t15_t20")
    # files.append("./data/sensitivity/stream_data_offset_t15_t20")


    # files.append("./data/sensitivity/Raw_data_4cm_step_1cm_negative_direction_1")
    # files.append("./data/sensitivity/Raw_data_4cm_step_1cm_positive_direction_2")
    # files.append("./stream_data_raw_t25_t30")
    # files.append("./stream_data_raw_t40_t45")


for file in files:
    x, y = get_data(file)
    # idx = np.where((x > 28) & (x < 29))
    # x = x[idx]
    # y = y[idx]
    plt.figure()
    filename = re.findall("[^/]+", file)[-1].replace('_', ' ')
    # plt.title(filename)
    x = np.linspace(0, 5, len(x))
    plt.plot(x,y)
    if 'raw' in filename.lower():
        plt.xlabel("Time [s]")
        plt.ylabel("Target Capacitance [1/640 pF]")
    elif 'offset' in filename.lower():
        plt.xlabel("Time [s]")
        plt.ylabel("Target Capacitance [1/50 pF]")
        plt.title('Highpass IIR filter - Offset')
    elif 'cusum' in filename.lower() or 'glr' in filename.lower():
        plt.xlabel('Time [s]')
        plt.ylabel('Decision function g(k)')
        # plt.title('GLR sample size 700 - Offset')
    # plt.ylim(675, 688)

plt.show()
