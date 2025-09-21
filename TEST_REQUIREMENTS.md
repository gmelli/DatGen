# DatGen Test Requirements Specification

## Overview
This document formally specifies all test requirements that must be satisfied before the Python port of DatGen can be considered complete. Each requirement has a unique ID for traceability.

**Test Coverage Target**: 100% for compatibility, 90% for new features
**Performance Target**: Python version within 5x of C version speed

---

## 1. C Version Compatibility Tests

### 1.1 Exact Output Matching
| ID | Requirement | Priority | Status |
|----|-------------|----------|---------|
| C001 | Given identical parameters (n=100, m=10, c=2, s=42), Python output must match C output byte-for-byte | P1 | ⬜ Not Started |
| C002 | Given identical seed, multiple runs must produce identical output (determinism) | P1 | ⬜ Not Started |
| C003 | Given no seed, multiple runs must produce different output (randomness) | P1 | ⬜ Not Started |
| C004 | Output format must match C version exactly (CSV structure, precision) | P1 | ⬜ Not Started |
| C005 | Header row format must match C version when headers enabled | P1 | ⬜ Not Started |

### 1.2 Parameter Compatibility
| ID | Requirement | Priority | Status |
|----|-------------|----------|---------|
| C010 | Parameter -n (samples) must accept range 1 to 10,000,000 | P1 | ⬜ Not Started |
| C011 | Parameter -m (features) must accept range 1 to 1,000 | P1 | ⬜ Not Started |
| C012 | Parameter -c (classes) must accept range 2 to 100 | P1 | ⬜ Not Started |
| C013 | Parameter -s (seed) must accept any valid integer | P1 | ⬜ Not Started |
| C014 | Parameter -noise must accept range 0.0 to 1.0 | P1 | ⬜ Not Started |
| C015 | Parameter -sparsity must accept range 0.0 to 1.0 | P2 | ⬜ Not Started |
| C016 | All C version flags must be recognized (even if deprecated) | P2 | ⬜ Not Started |

### 1.3 Edge Cases
| ID | Requirement | Priority | Status |
|----|-------------|----------|---------|
| C020 | Handle n=1 (single sample) correctly | P2 | ⬜ Not Started |
| C021 | Handle m=1 (single feature) correctly | P2 | ⬜ Not Started |
| C022 | Handle c=2 (binary classification) correctly | P1 | ⬜ Not Started |
| C023 | Handle c=n (each sample different class) correctly | P2 | ⬜ Not Started |
| C024 | Handle noise=0 (no noise) correctly | P1 | ⬜ Not Started |
| C025 | Handle noise=1 (maximum noise) correctly | P2 | ⬜ Not Started |

---

## 2. Statistical Properties Tests

### 2.1 Distribution Tests
| ID | Requirement | Priority | Status |
|----|-------------|----------|---------|
| S001 | Gaussian distribution must have mean≈0, std≈1 (tolerance ±0.01 for n>10000) | P1 | ⬜ Not Started |
| S002 | Uniform distribution must have all values in [0,1] | P1 | ⬜ Not Started |
| S003 | Class distribution must be approximately balanced (±5% for n>1000) | P1 | ⬜ Not Started |
| S004 | Feature correlations must be minimal (<0.1) when not specified | P2 | ⬜ Not Started |

### 2.2 Reproducibility Tests
| ID | Requirement | Priority | Status |
|----|-------------|----------|---------|
| S010 | Same seed must produce identical statistical properties across runs | P1 | ⬜ Not Started |
| S011 | Different seeds must produce different but valid distributions | P1 | ⬜ Not Started |
| S012 | Statistical properties must be stable across platforms (Linux/Mac/Windows) | P1 | ⬜ Not Started |

---

## 3. Performance Tests

### 3.1 Speed Benchmarks
| ID | Requirement | Priority | Status |
|----|-------------|----------|---------|
| P001 | Generate 1K samples, 10 features < 0.1 seconds | P1 | ⬜ Not Started |
| P002 | Generate 100K samples, 10 features < 2 seconds | P1 | ⬜ Not Started |
| P003 | Generate 1M samples, 10 features < 20 seconds | P2 | ⬜ Not Started |
| P004 | Memory usage must be O(n×m) without memory leaks | P1 | ⬜ Not Started |

### 3.2 Comparison with C Version
| ID | Requirement | Priority | Status |
|----|-------------|----------|---------|
| P010 | Python version must be within 5x slower than C for typical use | P1 | ⬜ Not Started |
| P011 | Python+Numpy must be within 3x slower than C | P2 | ⬜ Not Started |
| P012 | Python+Numba/Cython must be within 1.5x slower than C | P3 | ⬜ Not Started |

