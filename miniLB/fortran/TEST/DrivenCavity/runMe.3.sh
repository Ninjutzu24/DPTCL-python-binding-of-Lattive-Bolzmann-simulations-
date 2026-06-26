echo "Lid Driven cavity at Re=100 for different precision"
echo "------------------------------"
echo "run #1 of 2: single precision "
sleep 5
./run.single.serial.original.gnu.x
echo "------------------------------"
echo "run #2 of 2: double precision "
sleep 5 
./run.double.serial.fused.gnu.x
echo "------------------------------"
echo "to plot: use gnuplot and load 'plot.R00100.gnu'"

