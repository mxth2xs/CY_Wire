#!/bin/bash

# Shell script for the C-Wire Project
# Usage: ./c-wire.sh csv_file_path station_type consumer_type [plant_id]

# Check for help option
for arg in "$@"; do
    if [ "$arg" = "-h" ] || [ "$arg" = "--help" ]; then
        echo "Usage: $0 csv_file_path station_type consumer_type [plant_id]"
        echo
        echo "Parameters:"
        echo "  csv_file_path : Path to the CSV input file"
        echo "  station_type  : hvb | hva | lv"
        echo "  consumer_type : comp | indiv | all"
        echo "  plant_id      : Optional, defaults to -1 if not provided"
        echo
        echo "Examples:"
        echo "  $0 data.csv lv all"
        echo "  $0 data.csv hvb comp 1234"
        exit 0
    fi
done

start=$(date +%s)

# Parameter count check
if [ "$#" -lt 3 ]; then
    echo "Error: Insufficient number of parameters."
    echo "Use -h for help."
    exit 1
fi

# Retrieving parameters
csv_file="$1"
station_type="$2"
consumer_type="$3"
plant_id="${4:--1}"

# Input file validation
if [ ! -f "$csv_file" ]; then
    echo "Error: The file $csv_file does not exist."
    exit 1
fi

# Station type validation
if [[ "$station_type" != "hvb" && "$station_type" != "hva" && "$station_type" != "lv" ]]; then
    echo "Error: Invalid station type. Valid options: hvb, hva, lv."
    exit 1
fi

# Consumer type validation
if [[ "$consumer_type" != "comp" && "$consumer_type" != "indiv" && "$consumer_type" != "all" ]]; then
    echo "Error: Invalid consumer type. Valid options: comp, indiv, all."
    exit 1
fi

# Plant ID validation if provided
if [ "$plant_id" != "-1" ]; then
    echo "Checking plant ID: $plant_id"
    valid_id=$(awk -F';' -v id="$plant_id" '$1 == id {print $1; exit}' "$csv_file")
    if [ -z "$valid_id" ]; then
        echo "Error: The plant ID $plant_id does not exist in the file."
        exit 1
    fi
fi

# Prohibited options
if [[ "$station_type" == "hvb" && ( "$consumer_type" == "all" || "$consumer_type" == "indiv" ) ]]; then
    echo "Error: hvb all or hvb indiv options are not allowed."
    exit 1
fi

if [[ "$station_type" == "hva" && ( "$consumer_type" == "all" || "$consumer_type" == "indiv" ) ]]; then
    echo "Error: hva all or hva indiv options are not allowed."
    exit 1
fi

# Create necessary directories
mkdir -p tmp output

# Clear tmp and output directories
rm -f tmp/*
rm -f output/*

if [ "$plant_id" == "-1" ]; then
    filtered_file="tmp/filter_${station_type}_${consumer_type}.csv"
else
    filtered_file="tmp/filter_${station_type}_${consumer_type}_${plant_id}.csv"
fi

# Filtering data
awk -F';' -v station="$station_type" -v consumer="$consumer_type" -v plant="$plant_id" '
BEGIN { OFS=";" }
{
    if (station == "hvb" && $2 != "-" && $6 == "-" && (consumer == "comp" || consumer == "all")) {
        if (plant == "-1" || $1 == plant) print $0;
    }
    else if (station == "hva" && $3 != "-" && $6 == "-" && (consumer == "comp" || consumer == "all")) {
        if (plant == "-1" || $1 == plant) print $0;
    }
    else if (station == "lv" && $4 != "-" && $1 != "Power plant" && (consumer == "comp" || consumer == "indiv" || consumer == "all")) {
        if (plant == "-1" || $1 == plant) print $0;
    }
}' "$csv_file" > "$filtered_file"

if [ ! -s "$filtered_file" ]; then
    echo "Error: No data found for the specified parameters."
    exit 1
fi

echo "Filtered data saved to $filtered_file"

echo "Cleaning previous builds..."
make -C codeC clean

echo "Compiling the C program..."
make -s -C codeC
if [ ! -f "codeC/bin/main" ]; then
    echo "Error: Failed to compile the C program."
    exit 1
fi

echo "Executing the C program..."
./codeC/bin/main "$station_type" "$consumer_type" "$plant_id"
if [ $? -ne 0 ]; then
    echo "Error: Failed to execute the C program."
    exit 1
fi

# If station_type=lv and consumer_type=all, generate graphs
if [[ "$station_type" == "lv" && "$consumer_type" == "all" ]]; then
    echo "Generating graphs for LV all..."
    
    top10_data="output/top10_lv_all.csv"
    bottom10_data="output/bottom10_lv_all.csv"
    script_path="output/plot_lv_all.gp"
    chart_output="output/chart_lv_all.png"

    if [[ -f "$top10_data" && -f "$bottom10_data" ]]; then
        echo "Executing Gnuplot..."
        gnuplot "$script_path"

        if [ $? -eq 0 ]; then
            echo "Graph successfully generated: $chart_output"
        else
            echo "Error: Failed to generate the graph."
        fi
    else
        echo "Error: Data files for Gnuplot not found."
    fi
fi

echo "Processing complete."

end=$(date +%s)
duration=$((end - start))
echo "Execution time: $duration s"
