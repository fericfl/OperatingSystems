echo "Total number of warnings and errors is: " && gcc -Wall "$1" 2>&1 | grep "warning:\|eror:" | wc -l 

