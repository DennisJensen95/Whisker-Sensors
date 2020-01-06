import numpy as np
import matplotlib.pyplot as plt
import scipy.fftpack
import scipy.signal
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

x_list = []
y_list = []
x, y, fs_log = get_log_saved_data('./../dataLogging/data/object_class/yoga_mat_signal_2')
x_list.append(x)
y_list.append(y)
print(fs_log)
x, y, fs_log = get_log_saved_data('./../dataLogging/data/object_class/potholder_signal')
x_list.append(x)
y_list.append(y)
print(fs_log)
x, y, fs_log = get_log_saved_data('./../dataLogging/data/object_class/oven_grate_signal_2')
x_list.append(x)
y_list.append(y)
print(fs_log)
# x, y = get_data('./../dataLogging/LivePlotting/data/signal_test/stream_data_raw_t5_t10')
# x, y = get_data('./../dataLogging/LivePlotting/data/spectrum_ana/stream_data_raw_t10_t15')
# x, y, fs_log = get_log_saved_data('./../dataLogging/data/random_signals/test_2')
# x, y, fs_log = get_log_saved_data('./../dataLogging/data/potholder_signal')

# x_list.append(x)
# y_list.append(y)

# x1, y1 = get_data('./../dataLogging/LivePlotting/stream_data_offset_t5_t10')
# idx_list = [[40.6, 42.2], [60.6, 60.85], [181.2, 181.9]]
idx_list = [[0, 10], [0, 10], [0, 10]]
titles = ['yoga_mat test', 'potholder test', 'oven_grate test']

only_test = True
power_spectral_desnity = True

spectrum = []
padding = [1000, 150, 400, 0]
for i in range(len(x_list)):
    x = x_list[i]
    y = y_list[i]
    N = len(x)
    # sample spacing
    fs = 21768
    T = 1/fs

    if only_test:
        if 'test' in titles[i]:
            plt.figure()
            plt.plot(x, y)
            # plt.title(titles[i])
            plt.xlabel('Time [s]')
            plt.ylabel('Target Capacitance [1/50 pF]')

    idx = np.where((x > idx_list[i][0]) & (x <= idx_list[i][1]))
    x = x[idx]
    y = y[idx]

    from scipy import optimize
    # subtract drift
    # lin = lambda x, a, b : a * x + b
    # coeff, _ = optimize.curve_fit(lin,x, y)
    # dmy= y- coeff[0] * x + coeff[0]
    if only_test:
        if 'test' in titles[i]:
            plt.figure()
            # plt.title(titles[i])
            plt.plot(x, y)

    yf =  y - np.mean(y)
    # print(np.mean(yf))
    # yf = scipy.signal.detrend(yf)
    # y = np.pad(y, (0, padding[i]) ,mode='constant')
    yf = np.abs(scipy.fftpack.fft(yf))

    if 'log' in titles[i]:
        spectrum.append(yf)
        freqs = scipy.fftpack.fftfreq(len(y)) * fs_log
    else:
        spectrum.append(yf)
        freqs = scipy.fftpack.fftfreq(len(y)) * fs

    idx = np.where(freqs >= 1)
    yf = yf[idx]
    freqs = freqs[idx]

    # plt.figure()
    # plt.plot(x1, y1)
    if only_test:
        if 'test' in titles[i]:
            plt.figure()
            # plt.title(titles[i])
            # plt.plot(scipy.fftpack.fftshift(freqs), scipy.fftpack.fftshift(yf))
            plt.plot(freqs, yf)
            plt.xlabel('Frequency [Hz]')
            plt.ylabel('Target Capacitance [1/50 pF]')

    if power_spectral_desnity and 'test' in titles[i]:
        plt.figure()
        plt.psd(yf, Fs=fs_log)


plt.show()



# print(np.correlate(spectrum[0], spectrum[1]))