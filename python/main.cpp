#include <pybind11/pybind11.h>

namespace py = pybind11;

void init_FCNBase(py::module &);
void init_MinuitParameter(py::module &);
void init_MnUserParameterState(py::module &);
void init_MnUserParameters(py::module &);
void init_MnUserCovariance(py::module &);
void init_FunctionMinimum(py::module &);

PYBIND11_MODULE(minuit2, m) {
    m.doc() = "Python interface for Minuit2";

    init_FCNBase(m);
    init_MinuitParameter(m);
    init_MnUserParameterState(m);
    init_MnUserParameters(m);
    init_MnUserCovariance(m);
    init_FunctionMinimum(m);
}
