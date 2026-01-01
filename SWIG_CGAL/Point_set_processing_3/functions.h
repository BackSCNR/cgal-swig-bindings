// ------------------------------------------------------------------------------
// Copyright (c) 2013 GeometryFactory (FRANCE)
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-Commercial
// ------------------------------------------------------------------------------

#ifndef SWIG_CGAL_POINT_SET_PROCESSING_3_H
#define SWIG_CGAL_POINT_SET_PROCESSING_3_H

#include <SWIG_CGAL/Kernel/Point_3.h>
#include <SWIG_CGAL/Kernel/Vector_3.h>
#include <SWIG_CGAL/Point_set_3/Point_set_3.h>

#include <CGAL/bilateral_smooth_point_set.h>
#include <CGAL/compute_average_spacing.h>
#include <CGAL/edge_aware_upsample_point_set.h>
#include <CGAL/estimate_scale.h>
#include <CGAL/grid_simplify_point_set.h>
#include <CGAL/hierarchy_simplify_point_set.h>
#include <CGAL/jet_estimate_normals.h>
#include <CGAL/jet_smooth_point_set.h>
#include <CGAL/mst_orient_normals.h>
#include <CGAL/pca_estimate_normals.h>
#include <CGAL/random_simplify_point_set.h>
#include <CGAL/remove_outliers.h>
#include <CGAL/vcm_estimate_normals.h>
#include <CGAL/wlop_simplify_and_regularize_point_set.h>

#include <CGAL/OpenGR/register_point_sets.h>
#include <CGAL/pointmatcher/register_point_sets.h>
#include <vector>
#include <map>
#include <string>

#ifdef CGAL_LINKED_WITH_TBB
typedef CGAL::Parallel_tag Concurrency_tag;
#else
typedef CGAL::Sequential_tag Concurrency_tag;
#endif

