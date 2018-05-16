// @(#)root/minuit2:$Id$
// Authors: M. Winkler, F. James, L. Moneta, A. Zsenei   2003-2005

/**********************************************************************
 *                                                                    *
 * Copyright (c) 2005 LCG ROOT Math team,  CERN/PH-SFT                *
 *                                                                    *
 **********************************************************************/

#include "Minuit2/VariableMetricBuilder.h"
#include "Minuit2/GradientCalculator.h"
#include "Minuit2/MinimumState.h"
#include "Minuit2/MinimumError.h"
#include "Minuit2/FunctionGradient.h"
#include "Minuit2/FunctionMinimum.h"
#include "Minuit2/MnLineSearch.h"
#include "Minuit2/MinimumSeed.h"
#include "Minuit2/MnFcn.h"
#include "Minuit2/MnMachinePrecision.h"
#include "Minuit2/MnPosDef.h"
#include "Minuit2/MnParabolaPoint.h"
#include "Minuit2/LaSum.h"
#include "Minuit2/LaProd.h"
#include "Minuit2/MnStrategy.h"
#include "Minuit2/MnHesse.h"

//#define DEBUG
#include "Minuit2/MnPrint.h"

// #if defined(DEBUG) || defined(WARNINGMSG)
// #endif



namespace ROOT {

