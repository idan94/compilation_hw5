#!/bin/bash

numtests=`ls hw5_tests/ | wc -w`
dev_by=2
numtests=`echo $((numtests / dev_by))`
echo "Running $numtests tests"

tmpdir="running_tests_temp"
mkdir "$tmpdir" &> /dev/null
cp ./hw5 ./$tmpdir
cp -r ./hw5_tests ./$tmpdir
cd "$tmpdir"

i="1"
while [ $i -le $numtests ]
	do
		./hw5 < hw5_tests/t$i.in 2>&1 > t$i.ll
		lli t$i.ll > t$i.res
		diff t$i.res hw5_tests/t$i.out &> /dev/null
		if [[ $? != 0 ]] 
			then
				echo "Failed test #"$i"!"
				exit
		fi
		i=$[$i+1]
done

cd - &> /dev/null
echo "All tests passed :)   hw5_tests"
rm -rf "$tmpdir"

numtests=`ls hw5_all_tests/ | wc -w`
dev_by=2
numtests=`echo $((numtests / dev_by))`
echo "Running $numtests tests"

tmpdir="running_tests_temp"
mkdir "$tmpdir" &> /dev/null
cp ./hw5 ./$tmpdir
cp -r ./hw5_all_tests ./$tmpdir
cd "$tmpdir"

i="1"
while [ $i -le $numtests ]
	do
		./hw5 < hw5_all_tests/t$i.in 2>&1 > t$i.ll
		lli t$i.ll > t$i.res
		diff t$i.res hw5_all_tests/t$i.out &> /dev/null
		if [[ $? != 0 ]] 
			then
				echo "Failed test #"$i"!"
				exit
		fi
     echo "Passed test #"$i"!"
		i=$[$i+1]
done

cd - &> /dev/null
echo "All tests passed :)   hw5_all_tests"
rm -rf "$tmpdir"