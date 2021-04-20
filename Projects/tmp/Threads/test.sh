#!/bin/bash

for i in 1 2 3 4 5 6 7 8 9 10 11 12 13 12000
do
  printf "\n$i\n"
  time ./calc $i | egrep "res|real"
done
