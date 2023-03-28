######################################
# 
# Modul:  run-tests.sh
#
# Author: Michal Zapletal
# Email:  xzaple41@stud.fit.vutbr.cz
#
######################################

#!/bin/bash

pushd ..

while getopts p:h:m: flag
do
    case "${flag}" in
        h) ip=${OPTARG};;
        p) port=${OPTARG};;
        m) mode=${OPTARG};;
    esac
done

if ! [ "$ip" = "" ]; then
    arg=$arg"-h "$ip
fi
if ! [ "$port" = "" ]; then
    arg=$arg" -p "$port
fi
if [ "$mode" = "" ]; then
    mode="tcp"
fi
echo "[START] Tests from \"$mode\" folder"

test_ok=0
test_not_ok=0

DIR=`ls ./tests/$mode/*`
#echo $DIR
for file in $DIR;
do
    files=$(exec ls)
    case "$files" in
        *output.txt*) rm output.txt;
    esac
    
    regex=".*\.in"
    if ! [[ $file =~ $regex ]]; then
        continue
    fi
    IFS='/'
    read -ra strarr <<< $file
    IFS='.'
    read -ra strarr <<< ${strarr[3]}
    unset IFS
    var=`cat ./tests/${mode}/${strarr[0]}.arg`
    final_arg=$arg" "$var
    if ! [ "$mode" = "udp" ]; then
        timeout 1 ./ipkcpc $final_arg < $file > output.txt
    else
        timeout 0.5s sleep 0.5s ./ipkcpc $final_arg < $file > output.txt &
        PID=$!
        kill -SIGINT $PID
    fi
    exit_code=$?
    expected_exit_code=`cat ./tests/${mode}/${strarr[0]}.errcode`
    echo "[TEST] \"${strarr[0]}\" [ARG] \"${final_arg}\""
    if test -f output.txt; then
        var=`diff ./output.txt ./tests/${mode}/${strarr[0]}.out`
        if ! [ "$var" = "" ]; then
            echo "[OUT] Expected output:"
            cat ./tests/${mode}/${strarr[0]}.out
            echo "[OUT] Got output:"
            cat output.txt
            test_not_ok=$((test_not_ok+1))
            continue
        fi
    else
        if [ $exit_code != $expected_exit_code ]; then
            echo "[ERR] File \"output.txt\" was not found"
            test_not_ok=$((test_not_ok+1))
            continue
        fi
    fi
    if [ $exit_code != $expected_exit_code ]
    then
        echo -e "[ERR] Exit code $exit_code, expected: $expected_exit_code"
        test_not_ok=$((test_not_ok+1))
        continue
    fi
    test_ok=$((test_ok+1))
done

echo "[END] $test_ok tests passed, $test_not_ok failed"