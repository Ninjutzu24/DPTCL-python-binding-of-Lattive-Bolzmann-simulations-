#	
# loop over targets
for target in `cat target.list`; do 
	# loop over compilers
	for comp in `cat compiler.list`; do 
		# loop over implementations
		for imp in `cat implementation.list`; do 
                	# loop over precision
		        for prec in `cat precision.list`; do 
                        	# loop over testcase.list
				for test in `cat testcase.list`; do 
					echo "make clean; make " $comp $target $imp $prec $test
				done
			done
		done
	done
done

