import argparse
import glob
import os
import serial


BAUD_RATE = 115200

parser = argparse.ArgumentParser()
parser.add_argument("port")


def run_test(dir):
    with serial.Serial(PORT, BAUD_RATE) as s:
        # transmit test input
        with open(f"{dir}/i", "r") as i:
            for line in i:
                byte_str = line.encode("utf-8")
                s.write(byte_str)
                s.flush()

        # await response
        while not s.in_waiting:
            continue

        res = s.read(s.in_waiting)

    print(res.decode("utf-8"), end="")

    with open(f"{dir}/linopt.sol", "r") as sol:
        # only print max z
        line0 = sol.readline()
        print(line0)


def main():
    args = parser.parse_args()

    global PORT
    PORT = args.port

    dirs_with_tests = list(filter(lambda f: os.path.isdir(f), glob.glob('*/*/*/*')))

    for dir in dirs_with_tests:
        run_test(dir)


if __name__ == '__main__':
    main()