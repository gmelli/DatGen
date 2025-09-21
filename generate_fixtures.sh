#!/bin/bash
# Generate reference outputs from C version for test fixtures

echo "Generating C reference fixtures..."
cd src

# Basic test case - 100 objects, 5 attributes, 2 classes
echo "  Generating c_100_5_2.csv..."
./datgen -O 100 -A 5 -d 10 -p > ../tests/fixtures/c_100_5_2.csv

# Larger test - 1000 objects, 10 attributes, 3 classes
echo "  Generating c_1000_10_3.csv..."
./datgen -O 1000 -A 10 -d 20 -R 3 -p > ../tests/fixtures/c_1000_10_3.csv

# Small test for exact matching
echo "  Generating c_10_3_2.csv..."
./datgen -O 10 -A 3 -d 5 -p > ../tests/fixtures/c_10_3_2.csv

# Test without pseudo-random (true random)
echo "  Generating c_100_5_2_random.csv..."
./datgen -O 100 -A 5 -d 10 > ../tests/fixtures/c_100_5_2_random.csv

echo "✓ Generated 4 reference fixtures"
echo ""
echo "Note: The C version uses different parameters than modern ML tools:"
echo "  -O = number of objects (samples)"
echo "  -A = number of attributes (features)"
echo "  -d = domain size for attributes"
echo "  -R = number of rules (affects classes)"
echo "  -p = pseudo-random (deterministic)"

cd ..