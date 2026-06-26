echo "VK stress for different paradigm"
echo "------------------------------"
echo "run #1 of 3: Do concurrent    "
sleep 5
./run.single.gpu.fused.dc.nv.x
echo "------------------------------"
echo "run #2 of 3: openACC          "
sleep 5
./run.single.gpu.fused.openacc.nv.x
echo "------------------------------"
echo "run #3 of 3: OMP offload      "
sleep 5 
./run.single.gpu.fused.offload.nv.x
echo "------------------------------"
echo "to plot: use gnuplot and load 'plot.gnu'"

