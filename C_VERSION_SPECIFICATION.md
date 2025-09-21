# C Version Specification (Gold Standard)
*This document defines the actual behavior of the C version (v3.1.1) which serves as the gold standard for the Python port*

## Data Types Supported

The C version supports multiple data types through the `-X` parameter:

### 1. **Nominal (N)** - Categorical Data
- Default behavior when using `-A` parameter
- Generates letters from 'a' to domain size (e.g., a-j for domain=10)
- Example: `./datgen -O 10 -A 5 -d 10`
- Output: Letters like `a`, `b`, `c`, etc.

### 2. **Continuous (C)** - Numeric Data
- Enabled via `-X` parameter with `C` flag
- Generates floating-point numbers within specified range
- Example: `./datgen -O 10 -X"-1/1,1,R,C"`
- Output: Numbers like `-0.106833`, `0.772857`, etc.

### 3. **Ordinal (O)** - Ordered Categorical
- Enabled via `-X` parameter with `O` flag
- Similar to Nominal but with inherent ordering

## Parameter Mapping

| C Version | Python Port | Description |
|-----------|------------|-------------|
| `-O` | `n_samples` | Number of objects/instances |
| `-A` | `n_features` | Number of attributes (generates Nominal by default) |
| `-d` | `domain_size` | Domain size for attributes |
| `-R` | (affects `n_classes`) | Number of DNF rules |
| `-p` | `random_state` | Pseudo-random (deterministic) mode |
| `-r` | `distribution` | Rule distribution (0=uniform, 1=random, 2=standard normal) |
| `-X` | `attribute_spec` | Explicit attribute definition |

## C Version Capabilities

### Distribution Types (`-r` parameter)
- `0`: Uniform distribution
- `1`: Random distribution (default)
- `2`: Standard normal distribution

### Attribute Specification Format (`-X`)
```
DOM#,DISJ#,[R,I],[N,O,C],[V,M]
```
Where:
- `DOM#`: Domain size (min/max for Continuous, single value for Nominal)
- `DISJ#`: Number of disjunctions
- `R/I`: Relevant or Irrelevant attribute
- `N/O/C`: Nominal, Ordinal, or Continuous
- `V/M`: Visible or Masked

### Examples

#### Categorical Data (Default)
```bash
./datgen -O 100 -A 5 -d 10 -p
# Generates 100 samples with 5 categorical features (letters a-j)
```

#### Continuous Data
```bash
./datgen -O 100 -X"-1/1,1,R,C:-1/1,1,R,C:-1/1,1,R,C" -p
# Generates 100 samples with 3 continuous features (range -1 to 1)
```

#### Mixed Types
```bash
./datgen -O 100 -X"10,1,R,N:0/1,1,R,C:-5/5,1,R,C" -p
# Feature 1: Nominal (categorical)
# Feature 2: Continuous [0,1]
# Feature 3: Continuous [-5,5]
```

## Known Issues in C Version

1. **Memory Corruption**: Some parameter combinations cause class label corruption
   - Manifests as garbage values in class column (e.g., `c825830960`)
   - Appears more frequently with continuous data generation

2. **Buffer Overflows**: Large parameter values can cause segmentation faults

3. **Limited Error Handling**: Invalid parameters may produce undefined behavior

## Test Implications

Based on the C version being the gold standard:

1. **Statistical tests** should be adapted to handle both:
   - Categorical data (when using `-A` parameter)
   - Continuous data (when using `-X` with `C` flag)

2. **Performance benchmarks** should consider:
   - Categorical generation is the default mode
   - Continuous generation requires explicit `-X` specification

3. **Compatibility tests** should:
   - Match the exact output format including whitespace
   - Handle both categorical and continuous modes
   - Work around known memory corruption issues

## Python Implementation Requirements

The Python port must:

1. **Support both modes**:
   - Default categorical mode (matching `-A` behavior)
   - Continuous mode (matching `-X ... C` behavior)

2. **Maintain parameter compatibility**:
   - Accept C-style parameters for backward compatibility
   - Map to Python-friendly parameter names

3. **Reproduce exact behavior**:
   - Same random sequences with same seeds
   - Same data distributions
   - Same output format

4. **Improve on known issues**:
   - Fix memory corruption problems
   - Add proper error handling
   - Maintain backward compatibility despite fixes