
# DatGen - Synthetic Data Generator (1997)

[![Historical](https://img.shields.io/badge/Historical-1997-blue.svg)](https://www.datasetgenerator.com/)
[![Language](https://img.shields.io/badge/Language-C-green.svg)](src/datgen.c)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)
[![Modernization](https://img.shields.io/badge/Modernization-In%20Progress-orange.svg)](#modernization-roadmap)

> **Original project site**: http://www.datasetgenerator.com/

## 🏛️ Historical Significance

DatGen is one of the **earliest synthetic data generators** (1997-1999), created before synthetic data became a mainstream concept. This tool pioneered approaches that modern synthetic data platforms still use today.

### Timeline
- **1997**: Initial development for classification algorithm research
- **1998**: Published as "Synthetic Classification Data Sets (SCDS) generator"
- **1999**: Version 3.1 released (current C implementation)
- **2025**: Modernization initiative launched

## 📖 Original Purpose

DatGen was designed to solve critical problems in machine learning research:

1. **Controlled Experimentation**: Generate datasets with known statistical properties
2. **Reproducible Benchmarks**: Create consistent test data for algorithm comparison
3. **Privacy-Safe Testing**: Test algorithms without real-world data concerns
4. **Scalability Testing**: Generate datasets of arbitrary size

These needs remain relevant today, making DatGen's concepts timeless.

## 🔧 Classic Version (C Implementation)

### Quick Start
```bash
# Compile the classic version
cd src
make

# Generate a simple dataset
./datgen -n 1000 -m 10 -c 3 > dataset.csv
```

### Parameters
- `-n`: Number of instances to generate
- `-m`: Number of attributes/features
- `-c`: Number of classes
- `-s`: Random seed for reproducibility

## 🚀 Modernization Roadmap

### Phase 1: Python Port (2025 Q1)
- [ ] Core generation engine in Python
- [ ] NumPy/Pandas integration
- [ ] Maintain backward compatibility with C version
- [ ] Add JSON/Parquet output formats

### Phase 2: Modern Features (2025 Q2)
- [ ] Scikit-learn compatible API
- [ ] Time series data generation
- [ ] Text/NLP synthetic data
- [ ] Graph/network data structures

### Phase 3: Cloud Native (2025 Q3)
- [ ] REST API interface
- [ ] Docker containerization
- [ ] Streaming data generation
- [ ] Distributed generation for big data

### Phase 4: AI Enhancement (2025 Q4)
- [ ] GAN-based generation options
- [ ] Differential privacy guarantees
- [ ] LLM integration for semantic data

## 🎯 Modern Python API (Coming Soon)

```python
from datgen import DataGenerator

# Classic compatible mode
gen = DataGenerator.classic(
    n_samples=1000,
    n_features=10,
    n_classes=3,
    random_state=42
)
df = gen.generate()

# Modern fluent API
gen = DataGenerator() \
    .with_samples(10000) \
    .with_features(20, types=['numeric', 'categorical']) \
    .with_classes(5, balance='imbalanced') \
    .with_noise(0.1)

# Generate different formats
df = gen.to_pandas()
gen.to_parquet("data.parquet")
gen.to_json("data.json", streaming=True)
```

## 🔬 Why Synthetic Data Still Matters (2025)

Modern use cases that DatGen addresses:

1. **Privacy Compliance**: GDPR/CCPA compliant testing
2. **ML Pipeline Testing**: CI/CD for ML systems
3. **Edge Case Generation**: Test rare events
4. **Fairness Testing**: Generate balanced datasets
5. **Teaching Tool**: Learn ML without real data

## 📚 Academic Impact

DatGen influenced early synthetic data research and classification algorithm benchmarking.

### Citing DatGen
```bibtex
@software{melli1997datgen,
  author = {Melli, Gabor},
  title = {DatGen: A Synthetic Data Generator},
  year = {1997},
  url = {https://github.com/gmelli/DatGen}
}
```

## 🤝 Contributing

This project bridges 28 years of synthetic data evolution. Contributions welcome for:
- Python port development
- Modern feature additions
- Documentation improvements
- Use case examples

## 📜 License

MIT License - Same as 1997, because good ideas should be free.

---

*"Perfect data for an imperfect world" - Still true after 28 years*

**Repository Status**: 🏛️ Historical Artifact | 🚧 Active Modernization | 📚 Educational Resource
