######################################
# 
# Modul:  create-test.sh
#
# Author: Michal Zapletal
# Email:  xzaple41@stud.fit.vutbr.cz
#
######################################

#!/bin/bash

echo -n "Your script name:"
read name
echo -n "Destination [udp/tcp]:"
read mode
touch ./$mode/$name.in
touch ./$mode/$name.out
touch ./$mode/$name.arg
touch ./$mode/$name.errcode
if [ $mode = "udp" ]; then
    echo -n "-m udp" > ./$mode/$name.arg
fi