   namespace Minuit2 {


double inner_product(const LAVector&, const LAVector&);

void VariableMetricBuilder::AddResult( std::vector<MinimumState>& result, const MinimumState & state) const {
   // // if (!store) store = StorageLevel();
   // // store |= (result.size() == 0);
   // if (store)
   result.push_back(state);
   //  else {
   //     result.back() = state;
   //  }
   if (TraceIter() ) TraceIteration(result.size()-1, result.back() );
   else {
      if (PrintLevel() > 1) {
         MnPrint::PrintState(std::cout, result.back(), "VariableMetric: Iteration # ",result.size()-1);
      }
   }
}


FunctionMinimum VariableMetricBuilder::Minimum(const MnFcn& fcn, const GradientCalculator& gc, const MinimumSeed& seed, const MnStrategy& strategy, unsigned int maxfcn, double edmval) const {
   // top level function to find minimum from a given initial seed
   // iterate on a minimum search in case of first attempt is not successful

   // to be consistent with F77 Minuit
   // in Minuit2 edm is correct and is ~ a factor of 2 smaller than F77Minuit
   // There are also a check for convergence if (edm < 0.1 edmval for exiting the loop)
   // LM: change factor to 2E-3 to be consistent with F77Minuit
   edmval *= 0.002;

   int printLevel = PrintLevel();


#ifdef DEBUG
   std::cout<<"VariableMetricBuilder convergence when edm < "<<edmval<<std::endl;
#endif

   if(seed.Parameters().Vec().size() == 0) {
      return FunctionMinimum(seed, fcn.Up());
   }


   //   double edm = Estimator().Estimate(seed.Gradient(), seed.Error());
   double edm = seed.State().Edm();

   FunctionMinimum min(seed, fcn.Up() );

   if(edm < 0.) {
#ifdef WARNINGMSG
      MN_INFO_MSG("VariableMetricBuilder: initial matrix not pos.def.");
#endif
      //assert(!seed.Error().IsPosDef());
      return min;
   }

   std::vector<MinimumState> result;
   if (StorageLevel() > 0)
      result.reserve(10);
   else
      result.reserve(2);


   // do actual iterations
   if (printLevel >1) {
      std::cout << "VariableMetric: start iterating until Edm is < " << edmval << std::endl;
      MnPrint::PrintState(std::cout, seed.State(), "VariableMetric: Initial state  ");
   }

   AddResult( result, seed.State());


   // try first with a maxfxn = 80% of maxfcn
   int maxfcn_eff = maxfcn;
   int ipass = 0;
   bool iterate = false;

   do {

      iterate = false;

#ifdef DEBUG
      std::cout << "start iterating... " << std::endl;
      if (ipass > 0)  std::cout << "continue iterating... " << std::endl;
#endif

      min = Minimum(fcn, gc, seed, result, maxfcn_eff, edmval);

      // if max function call reached exits
      if ( min.HasReachedCallLimit() ) {
#ifdef WARNINGMSG
         MN_INFO_MSG("VariableMetricBuilder: FunctionMinimum is invalid, reached the function call limit");
#endif
         return min;
      }

      // second time check for validity of function Minimum
      if (ipass > 0) {
         if(!min.IsValid()) {
#ifdef WARNINGMSG
            MN_INFO_MSG("VariableMetricBuilder: FunctionMinimum is invalid after second try");
#endif
            return min;
         }
      }

      // resulting edm of minimization
      edm = result.back().Edm();
      // need to re-coorect for Dcovar ?

      if( (strategy.Strategy() == 2) ||
          (strategy.Strategy() == 1 && min.Error().Dcovar() > 0.05) ) {

#ifdef DEBUG
         std::cout<<"MnMigrad will verify convergence and Error matrix. "<< std::endl;
         std::cout<<"dcov is =  "<<  min.Error().Dcovar() << std::endl;
#endif

         MinimumState st = MnHesse(strategy)(fcn, min.State(), min.Seed().Trafo(),maxfcn);

         if (printLevel > 1) {
            MnPrint::PrintState(std::cout, st, "VariableMetric: After Hessian  ");
         }
         AddResult( result, st);

         // check new edm
         edm = st.Edm();
#ifdef DEBUG
         std::cout << "edm after Hesse calculation " << edm << " requested " << edmval << std::endl;
#endif
         if (edm > edmval) {
            // be careful with machine precision and avoid too small edm
            double machineLimit = fabs(seed.Precision().Eps2()*result.back().Fval());
            if (edm >= machineLimit)   {
               iterate = true;
#ifdef WARNINGMSG
               MN_INFO_MSG("VariableMetricBuilder: Tolerance is not sufficient, continue the minimization");
               MN_INFO_VAL2("Current  Edm is",edm);
               MN_INFO_VAL2("Required Edm is",edmval);
#endif
            }
            else {
#ifdef WARNINGMSG
               MN_INFO_MSG("VariableMetricBuilder: Stop the minimization - reached machine accuracy limit");
               MN_INFO_VAL2("Edm is smaller than machine accuracy",machineLimit);
               MN_INFO_VAL2("Current  Edm is",edm);
               MN_INFO_VAL2("Required Edm is",edmval);
#endif
            }

         }
      }


      // end loop on iterations
      // ? need a maximum here (or max of function calls is enough ? )
      // continnue iteration (re-calculate function Minimum if edm IS NOT sufficient)
      // no need to check that hesse calculation is done (if isnot done edm is OK anyway)
      // count the pass to exit second time when function Minimum is invalid
      // increase by 20% maxfcn for doing some more tests
      if (ipass == 0) maxfcn_eff = int(maxfcn*1.3);
      ipass++;
   }  while ( iterate );


   // Add latest state (Hessian calculation)
   // and check edm (add a factor of 10 in tolerance )
   if (edm > 10*edmval) {
      min.Add( result.back(), FunctionMinimum::MnAboveMaxEdm() );
#ifdef WARNINGMSG
      MN_INFO_VAL2("VariableMetricBuilder: INVALID function minimum - edm is above tolerance,",edm);
      MN_INFO_VAL2("VariableMetricBuilder: Required tolerance  is 10 x edmval ",edmval);
#endif
   }
   else {
      // check if minimum has edm above max before
#ifdef WARNINGMSG
      if ( min.IsAboveMaxEdm() ) {
         MN_INFO_MSG("VariableMetricBuilder: Edm has been re-computed after Hesse");
         MN_INFO_VAL2("new value is now smaller than the required tolerance,",edm);
      }
#endif
      min.Add( result.back()  );
   }

#ifdef DEBUG
   std::cout << "Obtained function minimum " << min << std::endl;
#endif

   return min;
}

FunctionMinimum VariableMetricBuilder::Minimum(const MnFcn& fcn, const GradientCalculator& gc, const MinimumSeed& seed, std::vector<MinimumState>& result, unsigned int maxfcn, double edmval) const {
   // function performing the minimum searches using the Variable Metric  algorithm (MIGRAD)
   // perform first a line search in the - Vg direction and then update using the Davidon formula (Davidon Error updator)
   // stop when edm reached is less than required (edmval)

   // after the modification when I iterate on this functions, so it can be called many times,
   //  the seed is used here only to get precision and construct the returned FunctionMinimum object



   const MnMachinePrecision& prec = seed.Precision();


   //   result.push_back(MinimumState(seed.Parameters(), seed.Error(), seed.Gradient(), edm, fcn.NumOfCalls()));
   const MinimumState & initialState = result.back();

   double edm = initialState.Edm();


#ifdef DEBUG
   std::cout << "\n\nDEBUG Variable Metric Builder  \nInitial State: "
             << " Parameter " << initialState.Vec()
             << " Gradient " << initialState.Gradient().Vec()
             << " Inv Hessian " << initialState.Error().InvHessian()
             << " edm = " << initialState.Edm() << std::endl;
#endif



   // iterate until edm is small enough or max # of iterations reached
   edm *= (1. + 3.*initialState.Error().Dcovar());
   MnLineSearch lsearch;
   MnAlgebraicVector step(initialState.Gradient().Vec().size());
   // keep also prevStep
   MnAlgebraicVector prevStep(initialState.Gradient().Vec().size());

   MinimumState s0 = result.back();
   assert(s0.IsValid() ); 

   do {

      //MinimumState s0 = result.back();

      step = -1.*s0.Error().InvHessian()*s0.Gradient().Vec();

#ifdef DEBUG
      std::cout << "\n\n---> Iteration - " << result.size()
                << "\nFval = " << s0.Fval() << " numOfCall = " << fcn.NumOfCalls()
                << "\nInternal Parameter values " << s0.Vec()
                << " Newton step " << step << std::endl;
#endif

      // check if derivatives are not zero
      if ( inner_product(s0.Gradient().Vec(),s0.Gradient().Vec() )  <= 0 )  {
#ifdef DEBUG
         std::cout << "VariableMetricBuilder: all derivatives are zero - return current status" << std::endl;
#endif
         break;
      }


      double gdel = inner_product(step, s0.Gradient().Grad());

#ifdef DEBUG
      std::cout << " gdel = " << gdel << std::endl;
#endif


      if(gdel > 0.) {
#ifdef WARNINGMSG
         MN_INFO_MSG("VariableMetricBuilder: matrix not pos.def, gdel > 0");
         MN_INFO_VAL(gdel);
#endif
         MnPosDef psdf;
         s0 = psdf(s0, prec);
         step = -1.*s0.Error().InvHessian()*s0.Gradient().Vec();
         // #ifdef DEBUG
         //       std::cout << "After MnPosdef - Error  " << s0.Error().InvHessian() << " Gradient " << s0.Gradient().Vec() << " step " << step << std::endl;
         // #endif
         gdel = inner_product(step, s0.Gradient().Grad());
#ifdef WARNINGMSG
         MN_INFO_VAL(gdel);
#endif
         if(gdel > 0.) {
            AddResult(result, s0);
               
            return FunctionMinimum(seed, result, fcn.Up());
         }
      }
      
      MnParabolaPoint pp = lsearch(fcn, s0.Parameters(), step, gdel, prec);

      // <= needed for case 0 <= 0
      if(fabs(pp.Y() - s0.Fval()) <=  fabs(s0.Fval())*prec.Eps() ) {
#ifdef WARNINGMSG
         MN_INFO_MSG("VariableMetricBuilder: no improvement in line search");
#endif
         // no improvement exit   (is it really needed LM ? in vers. 1.22 tried alternative )
         // add new state when only fcn changes
         if (result.size() <= 1 ) 
            AddResult(result, MinimumState(s0.Parameters(), s0.Error(), s0.Gradient(), s0.Edm(), fcn.NumOfCalls()));
         else
            // no need to re-store the state
            AddResult(result, MinimumState(pp.Y(), s0.Edm(), fcn.NumOfCalls()));

         break;


      }

#ifdef DEBUG
      std::cout << "Result after line search : \nx = " << pp.X()
                << "\nOld Fval = " << s0.Fval()
                << "\nNew Fval = " << pp.Y()
                << "\nNFcalls = " << fcn.NumOfCalls() << std::endl;
#endif

      MinimumParameters p(s0.Vec() + pp.X()*step, pp.Y());


      FunctionGradient g = gc(p, s0.Gradient());


      edm = Estimator().Estimate(g, s0.Error());


      if(edm < 0.) {
#ifdef WARNINGMSG
         MN_INFO_MSG("VariableMetricBuilder: matrix not pos.def. : edm is < 0. Make pos def...");
#endif
         MnPosDef psdf;
         s0 = psdf(s0, prec);
         edm = Estimator().Estimate(g, s0.Error());
         if(edm < 0.) {
#ifdef WARNINGMSG
            MN_INFO_MSG("VariableMetricBuilder: matrix still not pos.def. : exit iterations ");
#endif
            AddResult(result, s0);

            return FunctionMinimum(seed, result, fcn.Up());
         }
      }
      MinimumError e = ErrorUpdator().Update(s0, p, g);

#ifdef DEBUG
      std::cout << "Updated new point: \n "
                << " Parameter " << p.Vec()
                << " Gradient " << g.Vec()
                << " InvHessian " << e.Matrix()
                << " Hessian " << e.Hessian()
                << " edm = " << edm << std::endl << std::endl;
#endif

      // update the state
      s0 =  MinimumState(p, e, g, edm, fcn.NumOfCalls());
      if (StorageLevel() || result.size() <= 1) 
         AddResult(result, s0);
      else
         // use a reduced state for not-final iterations
         AddResult(result, MinimumState(p.Fval(), edm, fcn.NumOfCalls()));

      // correct edm
      edm *= (1. + 3.*e.Dcovar());

#ifdef DEBUG
      std::cout << "Dcovar = " << e.Dcovar() << "\tCorrected edm = " << edm << std::endl;
#endif



   } while(edm > edmval && fcn.NumOfCalls() < maxfcn);  // end of iteration loop

   // save last result in case of no complete final states
   if ( ! result.back().IsValid() )
      result.back() = s0; 


   if(fcn.NumOfCalls() >= maxfcn) {
#ifdef WARNINGMSG
      MN_INFO_MSG("VariableMetricBuilder: call limit exceeded.");
#endif
      return FunctionMinimum(seed, result, fcn.Up(), FunctionMinimum::MnReachedCallLimit());
   }

   if(edm > edmval) {
      if(edm < fabs(prec.Eps2()*result.back().Fval())) {
#ifdef WARNINGMSG
         MN_INFO_MSG("VariableMetricBuilder: machine accuracy limits further improvement.");
#endif
         return FunctionMinimum(seed, result, fcn.Up());
      } else if(edm < 10*edmval) {
         return FunctionMinimum(seed, result, fcn.Up());
      } else {
#ifdef WARNINGMSG
         MN_INFO_MSG("VariableMetricBuilder: iterations finish without convergence.");
         MN_INFO_VAL2("VariableMetricBuilder",edm);
         MN_INFO_VAL2("            requested",edmval);
#endif
         return FunctionMinimum(seed, result, fcn.Up(), FunctionMinimum::MnAboveMaxEdm());
      }
   }
   //   std::cout<<"result.back().Error().Dcovar()= "<<result.back().Error().Dcovar()<<std::endl;

#ifdef DEBUG
   std::cout << "Exiting successfully Variable Metric Builder \n"
             << "NFCalls = " << fcn.NumOfCalls()
             << "\nFval = " <<  result.back().Fval()
             << "\nedm = " << edm << " requested = " << edmval << std::endl;
#endif

   return FunctionMinimum(seed, result, fcn.Up());
}

   }  // namespace Minuit2

}  // namespace ROOT
