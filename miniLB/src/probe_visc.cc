#include <file_manager.hh>
#include <probe_visc.hh>

namespace bgk {
void probe_visc(storage &bgk_storage, const int itime, const int i0, const int j0) {
    real_kinds::mykind x01, x03, x05, x08, x10;
    real_kinds::mykind x12, x14, x17, x19;
    real_kinds::mykind e01, e03, e05, e08, e10;
    real_kinds::mykind e12, e14, e17, e19;
    real_kinds::mykind n01, n03, n05, n08, n10;
    real_kinds::mykind n12, n14, n17, n19;
    real_kinds::mykind rho, vx, vy, vx2, vy2, vsq;
    real_kinds::mykind vxpy, vxmy, rp1, rp2, rp0;
    real_kinds::mykind qxpy, qxmy, qx, qy, q0;
    real_kinds::mykind Pxx, Pxy, Pyx, Pyy, Ptotal;
    real_kinds::mykind Ts, omega;
    real_kinds::mykind cte1, cte0;

#ifdef NOSHIFT
    cte1 = storage::zero;
#else
    cte1 = storage::uno;
#endif
    cte0 = storage::uno - cte1;

    x01 = bgk_storage.a01_host(i0, j0);
    x03 = bgk_storage.a03_host(i0, j0);
    x05 = bgk_storage.a05_host(i0, j0);
    x08 = bgk_storage.a08_host(i0, j0);
    x10 = bgk_storage.a10_host(i0, j0);
    x12 = bgk_storage.a12_host(i0, j0);
    x14 = bgk_storage.a14_host(i0, j0);
    x17 = bgk_storage.a17_host(i0, j0);
    x19 = bgk_storage.a19_host(i0, j0);

    rho = (x01 + x03 + x05 + x08 + x10 + x12 + x14 + x17 + x19) + cte1;

    vx = (x01 + x03 + x05 - x10 - x12 - x14) / rho;
    vy = (x03 + x08 + x12 - x01 - x10 - x17) / rho;

    vx2 = vx * vx;
    vy2 = vy * vy;
    vsq = vx2 + vy2;

    vxpy = vx + vy;
    vxmy = vx - vy;

    qxpy = cte0 + storage::qf * (storage::tre * vxpy * vxpy - vsq);
    qxmy = cte0 + storage::qf * (storage::tre * vxmy * vxmy - vsq);
    qx = cte0 + storage::qf * (storage::tre * vx2 - vsq);
    qy = cte0 + storage::qf * (storage::tre * vy2 - vsq);
    q0 = cte0 + storage::qf * (-vsq);

    vx = storage::rf * vx;
    vy = storage::rf * vy;
    vxpy = storage::rf * vxpy;
    vxmy = storage::rf * vxmy;

    rp0 = rho * storage::p0;
    rp1 = rho * storage::p1;
    rp2 = rho * storage::p2;

    e01 = rp2 * (vxmy + qxmy) + cte1 * (rp2 - storage::p2);
    e03 = rp2 * (vxpy + qxpy) + cte1 * (rp2 - storage::p2);
    e05 = rp1 * (vx + qx) + cte1 * (rp1 - storage::p1);
    e08 = rp1 * (vy + qy) + cte1 * (rp1 - storage::p1);
    e10 = rp2 * (-vxpy + qxpy) + cte1 * (rp2 - storage::p2);
    e12 = rp2 * (-vxmy + qxmy) + cte1 * (rp2 - storage::p2);
    e14 = rp1 * (-vx + qx) + cte1 * (rp1 - storage::p1);
    e17 = rp1 * (-vy + qy) + cte1 * (rp1 - storage::p1);
    e19 = rp0 * (q0) + cte1 * (rp0 - storage::p0);

    n01 = x01 - e01;
    n03 = x03 - e03;
    n05 = x05 - e05;
    n08 = x08 - e08;
    n10 = x10 - e10;
    n12 = x12 - e12;
    n14 = x14 - e14;
    n17 = x17 - e17;

    Pxx = n01 + n03 + n05 + n10 + n12 + n14;
    Pyy = n01 + n03 + n08 + n10 + n12 + n17;
    Pxy = -n01 + n03 + n10 - n12;
    Pyx = Pxy;

    Ptotal = std::sqrt((Pxx * Pxx) + (2 * Pxy * Pyx) + (Pyy * Pyy));

    Ts = 1 / (2 * bgk_storage.omega1)
        + std::sqrt(18.0 * (bgk_storage.cteS * bgk_storage.cteS) * Ptotal
              + (1.0 / bgk_storage.omega1) * (1.0 / bgk_storage.omega1))
            / 2;
    omega = 1 / Ts;

    auto &file_manager = debug::file_manager::instance();
    file_manager.write_format<" Timestep {:8} {:14.6e} {:14.6e}\n">(
        65, itime, (2 / bgk_storage.omega1 - 1) / 6, (2 / omega - 1) / 6);

#ifdef DEBUG_2
    if(bgk_storage.myrank == 0) { fmt::print("DEBUG2: Exiting from sub. probe {}, {}\n", i0, j0); }
#endif
}
} // namespace bgk