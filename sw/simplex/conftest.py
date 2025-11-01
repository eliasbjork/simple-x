import csv
import datetime
import pytest
from pathlib import Path


_test_results = []


def pytest_addoption(parser):
    parser.addoption("--platform", choices=["host", "veerwolf", "el2sim"], default="host")
    parser.addoption("--port", default=None)
    parser.addoption("--baudrate", type=int, default=115200)
    parser.addoption("--logfile", action="store", default=None)


@pytest.fixture
def platform(request):
    return request.config.getoption("--platform")


@pytest.fixture
def port(request):
    return request.config.getoption("--port")


@pytest.fixture
def baudrate(request):
    return request.config.getoption("--baudrate")


@pytest.fixture
def test_log(request):
    """Provides a dict that a test can populate with arbitrary fields."""

    data = {
        "test_name": request.node.nodeid,
        "timestamp": datetime.datetime.now().isoformat(timespec="seconds"),
    }
    yield data

    _test_results.append(data)


def pytest_configure(config):
    output = config.getoption("--logfile")
    if output:
        log_file = Path(output).expanduser().resolve()
    else:
        log_file = Path(config.rootpath) / "test_results.csv"

    log_file.parent.mkdir(parents=True, exist_ok=True)
    config._csv_log_file = log_file


def pytest_runtest_logreport(report):
    """Called after each test phase. We fill in outcome/duration once per test."""

    if report.when != "call":
        return

    # find the matching entry for this test
    for entry in _test_results:
        if entry["test_name"] == report.nodeid:
            entry.update({
                "outcome": report.outcome,
                "duration_sec": f"{report.duration:.3f}",
                "message": (
                    getattr(report, "longreprtext", "").splitlines()[-1]
                    if report.failed
                    else ""
                ),
            })
            break


def pytest_sessionfinish(session, exitstatus):
    """When all tests are done, write the CSV."""

    log_file = session.config._csv_log_file
    if not _test_results:
        return

    # collect all fieldnames dynamically (union of all keys)
    fieldnames = sorted({k for d in _test_results for k in d.keys()})

    with open(log_file, mode="w", newline="") as f:
        writer = csv.DictWriter(f, fieldnames=fieldnames)
        writer.writeheader()
        writer.writerows(_test_results)

    print(f"\nCSV log written to: {log_file}")
