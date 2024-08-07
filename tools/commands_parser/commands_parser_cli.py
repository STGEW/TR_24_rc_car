import logging
import argparse
import serial
import ctypes

import commands_parser


logging.basicConfig(
    level=logging.DEBUG,
    format='%(asctime)s - %(name)s - %(levelname)s - %(message)s',
    handlers=[logging.StreamHandler()]
)

logger = logging.getLogger(__name__)


class Point2D(ctypes.Structure):
    _fields_ = [("x", ctypes.c_float),
                ("y", ctypes.c_float)]
    _pack_ = 1  # Ensure the struct is packed (no padding between fields)


def new_path_callback(point):
    logger.info(f"New path: x={point.x}, y={point.y}")

def stop_callback():
    logger.info("Stop command received")

# Create a CommandsParser instance
cmd_parser = commands_parser.CommandsParser(new_path_callback, stop_callback)


def main():
    logger.info('Parsing arguments')
    parser = argparse.ArgumentParser()
    parser.add_argument(
        '--port', 
        required=True,
        type=str,
        help='Path to serial port')
    parser.add_argument(
        '--br',
        required=True,
        type=int,
        help="Port's baudrate")
    parser.add_argument(
        '--send',
        required=False,
        action="store_true",
        help="Open in a send mode")
    args = parser.parse_args()
    logger.info(f'Arguments are: {args}')
    port = args.port
    br = args.br

    ser = serial.Serial(port, br)

    try:
        if args.send:
            while True:
                inp = input("Enter one of following: "
                    "stop/done/ok/abort/point 3.33 6.66\r\n")
                if inp == 'stop':
                    ser.write('TR4stop'.encode())
                elif inp == 'done':
                    ser.write('done'.encode())
                elif inp == 'ok':
                    ser.write('ok'.encode())
                elif inp == 'abort':
                    ser.write('abort'.encode())
                elif 'point' in inp:
                    _, x, y = inp.split(' ')
                    point = Point2D(x=float(x), y=float(y))
                    # Convert the struct to bytes
                    point_bytes = ctypes.string_at(ctypes.byref(point), ctypes.sizeof(point))
                    ser.write("LF".encode())
                    size_of_point = ctypes.sizeof(point)
                    ser.write(ctypes.c_ubyte(size_of_point).value.to_bytes(1, byteorder='little'))
                    ser.write(point_bytes)
                else:
                    print('Unknown command. Stopping')
                    return
        else:
            while True:
                # Read the incoming data
                data = ser.read()
                logger.info(f'Data: {data}, {type(data)}')
                cmd_parser.parse(data)
                logger.info(f"Received: {data}")
    except KeyboardInterrupt:
        logger.info("Exiting program.")

    finally:
        # Close the serial connection
        ser.close()


if __name__ == "__main__":
    main()
