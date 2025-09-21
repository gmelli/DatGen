#!/usr/bin/env python3
"""
Generate pytest test files from test_manifest.yaml

This script reads the test manifest and generates actual pytest test functions
that can be run to validate the DatGen implementation.
"""

import yaml
import os
from pathlib import Path
from textwrap import dedent, indent

def load_manifest(manifest_path="test_manifest.yaml"):
    """Load the test manifest YAML file"""
    with open(manifest_path, 'r') as f:
        return yaml.safe_load(f)

def generate_test_function(test_spec, suite_name):
    """Generate a single pytest function from test specification"""
    test_id = test_spec['id']
    test_name = test_spec['name'].replace(' ', '_').lower()
    priority = test_spec['priority']

    # Create pytest marker based on priority
    marker = f"@pytest.mark.{priority.lower()}"
    if priority == 'P1':
        marker += "\n@pytest.mark.critical"

    # Generate function signature
    func_name = f"test_{test_id.lower()}_{test_name}"

    # Generate test body based on expected type
    expected = test_spec.get('expected', {})
    test_type = expected.get('type', 'basic')

    if test_type == 'exact_file_match':
        test_body = generate_exact_match_test(test_spec)
    elif test_type == 'identical_on_multiple_runs':
        test_body = generate_determinism_test(test_spec)
    elif test_type == 'different_on_multiple_runs':
        test_body = generate_randomness_test(test_spec)
    elif test_type == 'statistical':
        test_body = generate_statistical_test(test_spec)
    elif test_type == 'timing':
        test_body = generate_performance_test(test_spec)
    elif test_type == 'shape_check':
        test_body = generate_shape_test(test_spec)
    else:
        test_body = generate_basic_test(test_spec)

    # Combine into complete test function
    return f'''
{marker}
def {func_name}():
    """Test {test_id}: {test_spec['name']}"""
{indent(test_body, '    ')}
'''

def generate_exact_match_test(spec):
    """Generate test for exact file matching"""
    params = spec['params']
    expected_file = spec['expected']['file']

    return f'''
# Load reference output from C version
reference_file = Path(__file__).parent / "{expected_file}"
if not reference_file.exists():
    pytest.skip(f"Reference file {{reference_file}} not found - run generate_fixtures.sh first")

reference_df = pd.read_csv(reference_file)

# Generate Python output with same parameters
gen = DatGenClassic(
    n_samples={params['n']},
    n_features={params['m']},
    n_classes={params['c']},
    random_state={params.get('seed', 'None')}
)
result_df = gen.generate()

# Compare outputs
pd.testing.assert_frame_equal(reference_df, result_df, check_dtype=False, rtol=1e-6)
'''

def generate_determinism_test(spec):
    """Generate test for deterministic output"""
    params = spec['params']
    runs = spec['expected'].get('runs', 3)

    return f'''
# Generate multiple outputs with same seed
outputs = []
for _ in range({runs}):
    gen = DatGenClassic(
        n_samples={params['n']},
        n_features={params['m']},
        n_classes={params['c']},
        random_state={params.get('seed', 'None')}
    )
    outputs.append(gen.generate())

# All outputs should be identical
for i in range(1, len(outputs)):
    pd.testing.assert_frame_equal(outputs[0], outputs[i])
'''

def generate_randomness_test(spec):
    """Generate test for random output"""
    params = spec['params']
    runs = spec['expected'].get('runs', 3)

    return f'''
# Generate multiple outputs without seed
outputs = []
for _ in range({runs}):
    gen = DatGenClassic(
        n_samples={params['n']},
        n_features={params['m']},
        n_classes={params['c']},
        random_state=None
    )
    outputs.append(gen.generate())

# All outputs should be different
for i in range(1, len(outputs)):
    with pytest.raises(AssertionError):
        pd.testing.assert_frame_equal(outputs[0], outputs[i])
'''

def generate_statistical_test(spec):
    """Generate test for statistical properties"""
    params = spec['params']
    expected = spec['expected']

    return f'''
# Generate data
gen = DatGenClassic(
    n_samples={params['n']},
    n_features={params['m']},
    n_classes={params['c']},
    random_state={params.get('seed', 'None')}
)
df = gen.generate()

# Extract features (excluding class column)
features = df.iloc[:, :-1]

# Check statistical properties
actual_mean = features.mean().mean()
actual_std = features.std().mean()

assert abs(actual_mean - {expected.get('mean', 0.0)}) < {expected.get('mean_tolerance', 0.01)}, \\
    f"Mean {{actual_mean}} not within tolerance of {expected.get('mean', 0.0)}"

assert abs(actual_std - {expected.get('std', 1.0)}) < {expected.get('std_tolerance', 0.01)}, \\
    f"Std {{actual_std}} not within tolerance of {expected.get('std', 1.0)}"
'''

