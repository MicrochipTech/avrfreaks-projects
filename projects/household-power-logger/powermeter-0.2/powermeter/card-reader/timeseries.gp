set xdata time
set timefmt '%b/%d/%H:%M:%S'
set format x '%H:%M'
set ylabel 'Watts'
set key off
p 'zz' u 1:4 w l
