#clean
export numTest=`grep -v CEST Make_OK.txt | wc -l`
grep -v CEST Make_OK.txt > test.lst
echo $numTest
for test in `seq 1 $numTest`; do
	opt=`head -n $test test.lst | tail -n 1`
	echo $test $opt
	cd ../SRC
	make clean
	make $opt VER=try
	cd -
	cp ../RUN/bgk2d.try.x .
	./bgk2d.try.x > output.$test.log
	mv diagno.dat diagno.$test.dat
done
