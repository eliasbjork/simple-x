import pytest


def pytest_addoption(parser):
    parser.addoption("--platform", choices=["host", "veerwolf", "el2sim"], default="host")
    parser.addoption("--port", default=None)
    parser.addoption("--baudrate", type=int, default=115200)


@pytest.fixture
def platform(request):
    return request.config.getoption("--platform")


@pytest.fixture
def port(request):
    return request.config.getoption("--port")


@pytest.fixture
def baudrate(request):
    return request.config.getoption("--baudrate")