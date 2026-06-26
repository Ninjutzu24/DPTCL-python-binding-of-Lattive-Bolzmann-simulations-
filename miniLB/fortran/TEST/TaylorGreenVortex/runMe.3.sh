echo "TG for different precision"
echo "------------------------------"
echo "run #1 of 3: single precision "
sleep 5
./run.single.serial.fused.gnu.x
echo "------------------------------"
echo "run #2 of 3: mixed (single/double) precision "
sleep 5 
./run.mixed2.serial.fused.gnu.x
echo "------------------------------"
echo "run #3 of 3: double precision "
sleep 5 
./run.double.serial.fused.gnu.x
echo "------------------------------"
echo "to plot: use gnuplot and load 'plot.small.gnu'"

