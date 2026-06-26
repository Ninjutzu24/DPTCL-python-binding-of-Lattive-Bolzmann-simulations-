echo "TG for different precision"
echo "------------------------------"
echo "run #1 of 4: mixed (half/single) precision "
sleep 5
./run.mixed1.gpu.fused.dc.nv.x
echo "------------------------------"
echo "run #2 of 4: single precision "
sleep 5
./run.single.gpu.fused.dc.nv.x
echo "------------------------------"
echo "run #3 of 4: mixed (single/double) precision "
sleep 5 
./run.mixed2.gpu.fused.dc.nv.x
echo "------------------------------"
echo "run #4 of 4: double precision "
sleep 5 
./run.double.gpu.fused.dc.nv.x
echo "------------------------------"
echo "to plot: use gnuplot and load 'plot.gnu'"

