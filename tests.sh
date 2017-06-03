#!/bin/bash

GREEN='\033[0;32m'
RED='\033[0;31m'
NC='\033[0m'

IGNORED=(res/cant_be_empty res/recursive_type res/typename_redeclaration)

make &&
for dir in res/*
do
    datafile=(`find $dir -name "*.kappa"`)
    if [ ${#datafile[@]} -ne 1 ]
    then
        echo -e "${RED}There is more than one .kappa file in $dir${NC}"
        continue
    fi
    datafile=${datafile[0]}
    for binfile in `find $dir -name "*.bin"`
    do
        expected_file="${binfile%.*}"".exp"
        if [ ! -f $expected_file ]
        then
            echo -e "${RED}There wasn't $expected_file for corresponding `basename $binfile`${NC}"
            continue
        fi
        received_output=(`./kappalang -b $binfile -d $datafile -t`)
        expected_output=(`cat $expected_file`)

        if [ ${#received_output[@]} -ne ${#expected_output[@]} ]
        then
            echo -e "${RED}Test `basename $dir` for `basename $binfile` failed${NC}"
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
                echo -e "${RED}Test `basename $dir` for `basename $binfile` failed${NC}"
                echo "Got ${received_output[@]}"
                echo "Expected ${expected_output[@]}"
                echo "Mismatch at position $i"
                passed=0
                break
            fi
        done
        if [ $passed -eq 1 ]
        then
            echo -e "${GREEN}Test `basename $dir` for `basename $binfile` passed${NC}"
        fi
    done
done
