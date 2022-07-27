#!/bin/bash

for i in {1..3}
do
    echo "Running uniform 1000x1000 $i"    
    ./uniform_1000x1000.sh & ./uniform_1000x1000.sh & ./uniform_1000x1000.sh & ./uniform_1000x1000.sh    
done
