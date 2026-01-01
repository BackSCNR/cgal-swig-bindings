#!/usr/bin/env python3
"""
Test script to verify Point_set_processing_3 bindings and registration functions.
"""

import sys

# Add the build output to Python path
sys.path.insert(0, '/Users/weichen/Downloads/cgal-swig-bindings/build/build-python')

print("=" * 60)
print("Testing CGAL Point Set Processing Bindings")
print("=" * 60)

# Test basic imports
print("\n1. Testing basic imports...")
try:
    from CGAL.CGAL_Kernel import Point_3, Vector_3
    from CGAL.CGAL_Point_set_3 import Point_set_3
    from CGAL.CGAL_Point_set_processing_3 import *
    print("✓ All basic imports successful")
except ImportError as e:
    print(f"✗ Import failed: {e}")
    sys.exit(1)

# Test basic Point_set_processing_3 functions
print("\n2. Testing basic point set processing functions...")
try:
    # Create a simple point set
    ps = Point_set_3()
    ps.insert(Point_3(0, 0, 0))
    ps.insert(Point_3(1, 0, 0))
    ps.insert(Point_3(0, 1, 0))
    ps.insert(Point_3(0, 0, 1))
    ps.insert(Point_3(1, 1, 1))

    print(f"  Created point set with {ps.size()} points")

    # Test normal estimation
    pca_estimate_normals(ps, 3)
    print(f"  ✓ Normal estimation works")

    # Test average spacing
    spacing = compute_average_spacing(ps, 3)
    print(f"  ✓ Average spacing: {spacing:.4f}")

    print("✓ Basic functions work correctly")
except Exception as e:
    print(f"✗ Error testing basic functions: {e}")
    import traceback
    traceback.print_exc()
    sys.exit(1)

# Test registration functions availability
print("\n3. Checking registration function availability...")

# Check for OpenGR
try:
    # Try to access the function
    register_point_sets_opengr
    print("  ✓ register_point_sets_opengr is available")
    opengr_available = True
except NameError:
    print("  ✗ register_point_sets_opengr is NOT available")
    print("    (OpenGR library was not found during compilation)")
    opengr_available = False

# Check for PointMatcher
try:
    register_point_sets_pointmatcher
    print("  ✓ register_point_sets_pointmatcher is available")
    pointmatcher_available = True
except NameError:
    print("  ✗ register_point_sets_pointmatcher is NOT available")
    print("    (libpointmatcher library was not found during compilation)")
    pointmatcher_available = False

# Check for ICP_config_wrapper
try:
    ICP_config_wrapper
    print("  ✓ ICP_config_wrapper class is available")
except NameError:
    print("  ✗ ICP_config_wrapper class is NOT available")

print("\n" + "=" * 60)
print("Summary:")
print("=" * 60)
print(f"Basic Point Set Processing: ✓ Working")
print(f"OpenGR Registration:        {'✓ Available' if opengr_available else '✗ Not Available'}")
print(f"PointMatcher Registration:  {'✓ Available' if pointmatcher_available else '✗ Not Available'}")

if not (opengr_available or pointmatcher_available):
    print("\nNote: Registration functions are not available because")
    print("OpenGR and libpointmatcher were not found during compilation.")
    print("\nTo enable them:")
    print("  1. Install OpenGR: https://github.com/STORM-IRIT/OpenGR")
    print("  2. Install libpointmatcher: https://github.com/norlab-ulaval/libpointmatcher")
    print("  3. Rebuild the bindings")

print("\n" + "=" * 60)
print("Test completed successfully!")
print("=" * 60)
