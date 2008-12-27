#!/bin/sh
rrdtool graph minute.png --start end-60s DEF:linea=temperature.rrd:temperature:AVERAGE LINE1:linea#FF0000
rrdtool graph hour.png --start end-3600s DEF:linea=temperature.rrd:temperature:AVERAGE LINE1:linea#FF0000
