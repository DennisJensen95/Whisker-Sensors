import re
import os
import time
import serial
import serial.tools.list_ports as port_list

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

console_camera = CreateConnection()

CheckConnection()

start = time.time()
values_x = []
values_y = []
touchValues = [values_x, values_y]
while True:
    timeout = start + 10
    time.sleep(0.5)
    text = console_camera.read_all().decode('utf-8')
    if text == '':
        print("Empty")
    else:
        print("Saving data from touch sensors.")
        touchPin = re.findall("Pin\d+:\d+", text)
        offSetPin = re.findall("Offset\d+:\d+", text)
        for pin in touchPin:
            if '15' in pin:
                result = re.findall(":(\d+)", pin)[0]
                values_x.append(result)

        for pin in offSetPin:
            if '15' in pin:
                result = re.findall(":(\d+)", pin)[0]
                values_y.append(result)


    if time.time() > timeout:
        print("Ending")
        break

if os.path.exists('./datalog_pin15.txt'):
    os.remove('./datalog_pin15.txt')
if os.path.exists('./datalog_offset_pin15.txt'):
    os.remove('./datalog_offset_pin15.txt')

data_x = ','.join(values_x)
data_y = ','.join(values_y)

file_x = open('./datalog_pin15.txt', 'w+')
file_y = open('./datalog_offset_pin15.txt', 'w+')
file_x.write(data_x)
file_y.write(data_y)
file_x.close()
file_y.close()

console_camera.close()

CheckConnection()





