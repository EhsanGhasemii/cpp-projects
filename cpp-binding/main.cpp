#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

int add(int a, int b) {
    return a + b;
}

PYBIND11_MODULE(mymodule, m) {
    m.def("add", &add, "Add two numbers");
}


