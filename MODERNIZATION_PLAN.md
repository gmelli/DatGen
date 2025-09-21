# DatGen Modernization Plan

## Architecture Overview

### Current State (1997 C Version)
```
datgen.c (87KB monolithic C file)
   ├── Command line parser
   ├── Random number generation
   ├── Statistical distributions
   ├── Classification boundary generation
   └── CSV output formatter
```

### Target State (2025 Python Version)
```
datgen/
├── core/
│   ├── generator.py       # Main generation engine
│   ├── distributions.py   # Statistical distributions
│   ├── classifiers.py     # Classification boundaries
│   └── legacy.py          # C-compatible mode
├── io/
│   ├── formats.py         # Multiple output formats
│   └── streaming.py       # Streaming generation
├── api/
│   ├── sklearn_api.py     # Scikit-learn compatible
│   └── fluent_api.py      # Modern builder pattern
├── cli/
│   └── cli.py            # Command line interface
└── _speedups/
    └── datgen_core.c      # Optional C extensions
```

## Implementation Phases

### Phase 1: Core Python Port (Week 1-2)

**File: `datgen_modern.py`** (Initial standalone version)
```python
import numpy as np
import pandas as pd
from typing import Optional, Tuple, Union
from dataclasses import dataclass

@dataclass
class DatGenConfig:
    """Configuration matching original C parameters"""
    n_samples: int = 1000
    n_features: int = 10
    n_classes: int = 2
    noise_level: float = 0.0
    random_state: Optional[int] = None

class DatGenClassic:
    """Direct port of C functionality"""

    def __init__(self, config: DatGenConfig):
        self.config = config
        self.rng = np.random.RandomState(config.random_state)

    def generate(self) -> pd.DataFrame:
        """Generate data matching C version output"""
        # Port the C algorithm here
        pass

class DatGenModern:
    """Enhanced modern API"""

    def __init__(self):
        self.config = {}

    def with_samples(self, n: int) -> 'DatGenModern':
        self.config['n_samples'] = n
        return self

    def with_features(self, n: int, types=None) -> 'DatGenModern':
        self.config['n_features'] = n
        self.config['feature_types'] = types or ['numeric'] * n
        return self

    def generate(self) -> pd.DataFrame:
        """Generate with modern enhancements"""
        pass
```

### Phase 2: Feature Parity Checklist

- [ ] **Random Seeds**: Ensure reproducibility matches C version
- [ ] **Distributions**: Gaussian, Uniform, Exponential
- [ ] **Classification**: Linear, Quadratic, Complex boundaries
- [ ] **Noise Models**: Additive, Multiplicative, Label noise
- [ ] **Output Formats**: CSV (original), JSON, Parquet, HDF5
- [ ] **Performance**: Benchmark against C version

### Phase 3: Modern Enhancements

```python
# Advanced features not in original
class AdvancedGenerator(DatGenModern):

    def with_correlations(self, correlation_matrix):
        """Add feature correlations"""
        pass

    def with_temporal_drift(self, drift_rate):
        """Add concept drift over time"""
        pass

    def with_imbalance(self, ratios):
        """Create imbalanced datasets"""
        pass

    def with_missing_data(self, missing_rate, pattern='MCAR'):
        """Add missing data patterns"""
        pass
```

### Phase 4: Testing Strategy

```python
# tests/test_compatibility.py
def test_c_compatibility():
    """Ensure output matches C version exactly"""
    # Run C version
    c_output = subprocess.run(['./datgen', '-n', '100'], capture_output=True)

    # Run Python version
    gen = DatGenClassic(DatGenConfig(n_samples=100))
    py_output = gen.generate()

    # Compare outputs
    assert_almost_equal(c_output, py_output)

# tests/test_modern.py
def test_modern_api():
    """Test new features"""
    gen = DatGenModern() \
        .with_samples(1000) \
        .with_features(10) \
        .with_correlations(0.7)

    df = gen.generate()
    assert df.shape == (1000, 10)
    assert df.corr().mean() > 0.6
```

## Migration Guide

### For Existing Users
```bash
# Old C way
./datgen -n 1000 -m 10 -c 3 > data.csv

# New Python CLI (compatible)
python -m datgen classic -n 1000 -m 10 -c 3 > data.csv

# New Python API
from datgen import DatGenClassic, DatGenConfig
gen = DatGenClassic(DatGenConfig(n_samples=1000, n_features=10, n_classes=3))
df = gen.generate()
df.to_csv('data.csv', index=False)
```

### For New Users
```python
from datgen import DataGenerator

# Simple
gen = DataGenerator()
df = gen.quick_generate(1000, 10)  # 1000 samples, 10 features

# Advanced
gen = DataGenerator() \
    .for_classification(n_classes=3) \
    .with_continuous_features(5) \
    .with_categorical_features(3) \
    .with_text_features(2) \
    .with_realistic_noise() \
    .with_missing_data(0.05)

train_df = gen.generate(8000)
test_df = gen.generate(2000)  # Same distribution
```

## Performance Targets

| Metric | C Version | Python Goal | Python+C Extension |
|--------|-----------|-------------|-------------------|
| 1M rows, 10 features | 0.5s | 2s | 0.7s |
| 10M rows, 10 features | 5s | 20s | 7s |
| Memory usage | O(1) streaming | O(n) or streaming | O(1) streaming |

## Success Criteria

1. **Compatibility**: Python version can replicate any C version output
2. **Performance**: Within 5x of C performance for typical use cases
3. **Usability**: Modern Python API that feels native to data scientists
4. **Extensibility**: Easy to add new generation strategies
5. **Documentation**: Complete API docs with examples
6. **Testing**: >90% test coverage

## Timeline

- **Week 1-2**: Core port with compatibility
- **Week 3**: Modern API and features
- **Week 4**: Performance optimization
- **Week 5**: Documentation and examples
- **Week 6**: Release v4.0 (first Python version)

## Future Roadmap (Post-Launch)

1. **Integration Packages**:
   - `datgen-spark`: PySpark integration
   - `datgen-ray`: Distributed generation
   - `datgen-dask`: Dask DataFrame support

2. **Specialized Generators**:
   - `datgen-timeseries`: Time series focus
   - `datgen-text`: NLP data generation
   - `datgen-graph`: Network/graph data

3. **AI Enhancement**:
   - GAN-based generation
   - VAE for complex distributions
   - Differential privacy guarantees

---

*This plan bridges 28 years of evolution while maintaining the simplicity that made DatGen useful.*