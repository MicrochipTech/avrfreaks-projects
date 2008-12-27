#!/usr/bin/rrdcgi
<HTML>
<HEAD>
<META http-equiv="refresh" content="10">
<TITLE>Temperature</TITLE>
</HEAD>
<BODY>
<h1>Temperature</h1>
<table>
<tr><td>
<RRD::GRAPH 
	chart_300.png
	--imginfo '<IMG SRC=%s WIDTH=%lu HEIGHT=%lu>'
	--lazy
	--start -300
	--title "Last 5 Minutes"
	-u 90 -l 65 -r
	DEF:room_max=temperature.rrd:temperature:MAX
	AREA:room_max#0000FF
	DEF:room_min=temperature.rrd:temperature:MIN
	AREA:room_min#FFFFFF
	DEF:room_avg=temperature.rrd:temperature:AVERAGE
	LINE1:room_avg#FF0000
>
</td><td>
<RRD::GRAPH 
	chart_hour.png
	--imginfo '<IMG SRC=%s WIDTH=%lu HEIGHT=%lu>'
	--lazy
	--start -3600
	--title "Last Hour"
	-u 90 -l 65 -r
	DEF:room_max=temperature.rrd:temperature:MAX
	AREA:room_max#0000FF
	DEF:room_min=temperature.rrd:temperature:MIN
	AREA:room_min#FFFFFF
	DEF:room_avg=temperature.rrd:temperature:AVERAGE
	LINE1:room_avg#FF0000
>
</td></tr>
<tr><td>
<RRD::GRAPH 
	chart_day.png
	--imginfo '<IMG SRC=%s WIDTH=%lu HEIGHT=%lu>'
	--lazy
	--start -86400
	--title "Last Day"
	-u 90 -l 65 -r
	DEF:room_max=temperature.rrd:temperature:MAX
	AREA:room_max#0000FF
	DEF:room_min=temperature.rrd:temperature:MIN
	AREA:room_min#FFFFFF
	DEF:room_avg=temperature.rrd:temperature:AVERAGE
	LINE1:room_avg#FF0000
>
</td><td>
<RRD::GRAPH 
	chart_week.png
	--imginfo '<IMG SRC=%s WIDTH=%lu HEIGHT=%lu>'
	--lazy
	--start -604800
	--title "Last Week"
	-u 90 -l 65 -r
	DEF:room_max=temperature.rrd:temperature:MAX
	AREA:room_max#0000FF
	DEF:room_min=temperature.rrd:temperature:MIN
	AREA:room_min#FFFFFF
	DEF:room_avg=temperature.rrd:temperature:AVERAGE
	LINE1:room_avg#FF0000
>
</td></tr>
<tr><td>
<RRD::GRAPH 
	chart_month.png
	--imginfo '<IMG SRC=%s WIDTH=%lu HEIGHT=%lu>'
	--lazy
	--start -2678400
	--title "Last Month"
	-u 90 -l 65 -r
	DEF:room_max=temperature.rrd:temperature:MAX
	AREA:room_max#0000FF
	DEF:room_min=temperature.rrd:temperature:MIN
	AREA:room_min#FFFFFF
	DEF:room_avg=temperature.rrd:temperature:AVERAGE
	LINE1:room_avg#FF0000
>
</td><td>
<RRD::GRAPH 
	chart_year.png
	--imginfo '<IMG SRC=%s WIDTH=%lu HEIGHT=%lu>'
	--lazy
	--start -31622400
	--title "Last Year"
	-u 90 -l 65 -r
	DEF:room_max=temperature.rrd:temperature:MAX
	AREA:room_max#0000FF
	DEF:room_min=temperature.rrd:temperature:MIN
	AREA:room_min#FFFFFF
	DEF:room_avg=temperature.rrd:temperature:AVERAGE
	LINE1:room_avg#FF0000
>
</td></tr>
</table>
</BODY>
</HTML>
