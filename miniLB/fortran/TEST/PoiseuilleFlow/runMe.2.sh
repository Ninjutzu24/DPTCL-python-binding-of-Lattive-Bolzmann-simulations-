echo "Poiseuille Flow with different implementations"
echo "------------------------------"
echo "run #1 of 2: fused (CPU)"
sleep 5
./run.single.serial.fused.gnu.x
echo "------------------------------"
echo "run #2 of 2: orignal (CPU)"
sleep 5
./run.single.serial.original.gnu.x
echo "------------------------------"
echo "to plot: use gnuplot and load 'plot.small.gnu'"

