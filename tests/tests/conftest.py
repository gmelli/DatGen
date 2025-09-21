"""
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
