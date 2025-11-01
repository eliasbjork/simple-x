import glob
import math
import os
import pathlib
import pytest
import re
import serial
import subprocess
import textwrap


# allowed error margin
TOLERANCE = 1e-5


def collect_test_dirs():
    dirs_with_tests = list(
        filter(lambda path: os.path.isdir(path), glob.glob(f"{pathlib.Path(__file__).parent}/test/*/*/*"))
    )
    return dirs_with_tests


def _parse_value(v: str):
    """Try to convert numeric values, otherwise return string."""
    try:
        if "." in v:
            return float(v)
        return int(v)
    except ValueError:
        return v


def parse_stats_from_output(output):
    """Extract key = value pairs between '========stats========' and '=====================' markers.

    Returns a dict of {key: value}.
    """
    m = re.search(r"========stats========\s*(.*?)\s*=====================", output, flags=re.S | re.I)
    if not m:
        return {}
    block = m.group(1)
    pairs = re.findall(r"(\w+)\s* = \s*([^\s]+)", block)
    result = {}
    for k, v in pairs:
        result[k] = _parse_value(v)
    return result


def run_test_on_host(input_path):
    proc = subprocess.run(
        ["./simplex"],
        input=input_path.read_bytes(),
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE
    )

    return proc.stdout


def run_test_on_el2sim(input_path, test_log):
    cwd = pathlib.Path.cwd()
    input_path = pathlib.Path(input_path)

    proc = subprocess.run(
        # FIXME: right now we had to add ZFINX=1 here, in case simplex isn't compiled. We should be able to specify not having ZFINX...
        #        but idk, the fact that testcases are run via the makefile is kinda weird from the get go...
        ["make", "-C", f"{pathlib.Path(__file__).parent}/../..", "el2sim", "TARGET=simplex.elf", "PLATFORM=el2sim", "ZFINX=1", f"SIMPLEX_TESTCASE={cwd/input_path}"],
        capture_output=True,
        text=True,
        check=True
    )

    output = proc.stdout

    stats = parse_stats_from_output(output)
    test_log.update(stats)

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
def test_linopt(dir, platform, port, baudrate, test_log):
    input_path = pathlib.Path(f"{dir}")/"i"
    sol_path = pathlib.Path(f"{dir}")/"linopt.sol"

    if platform == "host":
        res_bytes = run_test_on_host(input_path)
    elif platform == "veerwolf":
        res_bytes = run_test_on_veerwolf(input_path, port, baudrate)
    elif platform == "el2sim":
        res_str = run_test_on_el2sim(input_path, test_log)
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
