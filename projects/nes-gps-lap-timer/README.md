# Nes' GPS Lap Timer

Uploaded by neils7 on 2014-01-21 01:12:00 (rating 0 out of 5)

## Summary

A lap timer with speedometer and recent history display. Input: NMEA strings spat out by a generic GPS receiver. Output: Drives a standard 20x4 character LCD (4 bit interface). Dumps recorded data in csv format to a host computer via USB. Operation: Switch on the device, allowing sufficient time for the GPS receiver to obtain a lock. Position the device at a convenient point on your circuit and press a button. The current location is stored. The device from then on displays a live update of current speed and time since leaving the way point. Once the waypoint is reached again, the time taken is recorded in NV memory. Up to 100 lap times can be recorded before the memory is full. The most recent three recorded lap times are displayed. Once the session is finished, switch off the power. Connect the device to a host computer via USB and switch it on. It will enumerate itself as a keyboard. Press the button and it will 'type' in annotated csv format all the stored data. This may be 'echoed' to a file on bsd/linux systems or saved in Notepad on Windows systems. This process can be repeated as required. Features: interpolates between successive GPS position strings to achieve a tenth of a second precision in the lap times. Example of annotated csv output: Top speed, 30.40,km/h, at, 1144:08, lap, 27 Total distance, 26.961,km Total run time, 93:02.0 Total rest time, 56:18.0 LAP , SPLIT , TIME Start, , 0942:39 1, 33:07.36, 1015:47 2, 2:32.01, 1018:19 3, 2:26.20, 1020:45 4, 2:04.47, 1022:49 5, 2:41.28, 1025:31 6, 2:40.76, 1028:11 7, 2:37.21, 1030:49 8, 2:01.44, 1032:50 9, 2:26.47, 1035:17 10, 2:34.25, 1037:51 11, 2:25.06, 1040:16 12, 2:19.86, 1042:36 13, 2:45.15, 1045:21 14, 2:36.44, 1047:57 15, 8:31.17, 1056:28 16, 10:05.92, 1106:34 17, 2:53.33, 1109:28 18, 3:02.65, 1112:30 19, 4:02.50, 1116:33 20, 2:55.85, 1119:29 21, 3:54.45, 1123:23 22, 4:11.76, 1127:35 23, 2:57.02, 1130:32 24, 2:57.04, 1133:29 25, 3:13.60, 1136:43 26, 6:28.39, 1143:11 27, 3:23.46, 1146:35 28, 7:20.21, 1153:55 29, 4:16.87, 1158:12 30, 2:21.15, 1200:33 31, 2:48.28, 1203:21 32, 2:36.10, 1205:57 33, 2:55.21, 1208:52 34, 3:28.21, 1212:21

## Compilers

- WinAVR (GNU GCC)

## Tags

- Complete code with HW
- WinAVR (GNU GCC)
