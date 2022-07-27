#!/bin/bash

for i in {1..3}
do
    echo "Running uniform 100x100 $i"
    ./uniform_100x100.sh & ./uniform_100x100.sh & ./uniform_100x100.sh & ./uniform_100x100.sh
done
