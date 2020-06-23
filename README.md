# Getting started with DHT22 on Mbed OS

This guide reviews the steps required to get samples from the DHT22 sensor on Mbed OS platform.

Please install [mbed CLI](https://github.com/ARMmbed/mbed-cli#installing-mbed-cli).

## Import the example application

From the command-line, import the example:

```bash
$ mbed import git@github.com:geoffrey-vl/mbed-dht22-example.git
$ cd mbed-dht22-example
```

### Now compile

Invoke `mbed compile`, and specify the name of your platform and your favorite toolchain (`GCC_ARM`, `ARM`, `IAR`). For example, for the ARM Compiler 5:

```bash
$ mbed compile -m NUCLEO_F091RC -t GCC_ARM --profile=release
```

Your PC may take a few minutes to compile your code. At the end, you see the following result:

```bash
[Warning] @,: Compiler version mismatch: Have 9.2.1; expected version >= 6.0.0 and < 7.0.0
Building project mbed-dht22-example (NUCLEO_F091RC, GCC_ARM)
Scan: mbed-dht22-example
Link: mbed-dht22-example
Elf2Bin: mbed-dht22-example
| Module           |     .text |    .data |     .bss |
|------------------|-----------|----------|----------|
| DHT.o            |   666(+0) |    0(+0) |    0(+0) |
| [fill]           |    56(+0) |    8(+0) |   32(+0) |
| [lib]/c.a        | 29088(+0) | 2472(+0) |   89(+0) |
| [lib]/gcc.a      | 15288(+0) |    0(+0) |    0(+0) |
| [lib]/misc       |   192(+0) |    4(+0) |   28(+0) |
| [lib]/nosys.a    |    32(+0) |    0(+0) |    0(+0) |
| [lib]/stdc++.a   |  4760(+0) |    8(+0) |   44(+0) |
| main.o           |   534(+0) |    0(+0) |  116(+0) |
| mbed-os/drivers  |   442(+0) |    0(+0) |    0(+0) |
| mbed-os/hal      |  1264(+0) |    4(+0) |   66(+0) |
| mbed-os/platform |  3428(+0) |  276(+0) |  388(+0) |
| mbed-os/rtos     |  6418(+0) |  168(+0) | 5969(+0) |
| mbed-os/targets  |  8328(+0) |    4(+0) | 1100(+0) |
| Subtotals        | 70496(+0) | 2944(+0) | 7832(+0) |
Total Static RAM memory (data + bss): 10776(+0) bytes
Total Flash memory (text + data): 73440(+0) bytes

Image: ./BUILD/NUCLEO_F091RC/GCC_ARM-RELEASE/mbed-dht22-example.bin
```

### Wirings

![wiring DHT22](doc/dht22_pinout.jpeg)

### Program your board

1. Connect your Mbed device to the computer over USB.
1. Copy the binary file to the Mbed device.
1. Press the reset button to start the program.

### Program output

```bash
$ minicom -b 115200 -D /dev/ttyACM3
21.5
21.5
21.8
22.6
23.7
```

### Logging to a .csv file

We can automate logging to a CSV file, and for example also add some other values such as CPU temperature.
Include is a example script written in python that does exactly that, and takes samples each 5 minutes.
Use the `usbtemp.py` script added to this repo, and copy it over to your target system:
Make sure the target has **python 3** and that you have the *pyserial* library installed:

```bash
$ pip3 install pyserial
```

Execute the python script:

```bash
$ python3 usbtemp.py /deb/ttyACM3
Press Ctrl+C to quit
Collecting data... (USB =  /dev/ttyACM0 )
Creating log file:  temp.csv
Log:  2020-06-17 11:35:24,24.3,48.0
Zzz...
Collecting data... (USB =  /dev/ttyACM0 )
Log:  2020-06-17 11:35:34,24.3,48.0
Zzz...
```

To stop the python script from executing: press `ctrl+c`.
On the target, in the folder where you executed the script, a CSV file is generated.

File content:

```csv
TIMESTAMP,DHT22,CPU
2020-06-17 11:35:24,24.3,48.0
2020-06-17 11:35:34,24.3,48.0
2020-06-17 11:35:44,24.3,48.0
```

Unortunaltey is you disconnect over SSH the python child process is terminated too.
If you want to keep the python script running (as a background process) after you've exited your SSH session you should use `nohup`:

```bash
$ nohup python3 usbtemp.py /deb/ttyACM3 &
```

### License and contributions

The software is provided under Apache-2.0 license. Contributions to this project are accepted under the same license.

This project contains code from other projects. The original license text is included in those source files. They must comply with our license guide.
