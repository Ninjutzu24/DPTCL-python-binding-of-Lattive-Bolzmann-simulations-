import dpctl
import dpnp as np


def main():
    print("DPTCL / dpctl + dpnp vector addition demo")
    print("=========================================")

    # Selectăm device-ul SYCL disponibil.
    # În WSL-ul nostru, backend-ul disponibil este OpenCL CPU.
    queue = dpctl.SyclQueue("opencl:cpu")

    print("Selected SYCL device:")
    print(queue.sycl_device)

    n = 10

    # Vectori creați direct prin dpnp, asociați cu queue-ul SYCL.
    a = np.arange(n, dtype=np.float32, sycl_queue=queue)
    b = np.ones(n, dtype=np.float32, sycl_queue=queue)

    # Operația vectorială se execută prin dpnp/dpctl, nu prin executabilul miniLB.
    c = a + b

    queue.wait()

    print("\nVector a:")
    print(np.asnumpy(a))

    print("\nVector b:")
    print(np.asnumpy(b))

    print("\nResult c = a + b:")
    print(np.asnumpy(c))

    expected = np.asnumpy(a) + np.asnumpy(b)
    result = np.asnumpy(c)

    if (result == expected).all():
        print("\nResult check: OK")
    else:
        print("\nResult check: FAILED")


if __name__ == "__main__":
    main()
