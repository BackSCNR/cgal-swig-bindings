# Building CGAL SWIG Bindings with Registration Support

This guide covers building the CGAL SWIG bindings with OpenGR and libpointmatcher registration support.

## Prerequisites

- Python 3.12 (NOT 3.13 - compatibility issues exist)
- CMake 3.5+
- SWIG
- Boost
- CGAL

**macOS:**
```bash
brew install cgal swig boost yaml-cpp cmake
```

**Ubuntu/Debian:**
```bash
sudo apt-get install libcgal-dev swig libboost-all-dev libyaml-cpp-dev cmake
```

## Build Instructions

All commands assume you're in the `cgal-swig-bindings` root directory.

### 1. Initialize Submodules

```bash
git submodule update --init --recursive
```

⚠️ **Important:** Eigen is pinned to tag 3.4.0 (not 5.x) due to C++17 compatibility issues with libnabo and libpointmatcher.

### 2. Build Eigen

```bash
cd external/eigen
mkdir -p build && cd build
cmake .. \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_INSTALL_PREFIX=$PWD/../../install
cmake --install .
cd ../../..
```

### 3. Build OpenGR

```bash
cd external/OpenGR
mkdir -p build && cd build
cmake .. \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_INSTALL_PREFIX=$PWD/../../install \
  -DIO_USE_OPENCV=OFF
cmake --build . --config Release -j4
cmake --install . --config Release
cd ../../..
```

### 4. Build libnabo

```bash
cd external/libnabo
mkdir -p build && cd build
cmake .. \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_INSTALL_PREFIX=$PWD/../../install \
  -DCMAKE_CXX_STANDARD=17 \
  -DEIGEN_INCLUDE_DIR=$PWD/../../eigen
cmake --build . --config Release -j4
cmake --install . --config Release
cd ../../..
```

### 5. Build libpointmatcher

```bash
cd external/libpointmatcher
mkdir -p build && cd build
cmake .. \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_INSTALL_PREFIX=$PWD/../../install \
  -DCMAKE_CXX_STANDARD=17 \
  -DEIGEN_INCLUDE_DIR=$PWD/../../eigen \
  -Dlibnabo_DIR=$PWD/../../install/share/libnabo/cmake
cmake --build . --config Release -j4
cmake --install . --config Release
cd ../../..
```

### 6. Build CGAL Bindings

```bash
# Build Python wheel (recommended)
python setup.py bdist_wheel --cmake-prefix-path="$PWD/external/install"

# Or using CMake directly
mkdir -p build && cd build
cmake .. \
  -DBUILD_PYTHON=ON \
  -DBUILD_JAVA=OFF \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_PREFIX_PATH="$PWD/../external/install"
make -j4
cd ..
```

## Testing

```bash
# Install the wheel
pip install dist/*.whl

# Test registration functions
python -c "
from CGAL import CGAL_Point_set_processing_3 as psp
print('Testing registration functions...')
assert hasattr(psp, 'register_point_sets_opengr'), 'OpenGR missing'
assert hasattr(psp, 'register_point_sets_pointmatcher'), 'PointMatcher missing'
assert hasattr(psp, 'ICP_config_wrapper'), 'ICP_config_wrapper missing'
print('✓ All registration functions available!')
"

# Run example
python examples/python/Point_set_registration_example.py
```

## Troubleshooting

**"OpenGR/libpointmatcher not found"**
- Ensure all external libraries are built and installed to `external/install/`
- Verify `CMAKE_PREFIX_PATH` includes the install directory

**"Eigen errors during build"**
- Confirm Eigen 3.4.0 is installed (not 5.x)
- Ensure `EIGEN_INCLUDE_DIR` points to the correct location

**"Python module import errors"**
- Check Python version is 3.12
- Verify the wheel was built with the correct Python version

## Usage

See `examples/python/Point_set_registration_example.py` for complete examples of using the registration functions.
