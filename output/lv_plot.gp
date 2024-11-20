set terminal png size 1200,600
set output "output/lv_chart.png"
set title "Top 10 and Bottom 10 LV Stations by Load"
set boxwidth 0.5 relative
set style fill solid
set ylabel "Load (kW)"
set xlabel "Station ID"
set xtics rotate by -45
plot "output/lv_top10.dat" using 0:3:2:3 with boxes lc rgb "red" title "Top 10",      "output/lv_bottom10.dat" using 0:3:2:3 with boxes lc rgb "green" title "Bottom 10"
