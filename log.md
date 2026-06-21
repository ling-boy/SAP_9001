root@imx6ulevk:~# ./modbus_9001
[2019-04-27 01:33:15.907] [INFO ] [main    ] [T:619] SAP_9001 V2.1 starting...
[2019-04-27 01:33:15.908] [WARN ] [main    ] [T:619] Config file not found, using defaults
[2019-04-27 01:33:15.908] [INFO ] [memory_pool] [T:619] Created pool for N3sap13DeviceContext12PacketBufferE, size=8
12V power on!
[2019-04-27 01:33:16.968] [INFO ] [gps     ] [T:622] Start GPS thread
[2019-04-27 01:33:17.968] [INFO ] [factory ] [T:619] Created LoRa strategy
[2019-04-27 01:33:17.968] [INFO ] [factory ] [T:619] Created WiFi strategy
[2019-04-27 01:33:17.968] [INFO ] [factory ] [T:619] Created Bluetooth strategy
[2019-04-27 01:33:17.969] [INFO ] [factory ] [T:619] Created LAN strategy
[2019-04-27 01:33:17.969] [INFO ] [factory ] [T:619] Created LAN Server strategy
[2019-04-27 01:33:17.969] [INFO ] [factory ] [T:619] Created 5 communication strategies
[2019-04-27 01:33:17.994] [INFO ] [lora    ] [T:619] LoRa initialized, fd=4
[2019-04-27 01:33:17.995] [INFO ] [comm_mgr] [T:619] Add device ID=1 success
[2019-04-27 01:33:17.995] [DEBUG] [comm_mgr] [T:619] Found device ID=1
[2019-04-27 01:33:17.995] [INFO ] [comm_mgr] [T:619] Callback registered for device ID=1 pos=0
[2019-04-27 01:33:17.995] [INFO ] [dev_init] [T:619] Init success: LoRa, fd=4
$GPTXT,01,01,02,MA=CASIC*27

$GPTXT,01,01,02,HW=ATGM332D,0001081817886*1A

$GPTXT,01,01,02,IC=AT6558-5N-31-0C510800,BG33CKJ-F3-010527*55

$GPTXT,01,01,02,SW=URANUS5,V5.3.0.0*1D

$GPTXT,01,01,02,TB=2020-04-28,13:43:10*40

$GPTXT,01,01,02,MO=GB*77

$GPTXT,01,01,02,BS=SOC_BootLoader,V6.2.0.2*34

$GPTXT,01,01,02,FI=00856014*71

$GNGGA,,,,,,0,00,25.5,,,,,,*64

$GNGLL,,,,,,V,M*79

$GPGSA,A,1,,,,,,,,,,,,,25.5,25.5,25.5*02

$BDGSA,A,1,,,,,,,,,,,,,25.5,25.5,25.5*13

$GPGSV,1,1,00*79

$BDGSV,1,1,00*68

$GNRMC,,V,,,,,,,,,,M*4E

$GNVTG,,,,,,,,,M*2D

$GNZDA,,,,,,*56

[2019-04-27 01:33:19.996] [INFO ] [wifi    ] [T:619] Bringing down wlan0...
$GNGGA,,,,,,0,00,25.5,,,,,,*64

$GNGLL,,,,,,V,M*79

$GPGSA,A,1,,,,,,,,,,,,,25.5,25.5,25.5*02

$BDGSA,A,1,,,,,,,,,,,,,25.5,25.5,25.5*13

$GPGSV,1,1,00*79

$BDGSV,1,1,00*68

$GNRMC,,V,,,,,,,,,,M*4E

$GNVTG,,,,,,,,,M*2D

$GNZDA,,,,,,*56

$GNGGA,,,,,,0,00,25.5,,,,,,*64

$GNGLL,,,,,,V,M*79

$GPGSA,A,1,,,,,,,,,,,,,25.5,25.5,25.5*02

$BDGSA,A,1,,,,,,,,,,,,,25.5,25.5,25.5*13

$GPGSV,1,1,00*79

$BDGSV,1,1,00*68

$GNRMC,,V,,,,,,,,,,M*4E

$GNVTG,,,,,,,,,M*2D

$GNZDA,,,,,,*56

$GPTXT,01,01,01,ANTENNA OK*35

killall: wpa_supplicant: no process killed
$GNGGA,,,,,,0,00,25.5,,,,,,*64

$GNGLL,,,,,,V,M*79

$GPGSA,A,1,,,,,,,,,,,,,25.5,25.5,25.5*02

$BDGSA,A,1,,,,,,,,,,,,,25.5,25.5,25.5*13

$GPGSV,1,1,00*79

$BDGSV,1,1,00*68

$GNRMC,,V,,,,,,,,,,M*4E

$GNVTG,,,,,,,,,M*2D

$GNZDA,,,,,,*56

$GPTXT,01,01,01,ANTENNA OK*35

killall: udhcpc: no process killed
$GNGGA,,,,,,0,00,25.5,,,,,,*64

$GNGLL,,,,,,V,M*79

$GPGSA,A,1,,,,,,,,,,,,,25.5,25.5,25.5*02

$BDGSA,A,1,,,,,,,,,,,,,25.5,25.5,25.5*13

$GPGSV,1,1,00*79

$BDGSV,1,1,00*68

$GNRMC,,V,,,,,,,,,,M*4E

$GNVTG,,,,,,,,,M*2D

$GNZDA,,,,,,*56

$GPTXT,01,01,01,ANTENNA OK*35

[2019-04-27 01:33:24.148] [INFO ] [wifi    ] [T:619] Starting wpa_supplicant with config: /home/root/wifi.conf
Successfully initialized wpa_supplicant
rfkill: Cannot open RFKILL control device
ieee80211 phy0: rt2x00lib_request_firmware: Info - Loading firmware file 'rt2870.bin'
ieee80211 phy0: rt2x00lib_request_firmware: Info - Firmware detected - version: 0.29
$GNGGA,,,,,,0,00,25.5,,,,,,*64

