import subprocess
import tempfile
import time
from itertools import chain

import matplotlib.pyplot as plt


def get_config_lines(precision=0.1, precision_rel=0.001, n_threads=1,
                     x1_from=-50, x1_to=50, x2_from=-50, x2_to=50, max_iterations=30):
    return f"""\
{precision:f}   // expected precision
{precision_rel} // expected relative precision
{n_threads}     // number of threads to run
{x1_from}       // X1 integration interval
{x1_to}         // X1 integration interval
{x2_from}       // X2 integration interval
{x2_to}         // X2 integration interval
{max_iterations} // max iterations

"""


run_file = '../cmake-build-debug/ThreadedIntegral'

if __name__ == '__main__':
    results = []
    n_tests = 10
    c = list(chain(range(1, 33)))
    for n_threads in c:
        tem_res = []
        for i in range(n_tests):
            with tempfile.NamedTemporaryFile('w') as f:
                f.write(get_config_lines(n_threads=n_threads, max_iterations=10))
                f.flush()
                res = subprocess.run([run_file, f.name], capture_output=True)
                _, _, _, t = map(float, res.stdout.split())
                tem_res.append(t)
                time.sleep(0.01)
        results.append(min(tem_res))
        print("Mean ", n_threads, " = ", results[-1])

    plt.plot(c, results)
    plt.xlabel('Number of threads')
    plt.ylabel('Median execution time (ns)')
    plt.show()