---

## 4. API Tests (Python-Specific)

### 4.1 Classic API
| ID | Requirement | Priority | Status |
|----|-------------|----------|---------|
| A001 | DatGenClassic must accept all C parameters as constructor args | P1 | ⬜ Not Started |
| A002 | generate() method must return pandas DataFrame | P1 | ⬜ Not Started |
| A003 | to_csv() method must produce identical output to C version | P1 | ⬜ Not Started |
| A004 | to_numpy() method must return numpy array | P2 | ⬜ Not Started |

### 4.2 Modern API
| ID | Requirement | Priority | Status |
|----|-------------|----------|---------|
| A010 | Fluent interface must support method chaining | P2 | ⬜ Not Started |
| A011 | Must integrate with scikit-learn pipelines | P2 | ⬜ Not Started |
| A012 | Must support streaming generation for large datasets | P3 | ⬜ Not Started |

---

## 5. CLI Tests

### 5.1 Command Line Compatibility
| ID | Requirement | Priority | Status |
|----|-------------|----------|---------|
| CLI001 | `python -m datgen` must accept same arguments as C version | P1 | ⬜ Not Started |
| CLI002 | Output to stdout must match C version format | P1 | ⬜ Not Started |
| CLI003 | Error messages must be informative | P2 | ⬜ Not Started |
| CLI004 | --help must document all parameters | P1 | ⬜ Not Started |

---

## 6. Integration Tests

### 6.1 Downstream Compatibility
| ID | Requirement | Priority | Status |
|----|-------------|----------|---------|
| I001 | Output must work with common ML libraries (sklearn, pandas) | P1 | ⬜ Not Started |
| I002 | Output must work with existing pipelines using C version | P1 | ⬜ Not Started |
| I003 | Must handle pipes and redirects correctly | P2 | ⬜ Not Started |

---

## Test Coverage Matrix

| Category | Total Tests | P1 (Critical) | P2 (Important) | P3 (Nice) | Completed |
|----------|-------------|---------------|----------------|-----------|-----------|
| Compatibility | 25 | 17 | 8 | 0 | 0 |
| Statistical | 12 | 10 | 2 | 0 | 0 |
| Performance | 12 | 5 | 4 | 3 | 0 |
| API | 14 | 4 | 6 | 4 | 0 |
| CLI | 4 | 3 | 1 | 0 | 0 |
| Integration | 3 | 2 | 1 | 0 | 0 |
| **TOTAL** | **70** | **41** | **22** | **7** | **0** |

---

## Test Data Requirements

### Reference Outputs Needed from C Version
```bash
# Core compatibility test cases
datgen -n 100 -m 10 -c 2 -s 42 > fixtures/c_100_10_2_s42.csv
datgen -n 1000 -m 5 -c 3 -s 123 > fixtures/c_1000_5_3_s123.csv
datgen -n 10000 -m 20 -c 5 -s 999 > fixtures/c_10000_20_5_s999.csv

# Edge cases
datgen -n 1 -m 1 -c 2 -s 1 > fixtures/c_1_1_2_s1.csv
datgen -n 100 -m 100 -c 100 -s 42 > fixtures/c_100_100_100_s42.csv

# Noise variations
datgen -n 1000 -m 10 -c 2 -s 42 -noise 0.0 > fixtures/c_1000_10_2_s42_n0.csv
datgen -n 1000 -m 10 -c 2 -s 42 -noise 0.5 > fixtures/c_1000_10_2_s42_n05.csv
datgen -n 1000 -m 10 -c 2 -s 42 -noise 1.0 > fixtures/c_1000_10_2_s42_n10.csv
```

---

## Success Criteria

1. **Phase 1 Complete**: All P1 compatibility tests passing (C001-C025)
2. **Phase 2 Complete**: All P1 statistical tests passing (S001-S012)
3. **Phase 3 Complete**: All P1 performance tests passing (P001-P010)
4. **Release Ready**: All P1 and P2 tests passing (63 tests)
5. **Feature Complete**: All tests passing (70 tests)

---

## How This Document Drives Development

1. **Before coding**: Generate all reference outputs from C version
2. **TDD approach**: Write failing tests for each requirement
3. **Incremental development**: Implement to make tests pass one by one
4. **Progress tracking**: Update status column as tests pass
5. **Release gates**: Cannot release until all P1 tests pass

---

*Last Updated: 2025-09-20*
*Next Review: After reference data generation*