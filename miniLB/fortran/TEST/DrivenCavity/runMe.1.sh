echo "Lid Driven cavity at Re=10000 for different paradgms"
echo "------------------------------"
echo "run #1 of 3: do concurrrent "
sleep 5
./run.single.gpu.fused.dc.nv.x
echo "------------------------------"
echo "run #2 of 3: openacc"
sleep 5 
./run.single.gpu.fused.openacc.nv.x
echo "------------------------------"
echo "run #3 of 3: offload"
sleep 5 
./run.single.gpu.fused.offload.nv.x
echo "------------------------------"
echo "to plot: use gnuplot and load 'plot.R10000.gnu'"

