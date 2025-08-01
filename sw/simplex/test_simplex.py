import glob
import math
import os
import pathlib
import pytest
import subprocess
import textwrap


# allowed error margin
TOLERANCE = 1e-5


def collect_test_dirs():
    dirs_with_tests = list(
        filter(lambda path: os.path.isdir(path), glob.glob("*/*/*/*"))
    )
    return dirs_with_tests


@pytest.mark.parametrize("dir", collect_test_dirs())
def test_linopt(dir):
    input_path = pathlib.Path(f"{dir}")/"i"
    sol_path = pathlib.Path(f"{dir}")/"linopt.sol"

    proc = subprocess.run(
        ["./simplex"],
        input=input_path.read_bytes(),
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE
    )

    try:
        res_str = proc.stdout.decode("utf-8", "strict")
    except UnicodeDecodeError:
        pytest.fail("Failed decoding simplex output.")

    res = float(res_str.split(" ")[-1])

    with open(sol_path, "r") as f:
        sol = float(f.readline().split(" ")[-1])

    if not ((math.isnan(res) and math.isnan(sol)) or
            (math.isinf(res) and math.isinf(sol)) or
            (math.fabs(sol - res) < TOLERANCE)):

        msg = textwrap.dedent(f"""\
            Output mismatch
            ===============
            Result:\t\tz = {res}
            Expected:\tz = {sol}
        """)

        pytest.fail(msg, pytrace=False)