$GNGLL,,,,,,V,M*79

$GPGSA,A,1,,,,,,,,,,,,,25.5,25.5,25.5*02

$BDGSA,A,1,,,,,,,,,,,,,25.5,25.5,25.5*13

$GPGSV,1,1,00*79

$BDGSV,1,1,00*68

$GNRMC,,V,,,,,,,,,,M*4E

$GNVTG,,,,,,,,,M*2D

$GNZDA,,,,,,*56

$GPTXT,01,01,01,ANTENNA OK*35

IPv6: ADDRCONF(NETDEV_UP): wlan0: link is not ready
$GNGGA,,,,,,0,00,25.5,,,,,,*64

$GNGLL,,,,,,V,M*79

$GPGSA,A,1,,,,,,,,,,,,,25.5,25.5,25.5*02

$BDGSA,A,1,,,,,,,,,,,,,25.5,25.5,25.5*13

$GPGSV,1,1,00*79

$BDGSV,1,1,00*68

$GNRMC,,V,,,,,,,,,,M*4E

$GNVTG,,,,,,,,,M*2D

$GNZDA,,,,,,*56

$GPTXT,01,01,01,ANTENNA OK*35

$GNGGA,,,,,,0,00,25.5,,,,,,*64

$GNGLL,,,,,,V,M*79

$GPGSA,A,1,,,,,,,,,,,,,25.5,25.5,25.5*02

$BDGSA,A,1,,,,,,,,,,,,,25.5,25.5,25.5*13

$GPGSV,1,1,00*79

$BDGSV,1,1,00*68

$GNRMC,,V,,,,,,,,,,M*4E

$GNVTG,,,,,,,,,M*2D

$GNZDA,,,,,,*56

$GPTXT,01,01,01,ANTENNA OK*35

$GNGGA,,,,,,0,00,25.5,,,,,,*64

$GNGLL,,,,,,V,M*79

$GPGSA,A,1,,,,,,,,,,,,,25.5,25.5,25.5*02

$BDGSA,A,1,,,,,,,,,,,,,25.5,25.5,25.5*13

$GPGSV,1,1,00*79

$BDGSV,1,1,00*68

$GNRMC,,V,,,,,,,,,,M*4E

$GNVTG,,,,,,,,,M*2D

$GNZDA,,,,,,*56

$GPTXT,01,01,01,ANTENNA OK*35

$GNGGA,,,,,,0,00,25.5,,,,,,*64

$GNGLL,,,,,,V,M*79

$GPGSA,A,1,,,,,,,,,,,,,25.5,25.5,25.5*02

$BDGSA,A,1,,,,,,,,,,,,,25.5,25.5,25.5*13

$GPGSV,1,1,00*79

$BDGSV,1,1,00*68

$GNRMC,,V,,,,,,,,,,M*4E

$GNVTG,,,,,,,,,M*2D

$GNZDA,,,,,,*56

$GPTXT,01,01,01,ANTENNA OK*35

$GNGGA,,,,,,0,00,25.5,,,,,,*64

$GNGLL,,,,,,V,M*79

$GPGSA,A,1,,,,,,,,,,,,,25.5,25.5,25.5*02

$BDGSA,A,1,,,,,,,,,,,,,25.5,25.5,25.5*13

$GPGSV,1,1,00*79

$BDGSV,1,1,00*68

$GNRMC,,V,,,,,,,,,,M*4E

$GNVTG,,,,,,,,,M*2D

$GNZDA,,,,,,*56

$GPTXT,01,01,01,ANTENNA OK*35

$GNGGA,,,,,,0,00,25.5,,,,,,*64

$GNGLL,,,,,,V,M*79

$GPGSA,A,1,,,,,,,,,,,,,25.5,25.5,25.5*02

$BDGSA,A,1,,,,,,,,,,,,,25.5,25.5,25.5*13

$GPGSV,1,1,00*79

$BDGSV,1,1,00*68

$GNRMC,,V,,,,,,,,,,M*4E

$GNVTG,,,,,,,,,M*2D

$GNZDA,,,,,,*56

$GPTXT,01,01,01,ANTENNA OK*35

$GNGGA,,,,,,0,00,25.5,,,,,,*64

$GNGLL,,,,,,V,M*79

$GPGSA,A,1,,,,,,,,,,,,,25.5,25.5,25.5*02

$BDGSA,A,1,,,,,,,,,,,,,25.5,25.5,25.5*13

$GPGSV,1,1,00*79

$BDGSV,1,1,00*68

$GNRMC,,V,,,,,,,,,,M*4E

$GNVTG,,,,,,,,,M*2D

$GNZDA,,,,,,*56

$GPTXT,01,01,01,ANTENNA OK*35

$GNGGA,,,,,,0,00,25.5,,,,,,*64

$GNGLL,,,,,,V,M*79

$GPGSA,A,1,,,,,,,,,,,,,25.5,25.5,25.5*02

$BDGSA,A,1,,,,,,,,,,,,,25.5,25.5,25.5*13

$GPGSV,1,1,00*79

$BDGSV,1,1,00*68

$GNRMC,,V,,,,,,,,,,M*4E

$GNVTG,,,,,,,,,M*2D

$GNZDA,,,,,,*56

$GPTXT,01,01,01,ANTENNA OK*35

$GNGGA,,,,,,0,00,25.5,,,,,,*64

$GNGLL,,,,,,V,M*79

$GPGSA,A,1,,,,,,,,,,,,,25.5,25.5,25.5*02

$BDGSA,A,1,,,,,,,,,,,,,25.5,25.5,25.5*13

