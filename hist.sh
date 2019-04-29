#!/bin/bash
echo -e "set datafile separator \",\"\nplot './$1' with boxes\n set output \"output.jpg\"" | gnuplot -p 
