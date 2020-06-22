import argparse     # for parsing arguments
import serial       # for reading from serial/usb port
import os           # for opening files
import signal       # for catching ctrl+c
import sys          # for catching ctrl+c
import time         # for sleep
from datetime import datetime     # for storing timestamps

BAUDRATE = 115200
CPUTEMP_SYSFS = "/sys/devices/virtual/thermal/thermal_zone0/temp"
LOG_FILENAME = "temp.csv"


def signal_handler(sig, frame):
    print('You pressed Ctrl+C! Quiting application')
    sys.exit(0)


def readSerialTemp(serialport):
    # print("Opening ", serialport)
    # open serial port
    with serial.Serial(serialport, BAUDRATE, timeout=2) as ser:
        # print("Device ", ser.name, " ready")  # check which port was really used
        # take 2 samples, but ignore first sample since we may have
        # started reading in the middle of a transmission
        cnt = 2
        dht22temp = ''
        while cnt > 0:
            dht22temp = ser.readline()
            # ignore empty results
            if len(dht22temp) < 1:
                continue
            cnt = cnt - 1
        dht22tempStr = dht22temp.decode('utf-8').rstrip()
        return float(dht22tempStr)


def readCpuTemp():
    while 1:
        try:
            f = open(CPUTEMP_SYSFS, "r")
            internalcpusensor_str = f.readline()
            f.close()
            internalcpusensor = 0.0
            internalcpusensor = float(internalcpusensor_str)
            return internalcpusensor / 1000
        except:
            time.sleep(1)
    return 0 


def createLogFile():
    f = open(LOG_FILENAME, "a")
    f.write("TIMESTAMP,DHT22,CPU\r\n")
    f.close()


def logToFile(dht22temp, cputemp):
    # first check if file is already created
    if not os.path.exists(LOG_FILENAME):
        # if not, lets create it with appropriate header
        print("Creating log file: ", LOG_FILENAME)
        createLogFile()

    # then check if nobody cleared the file content manually
    if os.stat(LOG_FILENAME).st_size < 1:
        print("Creating log file headers")
        createLogFile()

    # create timestamp in format: yyyy-MM-dd HH:mm:ss
    dt = datetime.now().strftime("%Y-%m-%d %H:%M:%S")

    str = '{},{:.1f},{:.1f}\r\n'.format(dt, dht22temp, cputemp)
    print("Log: ", str.rstrip())
    f = open(LOG_FILENAME, "a")
    f.write(str)
    f.close()


def main():
    # parse args
    parser = argparse.ArgumentParser(description='Log temp from USB device')
    parser.add_argument('usbdevice', help='a USB device, for example /dev/ttyACM3')
    args = parser.parse_args()

    # ctrl+c hook
    signal.signal(signal.SIGINT, signal_handler)
    print('Press Ctrl+C to quit')
    #signal.pause()

    # collect samples
    while 1:
        print("Collecting data... (USB = ", args.usbdevice, ")")
        usbTempSample = readSerialTemp(args.usbdevice)
        cpuTempSample = readCpuTemp()
        logToFile(usbTempSample, cpuTempSample)
        print("Zzz...")
        time.sleep(60*5)


if __name__ == '__main__':
    main()
