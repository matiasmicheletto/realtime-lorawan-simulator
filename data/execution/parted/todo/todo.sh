#!/bin/bash

for i in {1..3}
do
    echo "Running uniform 100x100 $i"
    ./uniform_100x100.sh & ./uniform_100x100.sh & ./uniform_100x100.sh & ./uniform_100x100.sh
done

for i in {1..3}
do
    echo "Running uniform 1000x1000 $i"    
    ./uniform_1000x1000.sh & ./uniform_1000x1000.sh & ./uniform_1000x1000.sh & ./uniform_1000x1000.sh    
done

for i in {1..3}
do
    echo "Running uniform 2000x2000 $i"    
    ./uniform_2000x2000.sh & ./uniform_2000x2000.sh & ./uniform_2000x2000.sh & ./uniform_2000x2000.sh
done

for i in {1..3}
do
    echo "Running clouds 100x100 $i"
    ./clouds_100x100.sh & ./clouds_100x100.sh & ./clouds_100x100.sh & ./clouds_100x100.sh
done

for i in {1..3}
do
    echo "Running clouds 1000x1000 $i"    
    ./clouds_1000x1000.sh & ./clouds_1000x1000.sh & ./clouds_1000x1000.sh & ./clouds_1000x1000.sh    
done

for i in {1..3}
do
    echo "Running clouds 2000x2000 $i"
    ./clouds_2000x2000.sh & ./clouds_2000x2000.sh & ./clouds_2000x2000.sh & ./clouds_2000x2000.sh
done
