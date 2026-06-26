echo "Lid Driven cavity at Re=10000 for different paradigms"
echo "------------------------------"
echo "run #1 of 3: do concurrrent "
sleep 5
./run.single.doconcurrent.fused.x NVIDIA=1
echo "------------------------------"
echo "run #2 of 3: openacc"
sleep 5 
./run.single.openacc.fused.x NVIDIA=1
echo "------------------------------"
echo "run #3 of 3: offload"
sleep 5 
./run.single.offload.fused.x NVIDIA=1
echo "------------------------------"