def generate_performance_test(spec):
    """Generate performance/timing test"""
    params = spec['params']
    max_seconds = spec['expected']['max_seconds']

    return f'''
import time

# Time the generation
start_time = time.time()

gen = DatGenClassic(
    n_samples={params['n']},
    n_features={params['m']},
    n_classes={params['c']}
)
df = gen.generate()

elapsed_time = time.time() - start_time

assert elapsed_time < {max_seconds}, \\
    f"Generation took {{elapsed_time:.2f}}s, exceeding limit of {max_seconds}s"

# Verify output is correct size
assert df.shape == ({params['n']}, {params['m']} + 1), \\
    f"Output shape {{df.shape}} incorrect"
'''

def generate_shape_test(spec):
    """Generate test for output shape"""
    param_tests = spec.get('param_tests', [spec.get('params', {})])

    test_lines = []
    for i, params in enumerate(param_tests):
        test_lines.append(f'''
# Test case {i+1}
gen = DatGenClassic(
    n_samples={params['n']},
    n_features={params['m']},
    n_classes={params['c']},
    random_state={params.get('seed', 'None')}
)
df = gen.generate()
assert df.shape == ({params['n']}, {params['m']} + 1), \\
    f"Shape {{df.shape}} != expected ({params['n']}, {params['m'] + 1})"
''')

    return '\n'.join(test_lines)

def generate_basic_test(spec):
    """Generate a basic test when type is not specified"""
    params = spec.get('params', {})

    return f'''
# Basic test - implementation needed
gen = DatGenClassic(
    n_samples={params.get('n', 100)},
    n_features={params.get('m', 10)},
    n_classes={params.get('c', 2)}
)
df = gen.generate()
assert df is not None, "Generator returned None"
assert len(df) > 0, "Generator returned empty DataFrame"
'''

def generate_test_file(suite_name, tests, manifest):
    """Generate a complete test file for a test suite"""

    header = f'''"""
Auto-generated pytest file for {suite_name} tests
Generated from test_manifest.yaml
DO NOT EDIT MANUALLY - regenerate with: python generate_tests.py
"""

import pytest
import pandas as pd
import numpy as np
from pathlib import Path
import time
import subprocess
import sys

# Import the module to test (will be implemented)
try:
    from datgen.classic import DatGenClassic
except ImportError:
    # Module not yet implemented - tests will fail
    class DatGenClassic:
        def __init__(self, **kwargs):
            raise NotImplementedError("DatGenClassic not yet implemented")
        def generate(self):
            raise NotImplementedError("generate() not yet implemented")
'''

    # Generate all test functions
    test_functions = []
    for test_spec in tests:
        test_func = generate_test_function(test_spec, suite_name)
        test_functions.append(test_func)

    return header + '\n'.join(test_functions)

def main():
    """Main function to generate all test files"""

    # Load manifest
    print("Loading test manifest...")
    manifest = load_manifest()

    # Create tests directory if it doesn't exist
    tests_dir = Path("tests")
    tests_dir.mkdir(exist_ok=True)

    # Generate test files for each suite
    for suite_name, suite_data in manifest['test_suites'].items():
        tests = suite_data['tests']

        # Generate test file
        print(f"Generating test_{suite_name}.py with {len(tests)} tests...")
        test_content = generate_test_file(suite_name, tests, manifest)

        # Write test file
        test_file = tests_dir / f"test_{suite_name}.py"
        with open(test_file, 'w') as f:
            f.write(test_content)

        print(f"  ✓ Created {test_file}")

    # Generate __init__.py
    init_file = tests_dir / "__init__.py"
    init_file.touch()

    # Generate conftest.py for pytest configuration
    conftest_content = '''"""
Pytest configuration and fixtures
"""

import pytest
import sys
from pathlib import Path

# Add parent directory to path so we can import datgen
sys.path.insert(0, str(Path(__file__).parent.parent))

# Define markers
def pytest_configure(config):
    config.addinivalue_line(
        "markers", "p1: Priority 1 - Critical tests"
    )
    config.addinivalue_line(
        "markers", "p2: Priority 2 - Important tests"
    )
    config.addinivalue_line(
        "markers", "p3: Priority 3 - Nice to have tests"
    )
    config.addinivalue_line(
        "markers", "critical: Critical tests that must pass"
    )
'''

    conftest_file = tests_dir / "conftest.py"
    with open(conftest_file, 'w') as f:
        f.write(conftest_content)
    print(f"  ✓ Created {conftest_file}")

    # Generate requirements file for testing
    requirements_content = '''# Testing requirements
pytest>=7.0.0
pyyaml>=6.0
pandas>=1.5.0
numpy>=1.20.0
'''

    req_file = "requirements-test.txt"
    with open(req_file, 'w') as f:
        f.write(requirements_content)
    print(f"  ✓ Created {req_file}")

    # Summary
    total_tests = sum(len(suite['tests']) for suite in manifest['test_suites'].values())
    print(f"\n✅ Generated {len(manifest['test_suites'])} test files with {total_tests} total tests")
    print("\nNext steps:")
    print("  1. Run: pip install -r requirements-test.txt")
    print("  2. Generate C reference outputs: cd src && make && ./generate_fixtures.sh")
    print("  3. Run tests: pytest tests/ -v")
    print("  4. Implement DatGenClassic to make tests pass!")

if __name__ == "__main__":
    main()