set terminal png size 1200,600
set output 'output/chart_lv_all.png'
set title 'Top 10 and Bottom 10 Stations by Difference'
set boxwidth 0.5 relative
set style fill solid border -1
set ylabel 'Difference (kW)'
set xlabel 'Station ID'
set xtics rotate by -45
set yrange [*:*]
set grid ytics
set datafile separator ":"
plot 'output/top10_lv_all.csv' using 4:xtic(1) with boxes title 'Overload (Top 10)' lc rgb 'red',\
     'output/bottom10_lv_all.csv' using 4:xtic(1) with boxes title 'Underutilized (Bottom 10)' lc rgb 'green'
