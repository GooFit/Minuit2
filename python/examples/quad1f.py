"""
This is the Quad1F example from Minuit2, implemented in Python.
"""

import minuit2

class Quad1F(minuit2.FCNBase):
    def Up(self):
        return 1.0
    def __call__(self, vect):
        return vect[0]**2

fcn = Quad1F();
upar = minuit2.MnUserParameters()
upar.Add("x", 1., 0.1)
migrad = minuit2.MnMigrad(fcn, upar)
min = migrad()
print(min)
