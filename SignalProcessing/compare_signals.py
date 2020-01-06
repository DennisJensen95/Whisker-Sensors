import numpy as np
import matplotlib.pyplot as plt
import scipy.fftpack
import scipy.signal
from itertools import combinations, permutations
import re

# Number of samplepoints
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

def get_log_saved_data(filename):
    i = 0
    y = np.array([])
    file = open(filename, 'r+')
    graph_data = file.read()
    file.close()
    sample_time = None
    lines = graph_data.split('\n')
    for line in lines:
        if i == 0:
            sample_time = re.findall('\d+\.\d+', line)[0]
        elif i < len(lines) - 2:
            val = float(re.findall('\d+', line)[0])
            y = np.append(y, val)
        i += 1
    fs_log = len(y)/float(sample_time)
    x = np.linspace(0, float(sample_time), len(y))
    return x, y, fs_log

objects_com = False
coherence_test = False

if objects_com:
    x_list = {}
    y_list = {}
    x, y, fs_log = get_log_saved_data('./../dataLogging/data/object_class/yoga_mat_signal_2')
    x_list.update({'yoga':x})
    y_list.update({'yoga':y})
    x, y, fs_log_2 = get_log_saved_data('./../dataLogging/data/object_class/potholder_signal')
    x_list.update({'pot':x})
    y_list.update({'pot':y})
    x, y, fs_log_3 = get_log_saved_data('./../dataLogging/data/object_class/oven_grate_signal_2')
    x_list.update({'oven':x})
    y_list.update({'oven':y})
    fs_log_4 = np.inf
else:
    x_list = {}
    y_list = {}
    x, y, fs_log = get_log_saved_data('./../dataLogging/data/object_class/oven_grate_signal_2')
    x_list.update({'true':x})
    y_list.update({'true':y})
    x, y, fs_log_2 = get_log_saved_data('./../dataLogging/data/object_class/oven_grate_signal_3')
    x_list.update({'second':x})
    y_list.update({'second':y})
    x, y, fs_log_3 = get_log_saved_data('./../dataLogging/data/object_class/oven_grate_signal_4')
    x_list.update({'third':x})
    y_list.update({'third':y})
    x, y, fs_log_4 = get_log_saved_data('./../dataLogging/data/object_class/oven_grate_signal_5')
    x_list.update({'fourth': x})
    y_list.update({'fourth': y})

min_list = []
for key in y_list:
    min_list.append(x_list[key].shape[0])

objects_freq = {'oven': 0.769, 'yoga': 5.128, 'pot': 3.846}


num_data = min(min_list)

y_old = y_list.copy()

for key in y_list:
    y_list[key] = y_list[key][0:num_data]

fs = [fs_log, fs_log_2, fs_log_3, fs_log_4]
if not objects_com:
    objects = list(y_list.keys())
    combs = objects[1:]
else:
    objects = y_list.keys()
    combs = combinations(objects, 2)
fs = int(min(fs))
i = 0

if coherence_test:
    for com in combs:
        print(com)
        if objects_com:
            first_sig = y_list[com[0]]
            first_sig = first_sig - np.mean(first_sig)
            second_sig = y_list[com[1]]
            second_sig = second_sig - np.mean(second_sig)
        else:
            first_sig = y_list['true']
            first_sig = first_sig - np.mean(first_sig)
            second_sig = y_list[com]
            second_sig = second_sig - np.mean(second_sig)

        x, y = scipy.signal.coherence(first_sig, second_sig, fs=fs, nperseg=5000)
        idx = np.where((x >= 0.1) & (x < 11))
        # idx_2 = np.where(x < 20)
        # print(idx_2)
        print(idx)
        x = x[idx]
        y = y[idx]

        plt.figure()
        plt.plot(x, y)
        # plt.cohere(first_sig, second_sig, Fs=fs)
        # if objects_com:
        #     plt.title(f'{com}')
        # else:
        #     plt.title(f'True:{com}')

        i += 1
    plt.show()
else:
    i = 0
    for key in y_list:
        if key == 'oven' or key == 'pot' or key == 'yoga':
            exp_freq = objects_freq[key]
        else:
            exp_freq = objects_freq['oven']

        sig = y_list[key]
        x = x_list[key]
        y = y_old[key]
        sig = sig - np.mean(sig)

        sig = np.abs(scipy.fftpack.fft(sig))
        freqs = scipy.fftpack.fftfreq(len(sig)) * fs
        idx = np.where((freqs >= 0.1) & (freqs < 10))
        print(len(idx[0]))
        sig = sig[idx]
        freqs = freqs[idx]
        print(key)
        plt.figure(i+1)
        # plt.title(key)
        plt.plot(freqs, sig)
        plt.vlines(exp_freq, 0, max(sig))
        plt.legend(['Signal Response', 'Exp high intensity frequency'])
        plt.xlabel('Frequency [Hz]')
        plt.ylabel('Target Capacitance [1/50 pF]')

        # plt.figure()
        # plt.title(key)
        # plt.plot(x, y)
        # plt.xlabel('Frequency [Hz]')
        # plt.ylabel('Target Capacitance [1/50 pF]')

        i += 1
    plt.show()
