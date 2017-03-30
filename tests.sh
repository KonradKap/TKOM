#!/bin/bash

for dir in res/*
do
    datafile=(`find $dir -name "*.kappa"`)
    if [ ${#datafile[@]} -ne 1 ]
    then
        echo "There are more than one .kappa file in $dir"
        continue
    fi
    datafile=${datafile[0]}
    for binfile in `find $dir -name "*.bin"`
    do
        expected_file="${binfile%.*}"".exp"
        if [ ! -f $expected_file ]
        then
            echo "There wasn't $expected_file for corresponding $binfile"
            continue
        fi
        received_output=(`./kappalang -b $binfile -d $datafile -t`)
        expected_output=(`cat $expected_file`)

        if [ ${#received_output[@]} -ne ${#expected_output[@]} ]
        then
            echo "Test in $dir for $(basename $binfile) failed"
            echo "Got ${received_output[@]}"
            echo "Expected ${expected_output[@]}"
            echo "Lengths are not equal"
            continue
        fi
        passed=1
        for i in `seq 0 ${#received_output[@]}`
        do
            if [ "${received_output[$i]}" != "${expected_output[$i]}" ]
            then
                echo "Test in $dir for $(basename $binfile) failed"
                echo "Got ${received_output[$i]}"
                echo "Expected ${expected_output[$i]}"
                echo "Mismatch at position $i"
                passed=0
                break
            fi
        done
        if [ $passed -eq 1 ]
        then
            echo "Test in $dir for $(basename $binfile) passed"
        fi
    done
done