$GPGSV,1,1,00*79

$BDGSV,1,1,00*68

$GNRMC,,V,,,,,,,,,,M*4E

$GNVTG,,,,,,,,,M*2D

$GNZDA,,,,,,*56

$GPTXT,01,01,01,ANTENNA OK*35

[2019-04-27 01:33:34.171] [INFO ] [wifi    ] [T:619] Starting udhcpc...
udhcpc (v1.24.1) started
Sending discover...
$GNGGA,,,,,,0,00,25.5,,,,,,*64

$GNGLL,,,,,,V,M*79

$GPGSA,A,1,,,,,,,,,,,,,25.5,25.5,25.5*02

$BDGSA,A,1,,,,,,,,,,,,,25.5,25.5,25.5*13

$GPGSV,1,1,00*79

$BDGSV,1,1,00*68

$GNRMC,,V,,,,,,,,,,M*4E

$GNVTG,,,,,,,,,M*2D

$GNZDA,,,,,,*56

$GPTXT,01,01,01,ANTENNA OK*35

$GNGGA,,,,,,0,00,25.5,,,,,,*64

$GNGLL,,,,,,V,M*79

$GPGSA,A,1,,,,,,,,,,,,,25.5,25.5,25.5*02

$BDGSA,A,1,,,,,,,,,,,,,25.5,25.5,25.5*13

$GPGSV,1,1,00*79

$BDGSV,1,1,00*68

$GNRMC,,V,,,,,,,,,,M*4E

$GNVTG,,,,,,,,,M*2D

$GNZDA,,,,,,*56

$GPTXT,01,01,01,ANTENNA OK*35

$GNGGA,,,,,,0,00,25.5,,,,,,*64

$GNGLL,,,,,,V,M*79

$GPGSA,A,1,,,,,,,,,,,,,25.5,25.5,25.5*02

$BDGSA,A,1,,,,,,,,,,,,,25.5,25.5,25.5*13

$GPGSV,1,1,00*79

$BDGSV,1,1,00*68

$GNRMC,,V,,,,,,,,,,M*4E

$GNVTG,,,,,,,,,M*2D

$GNZDA,,,,,,*56

$GPTXT,01,01,01,ANTENNA OK*35

Sending discover...
$GNGGA,,,,,,0,00,25.5,,,,,,*64

$GNGLL,,,,,,V,M*79

$GPGSA,A,1,,,,,,,,,,,,,25.5,25.5,25.5*02

$BDGSA,A,1,,,,,,,,,,,,,25.5,25.5,25.5*13

$GPGSV,1,1,00*79

$BDGSV,1,1,00*68

$GNRMC,,V,,,,,,,,,,M*4E

$GNVTG,,,,,,,,,M*2D

$GNZDA,,,,,,*56

$GPTXT,01,01,01,ANTENNA OK*35

$GNGGA,,,,,,0,00,25.5,,,,,,*64

$GNGLL,,,,,,V,M*79

$GPGSA,A,1,,,,,,,,,,,,,25.5,25.5,25.5*02

$BDGSA,A,1,,,,,,,,,,,,,25.5,25.5,25.5*13

$GPGSV,1,1,00*79

$BDGSV,1,1,00*68

$GNRMC,,V,,,,,,,,,,M*4E

$GNVTG,,,,,,,,,M*2D

$GNZDA,,,,,,*56

$GPTXT,01,01,01,ANTENNA OK*35

$GNGGA,,,,,,0,00,25.5,,,,,,*64

$GNGLL,,,,,,V,M*79

$GPGSA,A,1,,,,,,,,,,,,,25.5,25.5,25.5*02

$BDGSA,A,1,,,,,,,,,,,,,25.5,25.5,25.5*13

$GPGSV,1,1,00*79

$BDGSV,1,1,00*68

$GNRMC,,V,,,,,,,,,,M*4E

$GNVTG,,,,,,,,,M*2D

$GNZDA,,,,,,*56

$GPTXT,01,01,01,ANTENNA OK*35

Sending discover...
$GNGGA,,,,,,0,00,25.5,,,,,,*64

$GNGLL,,,,,,V,M*79

$GPGSA,A,1,,,,,,,,,,,,,25.5,25.5,25.5*02

$BDGSA,A,1,,,,,,,,,,,,,25.5,25.5,25.5*13

$GPGSV,1,1,00*79

$BDGSV,1,1,00*68

$GNRMC,,V,,,,,,,,,,M*4E

$GNVTG,,,,,,,,,M*2D

$GNZDA,,,,,,*56

$GPTXT,01,01,01,ANTENNA OK*35

$GNGGA,,,,,,0,00,25.5,,,,,,*64

$GNGLL,,,,,,V,M*79

$GPGSA,A,1,,,,,,,,,,,,,25.5,25.5,25.5*02

$BDGSA,A,1,,,,,,,,,,,,,25.5,25.5,25.5*13

$GPGSV,1,1,00*79

$BDGSV,1,1,00*68

$GNRMC,,V,,,,,,,,,,M*4E

$GNVTG,,,,,,,,,M*2D

$GNZDA,,,,,,*56

$GPTXT,01,01,01,ANTENNA OK*35

$GNGGA,,,,,,0,00,25.5,,,,,,*64

$GNGLL,,,,,,V,M*79

$GPGSA,A,1,,,,,,,,,,,,,25.5,25.5,25.5*02

$BDGSA,A,1,,,,,,,,,,,,,25.5,25.5,25.5*13

$GPGSV,1,1,00*79

$BDGSV,1,1,00*68

$GNRMC,,V,,,,,,,,,,M*4E

$GNVTG,,,,,,,,,M*2D

$GNZDA,,,,,,*56

$GPTXT,01,01,01,ANTENNA OK*35

