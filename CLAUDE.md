# Claude Assistant Instructions for DatGen Project

## Project Context
DatGen is a synthetic data generator from 1997 being modernized with a Python port. The C version is the gold standard for behavior.

## Key Commands and Phrases

### Git Workflow Commands
When the user says these phrases, take the following actions:

#### "wind down" / "sign off" / "check in"
- Run the `./sign_off.sh` script which will:
  - Check both repositories for uncommitted changes
  - Commit any changes with descriptive message
  - Push all commits to GitHub
  - Verify everything is saved
- Alternative manual process:
  - Stage all changes with `git add -A`
  - Create a comprehensive commit message describing the work done
  - Commit the changes using git
  - Push to GitHub with `git push`
  - Verify the commit succeeded
  - Provide a brief summary of what was committed

#### "commit" / "save progress"
- Same as above - use `./sign_off.sh` or manually commit and push

## Project-Specific Knowledge

### Testing Approach
- Use TDD (Test-Driven Development) methodology
- The C version is the gold standard - tests should match its behavior
- Run tests with: `PYTHONPATH=. python -m pytest tests/tests/`

### C Version Capabilities
- **Categorical Mode** (default): Generates letters (a-j with domain=10)
  - Command: `./src/datgen -O 100 -A 5 -d 10`
- **Continuous Mode**: Generates numeric data with distributions
  - Command: `./src/datgen -O 100 -X"-1/1,1,R,C"`
  - Distributions: uniform (0), random (1), gaussian/normal (2)

### Python Implementation
- Main class: `DatGenClassic` in `datgen/classic.py`
- Supports both categorical and continuous modes
- Parameters map: `-O` → `n_samples`, `-A` → `n_features`, `-d` → `domain_size`

### File Structure
```
DatGen/
├── src/                  # C implementation
│   ├── datgen           # Compiled executable
│   └── datgen.c         # Source code
├── datgen/              # Python implementation
│   ├── __init__.py
│   └── classic.py
├── tests/
│   ├── fixtures/        # Reference outputs from C version
│   ├── tests/          # Pytest test files
│   └── test_manifest.yaml
├── C_VERSION_SPECIFICATION.md
├── TDD_STATUS.md
└── MODERNIZATION_PLAN.md
```

### Common Tasks

#### Running Tests
```bash
# Run all tests
PYTHONPATH=. python -m pytest tests/tests/ -v

# Run specific test suite
PYTHONPATH=. python -m pytest tests/tests/test_compatibility.py -v

# Run with coverage
PYTHONPATH=. python -m pytest tests/tests/ --cov=datgen
```

#### Generating C Reference Fixtures
```bash
./generate_fixtures.sh
# Or manually:
./src/datgen -O 100 -A 5 -d 10 -p > tests/fixtures/reference.csv
```

#### Performance Testing
- Small dataset target: <0.1s for 1k samples
- Medium dataset target: <2s for 100k samples
- Large dataset target: <20s for 1M samples

## Development Guidelines

1. **C Version is Gold Standard**: Any behavior in the C version is correct by definition
2. **Dual Mode Support**: Always maintain both categorical and continuous modes
3. **Test-First**: Write/update tests before implementing features
4. **Document Changes**: Update TDD_STATUS.md with test results
5. **Commit Frequently**: Use descriptive commit messages following conventional commits

## Notes
- C version has known memory issues with certain parameter combinations
- Performance: Python will be slower than C (acceptable: within 5x)
- Default mode is categorical to match C version behavior