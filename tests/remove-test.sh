######################################
# 
# Modul:  remove-test.sh
#
# Author: Michal Zapletal
# Email:  xzaple41@stud.fit.vutbr.cz
#
######################################

#!/bin/bash

echo -n "Name of the test you want to delete:"
read name
echo -n "From folder TCP or UDP ? [tcp/udp]:"
read mode
echo -n "Do you really want to delete test \"$name\"? [y/n]"
read opt
if [ "$opt" = "y" ] || [ "$opt" = "Y" ]; then
    file=../tests/$mode/$name.in
    if [[ -f "$file" ]]; then
        rm $file
    fi
    file=../tests/$mode/$name.arg
    if [[ -f "$file" ]]; then
        rm $file
    fi
    file=../tests/$mode/$name.out
    if [[ -f "$file" ]]; then
        rm $file
    fi
    file=../tests/$mode/$name.errcode
    if [[ -f "$file" ]]; then
        rm $file
    fi
fi