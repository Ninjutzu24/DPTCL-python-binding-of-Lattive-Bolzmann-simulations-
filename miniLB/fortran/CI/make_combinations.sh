#	
# setup
#
date > Make_OK.txt
date > Make_KO.txt
#
# loop over targets
for target in `cat target.list`; do 
	# loop over compilers
	for comp in `cat compiler.list`; do 
		# loop over implementations
		for imp in `cat implementation.list`; do 
			# signal (here I'm)
                        echo "testing " $target $comp $imp
                	# loop over precision
		        for prec in `cat precision.list`; do 
                        	# loop over testcase.list
				for test in `cat testcase.list`; do 
					echo $comp $target $imp $prec $test
					./try.sh $target $comp $imp $prec $test
				done
			done
		done
	done
done

