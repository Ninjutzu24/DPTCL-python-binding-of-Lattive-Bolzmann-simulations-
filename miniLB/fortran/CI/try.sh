# 
# target, compiler, implmentation, precision, testcase
#echo $1 $2 $3 $4 $5
export EXE="../RUN/bgk2d.$1.x"
#echo $EXE
cd ../SRC
pwd
#
make clean          >  temp.$1.$2.$3.$4.$5
make $1 $2 $3 $4 $5 >> temp.$1.$2.$3.$4.$5
#
cd -
#
if [ -f "$EXE" ]; then  
#   Compilation succesfull" $EXE
    rm -rf $EXE
    echo $1 $2 $3 $4 $5 >> Make_OK.txt 
else
#    echo "Compilattion failed"
    echo $1 $2 $3 $4 $5 >> Make_KO.txt 
fi
