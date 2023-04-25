if test -f "$1"
then
    gcc -Wall "$1" 2>&1 | grep "warning:\|eror:" | wc -l 
else
    echo "not a file?"
fi