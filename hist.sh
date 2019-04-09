#!/bin/bash
echo -e "set terminal dumb \",\"set datafile separator \",\"\nplot './$1' with boxes\n" | gnuplot -p &
