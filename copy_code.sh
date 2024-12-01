#!/bin/bash

# Check if at least two arguments (directories) are passed
if [ "$#" -lt 2 ]; then
  echo "Usage: $0 <directory1> <directory2> ... <output_file>"
  exit 1
fi

# Set the last argument as the output file
output_file="${!#}"

# Remove the output file if it exists to start fresh
> "$output_file"

# Iterate over all directories except the last argument (output file)
for dir in "$@"; do
  if [ "$dir" != "$output_file" ]; then
    # Find .h and .cpp files in the directory and subdirectories
    find "$dir" -type f \( -name "*.h" -o -name "*.cpp" \) | while read -r file; do
      # Append the file contents to the output file with a separator
      echo "----- Start of $file -----" >> "$output_file"
      cat "$file" >> "$output_file"
      echo -e "\n----- End of $file -----\n" >> "$output_file"
    done
  fi
done

echo "Contents copied to $output_file."