Sending discover...
$GNGGA,,,,,,0,00,25.5,,,,,,*64

$GNGLL,,,,,,V,M*79

$GPGSA,A,1,,,,,,,,,,,,,25.5,25.5,25.5*02

$BDGSA,A,1,,,,,,,,,,,,,25.5,25.5,25.5*13

$GPGSV,1,1,00*79

$BDGSV,1,1,00*68

$GNRMC,,V,,,,,,,,,,M*4E

$GNVTG,,,,,,,,,M*2D

$GNZDA,,,,,,*56

$GPTXT,01,01,01,ANTENNA OK*35

$GNGGA,,,,,,0,00,25.5,,,,,,*64

$GNGLL,,,,,,V,M*79

$GPGSA,A,1,,,,,,,,,,,,,25.5,25.5,25.5*02

$BDGSA,A,1,,,,,,,,,,,,,25.5,25.5,25.5*13

$GPGSV,1,1,00*79

$BDGSV,1,1,00*68

$GNRMC,,V,,,,,,,,,,M*4E

$GNVTG,,,,,,,,,M*2D

$GNZDA,,,,,,*56

$GPTXT,01,01,01,ANTENNA OK*35

$GNGGA,,,,,,0,00,25.5,,,,,,*64

$GNGLL,,,,,,V,M*79

$GPGSA,A,1,,,,,,,,,,,,,25.5,25.5,25.5*02

$BDGSA,A,1,,,,,,,,,,,,,25.5,25.5,25.5*13

$GPGSV,1,1,00*79

$BDGSV,1,1,00*68

$GNRMC,,V,,,,,,,,,,M*4E

$GNVTG,,,,,,,,,M*2D

$GNZDA,,,,,,*56

$GPTXT,01,01,01,ANTENNA OK*35

Sending discover...
$GNGGA,,,,,,0,00,25.5,,,,,,*64

$GNGLL,,,,,,V,M*79

$GPGSA,A,1,,,,,,,,,,,,,25.5,25.5,25.5*02

$BDGSA,A,1,,,,,,,,,,,,,25.5,25.5,25.5*13

$GPGSV,1,1,00*79

$BDGSV,1,1,00*68

$GNRMC,,V,,,,,,,,,,M*4E

$GNVTG,,,,,,,,,M*2D

$GNZDA,,,,,,*56

$GPTXT,01,01,01,ANTENNA OK*35

$GNGGA,,,,,,0,00,25.5,,,,,,*64

$GNGLL,,,,,,V,M*79

$GPGSA,A,1,,,,,,,,,,,,,25.5,25.5,25.5*02

$BDGSA,A,1,,,,,,,,,,,,,25.5,25.5,25.5*13

$GPGSV,1,1,00*79

$BDGSV,1,1,00*68

$GNRMC,,V,,,,,,,,,,M*4E

$GNVTG,,,,,,,,,M*2D

$GNZDA,,,,,,*56

$GPTXT,01,01,01,ANTENNA OK*35

$GNGGA,,,,,,0,00,25.5,,,,,,*64

$GNGLL,,,,,,V,M*79

$GPGSA,A,1,,,,,,,,,,,,,25.5,25.5,25.5*02

$BDGSA,A,1,,,,,,,,,,,,,25.5,25.5,25.5*13

$GPGSV,1,1,00*79

$BDGSV,1,1,00*68

$GNRMC,,V,,,,,,,,,,M*4E

$GNVTG,,,,,,,,,M*2D

$GNZDA,,,,,,*56

$GPTXT,01,01,01,ANTENNA OK*35

Sending discover...
$GNGGA,,,,,,0,00,25.5,,,,,,*64

$GNGLL,,,,,,V,M*79

$GPGSA,A,1,,,,,,,,,,,,,25.5,25.5,25.5*02

$BDGSA,A,1,,,,,,,,,,,,,25.5,25.5,25.5*13

$GPGSV,1,1,00*79

$BDGSV,1,1,00*68

$GNRMC,,V,,,,,,,,,,M*4E

$GNVTG,,,,,,,,,M*2D

$GNZDA,,,,,,*56

$GPTXT,01,01,01,ANTENNA OK*35

$GNGGA,,,,,,0,00,25.5,,,,,,*64

$GNGLL,,,,,,V,M*79

$GPGSA,A,1,,,,,,,,,,,,,25.5,25.5,25.5*02

$BDGSA,A,1,,,,,,,,,,,,,25.5,25.5,25.5*13

$GPGSV,1,1,00*79

$BDGSV,1,1,00*68

$GNRMC,,V,,,,,,,,,,M*4E

$GNVTG,,,,,,,,,M*2D

$GNZDA,,,,,,*56

$GPTXT,01,01,01,ANTENNA OK*35

$GNGGA,,,,,,0,00,25.5,,,,,,*64

$GNGLL,,,,,,V,M*79

$GPGSA,A,1,,,,,,,,,,,,,25.5,25.5,25.5*02

$BDGSA,A,1,,,,,,,,,,,,,25.5,25.5,25.5*13

$GPGSV,1,1,00*79

$BDGSV,1,1,00*68

$GNRMC,,V,,,,,,,,,,M*4E

$GNVTG,,,,,,,,,M*2D

$GNZDA,,,,,,*56

$GPTXT,01,01,01,ANTENNA OK*35

Sending discover...
$GNGGA,,,,,,0,00,25.5,,,,,,*64

$GNGLL,,,,,,V,M*79

$GPGSA,A,1,,,,,,,,,,,,,25.5,25.5,25.5*02

$BDGSA,A,1,,,,,,,,,,,,,25.5,25.5,25.5*13

$GPGSV,1,1,00*79

$BDGSV,1,1,00*68

$GNRMC,,V,,,,,,,,,,M*4E

