#!/bin/bash

# Check if the directory argument is provided and valid
if [ $# -ne 1 ]; then
    echo "Please provide the path of the directory containing the images."
    exit 1
elif [ ! -d "$1" ]; then
    echo "The provided path is not a directory."
    exit 1
fi

# Change to the specified directory
cd "$1"

# Create an output directory for the resized images
mkdir -p resized

# Process all .png files in the directory
for file in *.png; do
    # Construct the output file name with a prefix
    output_file="resized/resized_${file}"
    
    # Check if the output file already exists
    if [ ! -f "$output_file" ]; then
        # Resize the image to fill the dimensions, potentially overlapping, then crop to exact dimensions
        convert "$file" -resize 512x512^ -gravity center -background none -extent 512x512 -alpha on "$output_file"
        echo "Resized $file to $output_file."
    else
        echo "Output file $output_file already exists. Skipping $file."
    fi
done

echo "All images have been processed."
