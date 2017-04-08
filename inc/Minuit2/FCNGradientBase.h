// @(#)root/minuit2:$Id$
// Authors: M. Winkler, F. James, L. Moneta, A. Zsenei   2003-2005

/**********************************************************************
 *                                                                    *
 * Copyright (c) 2005 LCG ROOT Math team,  CERN/PH-SFT                *
 *                                                                    *
 **********************************************************************/

#ifndef ROOT_Minuit2_FCNGradientBase
#define ROOT_Minuit2_FCNGradientBase

#include "Minuit2/FCNBase.h"

namespace ROOT {

   namespace Minuit2 {

//________________________________________________________________________
/** Extension of the FCNBase for providing the analytical Gradient of the
    function. The user-Gradient is checked at the beginning of the
    minimization against the Minuit internal numerical Gradient in order to
    spot problems in the analytical Gradient calculation. This can be turned
    off by overriding CheckGradient() to make it return "false".
    The size of the output Gradient vector must be equal to the size of the
    input Parameter vector.
    Minuit does a check of the user Gradient at the beginning, if this is not
    wanted the method "CheckGradient()" has to be overridden to return
    "false".
 */

class FCNGradientBase : public FCNBase {

public:

   virtual ~FCNGradientBase() {}

   virtual std::vector<double> Gradient(const std::vector<double>&) const = 0;

   virtual bool CheckGradient() const {return true;}

};

  }  // namespace Minuit2

}  // namespace ROOT

#endif  // ROOT_Minuit2_FCNGradientBase
