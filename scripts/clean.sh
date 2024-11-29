#!/bin/bash

# Perform git clean on the main repository
echo "Cleaning Git repository..."
git clean -fdX || { echo "Failed to clean the main repository."; exit 1; }

# Perform git clean recursively on submodules
if [ -f .gitmodules ]; then
    echo "Cleaning submodules recursively..."
    git submodule foreach --recursive 'git clean -fdX || exit 1'
else
    echo "No submodules found."
fi

echo "Git clean completed for the main repository and all submodules."