$GNVTG,,,,,,,,,M*2D

$GNZDA,,,,,,*56

$GPTXT,01,01,01,ANTENNA OK*35

$GNGGA,,,,,,0,00,25.5,,,,,,*64

$GNGLL,,,,,,V,M*79

$GPGSA,A,1,,,,,,,,,,,,,25.5,25.5,25.5*02

$BDGSA,A,1,,,,,,,,,,,,,25.5,25.5,25.5*13

$GPGSV,1,1,00*79

$BDGSV,1,1,00*68

$GNRMC,,V,,,,,,,,,,M*4E

$GNVTG,,,,,,,,,M*2D

$GNZDA,,,,,,*56

$GPTXT,01,01,01,ANTENNA OK*35

$GNGGA,,,,,,0,00,25.5,,,,,,*64

$GNGLL,,,,,,V,M*79

$GPGSA,A,1,,,,,,,,,,,,,25.5,25.5,25.5*02

$BDGSA,A,1,,,,,,,,,,,,,25.5,25.5,25.5*13

$GPGSV,1,1,00*79

$BDGSV,1,1,00*68

$GNRMC,,V,,,,,,,,,,M*4E

$GNVTG,,,,,,,,,M*2D

$GNZDA,,,,,,*56

$GPTXT,01,01,01,ANTENNA OK*35

Sending discover...
$GNGGA,,,,,,0,00,25.5,,,,,,*64

$GNGLL,,,,,,V,M*79

$GPGSA,A,1,,,,,,,,,,,,,25.5,25.5,25.5*02

$BDGSA,A,1,,,,,,,,,,,,,25.5,25.5,25.5*13

$GPGSV,1,1,00*79

$BDGSV,1,1,00*68

$GNRMC,,V,,,,,,,,,,M*4E

$GNVTG,,,,,,,,,M*2D

$GNZDA,,,,,,*56

$GPTXT,01,01,01,ANTENNA OK*35

$GNGGA,,,,,,0,00,25.5,,,,,,*64

$GNGLL,,,,,,V,M*79

$GPGSA,A,1,,,,,,,,,,,,,25.5,25.5,25.5*02

$BDGSA,A,1,,,,,,,,,,,,,25.5,25.5,25.5*13

$GPGSV,1,1,00*79

$BDGSV,1,1,00*68

$GNRMC,,V,,,,,,,,,,M*4E

$GNVTG,,,,,,,,,M*2D

$GNZDA,,,,,,*56

$GPTXT,01,01,01,ANTENNA OK*35

$GNGGA,,,,,,0,00,25.5,,,,,,*64

$GNGLL,,,,,,V,M*79

$GPGSA,A,1,,,,,,,,,,,,,25.5,25.5,25.5*02

$BDGSA,A,1,,,,,,,,,,,,,25.5,25.5,25.5*13

$GPGSV,1,1,00*79

$BDGSV,1,1,00*68

$GNRMC,,V,,,,,,,,,,M*4E

$GNVTG,,,,,,,,,M*2D

$GNZDA,,,,,,*56

$GPTXT,01,01,01,ANTENNA OK*35

$GNGGA,,,,,,0,00,25.5,,,,,,*64

$GNGLL,,,,,,V,M*79

$GPGSA,A,1,,,,,,,,,,,,,25.5,25.5,25.5*02

$BDGSA,A,1,,,,,,,,,,,,,25.5,25.5,25.5*13

$GPGSV,1,1,00*79

$BDGSV,1,1,00*68

$GNRMC,,V,,,,,,,,,,M*4E

$GNVTG,,,,,,,,,M*2D

$GNZDA,,,,,,*56

$GPTXT,01,01,01,ANTENNA OK*35

No lease, failing
$GNGGA,,,,,,0,00,25.5,,,,,,*64

$GNGLL,,,,,,V,M*79

$GPGSA,A,1,,,,,,,,,,,,,25.5,25.5,25.5*02

$BDGSA,A,1,,,,,,,,,,,,,25.5,25.5,25.5*13

$GPGSV,1,1,00*79

$BDGSV,1,1,00*68

$GNRMC,,V,,,,,,,,,,M*4E

$GNVTG,,,,,,,,,M*2D

$GNZDA,,,,,,*56

$GPTXT,01,01,01,ANTENNA OK*35

$GNGGA,,,,,,0,00,25.5,,,,,,*64

$GNGLL,,,,,,V,M*79

$GPGSA,A,1,,,,,,,,,,,,,25.5,25.5,25.5*02

$BDGSA,A,1,,,,,,,,,,,,,25.5,25.5,25.5*13

$GPGSV,1,1,00*79

$BDGSV,1,1,00*68

$GNRMC,,V,,,,,,,,,,M*4E

$GNVTG,,,,,,,,,M*2D

$GNZDA,,,,,,*56

$GPTXT,01,01,01,ANTENNA OK*35

$GNGGA,,,,,,0,00,25.5,,,,,,*64

$GNGLL,,,,,,V,M*79

$GPGSA,A,1,,,,,,,,,,,,,25.5,25.5,25.5*02

$BDGSA,A,1,,,,,,,,,,,,,25.5,25.5,25.5*13

$GPGSV,1,1,00*79

$BDGSV,1,1,00*68

$GNRMC,,V,,,,,,,,,,M*4E

$GNVTG,,,,,,,,,M*2D

$GNZDA,,,,,,*56

$GPTXT,01,01,01,ANTENNA OK*35

$GNGGA,,,,,,0,00,25.5,,,,,,*64

$GNGLL,,,,,,V,M*79

$GPGSA,A,1,,,,,,,,,,,,,25.5,25.5,25.5*02

$BDGSA,A,1,,,,,,,,,,,,,25.5,25.5,25.5*13

$GPGSV,1,1,00*79

$BDGSV,1,1,00*68

