sort -k5,5 -n -r weather_cordoba.in | head -n 1 | awk '{print $1, $2, $3}'