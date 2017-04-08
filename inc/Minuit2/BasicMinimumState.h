// @(#)root/minuit2:$Id$
// Authors: M. Winkler, F. James, L. Moneta, A. Zsenei   2003-2005

/**********************************************************************
 *                                                                    *
 * Copyright (c) 2005 LCG ROOT Math team,  CERN/PH-SFT                *
 *                                                                    *
 **********************************************************************/

#ifndef ROOT_Minuit2_BasicMinimumState
#define ROOT_Minuit2_BasicMinimumState

#include "Minuit2/MinimumParameters.h"
#include "Minuit2/MinimumError.h"
#include "Minuit2/FunctionGradient.h"

#include "Minuit2/StackAllocator.h"

namespace ROOT {

   namespace Minuit2 {


//extern StackAllocator gStackAllocator;

class BasicMinimumState {

public:

   // constructor without parameter values but with function value, edm and nfcn
   BasicMinimumState(unsigned int n, double fval, double edm, int nfcn) :
      fParameters(MinimumParameters(n,fval)), fError(MinimumError(n)),
    fGradient(FunctionGradient(n)), fEDM(edm), fNFcn(nfcn) {}
   
  BasicMinimumState(const MinimumParameters& states, const MinimumError& err,
                    const FunctionGradient& grad, double edm, int nfcn) :     
     fParameters(states), fError(err), fGradient(grad), fEDM(edm), fNFcn(nfcn) {}

   BasicMinimumState(const MinimumParameters& states, double edm, int nfcn) : fParameters(states), fError(MinimumError(states.Vec().size())),
                                                                              fGradient(FunctionGradient(states.Vec().size())), fEDM(edm), fNFcn(nfcn)
   {}

  ~BasicMinimumState() {}

  BasicMinimumState(const BasicMinimumState& state) :
     fParameters(state.fParameters), fError(state.fError), fGradient(state.fGradient), fEDM(state.fEDM), fNFcn(state.fNFcn) {}

  BasicMinimumState& operator=(const BasicMinimumState& state) {
    fParameters = state.fParameters;
    fError = state.fError;
    fGradient = state.fGradient;
    fEDM = state.fEDM;
    fNFcn = state.fNFcn;
    return *this;
  }

  void* operator new(size_t nbytes) {
    return StackAllocatorHolder::Get().Allocate(nbytes);
  }

  void operator delete(void* p, size_t /*nbytes */) {
    StackAllocatorHolder::Get().Deallocate(p);
  }

  const MinimumParameters& Parameters() const {return fParameters;}
  const MnAlgebraicVector& Vec() const {return fParameters.Vec();}
  int size() const {return fParameters.Vec().size();}

  const MinimumError& Error() const {return fError;}
  const FunctionGradient& Gradient() const {return fGradient;}
  double Fval() const {return fParameters.Fval();}
  double Edm() const {return fEDM;}
  int NFcn() const {return fNFcn;}


  bool IsValid() const {
    if(HasParameters() && HasCovariance())
      return Parameters().IsValid() && Error().IsValid();
    else if(HasParameters()) return Parameters().IsValid();
    else return false;
  }
  bool HasParameters() const {return fParameters.IsValid();}
  bool HasCovariance() const {return fError.IsAvailable();}

private:

  MinimumParameters fParameters;
  MinimumError fError;
  FunctionGradient fGradient;
  double fEDM;
  int fNFcn;
};

  }  // namespace Minuit2

}  // namespace ROOT

#endif  // ROOT_Minuit2_BasicMinimumState
