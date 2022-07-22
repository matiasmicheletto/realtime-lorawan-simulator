#!/bin/bash

for i in {1..3}
do
    echo "Running uniform 2000x2000 $i"    
    ./uniform_2000x2000.sh & ./uniform_2000x2000.sh & ./uniform_2000x2000.sh & ./uniform_2000x2000.sh
done
