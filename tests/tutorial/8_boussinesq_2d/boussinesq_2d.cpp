/** 
 * @file
 * @copyright University of Warsaw
 * @section LICENSE
 * GPLv3+ (see the COPYING file or http://www.gnu.org/licenses/)
 *
 * @brief test for pressure solvers (as in Smolarkiewicz & Pudykiewicz 1992, fig.3) 
 * buoyant convection in Boussinesq flow
 */

#include "boussinesq.hpp"
#include <libmpdata++/concurr/threads.hpp>
#include <libmpdata++/output/gnuplot.hpp>
using namespace libmpdataxx;

int main() 
{
  // compile-time parameters
//<listing-1>
  struct ct_params_t : ct_params_default_t
  {
    using real_t = double;
    enum { n_dims = 2 };
    enum { n_eqns = 3 };
    enum { rhs_scheme = solvers::trapez };
    enum { prs_scheme = solvers::cr };
    struct ix { enum {
      u, w, tht, 
      vip_i=u, vip_j=w, vip_den=-1
    }; };
  }; 
//</listing-1>
  using ix = typename ct_params_t::ix;
  using real_t = typename ct_params_t::real_t;

  const int r0 = 250; 
  const int nx = 201, ny = 201, nt = 800;
  typename ct_params_t::real_t Tht_ref = 300; //1; // reference state (constant throughout the domain)

  // conjugate residual
  using solver_t = output::gnuplot<boussinesq<ct_params_t>>;

  // run-time parameters
  solver_t::rt_params_t p;

  p.dt = .75;
  p.di = p.dj = 10.; 
  p.Tht_ref = Tht_ref; 

  p.outfreq = 100; //12;
  p.outvars = {
//    {ix::u,   {.name = "u",   .unit = "m/s"}}, 
//    {ix::w,   {.name = "w",   .unit = "m/s"}}, 
    {ix::tht, {.name = "tht", .unit = "K"  }}
  };
  p.gnuplot_view = "map";
  p.gnuplot_output = "figure_%s_%04d.svg";
  p.gnuplot_with = "lines";
  p.gnuplot_surface = false;
  p.gnuplot_contour = true;

  real_t eps = .01;

  if (false) // TODO
  {
    // physics-oriented plot
    p.gnuplot_cntrparam = "levels incremental 299.95, 0.1, 300.55";
    p.gnuplot_cbrange = "[299.95 : 300.55]";
    p.gnuplot_cbtics = "300.05, 0.1, 300.45";
    p.gnuplot_palette = "defined ("
      "299.95 '#ffffff', "
      "300.05 '#ffffff', 300.05 '#993399', "
      "300.15 '#993399', 300.15 '#00CCFF', "
      "300.25 '#00CCFF', 300.25 '#66CC00', "
      "300.35 '#66CC00', 300.35 '#FC8727', "
      "300.45 '#FC8727', 300.45 '#FFFF00', "
      "300.55 '#FFFF00'"
    ")";
  } 
  else
  {
    // numerics-oriented plot
    p.gnuplot_cntrparam = "levels discrete 299.99, 300.00, 300.05, 300.50, 300.51";
    p.gnuplot_cbrange = "[299.95 : 300.55]";
    p.gnuplot_cbtics = "('299.99' 299.95, '300.00' 300.00, '300.05' 300.05, '300.50' 300.50, '300.51' 300.55)"; // note: intentionally non-linear!!! TODO: use eps to construct the strings!
    p.gnuplot_palette = "defined ("
      "299.95 '#BACA66', "
      "300.00 '#BACA66', 300.00 '#ffffff', "
      "300.05 '#ffffff', 300.05 '#cccccc', "
      "300.50 '#cccccc',"
      "300.50 '#ff0000', 300.55 '#ff0000'"
    ")";
  }

  p.gnuplot_term = "svg";
//<listing-2>
  p.prs_tol = 1e-7;
//</listing-2>
  p.grid_size = {nx, ny};

  libmpdataxx::concurr::threads<
    solver_t, 
    bcond::cyclic, bcond::cyclic,
    bcond::cyclic, bcond::cyclic
  > slv(p);

  {
    // initial condition
    blitz::firstIndex i;
    blitz::secondIndex j;

    slv.advectee(ix::tht) = Tht_ref + where(
      // if
      pow(i * p.di - 4    * r0 , 2) + 
      pow(j * p.dj - 1.04 * r0 , 2) <= pow(r0, 2), 
      // then
      .5, 
      // else
      0
    );
std::cerr << "min(psi) = " << min(slv.advectee(ix::tht)) << "\n";
std::cerr << "max(u)^2 + max(w)^2 = " << max(pow(slv.advectee(ix::u),2) + pow(slv.advectee(ix::w),2)) << "\n";
    slv.advectee(ix::u) = 0; 
    slv.advectee(ix::w) = 0; 
  }

  // integration
  slv.advance(nt); 
std::cerr << "min(psi)-300 = " << min(slv.advectee(ix::tht))-300.0 << "\n";
std::cerr << "max(psi)-300 = " << max(slv.advectee(ix::tht))-300.5 << "\n";
std::cerr << "max(u)^2 + max(w)^2 = " << max(pow(slv.advectee(ix::u),2) + pow(slv.advectee(ix::w),2)) << "\n";
  if (min(slv.advectee(ix::tht)) < 300-eps || max(slv.advectee(ix::tht)) > 300.5+eps)
    throw std::runtime_error("too big under- or over-shots :("); 
};
