#!/bin/bash

# Shell Script for the C-Wire Project
# Author: Pfleger Paul, Da Costa Silva Mathias
# Usage: ./c-wire.sh csv_file_path station_type consumer_type [plant_id]

# Parameter validation
if [ "$#" -lt 3 ]; then
    echo "Error: Insufficient number of parameters."
    echo "Usage: ./c-wire.sh csv_file_path station_type consumer_type [plant_id]"
    exit 1
fi

# Retrieving parameters
csv_file="$1"
station_type="$2"
consumer_type="$3"
plant_id="${4:-}" # Optional

# Input file validation
if [ ! -f "$csv_file" ]; then
    echo "Error: The file $csv_file does not exist."
    exit 1
fi

# Station type validation
if [[ "$station_type" != "hvb" && "$station_type" != "hva" && "$station_type" != "lv" ]]; then
    echo "Error: Invalid station type. Valid options are: hvb, hva, lv."
    exit 1
fi

# Consumer type validation
if [[ "$consumer_type" != "comp" && "$consumer_type" != "indiv" && "$consumer_type" != "all" ]]; then
    echo "Error: Invalid consumer type. Valid options are: comp, indiv, all."
    exit 1
fi

# Plant ID validation (if provided)
if [ -n "$plant_id" ]; then
    echo "Checking plant ID: $plant_id"
    valid_id=$(awk -F';' -v id="$plant_id" '$1 == id {print $1; exit}' "$csv_file")
    if [ -z "$valid_id" ]; then
        echo "Error: The plant ID $plant_id does not exist in the file."
        exit 1
    fi
fi

# Prohibiting certain options
if [[ "$station_type" == "hvb" && ( "$consumer_type" == "all" || "$consumer_type" == "indiv" ) ]]; then
    echo "Error: Options hvb all or hvb indiv are not allowed."
    exit 1
fi

if [[ "$station_type" == "hva" && ( "$consumer_type" == "all" || "$consumer_type" == "indiv" ) ]]; then
    echo "Error: Options hva all or hva indiv are not allowed."
    exit 1
fi

# Creating necessary directories
mkdir -p tmp tests

# Clearing the tmp directory
rm -f tmp/* 

# Preparing the temporary file
filtered_file="tmp/filter_${station_type}_${consumer_type}.csv"

# Filtering data from the CSV file
awk -F';' -v station="$station_type" -v consumer="$consumer_type" -v plant="$plant_id" '
BEGIN { OFS=";" }
{
    if (station == "hvb" && $2 != "-" && $6 == "-" && (consumer == "comp" || consumer == "all")) {
        if (plant == "" || $1 == plant) print $0;
    }
    else if (station == "hva" && $3 != "-" && $6 == "-" && (consumer == "comp" || consumer == "all")) {
        if (plant == "" || $1 == plant) print $0;
    }
    else if (station == "lv" && $4 != "-" && (consumer == "comp" || consumer == "indiv" || consumer == "all")) {
        if (plant == "" || $1 == plant) print $0;
    }
}' "$csv_file" > "$filtered_file"

echo "Filtered data saved to $filtered_file"

# Checking and compiling the C program
if [ ! -f "codeC/c-wire" ]; then
    echo "Compiling the C program..."
    make -C codeC
    if [ $? -ne 0 ]; then
        echo "Error: Failed to compile the C program."
        exit 1
    fi
fi

# Executing the C program
echo "Executing the C program..."
./codeC/c-wire "$filtered_file" "tmp/result_${station_type}_${consumer_type}.csv"
if [ $? -ne 0 ]; then
    echo "Error: Failed to execute the C program."
    exit 1
fi

echo "Result available at tmp/result_${station_type}_${consumer_type}.csv"

echo "Processing complete."
