/*****************************************************************************
 *   Copyright (C) 2004-2013 The PaGMO development team,                     *
 *   Advanced Concepts Team (ACT), European Space Agency (ESA)               *
 *   http://apps.sourceforge.net/mediawiki/pagmo                             *
 *   http://apps.sourceforge.net/mediawiki/pagmo/index.php?title=Developers  *
 *   http://apps.sourceforge.net/mediawiki/pagmo/index.php?title=Credits     *
 *   act@esa.int                                                             *
 *                                                                           *
 *   This program is free software; you can redistribute it and/or modify    *
 *   it under the terms of the GNU General Public License as published by    *
 *   the Free Software Foundation; either version 2 of the License, or       *
 *   (at your option) any later version.                                     *
 *                                                                           *
 *   This program is distributed in the hope that it will be useful,         *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of          *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the           *
 *   GNU General Public License for more details.                            *
 *                                                                           *
 *   You should have received a copy of the GNU General Public License       *
 *   along with this program; if not, write to the                           *
 *   Free Software Foundation, Inc.,                                         *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.               *
 *****************************************************************************/

#include "hypervolume.h"

namespace pagmo { namespace util {

/// Constructor from population
/**
 * Constructs a hypervolume object, where points are elicited from the referenced population object.
 *
 * @param[in] pop reference to population object from which Pareto front is computed
 * @param[in] front_idx index of the front for which the hypervolume is to be computed
 */
hypervolume::hypervolume(boost::shared_ptr<population> pop, unsigned int front_idx) {
	std::vector<std::vector<population::size_type> > pareto_fronts = pop->compute_pareto_fronts();
	m_points.resize(pareto_fronts[front_idx].size());
	for (population::size_type idx = 0 ; idx < pareto_fronts[front_idx].size() ; ++idx) {
		m_points[idx] = fitness_vector(pop->get_individual(pareto_fronts[front_idx][idx]).cur_f);
	}

	verify_after_construct();
}

/// Constructor from a vector of points
/**
 * Constructs a hypervolume object from a provided set of points.
 *
 * @param[in] points vector of points for which the hypervolume is computed
 */
hypervolume::hypervolume(const std::vector<fitness_vector> &points) : m_points(points) {
	verify_after_construct();
}

/// Copy constructor.
/**
 * Will perform a deep copy of hypervolume object
 *
 * @param[in] hv hypervolume object to be copied
 */
hypervolume::hypervolume(const hypervolume &hv): m_points(hv.m_points) { }


/// Default constructor
/**
 * Initiates hypervolume with empty set of points.
 * Used for serialization purposes.
 */
hypervolume::hypervolume() {
	m_points.resize(0);
}

/// verify after construct
/**
 * Verifies whether basic requirements are met for the initial set of points.
 *
 * @throws value_error if point size is empty or when the dimensions among the points differ
 */
void hypervolume::verify_after_construct() {
	if ( m_points.size() == 0 ) {
		pagmo_throw(value_error, "Point set cannot be empty.");
	}
	fitness_vector::size_type reference_size = m_points[0].size();
	if (reference_size <= 1) {
		pagmo_throw(value_error, "Points of dimension > 1 required.");
	}
	for (std::vector<fitness_vector>::size_type idx = 1 ; idx < m_points.size() ; ++idx) {
		if ( m_points[idx].size() != reference_size ) {
			pagmo_throw(value_error, "All point set dimensions must be equal.");
		}
	}
}

// verify before compute
/**
 * Verifies whether reference point and the hypervolume method meet certain criteria.
 *
 * @param[in] r_point fitness vector describing the reference point
 *
 * @throws value_error if reference point's and point set dimension do not agree
 */
void hypervolume::verify_before_compute(const fitness_vector &r_point, hv_algorithm::base_ptr hv_algorithm) {
	if ( m_points[0].size() != r_point.size() ) {
		pagmo_throw(value_error, "Point set dimensions and reference point dimension must be equal.");
	}
	hv_algorithm->verify_before_compute(m_points, r_point);
}

// compute hypervolume
/**
 * Computes hypervolume provided a reference point and an algorithm object.
 *
 * @param[in] r_point fitness vector describing the reference point
 * @param[in] hv_algorithm algorithm object used for computing the hypervolume
 *
 * @return value representing the hypervolume
 */
double hypervolume::compute(const fitness_vector &r_point, hv_algorithm::base_ptr hv_algorithm) {
	verify_before_compute(r_point, hv_algorithm);
	return hv_algorithm->compute(m_points, r_point);
}

// compute exclusive contribution
/**
 * Computes exclusive hypervolume for given indivdual.
 *
 * @param[in] p_idx index of the individual for whom we compute the exclusive contribution to the hypervolume
 * @param[in] r_point fitness vector describing the reference point
 * @param[in] hv_algorithm algorithm object used for computing the hypervolume
 *
 * @return value representing the hypervolume
 */
double hypervolume::exclusive(const unsigned int p_idx, const fitness_vector &r_point, hv_algorithm::base_ptr hv_algorithm) {
	verify_before_compute(r_point, hv_algorithm);
	if (p_idx >= m_points.size()) {
		pagmo_throw(value_error, "Index of the individual is out of bounds.");

	}
	return hv_algorithm->exclusive(p_idx, m_points, r_point);
}

// locate the least contributing individual
/**
 * Locates the individual contributing the least to the total hypervolume.
 *
 * @param[in] r_point fitness vector describing the reference point
 * @param[in] hv_algorithm algorithm object used for computation
 *
 * @return index of the least contributing point
 */
unsigned int hypervolume::least_contributor(const fitness_vector &r_point, hv_algorithm::base_ptr hv_algorithm) {
	return hv_algorithm->least_contributor(m_points, r_point);

}

// locate the least contributing individual
/**
 * Locates the individual contributing the least to the total hypervolume.
 * This method chooses the best performing algorithm for given dimension.
 *
 * @param[in] r_point fitness vector describing the reference point
 *
 * @return index of the least contributing point
 */
unsigned int hypervolume::least_contributor(const fitness_vector &r_point) {
	unsigned int f_dim = m_points[0].size();
	if(f_dim == 2) {
		return least_contributor(r_point, util::hv_algorithm::base_ptr( new util::hv_algorithm::native2d()));
	} else if (f_dim == 3) {
		return least_contributor(r_point, util::hv_algorithm::base_ptr( new util::hv_algorithm::beume3d()));
	} else {
		return least_contributor(r_point, util::hv_algorithm::base_ptr( new util::hv_algorithm::wfg()));
	}
}

// calculate the nadir point
/**
 * Calculates the nadir point, used as the reference point
 *
 * @param[in] epsilon value that is to be added to each objective to assure strict domination nadir point by each other point in a set
 *
 * @return value representing the hypervolume
 */
fitness_vector hypervolume::get_nadir_point(const double epsilon) {
	fitness_vector nadir_point(m_points[0].begin(), m_points[0].end());
	for (std::vector<fitness_vector>::size_type idx = 0 ; idx < m_points.size() ; ++ idx){
		for (fitness_vector::size_type f_idx = 0 ; f_idx < m_points[0].size() ; ++f_idx){
			// assuming minimization
			nadir_point[f_idx] = fmax(nadir_point[f_idx], m_points[idx][f_idx]);
		}
	}
	for (fitness_vector::size_type f_idx = 0 ; f_idx < m_points[0].size() ; ++f_idx) {
		nadir_point[f_idx] += epsilon;
	}
	return nadir_point;
}


/// Get points
/**
 * Will return a vector containing the points as they were set up during construction of the hypervolume object.
 *
 * @return const reference to the vector containing the fitness_vectors representing the points in the hyperspace.
 */
const std::vector<fitness_vector> &hypervolume::get_points() const {
	return m_points;
}

hypervolume_ptr hypervolume::clone() const
{
	return hypervolume_ptr(new hypervolume(*this));
}

}}

BOOST_CLASS_EXPORT_IMPLEMENT(pagmo::util::hypervolume);
