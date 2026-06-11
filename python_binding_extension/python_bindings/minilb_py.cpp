#include <pybind11/pybind11.h>
#include <fstream>
#include <string>

namespace py = pybind11;

void create_input(
    const std::string& filename,
    int lx,
    int ly,
    double svisc,
    double u0,
    int itfin,
    int ivtim,
    int isignal,
    int icheck
) {
    std::ofstream file(filename);

    if (!file.is_open()) {
        throw std::runtime_error("Could not open input file for writing: " + filename);
    }

    file << "&parameters\n";
    file << "lx = " << lx << "\n";
    file << "ly = " << ly << "\n";
    file << "svisc=" << svisc << "\n";
    file << "u0=" << u0 << "\n";
    file << "itfin=" << itfin << "\n";
    file << "ivtim=" << ivtim << "\n";
    file << "isignal=" << isignal << "\n";
    file << "itsave=1000000\n";
    file << "icheck=" << icheck << "\n";
    file << "irestart=0\n";
    file << "init_v=0\n";
    file << "ipad=0        /\n";
}

PYBIND11_MODULE(minilb_py, m) {
    m.doc() = "Python bindings for miniLB / Lattice Boltzmann simulations";

    m.def(
        "create_input",
        &create_input,
        py::arg("filename"),
        py::arg("lx") = 256,
        py::arg("ly") = 256,
        py::arg("svisc") = 0.05,
        py::arg("u0") = 0.1,
        py::arg("itfin") = 5000,
        py::arg("ivtim") = 500,
        py::arg("isignal") = 500,
        py::arg("icheck") = 500,
        "Create a bgk.input file for a miniLB simulation"
    );
}
