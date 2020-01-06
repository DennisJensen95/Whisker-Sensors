import re
import os
import time
import serial
import serial.tools.list_ports as port_list
from threading import Thread


class LiveStream():
    def __init__(self):
        self.RawDataLine = 0
        self.OffsetLine = 0
        self.SignalLine = 0
        self.CusumLine = 0
        self.ThreshLine = 0
        self.serial = None
        self.stream = False
        self.signal_stream = None

    def FindTeensy(self):
        for usb in port_list.comports():
            description = usb.description
            name = usb.name
            usbPort = usb.device

            if 'tty' in name:
                print(usb.device)
                print(name)
                print(description)
                return usbPort

    def CreateConnection(self):
        device = self.FindTeensy()
        # console_camera = serial.Serial(port=device, baudrate=1000000, , stopbits=1, bytesize=8, timeout=.1)
        self.serial = serial.Serial(port=device, baudrate=9600)
        self.connection = True

        return self.serial

    def CheckConnection(self):
        if self.serial.is_open:
            print("Open")
        else:
            print("Closed")

    def WriteRawData(self, rawDataList):
        for data in rawDataList:
            with open('RawData.txt', 'a') as file:
                file.write(f'{self.RawDataLine},{data}\n')
                self.RawDataLine = self.RawDataLine + 1

    def WriteOffsetData(self, offsetDataList):
        for data in offsetDataList:
            with open('OffsetData.txt', 'a') as file:
                file.write(f'{self.OffsetLine},{data}\n')
                self.OffsetLine = self.OffsetLine + 1

    def WriteSignal(self, signalList):
        for data in signalList:
            with open('signal.txt', 'a') as file:
                file.write(f'{self.SignalLine},{data}\n')
                self.SignalLine = self.SignalLine + 1

    def WriteCusumData(self, cusum):
        for data in cusum:
            with open('cusum.txt', 'a') as file:
                file.write(f'{self.CusumLine},{data}\n')
                self.CusumLine = self.CusumLine + 1

    def WriteThreshData(self, Threshdata):
        for data in Threshdata:
            with open('thresh.txt', 'a') as file:
                file.write(f'{self.ThreshLine},{data}\n')
                self.ThreshLine = self.ThreshLine + 1

    def CloseConnection(self):
        self.serial.close()

    def ToggleStreaming(self):
        self.serial.write(b'stream')
        if self.stream:
            self.stream = False

        if not self.stream:
            self.stream = True

    def __del__(self):
        if self.connection:
            console_camera.close()

    def ToggleSignal(self):
        self.serial.write(b'signal')
        if self.signal_stream:
            self.signal_stream = False

        if not self.signal_stream:
            self.stream = True

if os.path.exists('RawData.txt'):
    os.remove('RawData.txt')
if os.path.exists('OffsetData.txt'):
    os.remove('OffsetData.txt')
if os.path.exists('signal.txt'):
    os.remove('signal.txt')
if os.path.exists('thresh.txt'):
    os.remove('thresh.txt')
if os.path.exists('cusum.txt'):
    os.remove('cusum.txt')

LiveStream = LiveStream()

console_camera = LiveStream.CreateConnection()

LiveStream.CheckConnection()
signal = 'all'

# if signal:
#     LiveStream.ToggleSignal()
# else:
#     LiveStream.ToggleStreaming()


if 'signal' in signal:
    while True:
        text = console_camera.read_all().decode('utf-8')

        if text=='':
            print("Empty")
        else:
            signal_data = re.findall("-?\d+\.?\d*", text)
            threading = None
            for sig in signal_data:
                threading = Thread(target=LiveStream.WriteSignal, args=(signal_data, ))
                threading.start()

            threading.join()
            time.sleep(0.05)

elif 'three' in signal:
    while True:
        text = console_camera.read_all().decode('utf-8')
        if text == '':
            print("Empty")
        else:
            touchPin = re.findall("Pin\d+:-?\d+\.?\d*", text)
            offSetPin = re.findall("Offset\d+:-?\d+\.?\d*", text)
            threshPin = re.findall("thresh\d+:-?\d+\.?\d*", text)
            cusumPin = re.findall("cusum\d+:-?\d+\.?\d*", text)
            rawData = None
            offset = None
            thresh = None
            cusum = None
            for pin in touchPin:
                if '19' in pin:
                    result = re.findall(":(\d+\.?\d*)", pin)
                    rawData = Thread(target=LiveStream.WriteRawData, args=(result, ))
                    rawData.start()

            for pin in offSetPin:
                if '19' in pin:
                    result = re.findall(":(-?\d+\.?\d*)", pin)
                    offset = Thread(target=LiveStream.WriteOffsetData, args=(result, ))
                    offset.start()
            for pin in threshPin:
                if '19' in pin:
                    result = re.findall(":(-?\d+\.?\d*)", pin)
                    thresh = Thread(target=LiveStream.WriteThreshData, args=(result,))
                    thresh.start()

            rawData.join()
            offset.join()
            #thresh.join()
            time.sleep(0.1)

else:
    step = 0
    while True:
        if (step == 0):
            time.sleep(0.1)

        step += 1
        text = console_camera.read_all().decode('utf-8')
        # print(text)
        if text == '':
            print("Empty")
        else:
            touchPin = re.findall("Pin\d+:-?\d+\.?\d*", text)
            offSetPin = re.findall("Offset\d+:-?\d+\.?\d*", text)
            threshPin = re.findall("thresh\d+:-?\d+\.?\d*", text)
            cusumPin = re.findall("cusum\d+:-?\d+\.?\d*", text)
            rawData = None
            offset = None
            thresh = None
            cusum = None
            Threads = [rawData, offset, thresh, cusum]
            pin_num = '19'
            if len(touchPin) > 0:
                for pin in touchPin:
                    if pin_num in pin:
                        result = re.findall(":(-?\d+\.?\d*)", pin)
                        Threads[0] = Thread(target=LiveStream.WriteRawData, args=(result, ))
                        Threads[0].start()

            if len(offSetPin) > 0:
                for pin in offSetPin:
                    if pin_num in pin:
                        result = re.findall(":(-?\d+\.?\d*)", pin)
                        Threads[1] = Thread(target=LiveStream.WriteOffsetData, args=(result, ))
                        Threads[1].start()

            if len(threshPin) > 0:
                for pin in threshPin:
                    if pin_num in pin:
                        result = re.findall(":(-?\d+\.?\d*)", pin)
                        Threads[2] = Thread(target=LiveStream.WriteThreshData, args=(result,))
                        Threads[2].start()

            if len(cusumPin) > 0:
                for pin in cusumPin:
                    if pin_num in pin:
                        result = re.findall(":(-?\d+\.?\d*)", pin)
                        Threads[3] = Thread(target=LiveStream.WriteCusumData, args=(result,))
                        Threads[3].start()

            for thread in Threads:
                if thread != None:
                    thread.join()

            # rawData.join()
            # offset.join()
            # thresh.join()
            # cusum.join()
            time.sleep(0.1)


# if signal:
#     LiveStream.ToggleSignal()
# else:
#     LiveStream.ToggleStreaming()

LiveStream.CloseConnection()