$GNRMC,,V,,,,,,,,,,M*4E

$GNVTG,,,,,,,,,M*2D

$GNZDA,,,,,,*56

$GPTXT,01,01,01,ANTENNA OK*35

$GNGGA,,,,,,0,00,25.5,,,,,,*64

$GNGLL,,,,,,V,M*79

$GPGSA,A,1,,,,,,,,,,,,,25.5,25.5,25.5*02

$BDGSA,A,1,,,,,,,,,,,,,25.5,25.5,25.5*13

$GPGSV,1,1,00*79

$BDGSV,1,1,00*68

$GNRMC,,V,,,,,,,,,,M*4E

$GNVTG,,,,,,,,,M*2D

$GNZDA,,,,,,*56

$GPTXT,01,01,01,ANTENNA OK*35

$GNGGA,,,,,,0,00,25.5,,,,,,*64

$GNGLL,,,,,,V,M*79

$GPGSA,A,1,,,,,,,,,,,,,25.5,25.5,25.5*02

$BDGSA,A,1,,,,,,,,,,,,,25.5,25.5,25.5*13

$GPGSV,1,1,00*79

$BDGSV,1,1,00*68

$GNRMC,,V,,,,,,,,,,M*4E

$GNVTG,,,,,,,,,M*2D

$GNZDA,,,,,,*56

$GPTXT,01,01,01,ANTENNA OK*35

$GNGGA,,,,,,0,00,25.5,,,,,,*64

$GNGLL,,,,,,V,M*79

$GPGSA,A,1,,,,,,,,,,,,,25.5,25.5,25.5*02

$BDGSA,A,1,,,,,,,,,,,,,25.5,25.5,25.5*13

$GPGSV,1,1,00*79

$BDGSV,1,1,00*68

$GNRMC,,V,,,,,,,,,,M*4E

$GNVTG,,,,,,,,,M*2D

$GNZDA,,,,,,*56

$GPTXT,01,01,01,ANTENNA OK*35

$GNGGA,,,,,,0,00,25.5,,,,,,*64

$GNGLL,,,,,,V,M*79

$GPGSA,A,1,,,,,,,,,,,,,25.5,25.5,25.5*02

$BDGSA,A,1,,,,,,,,,,,,,25.5,25.5,25.5*13

$GPGSV,1,1,00*79

$BDGSV,1,1,00*68

$GNRMC,,V,,,,,,,,,,M*4E

$GNVTG,,,,,,,,,M*2D

$GNZDA,,,,,,*56

$GPTXT,01,01,01,ANTENNA OK*35

[2019-04-27 01:34:06.924] [ERROR] [wifi    ] [T:619] Connect to 192.168.3.1:1234 failed: No route to host
[2019-04-27 01:34:06.925] [ERROR] [dev_init] [T:619] Failed to initialize WiFi
[2019-04-27 01:34:06.925] [ERROR] [usbctl  ] [T:619] Open bt fail!
[2019-04-27 01:34:06.925] [ERROR] [usbctl  ] [T:619] Can't Open Serial Port: No such file or directory
[2019-04-27 01:34:06.925] [ERROR] [usbctl  ] [T:619] Open bt fail!
[2019-04-27 01:34:06.925] [ERROR] [usbctl  ] [T:619] Can't Open Serial Port: No such file or directory
[2019-04-27 01:34:06.926] [ERROR] [usbctl  ] [T:619] Open bt fail!
[2019-04-27 01:34:06.926] [ERROR] [usbctl  ] [T:619] Can't Open Serial Port: No such file or directory
[2019-04-27 01:34:06.926] [ERROR] [usbctl  ] [T:619] Open bt fail!
[2019-04-27 01:34:06.926] [ERROR] [usbctl  ] [T:619] Can't Open Serial Port: No such file or directory
[2019-04-27 01:34:06.926] [ERROR] [usbctl  ] [T:619] Open bt fail!
[2019-04-27 01:34:06.927] [ERROR] [usbctl  ] [T:619] Can't Open Serial Port: No such file or directory
[2019-04-27 01:34:06.927] [ERROR] [bluetooth] [T:619] Bluetooth initialization failed
[2019-04-27 01:34:06.927] [ERROR] [dev_init] [T:619] Failed to initialize Bluetooth
[2019-04-27 01:34:06.927] [INFO ] [lan     ] [T:619] Configuring eth1: 192.168.2.235
$GNGGA,,,,,,0,00,25.5,,,,,,*64

$GNGLL,,,,,,V,M*79

$GPGSA,A,1,,,,,,,,,,,,,25.5,25.5,25.5*02

$BDGSA,A,1,,,,,,,,,,,,,25.5,25.5,25.5*13

$GPGSV,1,1,00*79

$BDGSV,1,1,00*68

$GNRMC,,V,,,,,,,,,,M*4E

$GNVTG,,,,,,,,,M*2D

$GNZDA,,,,,,*56

$GPTXT,01,01,01,ANTENNA OK*35

$GNGGA,,,,,,0,00,25.5,,,,,,*64

$GNGLL,,,,,,V,M*79

$GPGSA,A,1,,,,,,,,,,,,,25.5,25.5,25.5*02

$BDGSA,A,1,,,,,,,,,,,,,25.5,25.5,25.5*13

$GPGSV,1,1,00*79

$BDGSV,1,1,00*68

$GNRMC,,V,,,,,,,,,,M*4E

$GNVTG,,,,,,,,,M*2D

$GNZDA,,,,,,*56

$GPTXT,01,01,01,ANTENNA OK*35

$GNGGA,,,,,,0,00,25.5,,,,,,*64

$GNGLL,,,,,,V,M*79

$GPGSA,A,1,,,,,,,,,,,,,25.5,25.5,25.5*02

