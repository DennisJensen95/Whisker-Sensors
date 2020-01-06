import re
import time
import serial
import numpy as np
import serial.tools.list_ports as port_list
import matplotlib.pyplot as plt
import os

def FindTeensy():
    for usb in port_list.comports():
        description = usb.description
        name = usb.name
        usbPort = usb.device

        if 'tty' in name:
            print(usb.device)
            print(name)
            print(description)
            return usbPort

def CreateConnection():
    device = FindTeensy()
    # console_camera = serial.Serial(port=device, baudrate=1000000, , stopbits=1, bytesize=8, timeout=.1)
    console_camera = serial.Serial(port=device, baudrate=9600)

    return console_camera

def CheckConnection():
    if console_camera.is_open:
        print("Open")
    else:
        print("Closed")

def SendCommand(cmd):
    console_camera.write(cmd)

def save_data(filename, data, sample_time):
    file = open(filename, 'w+')
    file.write(f'Sample Time is:{sample_time}\n')
    for val in data:
        file.write(f'{val}\n')
    file.close()

def ReadLog():
    data = np.array([])
    sample_time = None
    start = time.time()
    print("Reading Data")
    while True:
        output = console_camera.read_all().decode('utf-8')
        if output != '':
            """"""
            # print(output)

        results = re.findall("log:(\d+)", output)
        for value in results:
            # print(value)
            val = float(value)
            data = np.append(data, val)

        if 'Sample time:' in output:
            sample_time = re.findall("Sample time: (\d+\.\d+)", output)[0]

        if 'Logged send done' in output:
            print("Log has been read.")
            break

        if 'No log saved' in output:
            print("There was no log saved")
            break

        if time.time() - start > 3:
            print("Still reading data")
            # print(sample_time)
            start = time.time()

    return data, float(sample_time)

def read_data_from_file(filename):
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
        elif i < len(lines)-2:
            val = float(re.findall('\d+', line)[0])
            y = np.append(y, val)
        i += 1

    return y, float(sample_time)

old_dir = os.getcwd()
os.chdir('./data')
filename = './object_class/oven_grate_signal_4'

threshold = False
# filename = 'GLR_dynamic_step_3cm_offset'
if filename == 'test':
    if os.path.exists(filename):
        os.remove(filename)
get_data_from_micro = os.path.exists(filename)

if not get_data_from_micro:
    console_camera = CreateConnection()
    CheckConnection()

    cmd = 'sendlog'.encode('utf-8')
    SendCommand(cmd)
    data, sample_time = ReadLog()

    print(sample_time)
    print(isinstance(sample_time, float))

    y = data
    # print(y)
    x = np.linspace(0, sample_time, len(y))

    save_data(filename, y, sample_time)
else:
    y, sample_time = read_data_from_file(filename)
    x = np.linspace(0, sample_time, len(y))
    fs_log = int(len(y) / float(sample_time))
    print(len(y))
    print(f'Sample frequency is: {fs_log}')

plt.plot(x, y)
plt.xlabel('Time [s]')
plt.ylabel('Target Capacitance [1/50 pF]')

if threshold:
    # plt.plot(x, np.ones(len(y))*0.1)
    # plt.plot(x, np.ones(len(y))*0.3)
    plt.plot(x, np.ones(len(y))*2)
    plt.plot(x, np.ones(len(y))*2.5)
    plt.plot(x, np.ones(len(y))*3)
    plt.legend(['Signal', 'Threshold 2 [1/50 pF]', 'Threshold 2.5 [1/50 pF]', 'Threshold 3 [1/50 pF]'], loc='upper right')

plt.show()
os.chdir(old_dir)