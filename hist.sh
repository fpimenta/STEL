#!/bin/bash
echo -e "set datafile separator \",\"\nplot './$1' with boxes\n" | gnuplot -p &
