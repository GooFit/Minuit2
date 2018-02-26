#include <pybind11/pybind11.h>

namespace py = pybind11;

void init_FunctionMinimum(py::module &);

PYBIND11_MODULE(minuit2, m) {
    m.doc() = "Python interface for Minuit2";

    init_FunctionMinimum(m);
}
