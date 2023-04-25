gcc -Wall "$1" 2>&1 | grep "warning:\|eror:" | wc -l 
