#!/bin/bash
set -e

echo "---- Lab Car Video System Setup ----"

# Install system packages
echo "Installing system dependencies (requires sudo)..."
sudo apt update
sudo apt install -y cmake g++ libcamera-apps ffmpeg can-utils

# Optional: GPIO library for Raspberry Pi (uncomment if you want to use pigpio)
# sudo apt install -y pigpio

# Create buffer and events directories if they don't exist
mkdir -p ../buffer
mkdir -p ../events

echo "Setup complete!"
echo ""
echo "To build the project:"
echo "  mkdir -p ../build && cd ../build"
echo "  cmake .."
echo "  make"
echo ""
echo "To run the video system:"
echo "  ./labcar"
echo ""
echo "If you encounter camera or CAN errors, check your hardware connections and configuration."