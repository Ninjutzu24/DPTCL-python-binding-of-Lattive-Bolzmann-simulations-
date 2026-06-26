#include <file_manager.hh>
#include <outdat.hh>

namespace bgk {
void outdat(storage &bgk_storage, const int itfin, const int itstart, const int ivtim, const int isignal,
    const int itsave, const int icheck) {
    auto &file_manager = debug::file_manager::instance();
    file_manager.write(16, " ", "\n");
    file_manager.write(16, "*********** size of the lattice **************", "\n");
    file_manager.write(16, "l (width x)  =\t", bgk_storage.l, "\n");
    file_manager.write(16, "m (width y)  =\t", bgk_storage.m, "\n");
    file_manager.write(16, "*********** fluid data **********************", "\n");
    file_manager.write(16, "viscosity    =\t", bgk_storage.svisc, "\n");
    file_manager.write(16, "u0           =\t", bgk_storage.u0, "\n");
    file_manager.write(16, "u00          =\t", bgk_storage.u00, "\n");
    file_manager.write(16, "omega        =\t", bgk_storage.omega1, "\n");
    file_manager.write(16, "tau          =\t", 1.0 / bgk_storage.omega1, "\n");
    file_manager.write(16, "Reynolds     =\t",
        0.5 * bgk_storage.u0 * bgk_storage.l / bgk_storage.svisc
            + 0.5 * bgk_storage.u00 * bgk_storage.l / bgk_storage.svisc,
        "\n");
    file_manager.write(16, "forcing1     =\t", bgk_storage.fgrad, "\n");
    file_manager.write(16, "forcing2     =\t", bgk_storage.u00 / (6.0), "\n");
    file_manager.write(16, "u_inflow     =\t", bgk_storage.u_inflow, "\n");
    file_manager.write(16, "LES: cteS    =\t", bgk_storage.cteS, "\n");
    file_manager.write(16, "*********** run data ************************", "\n");
    file_manager.write(16, "itfin        =\t", itfin, "\n");
    file_manager.write(16, "itstart      =\t", itstart, "\n");
    file_manager.write(16, "ivtim        =\t", ivtim, "\n");
    file_manager.write(16, "isignal      =\t", isignal, "\n");
    file_manager.write(16, "itsave       =\t", itsave, "\n");
    file_manager.write(16, "icheck       =\t", icheck, "\n");
    file_manager.write(16, "flag1        =\t", bgk_storage.flag1, "\n");
    file_manager.write(16, "flag2        =\t", bgk_storage.flag2, "\n");
    file_manager.write(16, "flag3        =\t", bgk_storage.flag3, "\n");
    file_manager.write(16, "ipad         =\t", bgk_storage.ipad, "\n");
    file_manager.write(16, "jpad         =\t", bgk_storage.jpad, "\n");
    file_manager.write(16, "radius       =\t", bgk_storage.radius, "\n");
    file_manager.write(16, "LES: cteS    =\t", bgk_storage.cteS, "\n");
    file_manager.write(16, "************** Further check ****************", "\n");
    file_manager.write(16, "zero         =\t", storage::zero, "\t", storage::zero_qp, "\n");
    file_manager.write(16, "uno          =\t", storage::uno, "\t", storage::uno_qp, "\n");
    file_manager.write(16, "tre          =\t", storage::tre, "\t", storage::tre_qp, "\n");
    file_manager.write(16, "rf           =\t", storage::rf, "\t", storage::rf_qp, "\n");
    file_manager.write(16, "qf           =\t", storage::qf, "\t", storage::qf_qp, "\n");
    file_manager.write(16, "p0           =\t", storage::p0, "\t", storage::p0_qp, "\n");
    file_manager.write(16, "p1           =\t", storage::p1, "\t", storage::p1_qp, "\n");
    file_manager.write(16, "p2           =\t", storage::p2, "\t", storage::p2_qp, "\n");
    file_manager.write(16, "*********************************************", "\n");
    file_manager.write(16, " ", "\n");

    std::cout << "*********** size of the lattice **************"
              << "\n";
    std::cout << "l (width x)  =\t" << bgk_storage.l << "\n";
    std::cout << "m (width y)  =\t" << bgk_storage.m << "\n";
    std::cout << "****************fluid data*******************"
              << "\n";
    std::cout << "viscosity    =\t" << bgk_storage.svisc << "\n";
    std::cout << "u0           =\t" << bgk_storage.u0 << "\n";
    std::cout << "u00          =\t" << bgk_storage.u00 << "\n";
    std::cout << "omega        =\t" << bgk_storage.omega1 << "\n";
    std::cout << "tau          =\t" << 1.0 / bgk_storage.omega1 << "\n";
    std::cout << "Reynolds     =\t"
              << 0.5 * bgk_storage.u0 * bgk_storage.l / bgk_storage.svisc
            + 0.5 * bgk_storage.u00 * bgk_storage.l / bgk_storage.svisc
              << "\n";
    std::cout << "forcing1     =\t" << bgk_storage.fgrad << "\n";
    std::cout << "forcing2     =\t" << bgk_storage.u00 / 6.0 << "\n";
    std::cout << "u_inflow     =\t" << bgk_storage.u_inflow << "\n";
    std::cout << "LES: cteS    =\t" << bgk_storage.cteS << "\n";
    std::cout << "**************** run data********************"
              << "\n";
    std::cout << "itfin        =\t" << itfin << "\n";
    std::cout << "itstart      =\t" << itstart << "\n";
    std::cout << "ivtim        =\t" << ivtim << "\n";
    std::cout << "isignal      =\t" << isignal << "\n";
    std::cout << "itsave       =\t" << itsave << "\n";
    std::cout << "icheck       =\t" << icheck << "\n";
    std::cout << "flag1        =\t" << bgk_storage.flag1 << "\n";
    std::cout << "flag2        =\t" << bgk_storage.flag2 << "\n";
    std::cout << "flag3        =\t" << bgk_storage.flag3 << "\n";
    std::cout << "ipad         =\t" << bgk_storage.ipad << "\n";
    std::cout << "jpad         =\t" << bgk_storage.jpad << "\n";
    std::cout << "radius       =\t" << bgk_storage.radius << "\n";
    std::cout << "*********************************************"
              << "\n";
    std::flush(std::cout);

#ifdef DEBUG_1
    if(bgk_storage.myrank == 0) { std::cout << "DEBUG1: Exiting from sub. outdat\n"; }
#endif
}
} // namespace bgk