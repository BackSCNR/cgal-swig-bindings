"""
Example demonstrating point cloud registration with OpenGR and PointMatcher.

IMPORTANT: Parameter Selection for OpenGR
==========================================
The 'accuracy' parameter must be scaled to your point cloud size:
  - accuracy should be 0.5-5% of point cloud diameter
  - For small objects (diameter ~1-10): use accuracy = 0.01-0.5
  - For large scenes (diameter ~1000-10000): use accuracy = 10-500
  - If registration freezes, accuracy is likely too tight!

Performance Tips:
  - For large clouds (>10k points), downsample first (use every Nth point)
  - Reduce number_of_samples to 100-200 for faster registration
  - Increase accuracy tolerance for faster convergence
  - Use maximum_running_time to prevent hanging

This example demonstrates registration on both synthetic and real data.
"""

from CGAL.CGAL_Point_set_3 import Point_set_3
import CGAL.CGAL_Point_set_processing_3 as psp
from CGAL.CGAL_Kernel import Point_3
import math
import random

def create_bunny_shape(num_points=500):
    """Create a simple bunny-like shape with synthetic points"""
    ps = Point_set_3()

    random.seed(42)

    # Body (ellipsoid)
    for i in range(int(num_points * 0.5)):
        theta = random.uniform(0, 2 * math.pi)
        phi = random.uniform(0, math.pi)
        r = random.gauss(1.0, 0.05)

        x = r * 0.6 * math.sin(phi) * math.cos(theta)
        y = r * 0.6 * math.sin(phi) * math.sin(theta)
        z = r * 0.8 * math.cos(phi)

        ps.insert(Point_3(x, y, z))

    # Head (sphere)
    for i in range(int(num_points * 0.3)):
        theta = random.uniform(0, 2 * math.pi)
        phi = random.uniform(0, math.pi)
        r = random.gauss(0.4, 0.02)

        x = r * math.sin(phi) * math.cos(theta)
        y = r * math.sin(phi) * math.sin(theta)
        z = r * math.cos(phi) + 1.0

        ps.insert(Point_3(x, y, z))

    # Ears
    for i in range(int(num_points * 0.2)):
        theta = random.uniform(0, math.pi / 4)
        phi = random.uniform(0, math.pi)
        r = random.gauss(0.15, 0.01)

        # Left ear
        x = r * math.sin(phi) * math.cos(theta) - 0.3
        y = r * math.sin(phi) * math.sin(theta)
        z = r * 2.0 * math.cos(phi) + 1.3
        ps.insert(Point_3(x, y, z))

        # Right ear
        x = r * math.sin(phi) * math.cos(theta) + 0.3
        ps.insert(Point_3(x, y, z))

    return ps

def apply_transformation(ps, tx=0, ty=0, tz=0, angle=0):
    """Apply translation and rotation to a copy of the point set"""
    # Create a copy by inserting points into a new point set
    ps_copy = Point_set_3()

    # Rotation matrix around Z-axis
    cos_a = math.cos(angle)
    sin_a = math.sin(angle)

    # Copy and transform each point
    for idx in range(ps.size()):
        p = ps.point(idx)

        # Rotate around Z-axis
        x_rot = p.x() * cos_a - p.y() * sin_a
        y_rot = p.x() * sin_a + p.y() * cos_a
        z_rot = p.z()

        # Translate
        x_final = x_rot + tx
        y_final = y_rot + ty
        z_final = z_rot + tz

        ps_copy.insert(Point_3(x_final, y_final, z_final))

    return ps_copy

# =============================================================================
# Example 1: OpenGR Registration (Super4PCS algorithm)
# =============================================================================
print("=" * 60)
print("OpenGR Registration Example")
print("=" * 60)

# Create synthetic point clouds
print("Creating synthetic point clouds...")
point_set_1 = create_bunny_shape(1000)
# Create second point set with transformation
point_set_2 = apply_transformation(point_set_1, tx=0.5, ty=0.3, tz=0.2, angle=math.pi/6)

print(f"Point set 1: {point_set_1.size()} points")
print(f"Point set 2: {point_set_2.size()} points (transformed)")

# Save original point sets for reference
point_set_1.write("original_source.ply")
point_set_2.write("original_target.ply")
print("Saved original point sets to 'original_source.ply' and 'original_target.ply'")

# Estimate normals
print("Estimating normals...")
psp.pca_estimate_normals(point_set_1, 24)
psp.pca_estimate_normals(point_set_2, 24)

# Perform OpenGR registration (Super4PCS)
print("\nPerforming OpenGR registration with Super4PCS...")
try:
    score = psp.register_point_sets_opengr(
        point_set_1,
        point_set_2,
        number_of_samples=200,
        maximum_normal_deviation=90.0,
        accuracy=0.01,
        overlap=0.8,             # High overlap since we're using copies
        maximum_running_time=60
    )
    print(f"Registration score: {score}")
    print("Point set 2 has been aligned to point set 1")

    # Save the aligned result
    point_set_2.write("aligned_opengr.ply")
    print("Saved aligned point set to 'aligned_opengr.ply'")
