#!/bin/bash

if [ $# -eq 0 ]; then
  echo "Нет аргументов. Пожалуйста, передайте числа в качестве аргументов."
  exit 1
fi

sum=0
count=0

for arg in "$@"; do
  sum=$((sum + arg))
  count=$((count + 1))
done

average=$(echo "$sum $count" | awk '{printf "%.2f", $1 / $2}')

echo "Количество аргументов: $count"
echo "Среднее арифметическое: $average"
