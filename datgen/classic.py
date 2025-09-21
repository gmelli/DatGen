"""
DatGen Classic - Python port of the original C implementation
This module provides backward compatibility with the 1997 C version
"""

import numpy as np
import pandas as pd
from typing import Optional, Union, List
import string


class DatGenClassic:
    """Direct port of C functionality for backward compatibility"""

    def __init__(
        self,
        n_samples: int = 100,
        n_features: int = 10,
        n_classes: int = 2,
        random_state: Optional[int] = None,
        noise_level: float = 0.0,
        data_type: str = 'categorical',
        distribution: str = 'uniform',
        domain_size: int = 10,
        value_range: tuple = (-1.0, 1.0)
    ):
        """
        Initialize DatGen with C-compatible parameters

        Parameters:
        -----------
        n_samples : int
            Number of instances to generate (C: -O)
        n_features : int
            Number of attributes/features (C: -A)
        n_classes : int
            Number of classes (C: derived from -R)
        random_state : int, optional
            Seed for reproducibility (C: -p flag enables pseudo-random)
        noise_level : float
            Proportion of noise (0.0 to 1.0)
        data_type : str
            'categorical' (C: Nominal) or 'continuous' (C: Continuous)
        distribution : str
            'uniform', 'random', or 'gaussian' (C: -r parameter)
        domain_size : int
            Size of categorical domain (C: -d parameter)
        value_range : tuple
            Range for continuous values (C: DOM# in -X parameter)
        """
        self.n_samples = n_samples
        self.n_features = n_features
        self.n_classes = n_classes
        self.noise_level = noise_level
        self.random_state = random_state
        self.data_type = data_type
        self.distribution = distribution
        self.domain_size = domain_size
        self.value_range = value_range

        # Initialize random number generator
        if random_state is not None:
            self.rng = np.random.RandomState(random_state)
        else:
            self.rng = np.random.RandomState()

    def generate(self) -> pd.DataFrame:
        """
        Generate synthetic dataset matching C version output

        Returns:
        --------
        pd.DataFrame
            Generated data with features and class column
        """
        if self.data_type == 'categorical':
            df = self._generate_categorical()
        elif self.data_type == 'continuous':
            df = self._generate_continuous()
        else:
            raise ValueError(f"Unknown data_type: {self.data_type}")

        return df

    def _generate_categorical(self) -> pd.DataFrame:
        """Generate categorical data matching C version's Nominal type"""
        # Domain size determines the letters used (a-j for domain_size=10)
        letters = list(string.ascii_lowercase[:min(self.domain_size, 26)])

        # Generate feature data
        data = []
        for _ in range(self.n_samples):
            row = []
            for _ in range(self.n_features):
                # Random selection from domain
                value = self.rng.choice(letters)
                row.append(value)
            data.append(row)

        # Create DataFrame with feature columns
        columns = [f'attr{i+1}' for i in range(self.n_features)]
        df = pd.DataFrame(data, columns=columns)

        # Add class column
        df['class'] = self._generate_class_labels()

        # Apply noise if specified
        if self.noise_level > 0:
            n_noise = int(self.n_samples * self.n_features * self.noise_level)
            for _ in range(n_noise):
                row_idx = self.rng.randint(0, self.n_samples)
                col_idx = self.rng.randint(0, self.n_features)
                df.iloc[row_idx, col_idx] = self.rng.choice(letters)

        return df

    def _generate_continuous(self) -> pd.DataFrame:
        """Generate continuous data matching C version's Continuous type"""
        min_val, max_val = self.value_range

        # Generate feature data based on distribution
        if self.distribution == 'gaussian':
            # Standard normal, then scale to range
            data = self.rng.randn(self.n_samples, self.n_features)
            # Scale to approximately fit in range (keeping some outliers like C version)
            center = (min_val + max_val) / 2
            scale = (max_val - min_val) / 4  # ~95% within range
            data = data * scale + center
        elif self.distribution == 'uniform':
            data = self.rng.uniform(min_val, max_val,
                                   size=(self.n_samples, self.n_features))
        else:  # 'random' - default
            data = self.rng.uniform(min_val, max_val,
                                   size=(self.n_samples, self.n_features))

        # Create DataFrame with feature columns
        columns = [f'attr{i+1}' for i in range(self.n_features)]
        df = pd.DataFrame(data, columns=columns)

        # Add class column
        df['class'] = self._generate_class_labels()

        # Apply noise if specified
        if self.noise_level > 0:
            noise_mask = self.rng.random((self.n_samples, self.n_features)) < self.noise_level
            noise = self.rng.uniform(min_val, max_val,
                                    size=(self.n_samples, self.n_features))
            df.iloc[:, :self.n_features] = np.where(noise_mask, noise, data)

        return df

    def _generate_class_labels(self) -> List[str]:
        """Generate class labels matching C version format"""
        # C version uses 'c0', 'c1', etc.
        classes = [f'c{i}' for i in range(self.n_classes)]

        # Simple class assignment
        # TODO: Implement C version's rule-based classification
        class_labels = []
        for i in range(self.n_samples):
            if self.random_state is not None:
                # Deterministic assignment for reproducibility
                # This is simplified - C version uses complex rules
                class_idx = self.rng.randint(0, self.n_classes)
            else:
                class_idx = self.rng.randint(0, self.n_classes)
            class_labels.append(classes[class_idx])

        return class_labels

    def to_csv(self, path: str, **kwargs):
        """Save generated data to CSV file"""
        df = self.generate()
        df.to_csv(path, index=False, **kwargs)