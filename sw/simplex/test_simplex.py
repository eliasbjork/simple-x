import glob
import math
import os
import pathlib
import pytest
import serial
import subprocess
import textwrap


# allowed error margin
TOLERANCE = 1e-5


def collect_test_dirs():
    dirs_with_tests = list(
        filter(lambda path: os.path.isdir(path), glob.glob("test/*/*/*"))
    )
    return dirs_with_tests


def run_test_on_host(input_path):
    proc = subprocess.run(
        ["./simplex"],
        input=input_path.read_bytes(),
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE
    )

    return proc.stdout


def run_test_on_el2sim(input_path):
    cwd = pathlib.Path.cwd()
    input_path = pathlib.Path(input_path)

    proc = subprocess.run(
        ["make", "-C", "../..", "el2sim", "TARGET=simplex.elf", "PLATFORM=el2sim", f"SIMPLEX_TESTCASE={cwd/input_path}"],
        capture_output=True,
        text=True,
        check=True
    )

    output = proc.stdout
    res_str = [line for line in output.splitlines() if "z = " in line][0]

    return res_str


def run_test_on_veerwolf(input_path, port, baudrate):
    if not port:
        raise RuntimeError("Serial port must be supplied with --port")

    with serial.Serial(port, baudrate) as s:
        # transmit test input
        with open(input_path, "r") as f:
            for line in f:
                byte_str = line.encode("utf-8")
                s.write(byte_str)
                s.flush()

        # await response
        while not s.in_waiting:
            continue

        return s.read(s.in_waiting)


@pytest.mark.parametrize("dir", collect_test_dirs())
def test_linopt(dir, platform, port, baudrate):
    input_path = pathlib.Path(f"{dir}")/"i"
    sol_path = pathlib.Path(f"{dir}")/"linopt.sol"

    if platform == "host":
        res_bytes = run_test_on_host(input_path)
    elif platform == "veerwolf":
        res_bytes = run_test_on_veerwolf(input_path, port, baudrate)
    elif platform == "el2sim":
        res_str = run_test_on_el2sim(input_path)
    else:
        raise RuntimeError("Unknown platform")

    if platform == "host" or platform == "veerwolf":
        try:
            res_str = res_bytes.decode("utf-8", "strict")
        except UnicodeDecodeError:
            pytest.fail("Failed decoding simplex output")

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