namespace CGAL_SWIG {

void bilateral_smooth_point_set (Point_set_3_wrapper<CGAL_PS3> point_set, int k,
                                 double neighbor_radius = 0.,
                                 double sharpness_angle = 30.)
{
  CGAL::bilateral_smooth_point_set<Concurrency_tag>
    (point_set.get_data(), k,
     point_set.get_data().parameters().neighbor_radius(neighbor_radius).
     sharpness_angle(sharpness_angle));
}

double compute_average_spacing(Point_set_3_wrapper<CGAL_PS3> point_set, int k)
{
  return CGAL::compute_average_spacing<Concurrency_tag> (point_set.get_data(), k);
}

void edge_aware_upsample_point_set (Point_set_3_wrapper<CGAL_PS3> point_set,
                                    double sharpness_angle = 30.,
                                    double edge_sensitivity = 1.,
                                    double neighbor_radius = -1.,
                                    int number_of_output_points = 1000)
{
  CGAL::edge_aware_upsample_point_set<Concurrency_tag>
    (point_set.get_data(),
     boost::make_function_output_iterator
     ([&](const std::pair<EPIC_Kernel::Point_3, EPIC_Kernel::Vector_3>& p)
      {
        point_set.get_data().insert (p.first, p.second);
      }),
     point_set.get_data().parameters().sharpness_angle(sharpness_angle).
     edge_sensitivity(edge_sensitivity).
     neighbor_radius(neighbor_radius).
     number_of_output_points(number_of_output_points));
}

int estimate_global_k_neighbor_scale (Point_set_3_wrapper<CGAL_PS3> point_set)
{
  return CGAL::estimate_global_k_neighbor_scale (point_set.get_data());
}

double estimate_global_range_scale (Point_set_3_wrapper<CGAL_PS3> point_set)
{
  return CGAL::estimate_global_range_scale (point_set.get_data(), point_set.get_data().parameters());
}

// TODO if needed: estimate local scales

void grid_simplify_point_set (Point_set_3_wrapper<CGAL_PS3> point_set, double epsilon)
{
  point_set.get_data().remove_from
    (CGAL::grid_simplify_point_set (point_set.get_data(), epsilon));
}

void hierarchy_simplify_point_set (Point_set_3_wrapper<CGAL_PS3> point_set,
                                   int size = 10,
                                   double maximum_variation = 1./3.)
{
  point_set.get_data().remove_from
    (CGAL::hierarchy_simplify_point_set (point_set.get_data(),
                                         point_set.get_data().parameters().size(size).
                                         maximum_variation(maximum_variation)));
}

void jet_estimate_normals (Point_set_3_wrapper<CGAL_PS3> point_set, int k,
                           double neighbor_radius = 0.,
                           int degree_fitting = 2)
{
  point_set.get_data().add_normal_map();
  CGAL::jet_estimate_normals<Concurrency_tag>
    (point_set.get_data(), k,
     point_set.get_data().parameters().neighbor_radius (neighbor_radius).
     degree_fitting (degree_fitting));
}

void jet_smooth_point_set (Point_set_3_wrapper<CGAL_PS3> point_set, int k,
                           double neighbor_radius = 0.,
                           int degree_fitting = 2,
                           int degree_monge = 2)
{
  CGAL::jet_smooth_point_set<Concurrency_tag>
    (point_set.get_data(), k,
     point_set.get_data().parameters().neighbor_radius(neighbor_radius).
     degree_fitting(degree_fitting).
     degree_monge(degree_monge));
}

void mst_orient_normals (Point_set_3_wrapper<CGAL_PS3> point_set, int k,
                         double neighbor_radius = 0.,
                         typename Point_set_3_wrapper<CGAL_PS3>::Int_map
                         constrained_map = typename Point_set_3_wrapper<CGAL_PS3>::Int_map())
{
  if (constrained_map.is_valid())
    point_set.get_data().remove_from
      (CGAL::mst_orient_normals
       (point_set.get_data(), k,
        point_set.get_data().parameters().neighbor_radius (neighbor_radius).
        point_is_constrained_map(constrained_map.get_data())));
  else
    point_set.get_data().remove_from
      (CGAL::mst_orient_normals
       (point_set.get_data(), k,
        point_set.get_data().parameters().neighbor_radius (neighbor_radius)));
}

void pca_estimate_normals (Point_set_3_wrapper<CGAL_PS3> point_set, int k,
                           double neighbor_radius = 0.)
{
  point_set.get_data().add_normal_map();
  CGAL::pca_estimate_normals<Concurrency_tag>
    (point_set.get_data(), k, point_set.get_data().parameters().neighbor_radius(neighbor_radius));
}

void random_simplify_point_set (Point_set_3_wrapper<CGAL_PS3> point_set, double removed_percentage)
{
  point_set.get_data().remove_from
    (CGAL::random_simplify_point_set (point_set.get_data(), removed_percentage));
}

void remove_outliers (Point_set_3_wrapper<CGAL_PS3> point_set, int k,
                      double neighbor_radius = 0.,
                      double threshold_percent = 10.,
                      double threshold_distance = 0.)
{
  point_set.get_data().remove_from(
      CGAL::remove_outliers<Concurrency_tag>
     (point_set.get_data(), k,
                            point_set.get_data().parameters().neighbor_radius(neighbor_radius).
                            threshold_percent(threshold_percent).
                            threshold_distance(threshold_distance)));
}

// TODO: structure_point_set() if/once Shape_detection is wrapped

void vcm_estimate_normals (Point_set_3_wrapper<CGAL_PS3> point_set,
                           double offset_radius, double convolution_radius, int k = 0)
{
  point_set.get_data().add_normal_map();
  if (k == 0)
    CGAL::vcm_estimate_normals (point_set.get_data(), offset_radius, convolution_radius);
  else
    CGAL::vcm_estimate_normals (point_set.get_data(), offset_radius, (unsigned int)k);
}

void wlop_simplify_and_regularize_point_set (Point_set_3_wrapper<CGAL_PS3> input,
                                             Point_set_3_wrapper<CGAL_PS3> output,
                                             double select_percentage = 5.,
                                             double neighbor_radius = -1.,
                                             int number_of_iterations = 35,
                                             bool require_uniform_sampling = false)
{
  CGAL::wlop_simplify_and_regularize_point_set<Concurrency_tag>
    (input.get_data(),
     output.get_data().point_back_inserter(),
     input.get_data().parameters().select_percentage(select_percentage).
     neighbor_radius(neighbor_radius).
     number_of_iterations(number_of_iterations).
     require_uniform_sampling(require_uniform_sampling));
}

// ==============================================================================
// Point Cloud Registration
// ==============================================================================

// OpenGR registration using Super4PCS algorithm
double register_point_sets_opengr(
    Point_set_3_wrapper<CGAL_PS3> point_set_1,
    Point_set_3_wrapper<CGAL_PS3> point_set_2,
    int number_of_samples = 200,
    double maximum_normal_deviation = 90.0,
    double accuracy = 5.0,
    double overlap = 0.2,
    int maximum_running_time = 1000)
{
  // Ensure both point sets have normals
  if (!point_set_1.get_data().has_normal_map())
    point_set_1.get_data().add_normal_map();
  if (!point_set_2.get_data().has_normal_map())
    point_set_2.get_data().add_normal_map();

  return CGAL::OpenGR::register_point_sets(
    point_set_1.get_data(),
    point_set_2.get_data(),
    point_set_1.get_data().parameters()
      .point_map(point_set_1.get_data().point_map())
      .normal_map(point_set_1.get_data().normal_map())
      .number_of_samples(number_of_samples)
      .maximum_normal_deviation(maximum_normal_deviation)
      .accuracy(accuracy)
      .overlap(overlap)
      .maximum_running_time(maximum_running_time),
    point_set_2.get_data().parameters()
      .point_map(point_set_2.get_data().point_map())
      .normal_map(point_set_2.get_data().normal_map())
  );
}

// ICP_config wrapper class for PointMatcher configuration
class ICP_config_wrapper
{
public:
  ICP_config_wrapper() {}
  ICP_config_wrapper(const std::string& config_name)
    : m_name(config_name) {}
  ICP_config_wrapper(const std::string& config_name,
                     const std::map<std::string, std::string>& params)
    : m_name(config_name), m_params(params) {}
  void set_name(const std::string& name) { m_name = name; }
  std::string get_name() const { return m_name; }
  void add_parameter(const std::string& key, const std::string& value) {
    m_params[key] = value;
  }
  void set_parameters(const std::map<std::string, std::string>& params) {
    m_params = params;
  }
  std::map<std::string, std::string> get_parameters() const { return m_params; }
  // Convert to CGAL ICP_config
  CGAL::pointmatcher::ICP_config to_cgal_config() const {
    return CGAL::pointmatcher::ICP_config{m_name, m_params};
  }

private:
  std::string m_name;
  std::map<std::string, std::string> m_params;
};

// Helper to convert vector of wrappers to vector of CGAL configs
std::vector<CGAL::pointmatcher::ICP_config> convert_icp_configs(const std::vector<ICP_config_wrapper>& wrappers)
{
  std::vector<CGAL::pointmatcher::ICP_config> configs;
  configs.reserve(wrappers.size());
  for (const auto& wrapper : wrappers)
    configs.push_back(wrapper.to_cgal_config());
  return configs;
}

// PointMatcher ICP registration with full parameter exposure
bool register_point_sets_pointmatcher(
    Point_set_3_wrapper<CGAL_PS3> point_set_1,
    Point_set_3_wrapper<CGAL_PS3> point_set_2,
    const std::vector<ICP_config_wrapper>& point_set_filters = {},
    const ICP_config_wrapper& matcher = CGAL_SWIG::ICP_config_wrapper("KDTreeMatcher", {{"knn", "1"}}),
    const std::vector<ICP_config_wrapper>& outlier_filters = {},
    const ICP_config_wrapper& error_minimizer =
        CGAL_SWIG::ICP_config_wrapper("PointToPlaneErrorMinimizer", {}),
    const std::vector<ICP_config_wrapper>& transformation_checkers =
        {CGAL_SWIG::ICP_config_wrapper("CounterTransformationChecker", {{"maxIterationCount", "150"}})})
{
  // Ensure both point sets have normals for ICP
  if (!point_set_1.get_data().has_normal_map())
    point_set_1.get_data().add_normal_map();
  if (!point_set_2.get_data().has_normal_map())
    point_set_2.get_data().add_normal_map();

  // Convert wrapper configs to CGAL configs
  auto filters = convert_icp_configs(point_set_filters);
  auto outliers = convert_icp_configs(outlier_filters);
  auto checkers = convert_icp_configs(transformation_checkers);
  auto match_config = matcher.to_cgal_config();
  auto minimizer_config = error_minimizer.to_cgal_config();

  // Build named parameters
  auto np1 = point_set_1.get_data().parameters()
    .point_map(point_set_1.get_data().point_map())
    .normal_map(point_set_1.get_data().normal_map());

  auto np2 = point_set_2.get_data().parameters()
    .point_map(point_set_2.get_data().point_map())
    .normal_map(point_set_2.get_data().normal_map())
    .matcher(match_config)
    .error_minimizer(minimizer_config);

  // Add optional filters and checkers if provided
  if (!filters.empty())
    np2 = np2.point_set_filters(filters);
  if (!outliers.empty())
    np2 = np2.outlier_filters(outliers);
  if (!checkers.empty())
    np2 = np2.transformation_checkers(checkers);

  return CGAL::pointmatcher::register_point_sets(
    point_set_1.get_data(),
    point_set_2.get_data(),
    np1,
    np2
  );
}

} // end namespace CGAL_SWIG

#endif //SWIG_CGAL_POINT_SET_PROCESSING_3_H