$BDGSA,A,1,,,,,,,,,,,,,25.5,25.5,25.5*13

$GPGSV,1,1,00*79

$BDGSV,1,1,00*68

$GNRMC,,V,,,,,,,,,,M*4E

$GNVTG,,,,,,,,,M*2D

$GNZDA,,,,,,*56

$GPTXT,01,01,01,ANTENNA OK*35

$GNGGA,,,,,,0,00,25.5,,,,,,*64

$GNGLL,,,,,,V,M*79

$GPGSA,A,1,,,,,,,,,,,,,25.5,25.5,25.5*02

$BDGSA,A,1,,,,,,,,,,,,,25.5,25.5,25.5*13

$GPGSV,1,1,00*79

$BDGSV,1,1,00*68

$GNRMC,,V,,,,,,,,,,M*4E

$GNVTG,,,,,,,,,M*2D

$GNZDA,,,,,,*56

$GPTXT,01,01,01,ANTENNA OK*35

$GNGGA,,,,,,0,00,25.5,,,,,,*64

$GNGLL,,,,,,V,M*79

$GPGSA,A,1,,,,,,,,,,,,,25.5,25.5,25.5*02

$BDGSA,A,1,,,,,,,,,,,,,25.5,25.5,25.5*13

$GPGSV,1,1,00*79

$BDGSV,1,1,00*68

$GNRMC,,V,,,,,,,,,,M*4E

$GNVTG,,,,,,,,,M*2D

$GNZDA,,,,,,*56

$GPTXT,01,01,01,ANTENNA OK*35

[2019-04-27 01:34:11.954] [ERROR] [lan     ] [T:619] Connect to 192.168.2.1:2234 failed: No route to host
[2019-04-27 01:34:11.954] [ERROR] [dev_init] [T:619] Failed to initialize LAN
[2019-04-27 01:34:11.955] [INFO ] [lan_server] [T:619] Configuring eth0: 192.168.31.101
$GNGGA,,,,,,0,00,25.5,,,,,,*64

$GNGLL,,,,,,V,M*79

$GPGSA,A,1,,,,,,,,,,,,,25.5,25.5,25.5*02

$BDGSA,A,1,,,,,,,,,,,,,25.5,25.5,25.5*13

$GPGSV,1,1,00*79

$BDGSV,1,1,00*68

$GNRMC,,V,,,,,,,,,,M*4E

$GNVTG,,,,,,,,,M*2D

$GNZDA,,,,,,*56

$GPTXT,01,01,01,ANTENNA OK*35

$GNGGA,,,,,,0,00,25.5,,,,,,*64

$GNGLL,,,,,,V,M*79

$GPGSA,A,1,,,,,,,,,,,,,25.5,25.5,25.5*02

$BDGSA,A,1,,,,,,,,,,,,,25.5,25.5,25.5*13

$GPGSV,1,1,00*79

$BDGSV,1,1,00*68

$GNRMC,,V,,,,,,,,,,M*4E

$GNVTG,,,,,,,,,M*2D

$GNZDA,,,,,,*56

$GPTXT,01,01,01,ANTENNA OK*35

[2019-04-27 01:34:13.995] [INFO ] [lan_server] [T:619] LAN server listening on 192.168.31.101:3234, fd=5
[2019-04-27 01:34:13.996] [INFO ] [dev_init] [T:619] Init success: LAN_Server, fd=5
$GNGGA,,,,,,0,00,25.5,,,,,,*64

$GNGLL,,,,,,V,M*79

$GPGSA,A,1,,,,,,,,,,,,,25.5,25.5,25.5*02

$BDGSA,A,1,,,,,,,,,,,,,25.5,25.5,25.5*13

$GPGSV,1,1,00*79

$BDGSV,1,1,00*68

$GNRMC,,V,,,,,,,,,,M*4E

$GNVTG,,,,,,,,,M*2D

$GNZDA,,,,,,*56

$GPTXT,01,01,01,ANTENNA OK*35

$GNGGA,,,,,,0,00,25.5,,,,,,*64

$GNGLL,,,,,,V,M*79

$GPGSA,A,1,,,,,,,,,,,,,25.5,25.5,25.5*02

$BDGSA,A,1,,,,,,,,,,,,,25.5,25.5,25.5*13

$GPGSV,1,1,00*79

$BDGSV,1,1,00*68

$GNRMC,,V,,,,,,,,,,M*4E

$GNVTG,,,,,,,,,M*2D

$GNZDA,,,,,,*56

$GPTXT,01,01,01,ANTENNA OK*35

[2019-04-27 01:34:15.996] [INFO ] [main    ] [T:619] Init communicate device success
$GNGGA,,,,,,0,00,25.5,,,,,,*64

$GNGLL,,,,,,V,M*79

$GPGSA,A,1,,,,,,,,,,,,,25.5,25.5,25.5*02

$BDGSA,A,1,,,,,,,,,,,,,25.5,25.5,25.5*13

$GPGSV,1,1,00*79

$BDGSV,1,1,00*68

$GNRMC,,V,,,,,,,,,,M*4E

$GNVTG,,,,,,,,,M*2D

$GNZDA,,,,,,*56

$GPTXT,01,01,01,ANTENNA OK*35

