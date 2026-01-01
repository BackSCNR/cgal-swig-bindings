#!/bin/bash
set -ex

# This script runs inside the manylinux2014 Docker container
# It builds all external dependencies and the CGAL wheel

# Environment variables expected:
# - CGAL_PYTHON_MODULE_VERSION
# - PYTHON_VERSION_MINOR (e.g., 12 for Python 3.12)

PYTHON_BIN="/opt/python/cp3${PYTHON_VERSION_MINOR}-cp3${PYTHON_VERSION_MINOR}/bin/python"
INSTALL_PREFIX="/build/install"
PAR_JOBS=$(nproc)

echo "=== Building external dependencies inside manylinux2014 container ==="
echo "Python: $PYTHON_BIN"
echo "Install prefix: $INSTALL_PREFIX"
echo "Parallel jobs: $PAR_JOBS"

cd /cgal-bindings

# Install build dependencies
yum install -y wget tar bzip2 boost-devel

# Build Eigen
echo "=== Building Eigen ==="
cd external/eigen
mkdir -p build && cd build
cmake .. \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_INSTALL_PREFIX=$INSTALL_PREFIX \
  -DBUILD_TESTING=OFF \
  -DEIGEN_BUILD_DOC=OFF \
  -DEIGEN_BUILD_PKGCONFIG=OFF \
  -DEIGEN_BUILD_BLAS=OFF \
  -DEIGEN_BUILD_LAPACK=OFF \
  -DEIGEN_TEST_NOQT=ON \
  -DEIGEN_LEAVE_TEST_IN_ALL_TARGET=OFF
cmake --install . --config Release
cd ../../..

# Build yaml-cpp
echo "=== Building yaml-cpp ==="
cd /tmp
wget -q https://github.com/jbeder/yaml-cpp/archive/refs/tags/0.8.0.tar.gz
tar xzf 0.8.0.tar.gz
cd yaml-cpp-0.8.0
mkdir build && cd build
cmake .. \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_INSTALL_PREFIX=$INSTALL_PREFIX \
  -DCMAKE_POLICY_VERSION_MINIMUM=3.5 \
  -DYAML_CPP_BUILD_TESTS=OFF \
  -DYAML_CPP_BUILD_TOOLS=OFF \
  -DYAML_BUILD_SHARED_LIBS=ON
cmake --build . --config Release -j$PAR_JOBS
cmake --install . --config Release
cd /cgal-bindings

# Build libnabo
echo "=== Building libnabo ==="
cd external/libnabo
mkdir -p build && cd build
cmake .. \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_INSTALL_PREFIX=$INSTALL_PREFIX \
  -DCMAKE_CXX_STANDARD=17 \
  -DEIGEN_INCLUDE_DIR=/cgal-bindings/external/eigen \
  -DUSE_OPEN_MP=OFF \
  -DBUILD_SHARED_LIBS=ON
cmake --build . --config Release -j$PAR_JOBS
cmake --install . --config Release
cd ../../..

# Build libpointmatcher
echo "=== Building libpointmatcher ==="
cd external/libpointmatcher
mkdir -p build && cd build
cmake .. \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_INSTALL_PREFIX=$INSTALL_PREFIX \
  -DCMAKE_CXX_STANDARD=17 \
  -DEIGEN_INCLUDE_DIR=/cgal-bindings/external/eigen \
  -Dlibnabo_DIR=$INSTALL_PREFIX/share/libnabo/cmake \
  -DBUILD_TESTS=OFF \
  -DBUILD_TESTING=OFF \
  -DENABLE_TESTING=OFF \
  -DBUILD_EXAMPLES=OFF \
  -DBUILD_SHARED_LIBS=ON
cmake --build . --config Release -j$PAR_JOBS
cmake --install . --config Release
# Workaround for include path issue
cd $INSTALL_PREFIX && ln -sf include/pointmatcher pointmatcher
cd /cgal-bindings/external/libpointmatcher

cd ../../..

# Build OpenGR
echo "=== Building OpenGR ==="
cd external/OpenGR
mkdir -p build && cd build
cmake .. \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_INSTALL_PREFIX=$INSTALL_PREFIX \
  -DIO_USE_OPENCV=OFF
cmake --build . --config Release -j$PAR_JOBS
cmake --install . --config Release
cd ../../..

# Build CGAL wheel
echo "=== Building CGAL wheel ==="
export LD_LIBRARY_PATH=$INSTALL_PREFIX/lib:$LD_LIBRARY_PATH
mkdir -p build/build-python/CGAL

$PYTHON_BIN -m pip install --upgrade pip setuptools wheel numpy

$PYTHON_BIN setup.py bdist_wheel \
  --cmake-prefix-path="$INSTALL_PREFIX;/cgal" \
  --python-executable=$PYTHON_BIN

# Repair wheel with auditwheel
echo "=== Repairing wheel with auditwheel ==="
$PYTHON_BIN -m pip install auditwheel patchelf

auditwheel repair dist/*.whl -w wheelhouse

echo "=== Build complete! ==="
ls -lh wheelhouse/
