/** @file
* @copyright University of Warsaw
* @section LICENSE
* GPLv3+ (see the COPYING file or http://www.gnu.org/licenses/)
*/

#pragma once

#include <libmpdata++/solvers/detail/solver_1d.hpp>
#include <libmpdata++/formulae/donorcell_formulae.hpp>

namespace libmpdataxx
{
  namespace solvers
  {
    template<
      typename real_t, 
      int n_eqs = 1,
      int halo = formulae::donorcell::halo
    >
    class donorcell_1d : public detail::solver_1d<
      real_t,
      1,
      n_eqs,
      formulae::donorcell::n_tlev, 
      detail::max(halo, formulae::donorcell::halo)
    > 
    {
      using parent_t = detail::solver_1d<
        real_t,
        1,
        n_eqs,
        formulae::donorcell::n_tlev, 
        detail::max(halo, formulae::donorcell::halo)
      >;
   
      void advop(int e)
      {
        formulae::donorcell::op_1d(
          this->mem->psi[e], this->n[e], this->mem->C[0], this->i
        );
      }

      public:

      struct params_t {};

      // ctor
      donorcell_1d(
        typename parent_t::ctor_args_t args, 
        const params_t &
      ) :
        parent_t(args)
      {}  
    };
  }; // namespace solvers
}; // namespace libmpdataxx