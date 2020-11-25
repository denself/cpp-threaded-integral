import subprocess
import tempfile
import unittest

run_file = 'cmake-build-debug/ThreadedIntegral'


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


class BaseTest(unittest.TestCase):

    def test_precision(self):
        precision_expected = 0.001
        precision_rel_expected = 1e-5
        with tempfile.NamedTemporaryFile('w') as f:
            f.write(get_config_lines(precision=precision_expected, precision_rel=precision_rel_expected, n_threads=8))
            f.flush()
            res = subprocess.run([run_file, f.name], capture_output=True)
        res, precision, precision_rel, time = map(float, res.stdout.split())
        self.assertAlmostEqual(res, 4.545447652e6, 3, msg=f"Actual precision: {precision}")
        self.assertLessEqual(precision, precision_expected)
        self.assertLessEqual(precision_rel, precision_rel_expected)

    def test_results_match(self):
        results = []
        for i in range(10):
            with tempfile.NamedTemporaryFile('w') as f:
                f.write(get_config_lines(n_threads=4))
                f.flush()
                res = subprocess.run([run_file, f.name], capture_output=True)
            res, precision, precision_rel, time = map(float, res.stdout.split())
            results.append(res)

        for r in results[1:]:
            self.assertEqual(results[0], r)

    def test_results_match_threads(self):
        results = []
        for i in range(16):
            with tempfile.NamedTemporaryFile('w') as f:
                f.write(get_config_lines(n_threads=i+1))
                f.flush()
                res = subprocess.run([run_file, f.name], capture_output=True)
            res, precision, precision_rel, time = map(float, res.stdout.split())
            results.append(res)

        for i in range(1, 16):
            self.assertEqual(results[0], results[i], f"Value for {i+1} threads doesn't match.")


if __name__ == '__main__':
    unittest.main()