[2019-04-27 01:34:16.991] [INFO ] [gps     ] [T:622] Open GPS device success
[2019-04-27 01:34:17.254] [INFO ] [main    ] [T:619] CPU Usage Rate: 0118
[2019-04-27 01:34:17.255] [INFO ] [regist  ] [T:645] device_regist start!!!
[2019-04-27 01:34:17.256] [INFO ] [wdt     ] [T:645] Thread device_regist request ID=0, timeout=60
[2019-04-27 01:34:17.256] [DEBUG] [wdt     ] [T:645] Feed softdog ID=0
[2019-04-27 01:34:17.256] [DEBUG] [wdt     ] [T:645] Feed softdog ID=0
$GNGGA,,,,,,0,00,25.5,,,,,,*64
$GNGLL,,,,,,V,M*79
$GPGSA,A,1,,,,,,,,,,,,,25.5,25.5,25.5*02
$BDGSA,A,1,,,,,,,,,,,,,25.5,25.5,25.5*13
$GPGSV,1,1,00*79
$BDGSV,1,1,00*68
$GNRMC,,V,,,,,,,,,,M*4E
$GNVTG,,,,,,,,,M*2D
$GNZDA,,,,,,*56
$GPTXT,01,01,01,ANTENNA OK*35
[2019-04-27 01:34:17.856] [WARN ] [gps     ] [T:622] GPS device closed
[2019-04-27 01:34:17.864] [INFO ] [gps     ] [T:622] GPS fd 6 closed by cleanup handler
[2019-04-27 01:34:32.270] [DEBUG] [wdt     ] [T:645] Feed softdog ID=0
[2019-04-27 01:34:33.271] [INFO ] [regist  ] [T:645] Regist: $011FF0000000037SAP2400000000000100000010NFFFFEFFFFF0118@
[2019-04-27 01:34:33.272] [DEBUG] [wdt     ] [T:645] Feed softdog ID=0
[2019-04-27 01:34:33.277] [DEBUG] [wdt     ] [T:645] Feed softdog ID=0
[2019-04-27 01:34:33.277] [DEBUG] [wdt     ] [T:645] Feed softdog ID=0
[2019-04-27 01:34:37.282] [DEBUG] [wdt     ] [T:645] Feed softdog ID=0
[2019-04-27 01:34:41.286] [DEBUG] [wdt     ] [T:645] Feed softdog ID=0
[2019-04-27 01:34:45.291] [DEBUG] [wdt     ] [T:645] Feed softdog ID=0
[2019-04-27 01:34:49.295] [DEBUG] [wdt     ] [T:645] Feed softdog ID=0
[2019-04-27 01:35:04.311] [DEBUG] [wdt     ] [T:645] Feed softdog ID=0
[2019-04-27 01:35:05.311] [INFO ] [regist  ] [T:645] Regist: $011FF0000000037SAP2400000000000100000010NFFFFEFFFFF0118@
[2019-04-27 01:35:05.312] [DEBUG] [wdt     ] [T:645] Feed softdog ID=0
[2019-04-27 01:35:05.317] [DEBUG] [wdt     ] [T:645] Feed softdog ID=0
[2019-04-27 01:35:05.317] [DEBUG] [wdt     ] [T:645] Feed softdog ID=0
[2019-04-27 01:35:09.322] [DEBUG] [wdt     ] [T:645] Feed softdog ID=0
[2019-04-27 01:35:13.325] [DEBUG] [wdt     ] [T:645] Feed softdog ID=0
[2019-04-27 01:35:17.327] [DEBUG] [wdt     ] [T:645] Feed softdog ID=0
[2019-04-27 01:35:21.331] [DEBUG] [wdt     ] [T:645] Feed softdog ID=0
^C^H^H[2019-04-27 01:35:36.347] [DEBUG] [wdt     ] [T:645] Feed softdog ID=0
[2019-04-27 01:35:37.347] [INFO ] [regist  ] [T:645] Regist: $011FF0000000037SAP2400000000000100000010NFFFFEFFFFF0118@
[2019-04-27 01:35:37.348] [DEBUG] [wdt     ] [T:645] Feed softdog ID=0
[2019-04-27 01:35:37.353] [DEBUG] [wdt     ] [T:645] Feed softdog ID=0
[2019-04-27 01:35:37.354] [DEBUG] [wdt     ] [T:645] Feed softdog ID=0
[2019-04-27 01:35:41.358] [DEBUG] [wdt     ] [T:645] Feed softdog ID=0
[2019-04-27 01:35:45.362] [DEBUG] [wdt     ] [T:645] Feed softdog ID=0
[2019-04-27 01:35:49.367] [DEBUG] [wdt     ] [T:645] Feed softdog ID=0
[2019-04-27 01:35:53.368] [DEBUG] [wdt     ] [T:645] Feed softdog ID=0
[2019-04-27 01:36:08.383] [DEBUG] [wdt     ] [T:645] Feed softdog ID=0
[2019-04-27 01:36:09.384] [INFO ] [regist  ] [T:645] Regist: $011FF0000000037SAP2400000000000100000010NFFFFEFFFFF0118@
[2019-04-27 01:36:09.384] [DEBUG] [wdt     ] [T:645] Feed softdog ID=0
[2019-04-27 01:36:09.390] [DEBUG] [wdt     ] [T:645] Feed softdog ID=0
[2019-04-27 01:36:09.390] [DEBUG] [wdt     ] [T:645] Feed softdog ID=0
[2019-04-27 01:36:13.393] [DEBUG] [wdt     ] [T:645] Feed softdog ID=0
[2019-04-27 01:36:17.398] [DEBUG] [wdt     ] [T:645] Feed softdog ID=0
[2019-04-27 01:36:21.402] [DEBUG] [wdt     ] [T:645] Feed softdog ID=0
[2019-04-27 01:36:25.407] [INFO ] [regist  ] [T:645] Switching to next comm device, backoff delay=5000ms
12V power off!
killall: udhcpc: no process killed
[2019-04-27 01:36:31.612] [ERROR] [regist  ] [T:645] all devices tried, exiting registration thread
[2019-04-27 01:36:31.612] [INFO ] [wdt     ] [T:645] Release thread device_regist ID=0
[2019-04-27 01:36:31.613] [ERROR] [main    ] [T:619] Device registration failed
root@imx6ulevk:~#
