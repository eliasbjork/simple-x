import argparse
import serial


BAUD_RATE = 115200

parser = argparse.ArgumentParser()
parser.add_argument("port")


def query(byte_str):
    with serial.Serial(PORT, BAUD_RATE) as s:
        s.write(byte_str)
        s.flush()

        while not s.in_waiting:
            continue

        res = s.read(s.in_waiting)
    return res.decode("utf-8")


def main():
    args = parser.parse_args()

    global PORT
    PORT = args.port

    print(query(b"a=2, b=3."))
    print(query(b"a=10, b=100."))
    print(query(b"a=120, b=-1."))
    print(query(b"a=150, b=-23."))


if __name__ == '__main__':
    main()