#!/bin/bash

if [ "$#" -ne 1 ]; then
    echo "Utilizare: $0 <caracter>"
    exit 1
fi

caracter=$1

contor=0

while IFS= read -r linie; do
    if [[ "$linie" =~ ^[A-Z][" ",0-9a-z]*[.?!]$ && ! "$linie" =~ ," "si && "$linie" =~ $caracter+ ]]; then
        ((contor++))
    fi
done

echo "Au fost identificate in total $contor propozitii corecte care contin caracterul $caracter"