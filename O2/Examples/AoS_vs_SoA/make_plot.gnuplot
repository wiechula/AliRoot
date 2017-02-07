set terminal pngcairo size 900,900 enhanced font 'Verdana,16'
set output 'bandwidth.png';
set logscale x
set xrange [*:1.2]
set grid
set ylabel "Effetcive Bandwidth (GB/s)";
set xlabel "Odds of particle passing the cut"
plot for [IDX=0:2] data i IDX u 1:5 w lp lw 2 t columnheader(1)
unset output;