except Exception as e:
    print(f"OpenGR registration failed or not available: {e}")

# =============================================================================
# Example 2: PointMatcher Registration (ICP algorithm)
# =============================================================================
print("\n" + "=" * 60)
print("PointMatcher ICP Registration Example")
print("=" * 60)

# Recreate point sets for ICP
point_set_1 = create_bunny_shape(1000)
point_set_2 = apply_transformation(point_set_1, tx=0.1, ty=0.1, tz=0.05, angle=math.pi/12)

# Estimate normals
psp.pca_estimate_normals(point_set_1, 24)
psp.pca_estimate_normals(point_set_2, 24)

# Configure ICP
print("\nConfiguring ICP parameters...")

try:
    # Simple ICP configuration for demonstration
    matcher = psp.ICP_config_wrapper("KDTreeMatcher")
    matcher.add_parameter("knn", "1")

    minimizer = psp.ICP_config_wrapper("PointToPlaneErrorMinimizer")

    checkers = psp.ICP_config_vector()
    counter_checker = psp.ICP_config_wrapper("CounterTransformationChecker")
    counter_checker.add_parameter("maxIterationCount", "150")
    checkers.append(counter_checker)

    # Perform ICP registration
    print("Performing ICP registration...")
    converged = psp.register_point_sets_pointmatcher(
        point_set_1,
        point_set_2,
        matcher=matcher,
        error_minimizer=minimizer,
        transformation_checkers=checkers
    )

    if converged:
        print("ICP converged successfully!")
        point_set_2.write("aligned_icp.ply")
        print("Saved aligned point set to 'aligned_icp.ply'")
    else:
        print("ICP did not converge - registration failed")

except Exception as e:
    print(f"PointMatcher registration failed or not available: {e}")

# =============================================================================
# Example 3: Using available point cloud data
# =============================================================================
print("\n" + "=" * 60)
print("Example with Real Data (if available)")
print("=" * 60)

try:
    # Try to load real data if available
    import os
    datadir = '../data'

    if os.path.exists(f'{datadir}/elephant.off'):
        print("Loading elephant.off...")
        ps = Point_set_3(f'{datadir}/elephant.off')

        # Create a transformed copy
        ps_transformed = apply_transformation(ps, tx=0.2, ty=0.1, tz=0.0, angle=math.pi/8)

        print(f"Loaded point set with {ps.size()} points")

        # Estimate normals
        if not ps.has_normal_map():
            print("Estimating normals...")
            psp.pca_estimate_normals(ps, 24)

        if not ps_transformed.has_normal_map():
            psp.pca_estimate_normals(ps_transformed, 24)

        # Register with OpenGR
        # Note: For large point clouds, we need to adjust parameters based on scale
        # Use downsampling for faster registration
        print("Downsampling for faster registration...")
        ps_downsampled = Point_set_3()
        ps_transformed_downsampled = Point_set_3()

        # Copy every 10th point for faster registration
        for i in range(0, ps.size(), 10):
            ps_downsampled.insert(ps.point(i))
        for i in range(0, ps_transformed.size(), 10):
            ps_transformed_downsampled.insert(ps_transformed.point(i))

        print(f"Downsampled to {ps_downsampled.size()} points")

        # Estimate normals on downsampled clouds
        psp.pca_estimate_normals(ps_downsampled, 24)
        psp.pca_estimate_normals(ps_transformed_downsampled, 24)

        print("Registering with OpenGR (on downsampled data)...")
        try:
            # Adjusted parameters for large-scale data:
            # - accuracy should be ~1-5% of point cloud diameter (9270 * 0.01 = ~93)
            # - fewer samples for faster computation
            # - shorter timeout since we're using downsampled data
            score = psp.register_point_sets_opengr(
                ps_downsampled,
                ps_transformed_downsampled,
                number_of_samples=200,
                accuracy=50.0,  # ~0.5% of diameter (9270 * 0.005 = 46)
                overlap=0.7,
                maximum_running_time=30
            )
            print(f"Registration score: {score}")
            ps_transformed_downsampled.write("example_aligned.ply")
            print("Saved aligned point set to 'example_aligned.ply'")
        except Exception as e:
            print(f"Registration failed: {e}")
    else:
        print("No real data found (this is OK - synthetic examples above demonstrate registration)")

except Exception as e:
    print(f"Real data example skipped: {e}")

print("\n" + "=" * 60)
print("Registration examples completed!")
print("=" * 60)
print("\nOpen these files in a viewer like MeshLab or CloudCompare to see the results!")
