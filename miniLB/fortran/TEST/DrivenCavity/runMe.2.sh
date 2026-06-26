echo "Lid Driven cavity at Re=1000 for different compilers"
echo "------------------------------"
echo "run #1 of 3: nvidia "
sleep 5
./run.single.multicore.fused.nv.x
echo "------------------------------"
echo "run #2 of 3: intel"
sleep 5 
./run.single.multicore.fused.intel.x
echo "------------------------------"
echo "run #3 of 3: gnu"
sleep 5 
#./run.single.multicore.fused.gnu.x
echo "------------------------------"
echo "to plot: use gnuplot and load 'plot.R01000.gnu'"

