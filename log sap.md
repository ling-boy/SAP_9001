root@imx6ulevk:~# ./modbus_9001
[2019-04-27 02:37:24.265] [INFO ] [main    ] [T:614] SAP_9001 V2.1 starting...
[2019-04-27 02:37:24.266] [WARN ] [main    ] [T:614] Config file not found, using defaults
[2019-04-27 02:37:24.266] [INFO ] [memory_pool] [T:614] Created pool for N3sap13DeviceContext12PacketBufferE, size=8
12V power on!
[2019-04-27 02:37:25.304] [INFO ] [gps     ] [T:617] Start GPS thread
[2019-04-27 02:37:26.304] [INFO ] [factory ] [T:614] Created LoRa strategy
[2019-04-27 02:37:26.305] [INFO ] [factory ] [T:614] Created WiFi strategy
[2019-04-27 02:37:26.305] [INFO ] [factory ] [T:614] Created Bluetooth strategy
[2019-04-27 02:37:26.305] [INFO ] [factory ] [T:614] Created LAN strategy
[2019-04-27 02:37:26.306] [INFO ] [factory ] [T:614] Created LAN Server strategy
[2019-04-27 02:37:26.306] [INFO ] [factory ] [T:614] Created 5 communication strategies
[2019-04-27 02:37:26.331] [INFO ] [lora    ] [T:614] LoRa initialized, fd=4
[2019-04-27 02:37:26.332] [INFO ] [comm_mgr] [T:614] Add device ID=1 success
[2019-04-27 02:37:26.332] [DEBUG] [comm_mgr] [T:614] Found device ID=1
[2019-04-27 02:37:26.332] [INFO ] [comm_mgr] [T:614] Callback registered for device ID=1 pos=0
[2019-04-27 02:37:26.332] [INFO ] [dev_init] [T:614] Init success: LoRa, fd=4
[2019-04-27 02:37:28.333] [INFO ] [wifi    ] [T:614] Bringing down wlan0...
killall: wpa_supplicant: no process killed
killall: udhcpc: no process killed
[2019-04-27 02:37:32.471] [INFO ] [wifi    ] [T:614] Starting wpa_supplicant with config: /home/root/wifi.conf
Successfully initialized wpa_supplicant
rfkill: Cannot open RFKILL control device
ieee80211 phy0: rt2x00lib_request_firmware: Info - Loading firmware file 'rt2870.bin'
ieee80211 phy0: rt2x00lib_request_firmware: Info - Firmware detected - version: 0.29
IPv6: ADDRCONF(NETDEV_UP): wlan0: link is not ready
wlan0: authenticate with 00:12:0e:e4:f3:32
wlan0: send auth to 00:12:0e:e4:f3:32 (try 1/3)
wlan0: authenticated
rt2800usb 1-1.1:1.0 wlan0: disabling HT/VHT due to WEP/TKIP use
rt2800usb 1-1.1:1.0 wlan0: disabling HT as WMM/QoS is not supported by the AP
rt2800usb 1-1.1:1.0 wlan0: disabling VHT as WMM/QoS is not supported by the AP
wlan0: associate with 00:12:0e:e4:f3:32 (try 1/3)
wlan0: RX AssocResp from 00:12:0e:e4:f3:32 (capab=0x411 status=0 aid=1)
IPv6: ADDRCONF(NETDEV_CHANGE): wlan0: link becomes ready
wlan0: associated
wlan0: deauthenticated from 00:12:0e:e4:f3:32 (Reason: 2=PREV_AUTH_NOT_VALID)
[2019-04-27 02:37:42.493] [INFO ] [wifi    ] [T:614] Starting udhcpc...
udhcpc (v1.24.1) started
Sending discover...
Sending discover...
wlan0: authenticate with 00:12:0e:e4:f3:32
wlan0: send auth to 00:12:0e:e4:f3:32 (try 1/3)
wlan0: authenticated
rt2800usb 1-1.1:1.0 wlan0: disabling HT/VHT due to WEP/TKIP use
rt2800usb 1-1.1:1.0 wlan0: disabling HT as WMM/QoS is not supported by the AP
rt2800usb 1-1.1:1.0 wlan0: disabling VHT as WMM/QoS is not supported by the AP
wlan0: associate with 00:12:0e:e4:f3:32 (try 1/3)
wlan0: RX AssocResp from 00:12:0e:e4:f3:32 (capab=0x411 status=0 aid=1)
wlan0: associated
wlan0: deauthenticated from 00:12:0e:e4:f3:32 (Reason: 2=PREV_AUTH_NOT_VALID)
Sending discover...
Sending discover...
Sending discover...
Sending discover...
Sending discover...
Sending discover...
No lease, failing
wlan0: authenticate with 00:12:0e:e4:f3:32
wlan0: send auth to 00:12:0e:e4:f3:32 (try 1/3)
wlan0: authenticated
rt2800usb 1-1.1:1.0 wlan0: disabling HT/VHT due to WEP/TKIP use
rt2800usb 1-1.1:1.0 wlan0: disabling HT as WMM/QoS is not supported by the AP
rt2800usb 1-1.1:1.0 wlan0: disabling VHT as WMM/QoS is not supported by the AP
wlan0: associate with 00:12:0e:e4:f3:32 (try 1/3)
wlan0: RX AssocResp from 00:12:0e:e4:f3:32 (capab=0x411 status=0 aid=1)
wlan0: associated
[2019-04-27 02:38:12.269] [INFO ] [wifi    ] [T:614] WiFi connected to 192.168.3.1:1234, fd=5
[2019-04-27 02:38:12.269] [INFO ] [comm_mgr] [T:614] Add device ID=2 success
[2019-04-27 02:38:12.270] [DEBUG] [comm_mgr] [T:614] Found device ID=2
[2019-04-27 02:38:12.270] [INFO ] [comm_mgr] [T:614] Callback registered for device ID=2 pos=0
[2019-04-27 02:38:12.270] [INFO ] [dev_init] [T:614] Init success: WiFi, fd=5
[2019-04-27 02:38:14.270] [ERROR] [usbctl  ] [T:614] Open bt fail!
[2019-04-27 02:38:14.271] [ERROR] [usbctl  ] [T:614] Can't Open Serial Port: No such file or directory
[2019-04-27 02:38:14.271] [ERROR] [usbctl  ] [T:614] Open bt fail!
[2019-04-27 02:38:14.271] [ERROR] [usbctl  ] [T:614] Can't Open Serial Port: No such file or directory
[2019-04-27 02:38:14.271] [ERROR] [usbctl  ] [T:614] Open bt fail!
[2019-04-27 02:38:14.272] [ERROR] [usbctl  ] [T:614] Can't Open Serial Port: No such file or directory
[2019-04-27 02:38:14.272] [ERROR] [usbctl  ] [T:614] Open bt fail!
[2019-04-27 02:38:14.272] [ERROR] [usbctl  ] [T:614] Can't Open Serial Port: No such file or directory
[2019-04-27 02:38:14.272] [ERROR] [usbctl  ] [T:614] Open bt fail!
[2019-04-27 02:38:14.272] [ERROR] [usbctl  ] [T:614] Can't Open Serial Port: No such file or directory
[2019-04-27 02:38:14.272] [ERROR] [bluetooth] [T:614] Bluetooth initialization failed
[2019-04-27 02:38:14.273] [ERROR] [dev_init] [T:614] Failed to initialize Bluetooth
[2019-04-27 02:38:14.273] [INFO ] [lan     ] [T:614] Configuring eth1: 192.168.2.235
[2019-04-27 02:38:16.307] [INFO ] [lan     ] [T:614] LAN connected to 192.168.2.1:2234, fd=6
[2019-04-27 02:38:16.307] [INFO ] [comm_mgr] [T:614] Add device ID=4 success
[2019-04-27 02:38:16.307] [DEBUG] [comm_mgr] [T:614] Found device ID=4
[2019-04-27 02:38:16.307] [INFO ] [comm_mgr] [T:614] Callback registered for device ID=4 pos=0
[2019-04-27 02:38:16.308] [INFO ] [dev_init] [T:614] Init success: LAN, fd=6
[2019-04-27 02:38:18.308] [INFO ] [lan_server] [T:614] Configuring eth0: 192.168.31.101
[2019-04-27 02:38:20.341] [INFO ] [lan_server] [T:614] LAN server listening on 192.168.31.101:3234, fd=7
[2019-04-27 02:38:20.342] [INFO ] [dev_init] [T:614] Init success: LAN_Server, fd=7
[2019-04-27 02:38:22.342] [INFO ] [main    ] [T:614] Init communicate device success
[2019-04-27 02:38:23.447] [INFO ] [main    ] [T:614] CPU Usage Rate: 0114
[2019-04-27 02:38:23.448] [INFO ] [regist  ] [T:642] device_regist start!!!
[2019-04-27 02:38:23.449] [INFO ] [wdt     ] [T:642] Thread device_regist request ID=0, timeout=60
[2019-04-27 02:38:23.449] [DEBUG] [wdt     ] [T:642] Feed softdog ID=0
[2019-04-27 02:38:23.449] [DEBUG] [wdt     ] [T:642] Feed softdog ID=0
[2019-04-27 02:38:25.328] [INFO ] [gps     ] [T:617] Open GPS device success
[2019-04-27 02:38:26.195] [WARN ] [gps     ] [T:617] GPS device closed
[2019-04-27 02:38:26.195] [DEBUG] [gps     ] [T:617] NMEA[0]:
[2019-04-27 02:38:26.205] [INFO ] [gps     ] [T:617] GPS fd 8 closed by cleanup handler
[2019-04-27 02:38:38.464] [DEBUG] [wdt     ] [T:642] Feed softdog ID=0
[2019-04-27 02:38:39.465] [INFO ] [regist  ] [T:642] Regist: $014FF0000000037SAP2400000000000110100010NFFFFEFFFFF0114@
[2019-04-27 02:38:39.466] [DEBUG] [wdt     ] [T:642] Feed softdog ID=0
[2019-04-27 02:38:39.468] [DEBUG] [wdt     ] [T:642] Feed softdog ID=0
[2019-04-27 02:38:39.468] [INFO ] [regist  ] [T:642] Gateway assigned ID: 01
[2019-04-27 02:38:39.468] [INFO ] [regist  ] [T:642] confirm_message: $024010001000012SAP240000000000001@
[2019-04-27 02:38:39.469] [DEBUG] [wdt     ] [T:642] Feed softdog ID=0
[2019-04-27 02:38:39.473] [INFO ] [regist  ] [T:642] Timestamp listen counter: 7
[2026-06-21 21:34:57.227] [INFO ] [regist  ] [T:642] Set system time success: 20260621213457227
[2026-06-21 21:34:57.227] [DEBUG] [comm_mgr] [T:642] Found device ID=4
[2026-06-21 21:34:57.227] [INFO ] [regist  ] [T:642] Device ID=4 enabled
[2026-06-21 21:34:57.227] [INFO ] [regist  ] [T:642] SAP register success
[2026-06-21 21:34:57.228] [INFO ] [wdt     ] [T:642] Release thread device_regist ID=0
[2026-06-21 21:34:57.228] [INFO ] [main    ] [T:614] Device Registration Successful
[2026-06-21 21:34:57.228] [ERROR] [protocol] [T:614] fopen /home/root/storefile.txt failed: No such file or directory
[2026-06-21 21:34:57.229] [INFO ] [main    ] [T:614] Read unsent packets: 0
[2026-06-21 21:34:58.229] [INFO ] [main    ] [T:614] Main thread waiting for signal...
[2026-06-21 21:34:58.230] [INFO ] [transmit] [T:645] start trans message to gateway
[2026-06-21 21:34:58.230] [INFO ] [wdt     ] [T:645] Thread trans_message request ID=0, timeout=30
[2026-06-21 21:34:58.230] [INFO ] [transmit] [T:645] Enable FD Num: 6
[2026-06-21 21:34:58.231] [INFO ] [wdt     ] [T:644] Thread get_ship_data request ID=1, timeout=30
Opening /dev/ttymxc2 at 9600 bauds (N, 8, 1)
[2026-06-21 21:34:58.234] [INFO ] [sensor  ] [T:644] sensor connected successfully
[2026-06-21 21:34:58.235] [INFO ] [sensor  ] [T:644] Start reading sensor 1 data
[01][03][00][01][00][02][95][CB]
Sending request using RTS signal
Waiting for a confirmation...
ERROR Connection timed out: select
[2026-06-21 21:34:58.747] [ERROR] [sensor  ] [T:644] Connection timed out
[2026-06-21 21:34:58.747] [WARN ] [sensor  ] [T:644] Read sensor 1 data failed
[2026-06-21 21:35:02.748] [DEBUG] [wdt     ] [T:644] Feed softdog ID=1
[2026-06-21 21:35:02.748] [INFO ] [sensor  ] [T:644] thread of sensor feed dog success
[2026-06-21 21:35:02.748] [INFO ] [sensor  ] [T:644] Start reading sensor 2 data
[02][03][00][01][00][02][95][F8]
Sending request using RTS signal
Waiting for a confirmation...
ERROR Connection timed out: select
[2026-06-21 21:35:03.260] [ERROR] [sensor  ] [T:644] Connection timed out
[2026-06-21 21:35:03.261] [WARN ] [sensor  ] [T:644] Read sensor 2 data failed
[2026-06-21 21:35:05.225] [INFO ] [transmit] [T:645] Message From The Gateway: $03100000100001A010001SAP24000000000000001@
[2026-06-21 21:35:05.226] [INFO ] [transmit] [T:645] Message counter: 1
[2026-06-21 21:35:05.226] [DEBUG] [wdt     ] [T:645] Feed softdog ID=0
[2026-06-21 21:35:05.226] [INFO ] [transmit] [T:645] thread of trans_message feed dog success
[2026-06-21 21:35:05.226] [INFO ] [transmit] [T:645] Enable FD Num: 6
[2026-06-21 21:35:07.261] [DEBUG] [wdt     ] [T:644] Feed softdog ID=1
[2026-06-21 21:35:07.261] [INFO ] [sensor  ] [T:644] thread of sensor feed dog success
[2026-06-21 21:35:07.261] [INFO ] [sensor  ] [T:644] Start reading sensor 3 data
[03][03][00][01][00][02][94][29]
Sending request using RTS signal
Waiting for a confirmation...
ERROR Connection timed out: select
[2026-06-21 21:35:07.773] [ERROR] [sensor  ] [T:644] Connection timed out
[2026-06-21 21:35:07.774] [WARN ] [sensor  ] [T:644] Read sensor 3 data failed
[2026-06-21 21:35:11.774] [DEBUG] [wdt     ] [T:644] Feed softdog ID=1
[2026-06-21 21:35:11.774] [INFO ] [sensor  ] [T:644] thread of sensor feed dog success
[2026-06-21 21:35:11.774] [INFO ] [sensor  ] [T:644] Start reading sensor 4 data
[04][03][00][01][00][02][95][9E]
Sending request using RTS signal
Waiting for a confirmation...
ERROR Connection timed out: select
[2026-06-21 21:35:12.287] [ERROR] [sensor  ] [T:644] Connection timed out
[2026-06-21 21:35:12.287] [WARN ] [sensor  ] [T:644] Read sensor 4 data failed
[2026-06-21 21:35:13.225] [INFO ] [transmit] [T:645] Message From The Gateway: $03100000100001A010001SAP24000000000000001@
[2026-06-21 21:35:13.225] [INFO ] [transmit] [T:645] Message counter: 2
[2026-06-21 21:35:13.226] [DEBUG] [wdt     ] [T:645] Feed softdog ID=0
[2026-06-21 21:35:13.226] [INFO ] [transmit] [T:645] thread of trans_message feed dog success
[2026-06-21 21:35:13.226] [INFO ] [transmit] [T:645] Enable FD Num: 6
[2026-06-21 21:35:16.287] [DEBUG] [wdt     ] [T:644] Feed softdog ID=1
[2026-06-21 21:35:16.288] [INFO ] [sensor  ] [T:644] thread of sensor feed dog success
[2026-06-21 21:35:16.288] [INFO ] [sensor  ] [T:644] Start reading sensor 5 data
[05][03][26][00][00][04][4E][C5]
Sending request using RTS signal
Waiting for a confirmation...
ERROR Connection timed out: select
[2026-06-21 21:35:16.800] [ERROR] [sensor  ] [T:644] Connection timed out
[2026-06-21 21:35:16.800] [WARN ] [sensor  ] [T:644] Read sensor 5 data failed
[2026-06-21 21:35:20.800] [DEBUG] [wdt     ] [T:644] Feed softdog ID=1
[2026-06-21 21:35:20.801] [INFO ] [sensor  ] [T:644] thread of sensor feed dog success
[2026-06-21 21:35:20.801] [INFO ] [sensor  ] [T:644] Start reading sensor 6 data
[06][03][26][00][00][04][4E][F6]
Sending request using RTS signal
Waiting for a confirmation...
[2026-06-21 21:35:21.225] [INFO ] [transmit] [T:645] Message From The Gateway: $03100000100001A010001SAP24000000000000001@
[2026-06-21 21:35:21.225] [INFO ] [transmit] [T:645] Message counter: 3
[2026-06-21 21:35:21.226] [DEBUG] [wdt     ] [T:645] Feed softdog ID=0
[2026-06-21 21:35:21.226] [INFO ] [transmit] [T:645] thread of trans_message feed dog success
[2026-06-21 21:35:21.226] [INFO ] [transmit] [T:645] Enable FD Num: 6
ERROR Connection timed out: select
[2026-06-21 21:35:21.313] [ERROR] [sensor  ] [T:644] Connection timed out
[2026-06-21 21:35:21.313] [WARN ] [sensor  ] [T:644] Read sensor 6 data failed
[2026-06-21 21:35:25.314] [DEBUG] [wdt     ] [T:644] Feed softdog ID=1
[2026-06-21 21:35:25.314] [INFO ] [sensor  ] [T:644] thread of sensor feed dog success
[2026-06-21 21:35:25.314] [INFO ] [sensor  ] [T:644] Start reading sensor 7 data
[07][03][00][00][00][02][C4][6D]
Sending request using RTS signal
Waiting for a confirmation...
ERROR Connection timed out: select
[2026-06-21 21:35:25.826] [ERROR] [sensor  ] [T:644] Connection timed out
[2026-06-21 21:35:25.826] [WARN ] [sensor  ] [T:644] Read sensor 7 data failed
[2026-06-21 21:35:29.225] [INFO ] [transmit] [T:645] Message From The Gateway: $03100000100001A010001SAP24000000000000001@
[2026-06-21 21:35:29.226] [INFO ] [transmit] [T:645] Message counter: 4
[2026-06-21 21:35:29.226] [DEBUG] [wdt     ] [T:645] Feed softdog ID=0
[2026-06-21 21:35:29.226] [INFO ] [transmit] [T:645] thread of trans_message feed dog success
[2026-06-21 21:35:29.226] [INFO ] [transmit] [T:645] Enable FD Num: 6
[2026-06-21 21:35:29.827] [DEBUG] [wdt     ] [T:644] Feed softdog ID=1
[2026-06-21 21:35:29.827] [INFO ] [sensor  ] [T:644] thread of sensor feed dog success
[2026-06-21 21:35:29.827] [INFO ] [sensor  ] [T:644] Start reading sensor 8 data
[07][03][00][06][00][02][24][6C]
Sending request using RTS signal
Waiting for a confirmation...
ERROR Connection timed out: select
[2026-06-21 21:35:30.340] [ERROR] [sensor  ] [T:644] Connection timed out
[2026-06-21 21:35:30.340] [WARN ] [sensor  ] [T:644] Read sensor 8 data failed
[2026-06-21 21:35:34.340] [DEBUG] [wdt     ] [T:644] Feed softdog ID=1
[2026-06-21 21:35:34.341] [INFO ] [sensor  ] [T:644] thread of sensor feed dog success
[2026-06-21 21:35:34.341] [INFO ] [sensor  ] [T:644] Start reading sensor 9 data
[08][03][00][00][00][01][84][93]
Sending request using RTS signal
Waiting for a confirmation...
ERROR Connection timed out: select
[2026-06-21 21:35:34.853] [ERROR] [sensor  ] [T:644] Connection timed out
[2026-06-21 21:35:34.853] [WARN ] [sensor  ] [T:644] Read sensor 9 data failed
[2026-06-21 21:35:37.225] [INFO ] [transmit] [T:645] Message From The Gateway: $03100000100001A010001SAP24000000000000001@
[2026-06-21 21:35:37.226] [INFO ] [transmit] [T:645] Message counter: 5
[2026-06-21 21:35:37.226] [DEBUG] [wdt     ] [T:645] Feed softdog ID=0
[2026-06-21 21:35:37.226] [INFO ] [transmit] [T:645] thread of trans_message feed dog success
[2026-06-21 21:35:37.226] [INFO ] [transmit] [T:645] Enable FD Num: 6
[2026-06-21 21:35:38.853] [DEBUG] [wdt     ] [T:644] Feed softdog ID=1
[2026-06-21 21:35:38.854] [INFO ] [sensor  ] [T:644] thread of sensor feed dog success
[2026-06-21 21:35:38.854] [INFO ] [sensor  ] [T:644] Start reading sensor 10 data
[08][03][00][01][00][01][D5][53]
Sending request using RTS signal
Waiting for a confirmation...
ERROR Connection timed out: select
[2026-06-21 21:35:39.366] [ERROR] [sensor  ] [T:644] Connection timed out
[2026-06-21 21:35:39.366] [WARN ] [sensor  ] [T:644] Read sensor 10 data failed
[2026-06-21 21:35:43.367] [DEBUG] [wdt     ] [T:644] Feed softdog ID=1
[2026-06-21 21:35:43.367] [INFO ] [sensor  ] [T:644] thread of sensor feed dog success
[2026-06-21 21:35:43.367] [INFO ] [sensor  ] [T:644] Start reading sensor 11 data
[08][03][00][02][00][01][25][53]
Sending request using RTS signal
Waiting for a confirmation...
ERROR Connection timed out: select
[2026-06-21 21:35:43.880] [ERROR] [sensor  ] [T:644] Connection timed out
[2026-06-21 21:35:43.880] [WARN ] [sensor  ] [T:644] Read sensor 11 data failed
[2026-06-21 21:35:45.225] [INFO ] [transmit] [T:645] Message From The Gateway: $03100000100001A010001SAP24000000000000001@
[2026-06-21 21:35:45.226] [INFO ] [transmit] [T:645] Message counter: 6
[2026-06-21 21:35:45.226] [DEBUG] [wdt     ] [T:645] Feed softdog ID=0
[2026-06-21 21:35:45.226] [INFO ] [transmit] [T:645] thread of trans_message feed dog success
[2026-06-21 21:35:45.226] [INFO ] [transmit] [T:645] Enable FD Num: 6
[2026-06-21 21:35:47.880] [DEBUG] [wdt     ] [T:644] Feed softdog ID=1
[2026-06-21 21:35:47.880] [INFO ] [sensor  ] [T:644] thread of sensor feed dog success
[2026-06-21 21:35:47.881] [INFO ] [sensor  ] [T:644] Start reading sensor 12 data
[08][03][00][03][00][02][34][92]
Sending request using RTS signal
Waiting for a confirmation...
ERROR Connection timed out: select
[2026-06-21 21:35:48.393] [ERROR] [sensor  ] [T:644] Connection timed out
[2026-06-21 21:35:48.393] [WARN ] [sensor  ] [T:644] Read sensor 12 data failed
[2026-06-21 21:35:52.393] [DEBUG] [wdt     ] [T:644] Feed softdog ID=1
[2026-06-21 21:35:52.394] [INFO ] [sensor  ] [T:644] thread of sensor feed dog success
[2026-06-21 21:35:52.394] [INFO ] [sensor  ] [T:644] Start reading sensor 13 data
[09][03][00][00][00][01][85][42]
Sending request using RTS signal
Waiting for a confirmation...
ERROR Connection timed out: select
[2026-06-21 21:35:52.906] [ERROR] [sensor  ] [T:644] Connection timed out
[2026-06-21 21:35:52.906] [WARN ] [sensor  ] [T:644] Read sensor 13 data failed
[2026-06-21 21:35:53.225] [INFO ] [transmit] [T:645] Message From The Gateway: $03100000100001A010001SAP24000000000000001@
[2026-06-21 21:35:53.226] [INFO ] [transmit] [T:645] Message counter: 7
[2026-06-21 21:35:53.226] [DEBUG] [wdt     ] [T:645] Feed softdog ID=0
[2026-06-21 21:35:53.226] [INFO ] [transmit] [T:645] thread of trans_message feed dog success
[2026-06-21 21:35:53.226] [INFO ] [transmit] [T:645] Enable FD Num: 6
[2026-06-21 21:35:56.906] [DEBUG] [wdt     ] [T:644] Feed softdog ID=1
[2026-06-21 21:35:56.907] [INFO ] [sensor  ] [T:644] thread of sensor feed dog success
[2026-06-21 21:35:56.907] [INFO ] [sensor  ] [T:644] Start reading sensor 14 data
[0A][04][00][00][00][02][70][B0]
Sending request using RTS signal
Waiting for a confirmation...
ERROR Connection timed out: select
[2026-06-21 21:35:57.419] [ERROR] [sensor  ] [T:644] Connection timed out
[2026-06-21 21:35:57.420] [WARN ] [sensor  ] [T:644] Read sensor 14 data failed
[2026-06-21 21:36:01.225] [INFO ] [transmit] [T:645] Message From The Gateway: $03100000100001A010001SAP24000000000000001@
[2026-06-21 21:36:01.226] [INFO ] [transmit] [T:645] Message counter: 8
[2026-06-21 21:36:01.226] [DEBUG] [wdt     ] [T:645] Feed softdog ID=0
[2026-06-21 21:36:01.226] [INFO ] [transmit] [T:645] thread of trans_message feed dog success
[2026-06-21 21:36:01.226] [INFO ] [transmit] [T:645] Enable FD Num: 6
[2026-06-21 21:36:01.420] [DEBUG] [wdt     ] [T:644] Feed softdog ID=1
[2026-06-21 21:36:01.420] [INFO ] [sensor  ] [T:644] thread of sensor feed dog success
[2026-06-21 21:36:01.420] [INFO ] [sensor  ] [T:644] Start reading sensor 15 data
[0B][03][00][00][00][01][84][A0]
Sending request using RTS signal
Waiting for a confirmation...
ERROR Connection timed out: select
[2026-06-21 21:36:01.932] [ERROR] [sensor  ] [T:644] Connection timed out
[2026-06-21 21:36:01.933] [WARN ] [sensor  ] [T:644] Read sensor 15 data failed
[2026-06-21 21:36:05.933] [DEBUG] [wdt     ] [T:644] Feed softdog ID=1
[2026-06-21 21:36:05.933] [INFO ] [sensor  ] [T:644] thread of sensor feed dog success
[2026-06-21 21:36:05.933] [INFO ] [sensor  ] [T:644] Start reading sensor 16 data
[0C][03][00][00][00][01][85][17]
Sending request using RTS signal
Waiting for a confirmation...
ERROR Connection timed out: select
[2026-06-21 21:36:06.446] [ERROR] [sensor  ] [T:644] Connection timed out
[2026-06-21 21:36:06.446] [WARN ] [sensor  ] [T:644] Read sensor 16 data failed
[2026-06-21 21:36:09.226] [INFO ] [transmit] [T:645] Message From The Gateway: $03100000100001A010001SAP24000000000000001@
[2026-06-21 21:36:09.226] [INFO ] [transmit] [T:645] Message counter: 9
[2026-06-21 21:36:09.226] [DEBUG] [wdt     ] [T:645] Feed softdog ID=0
[2026-06-21 21:36:09.226] [INFO ] [transmit] [T:645] thread of trans_message feed dog success
[2026-06-21 21:36:09.226] [INFO ] [transmit] [T:645] Enable FD Num: 6
[2026-06-21 21:36:10.446] [DEBUG] [wdt     ] [T:644] Feed softdog ID=1
[2026-06-21 21:36:10.446] [INFO ] [sensor  ] [T:644] thread of sensor feed dog success
[2026-06-21 21:36:10.447] [INFO ] [sensor  ] [T:644] Start reading sensor 17 data
[0C][03][00][01][00][01][D4][D7]
Sending request using RTS signal
Waiting for a confirmation...
ERROR Connection timed out: select
[2026-06-21 21:36:10.959] [ERROR] [sensor  ] [T:644] Connection timed out
[2026-06-21 21:36:10.959] [WARN ] [sensor  ] [T:644] Read sensor 17 data failed
[2026-06-21 21:36:14.959] [DEBUG] [wdt     ] [T:644] Feed softdog ID=1
[2026-06-21 21:36:14.960] [INFO ] [sensor  ] [T:644] thread of sensor feed dog success
[2026-06-21 21:36:14.960] [INFO ] [sensor  ] [T:644] Start reading sensor 18 data
[0C][03][00][02][00][01][24][D7]
Sending request using RTS signal
Waiting for a confirmation...
ERROR Connection timed out: select
[2026-06-21 21:36:15.472] [ERROR] [sensor  ] [T:644] Connection timed out
[2026-06-21 21:36:15.472] [WARN ] [sensor  ] [T:644] Read sensor 18 data failed
[2026-06-21 21:36:17.226] [INFO ] [transmit] [T:645] Message From The Gateway: $03100000100001A010001SAP24000000000000001@
[2026-06-21 21:36:17.226] [INFO ] [transmit] [T:645] Message counter: 10
[2026-06-21 21:36:17.226] [DEBUG] [wdt     ] [T:645] Feed softdog ID=0
[2026-06-21 21:36:17.226] [INFO ] [transmit] [T:645] thread of trans_message feed dog success
[2026-06-21 21:36:17.226] [INFO ] [transmit] [T:645] Enable FD Num: 6
[2026-06-21 21:36:19.473] [DEBUG] [wdt     ] [T:644] Feed softdog ID=1
[2026-06-21 21:36:19.473] [INFO ] [sensor  ] [T:644] thread of sensor feed dog success
[2026-06-21 21:36:19.473] [INFO ] [sensor  ] [T:644] Start reading sensor 19 data
[0C][03][00][03][00][01][75][17]
Sending request using RTS signal
Waiting for a confirmation...
ERROR Connection timed out: select
[2026-06-21 21:36:19.985] [ERROR] [sensor  ] [T:644] Connection timed out
[2026-06-21 21:36:19.985] [WARN ] [sensor  ] [T:644] Read sensor 19 data failed
[2026-06-21 21:36:23.986] [DEBUG] [wdt     ] [T:644] Feed softdog ID=1
[2026-06-21 21:36:23.986] [INFO ] [sensor  ] [T:644] thread of sensor feed dog success
[2026-06-21 21:36:23.986] [INFO ] [sensor  ] [T:644] Start reading sensor 20 data
[0C][03][00][04][00][01][C4][D6]
Sending request using RTS signal
Waiting for a confirmation...
ERROR Connection timed out: select
[2026-06-21 21:36:24.498] [ERROR] [sensor  ] [T:644] Connection timed out
[2026-06-21 21:36:24.499] [WARN ] [sensor  ] [T:644] Read sensor 20 data failed
[2026-06-21 21:36:25.226] [INFO ] [transmit] [T:645] Message From The Gateway: $03100000100001A010001SAP24000000000000001@
[2026-06-21 21:36:25.226] [INFO ] [transmit] [T:645] Message counter: 11
[2026-06-21 21:36:25.226] [DEBUG] [wdt     ] [T:645] Feed softdog ID=0
[2026-06-21 21:36:25.226] [INFO ] [transmit] [T:645] thread of trans_message feed dog success
[2026-06-21 21:36:25.227] [INFO ] [transmit] [T:645] Enable FD Num: 6
[2026-06-21 21:36:28.499] [DEBUG] [wdt     ] [T:644] Feed softdog ID=1
[2026-06-21 21:36:28.499] [INFO ] [sensor  ] [T:644] thread of sensor feed dog success
[2026-06-21 21:36:28.499] [INFO ] [sensor  ] [T:644] Start reading sensor 21 data
[0C][03][00][05][00][01][95][16]
Sending request using RTS signal
Waiting for a confirmation...
ERROR Connection timed out: select
[2026-06-21 21:36:29.012] [ERROR] [sensor  ] [T:644] Connection timed out
[2026-06-21 21:36:29.012] [WARN ] [sensor  ] [T:644] Read sensor 21 data failed
[2026-06-21 21:36:33.012] [DEBUG] [wdt     ] [T:644] Feed softdog ID=1
[2026-06-21 21:36:33.012] [INFO ] [sensor  ] [T:644] thread of sensor feed dog success
[2026-06-21 21:36:33.013] [INFO ] [sensor  ] [T:644] Start reading sensor 22 data
[0C][03][00][06][00][01][65][16]
Sending request using RTS signal
Waiting for a confirmation...
[2026-06-21 21:36:33.226] [INFO ] [transmit] [T:645] Message From The Gateway: $03100000100001A010001SAP24000000000000001@
[2026-06-21 21:36:33.226] [INFO ] [transmit] [T:645] Message counter: 12
[2026-06-21 21:36:33.226] [DEBUG] [wdt     ] [T:645] Feed softdog ID=0
[2026-06-21 21:36:33.226] [INFO ] [transmit] [T:645] thread of trans_message feed dog success
[2026-06-21 21:36:33.227] [INFO ] [transmit] [T:645] Enable FD Num: 6
ERROR Connection timed out: select
[2026-06-21 21:36:33.525] [ERROR] [sensor  ] [T:644] Connection timed out
[2026-06-21 21:36:33.525] [WARN ] [sensor  ] [T:644] Read sensor 22 data failed
[2026-06-21 21:36:37.525] [DEBUG] [wdt     ] [T:644] Feed softdog ID=1
[2026-06-21 21:36:37.526] [INFO ] [sensor  ] [T:644] thread of sensor feed dog success
[2026-06-21 21:36:37.526] [INFO ] [sensor  ] [T:644] Start reading sensor 23 data
[0D][03][00][00][00][01][84][C6]
Sending request using RTS signal
Waiting for a confirmation...
ERROR Connection timed out: select
[2026-06-21 21:36:38.038] [ERROR] [sensor  ] [T:644] Connection timed out
[2026-06-21 21:36:38.038] [WARN ] [sensor  ] [T:644] Read sensor 23 data failed
[2026-06-21 21:36:41.226] [INFO ] [transmit] [T:645] Message From The Gateway: $03100000100001A010001SAP24000000000000001@
[2026-06-21 21:36:41.226] [INFO ] [transmit] [T:645] Message counter: 13
[2026-06-21 21:36:41.226] [DEBUG] [wdt     ] [T:645] Feed softdog ID=0
[2026-06-21 21:36:41.227] [INFO ] [transmit] [T:645] thread of trans_message feed dog success
[2026-06-21 21:36:41.227] [INFO ] [transmit] [T:645] Enable FD Num: 6
[2026-06-21 21:36:42.039] [DEBUG] [wdt     ] [T:644] Feed softdog ID=1
[2026-06-21 21:36:42.039] [INFO ] [sensor  ] [T:644] thread of sensor feed dog success
[2026-06-21 21:36:42.039] [INFO ] [sensor  ] [T:644] --------------------------------
[2026-06-21 21:36:42.039] [INFO ] [sensor  ] [T:644] Data after one round:
[2026-06-21 21:36:42.039] [INFO ] [sensor  ] [T:644] 溶解氧: -999.000000
[2026-06-21 21:36:42.040] [INFO ] [sensor  ] [T:644] 氨氮: -999.000000
[2026-06-21 21:36:42.040] [INFO ] [sensor  ] [T:644] 铜离子: -999.000000
[2026-06-21 21:36:42.040] [INFO ] [sensor  ] [T:644] 镉离子: -999.000000
[2026-06-21 21:36:42.040] [INFO ] [sensor  ] [T:644] 蓝绿藻: -999.000000
[2026-06-21 21:36:42.040] [INFO ] [sensor  ] [T:644] 叶绿素: -999.000000
[2026-06-21 21:36:42.040] [INFO ] [sensor  ] [T:644] 电导率: -999.000000
[2026-06-21 21:36:42.041] [INFO ] [sensor  ] [T:644] TDS: -999.000000
[2026-06-21 21:36:42.041] [INFO ] [sensor  ] [T:644] 环境湿度: -999.000000
[2026-06-21 21:36:42.041] [INFO ] [sensor  ] [T:644] 环境温度: -999.000000
[2026-06-21 21:36:42.041] [INFO ] [sensor  ] [T:644] CO2: -999.000000
[2026-06-21 21:36:42.041] [INFO ] [sensor  ] [T:644] 光照: -999.000000
[2026-06-21 21:36:42.041] [INFO ] [sensor  ] [T:644] NH3: -999.000000
[2026-06-21 21:36:42.041] [INFO ] [sensor  ] [T:644] 深度: -999.000000
[2026-06-21 21:36:42.042] [INFO ] [sensor  ] [T:644] 浊度: -999.000000
[2026-06-21 21:36:42.042] [INFO ] [sensor  ] [T:644] 风向: -999.000000
[2026-06-21 21:36:42.042] [INFO ] [sensor  ] [T:644] 风速: -999.000000
[2026-06-21 21:36:42.042] [INFO ] [sensor  ] [T:644] 雨量: -999.000000
[2026-06-21 21:36:42.042] [INFO ] [sensor  ] [T:644] 光照度: -999.000000
[2026-06-21 21:36:42.042] [INFO ] [sensor  ] [T:644] 气象温度: -999.000000
[2026-06-21 21:36:42.043] [INFO ] [sensor  ] [T:644] 气象湿度: -999.000000
[2026-06-21 21:36:42.043] [INFO ] [sensor  ] [T:644] 大气压力: -999.000000
[2026-06-21 21:36:42.043] [INFO ] [sensor  ] [T:644] 噪声: -999.000000
[2026-06-21 21:36:42.043] [INFO ] [sensor  ] [T:644] --------------------------------
[2026-06-21 21:36:42.045] [INFO ] [sensor  ] [T:644] HJ212 packet: ##0608QN=20260621213642039;ST=26;CN=2011;PW=123456;MN=Air160000000000000000000000;Flag=4;CP=&&Gps=NFFFFEFFFFF;w01010-Rtd=-00999.000;w21003-Rtd=-00999.000;w20138-Rtd=-00999.000;w20143-Rtd=-00999.000;w00003-Rtd=-00999.000;w00004-Rtd=-00999.000;w01014-Rtd=-00999.000;w01006-Rtd=-00999.000;a01002-Rtd=-00999.000;q00005-Rtd=-00999.000;a05001-Rtd=-00999.000;a01004-Rtd=-00999.000;a21001-Rtd=-00999.000;w00007-Rtd=-00999.000;w01012-Rtd=-00999.000;a01008-Rtd=-00999.000;a01007-Rtd=-00999.000;a01003-Rtd=-0
[2026-06-21 21:36:42.148] [DEBUG] [memory_pool] [T:644] Allocated N3sap13DeviceContext12PacketBufferE, used=1/8
[2026-06-21 21:36:42.149] [DEBUG] [memory_pool] [T:644] Deallocated N3sap13DeviceContext12PacketBufferE, used=0/8
[2026-06-21 21:36:42.149] [INFO ] [sensor  ] [T:644] 06 packet: $064010001000295ISR2700000000027SAP24000000000001000914110100##0608QN=20260621213642039;ST=26;CN=2011;PW=123456;MN=Air160000000000000000000000;Flag=4;CP=&&Gps=NFFFFEFFFFF;w01010-Rtd=-00999.000;w21003-Rtd=-00999.000;w20138-Rtd=-00999.000;w20143-Rtd=-00999.000;w00003-Rtd=-00999.000;w00004-Rtd=-00999.000;w01014-Rtd=-00999.000;w01006-Rtd=-00999.000;a01002-Rtd=-00999.000;q00005-Rtd=-00999.000;a05001-Rtd=-00999.000;a01004-Rtd=-00999.000;a21001-Rtd=-00999.000;w00007-Rtd=-00999.000;w01012-Rtd=-00999.000
[2026-06-21 21:36:42.149] [INFO ] [sensor  ] [T:644] Start reading sensor 1 data
[01][03][00][01][00][02][95][CB]
Sending request using RTS signal
Waiting for a confirmation...
ERROR Connection timed out: select
[2026-06-21 21:36:42.661] [ERROR] [sensor  ] [T:644] Connection timed out
[2026-06-21 21:36:42.662] [WARN ] [sensor  ] [T:644] Read sensor 1 data failed
[2026-06-21 21:36:46.662] [DEBUG] [wdt     ] [T:644] Feed softdog ID=1
[2026-06-21 21:36:46.662] [INFO ] [sensor  ] [T:644] thread of sensor feed dog success
[2026-06-21 21:36:46.663] [INFO ] [sensor  ] [T:644] Start reading sensor 2 data
[02][03][00][01][00][02][95][F8]
Sending request using RTS signal
Waiting for a confirmation...
ERROR Connection timed out: select
[2026-06-21 21:36:47.175] [ERROR] [sensor  ] [T:644] Connection timed out
[2026-06-21 21:36:47.175] [WARN ] [sensor  ] [T:644] Read sensor 2 data failed
[2026-06-21 21:36:49.226] [INFO ] [transmit] [T:645] Message From The Gateway: $03100000100001A010001SAP24000000000000001@
[2026-06-21 21:36:49.226] [INFO ] [transmit] [T:645] Message counter: 14
[2026-06-21 21:36:49.226] [DEBUG] [wdt     ] [T:645] Feed softdog ID=0
[2026-06-21 21:36:49.227] [INFO ] [transmit] [T:645] thread of trans_message feed dog success
[2026-06-21 21:36:49.227] [INFO ] [transmit] [T:645] Transmit queue remaining: 0
[2026-06-21 21:36:49.228] [INFO ] [transmit] [T:645] hj_crc: 0
[2026-06-21 21:36:49.228] [INFO ] [transmit] [T:645] =======================
[2026-06-21 21:36:49.228] [INFO ] [transmit] [T:645] data_packet = $064010001000295ISR2700000000027SAP24000000000001000914110100##0608QN=20260621213642039;ST=26;CN=2011;PW=123456;MN=Air160000000000000000000000;Flag=4;CP=&&Gps=NFFFFEFFFFF;w01010-Rtd=-00999.000;w21003-Rtd=-00999.000;w20138-Rtd=-00999.000;w20143-Rtd=-00999.000;w00003-Rtd=-00999.000;w00004-Rtd=-00999.000;w01014-Rtd=-00999.000;w01006-Rtd=-00999.000;a01002-Rtd=-00999.000;q00005-Rtd=-00999.000;a05001-Rtd=-00999.000;a01004-Rtd=-00999.000;a21001-Rtd=-00999.000;w00007-Rtd=-00999.000;w01012-Rtd=-00999.
[2026-06-21 21:36:49.228] [INFO ] [transmit] [T:645] Trans success(deviceFd) 6
[2026-06-21 21:36:49.228] [INFO ] [transmit] [T:645] Packet length 680
[2026-06-21 21:36:49.229] [INFO ] [transmit] [T:645] Enable FD Num: 6
[2026-06-21 21:36:51.175] [DEBUG] [wdt     ] [T:644] Feed softdog ID=1
[2026-06-21 21:36:51.176] [INFO ] [sensor  ] [T:644] thread of sensor feed dog success
[2026-06-21 21:36:51.176] [INFO ] [sensor  ] [T:644] Start reading sensor 3 data
[03][03][00][01][00][02][94][29]
Sending request using RTS signal
Waiting for a confirmation...
ERROR Connection timed out: select
[2026-06-21 21:36:51.688] [ERROR] [sensor  ] [T:644] Connection timed out
[2026-06-21 21:36:51.688] [WARN ] [sensor  ] [T:644] Read sensor 3 data failed
[2026-06-21 21:36:55.689] [DEBUG] [wdt     ] [T:644] Feed softdog ID=1
[2026-06-21 21:36:55.689] [INFO ] [sensor  ] [T:644] thread of sensor feed dog success
[2026-06-21 21:36:55.689] [INFO ] [sensor  ] [T:644] Start reading sensor 4 data
[04][03][00][01][00][02][95][9E]
Sending request using RTS signal
Waiting for a confirmation...
ERROR Connection timed out: select
[2026-06-21 21:36:56.202] [ERROR] [sensor  ] [T:644] Connection timed out
[2026-06-21 21:36:56.202] [WARN ] [sensor  ] [T:644] Read sensor 4 data failed
[2026-06-21 21:36:57.226] [INFO ] [transmit] [T:645] Message From The Gateway: $03100000100001A010001SAP24000000000000001@
[2026-06-21 21:36:57.226] [INFO ] [transmit] [T:645] Message counter: 15
[2026-06-21 21:36:57.226] [DEBUG] [wdt     ] [T:645] Feed softdog ID=0
[2026-06-21 21:36:57.226] [INFO ] [transmit] [T:645] thread of trans_message feed dog success
[2026-06-21 21:36:57.227] [INFO ] [transmit] [T:645] Enable FD Num: 6
[2026-06-21 21:37:00.202] [DEBUG] [wdt     ] [T:644] Feed softdog ID=1
[2026-06-21 21:37:00.202] [INFO ] [sensor  ] [T:644] thread of sensor feed dog success
[2026-06-21 21:37:00.203] [INFO ] [sensor  ] [T:644] Start reading sensor 5 data
[05][03][26][00][00][04][4E][C5]
Sending request using RTS signal
Waiting for a confirmation...
ERROR Connection timed out: select
[2026-06-21 21:37:00.715] [ERROR] [sensor  ] [T:644] Connection timed out
[2026-06-21 21:37:00.715] [WARN ] [sensor  ] [T:644] Read sensor 5 data failed
[2026-06-21 21:37:04.715] [DEBUG] [wdt     ] [T:644] Feed softdog ID=1
[2026-06-21 21:37:04.716] [INFO ] [sensor  ] [T:644] thread of sensor feed dog success
[2026-06-21 21:37:04.716] [INFO ] [sensor  ] [T:644] Start reading sensor 6 data
[06][03][26][00][00][04][4E][F6]
Sending request using RTS signal
Waiting for a confirmation...
[2026-06-21 21:37:05.226] [INFO ] [transmit] [T:645] Message From The Gateway: $03100000100001A010001SAP24000000000000001@
[2026-06-21 21:37:05.226] [INFO ] [transmit] [T:645] Message counter: 16
[2026-06-21 21:37:05.227] [DEBUG] [wdt     ] [T:645] Feed softdog ID=0
[2026-06-21 21:37:05.227] [INFO ] [transmit] [T:645] thread of trans_message feed dog success
[2026-06-21 21:37:05.227] [INFO ] [transmit] [T:645] Enable FD Num: 6
ERROR Connection timed out: select
[2026-06-21 21:37:05.228] [ERROR] [sensor  ] [T:644] Connection timed out
[2026-06-21 21:37:05.228] [WARN ] [sensor  ] [T:644] Read sensor 6 data failed
[2026-06-21 21:37:09.229] [DEBUG] [wdt     ] [T:644] Feed softdog ID=1
[2026-06-21 21:37:09.229] [INFO ] [sensor  ] [T:644] thread of sensor feed dog success
[2026-06-21 21:37:09.229] [INFO ] [sensor  ] [T:644] Start reading sensor 7 data
[07][03][00][00][00][02][C4][6D]
Sending request using RTS signal
Waiting for a confirmation...
ERROR Connection timed out: select
[2026-06-21 21:37:09.741] [ERROR] [sensor  ] [T:644] Connection timed out
[2026-06-21 21:37:09.742] [WARN ] [sensor  ] [T:644] Read sensor 7 data failed
[2026-06-21 21:37:13.226] [INFO ] [transmit] [T:645] Message From The Gateway: $03100000100001A010001SAP24000000000000001@
[2026-06-21 21:37:13.227] [INFO ] [transmit] [T:645] Message counter: 17
[2026-06-21 21:37:13.227] [DEBUG] [wdt     ] [T:645] Feed softdog ID=0
[2026-06-21 21:37:13.227] [INFO ] [transmit] [T:645] thread of trans_message feed dog success
[2026-06-21 21:37:13.227] [INFO ] [transmit] [T:645] Enable FD Num: 6
[2026-06-21 21:37:13.742] [DEBUG] [wdt     ] [T:644] Feed softdog ID=1
[2026-06-21 21:37:13.742] [INFO ] [sensor  ] [T:644] thread of sensor feed dog success
[2026-06-21 21:37:13.742] [INFO ] [sensor  ] [T:644] Start reading sensor 8 data
[07][03][00][06][00][02][24][6C]
Sending request using RTS signal
Waiting for a confirmation...
ERROR Connection timed out: select
[2026-06-21 21:37:14.255] [ERROR] [sensor  ] [T:644] Connection timed out
[2026-06-21 21:37:14.255] [WARN ] [sensor  ] [T:644] Read sensor 8 data failed
[2026-06-21 21:37:18.255] [DEBUG] [wdt     ] [T:644] Feed softdog ID=1
[2026-06-21 21:37:18.256] [INFO ] [sensor  ] [T:644] thread of sensor feed dog success
[2026-06-21 21:37:18.256] [INFO ] [sensor  ] [T:644] Start reading sensor 9 data
[08][03][00][00][00][01][84][93]
Sending request using RTS signal
Waiting for a confirmation...
ERROR Connection timed out: select
[2026-06-21 21:37:18.768] [ERROR] [sensor  ] [T:644] Connection timed out
[2026-06-21 21:37:18.768] [WARN ] [sensor  ] [T:644] Read sensor 9 data failed
[2026-06-21 21:37:21.226] [INFO ] [transmit] [T:645] Message From The Gateway: $03100000100001A010001SAP24000000000000001@
[2026-06-21 21:37:21.227] [INFO ] [transmit] [T:645] Message counter: 18
[2026-06-21 21:37:21.227] [DEBUG] [wdt     ] [T:645] Feed softdog ID=0
[2026-06-21 21:37:21.227] [INFO ] [transmit] [T:645] thread of trans_message feed dog success
[2026-06-21 21:37:21.227] [INFO ] [transmit] [T:645] Enable FD Num: 6
[2026-06-21 21:37:22.768] [DEBUG] [wdt     ] [T:644] Feed softdog ID=1
[2026-06-21 21:37:22.769] [INFO ] [sensor  ] [T:644] thread of sensor feed dog success
[2026-06-21 21:37:22.769] [INFO ] [sensor  ] [T:644] Start reading sensor 10 data
[08][03][00][01][00][01][D5][53]
Sending request using RTS signal
Waiting for a confirmation...
ERROR Connection timed out: select
[2026-06-21 21:37:23.281] [ERROR] [sensor  ] [T:644] Connection timed out
[2026-06-21 21:37:23.281] [WARN ] [sensor  ] [T:644] Read sensor 10 data failed
[2026-06-21 21:37:27.282] [DEBUG] [wdt     ] [T:644] Feed softdog ID=1
[2026-06-21 21:37:27.282] [INFO ] [sensor  ] [T:644] thread of sensor feed dog success
[2026-06-21 21:37:27.282] [INFO ] [sensor  ] [T:644] Start reading sensor 11 data
[08][03][00][02][00][01][25][53]
Sending request using RTS signal
Waiting for a confirmation...
ERROR Connection timed out: select
[2026-06-21 21:37:27.794] [ERROR] [sensor  ] [T:644] Connection timed out
[2026-06-21 21:37:27.795] [WARN ] [sensor  ] [T:644] Read sensor 11 data failed
[2026-06-21 21:37:29.226] [INFO ] [transmit] [T:645] Message From The Gateway: $03100000100001A010001SAP24000000000000001@
[2026-06-21 21:37:29.227] [INFO ] [transmit] [T:645] Message counter: 19
[2026-06-21 21:37:29.227] [DEBUG] [wdt     ] [T:645] Feed softdog ID=0
[2026-06-21 21:37:29.227] [INFO ] [transmit] [T:645] thread of trans_message feed dog success
[2026-06-21 21:37:29.227] [INFO ] [transmit] [T:645] Enable FD Num: 6
[2026-06-21 21:37:31.795] [DEBUG] [wdt     ] [T:644] Feed softdog ID=1
[2026-06-21 21:37:31.795] [INFO ] [sensor  ] [T:644] thread of sensor feed dog success
[2026-06-21 21:37:31.795] [INFO ] [sensor  ] [T:644] Start reading sensor 12 data
[08][03][00][03][00][02][34][92]
Sending request using RTS signal
Waiting for a confirmation...
ERROR Connection timed out: select
[2026-06-21 21:37:32.308] [ERROR] [sensor  ] [T:644] Connection timed out
[2026-06-21 21:37:32.308] [WARN ] [sensor  ] [T:644] Read sensor 12 data failed
[2026-06-21 21:37:36.308] [DEBUG] [wdt     ] [T:644] Feed softdog ID=1
[2026-06-21 21:37:36.309] [INFO ] [sensor  ] [T:644] thread of sensor feed dog success
[2026-06-21 21:37:36.309] [INFO ] [sensor  ] [T:644] Start reading sensor 13 data
[09][03][00][00][00][01][85][42]
Sending request using RTS signal
Waiting for a confirmation...
ERROR Connection timed out: select
[2026-06-21 21:37:36.821] [ERROR] [sensor  ] [T:644] Connection timed out
[2026-06-21 21:37:36.821] [WARN ] [sensor  ] [T:644] Read sensor 13 data failed
[2026-06-21 21:37:37.226] [INFO ] [transmit] [T:645] Message From The Gateway: $03100000100001A010001SAP24000000000000001@
[2026-06-21 21:37:37.227] [INFO ] [transmit] [T:645] Message counter: 20
[2026-06-21 21:37:37.227] [DEBUG] [wdt     ] [T:645] Feed softdog ID=0
[2026-06-21 21:37:37.227] [INFO ] [transmit] [T:645] thread of trans_message feed dog success
[2026-06-21 21:37:37.228] [INFO ] [transmit] [T:645] Enable FD Num: 6
[2026-06-21 21:37:40.821] [DEBUG] [wdt     ] [T:644] Feed softdog ID=1
[2026-06-21 21:37:40.822] [INFO ] [sensor  ] [T:644] thread of sensor feed dog success
[2026-06-21 21:37:40.822] [INFO ] [sensor  ] [T:644] Start reading sensor 14 data
[0A][04][00][00][00][02][70][B0]
Sending request using RTS signal
Waiting for a confirmation...
ERROR Connection timed out: select
[2026-06-21 21:37:41.334] [ERROR] [sensor  ] [T:644] Connection timed out
[2026-06-21 21:37:41.334] [WARN ] [sensor  ] [T:644] Read sensor 14 data failed
[2026-06-21 21:37:45.226] [INFO ] [transmit] [T:645] Message From The Gateway: $03100000100001A010001SAP24000000000000001@
[2026-06-21 21:37:45.227] [INFO ] [transmit] [T:645] Message counter: 21
[2026-06-21 21:37:45.227] [DEBUG] [wdt     ] [T:645] Feed softdog ID=0
[2026-06-21 21:37:45.227] [INFO ] [transmit] [T:645] thread of trans_message feed dog success
[2026-06-21 21:37:45.228] [INFO ] [transmit] [T:645] Enable FD Num: 6
[2026-06-21 21:37:45.335] [DEBUG] [wdt     ] [T:644] Feed softdog ID=1
[2026-06-21 21:37:45.335] [INFO ] [sensor  ] [T:644] thread of sensor feed dog success
[2026-06-21 21:37:45.335] [INFO ] [sensor  ] [T:644] Start reading sensor 15 data
[0B][03][00][00][00][01][84][A0]
Sending request using RTS signal
Waiting for a confirmation...
ERROR Connection timed out: select
[2026-06-21 21:37:45.847] [ERROR] [sensor  ] [T:644] Connection timed out
[2026-06-21 21:37:45.847] [WARN ] [sensor  ] [T:644] Read sensor 15 data failed
[2026-06-21 21:37:49.848] [DEBUG] [wdt     ] [T:644] Feed softdog ID=1
[2026-06-21 21:37:49.848] [INFO ] [sensor  ] [T:644] thread of sensor feed dog success
[2026-06-21 21:37:49.848] [INFO ] [sensor  ] [T:644] Start reading sensor 16 data
[0C][03][00][00][00][01][85][17]
Sending request using RTS signal
Waiting for a confirmation...
ERROR Connection timed out: select
[2026-06-21 21:37:50.360] [ERROR] [sensor  ] [T:644] Connection timed out
[2026-06-21 21:37:50.361] [WARN ] [sensor  ] [T:644] Read sensor 16 data failed
[2026-06-21 21:37:53.227] [INFO ] [transmit] [T:645] Message From The Gateway: $03100000100001A010001SAP24000000000000001@
[2026-06-21 21:37:53.227] [INFO ] [transmit] [T:645] Message counter: 22
[2026-06-21 21:37:53.227] [DEBUG] [wdt     ] [T:645] Feed softdog ID=0
[2026-06-21 21:37:53.227] [INFO ] [transmit] [T:645] thread of trans_message feed dog success
[2026-06-21 21:37:53.228] [INFO ] [transmit] [T:645] Enable FD Num: 6
[2026-06-21 21:37:54.361] [DEBUG] [wdt     ] [T:644] Feed softdog ID=1
[2026-06-21 21:37:54.361] [INFO ] [sensor  ] [T:644] thread of sensor feed dog success
[2026-06-21 21:37:54.361] [INFO ] [sensor  ] [T:644] Start reading sensor 17 data
[0C][03][00][01][00][01][D4][D7]
Sending request using RTS signal
Waiting for a confirmation...
ERROR Connection timed out: select
[2026-06-21 21:37:54.874] [ERROR] [sensor  ] [T:644] Connection timed out
[2026-06-21 21:37:54.874] [WARN ] [sensor  ] [T:644] Read sensor 17 data failed
[2026-06-21 21:37:58.874] [DEBUG] [wdt     ] [T:644] Feed softdog ID=1
[2026-06-21 21:37:58.874] [INFO ] [sensor  ] [T:644] thread of sensor feed dog success
[2026-06-21 21:37:58.875] [INFO ] [sensor  ] [T:644] Start reading sensor 18 data
[0C][03][00][02][00][01][24][D7]
Sending request using RTS signal
Waiting for a confirmation...
ERROR Connection timed out: select
[2026-06-21 21:37:59.387] [ERROR] [sensor  ] [T:644] Connection timed out
[2026-06-21 21:37:59.387] [WARN ] [sensor  ] [T:644] Read sensor 18 data failed
