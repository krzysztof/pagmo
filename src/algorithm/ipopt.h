/*****************************************************************************
 *   Copyright (C) 2004-2009 The PaGMO development team,                     *
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

#ifndef IPOPT_H
#define IPOPT_H

#include "../config.h"
#include "base.h"
#include <coin/IpIpoptApplication.hpp>



namespace pagmo { namespace algorithm {

/// Wrapper for the IPOPT solver
/**
 * IPOPT
 *
 * @author Dario Izzo (dario.izzo@googlemail.com)
 *
 */

class __PAGMO_VISIBLE ipopt: public base
{
public:

	ipopt();
	base_ptr clone() const;
	void evolve(population &) const;

protected:
	std::string human_readable_extra() const;
private:
	::Ipopt::IpoptApplication app;


} //namespaces

#endif