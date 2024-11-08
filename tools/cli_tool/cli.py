import logging
import argparse
import serial
import ctypes
import time


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


class Vehicle2DPosition(ctypes.Structure):
    _fields_ = [("p", Point2D),
                ("phi", ctypes.c_float)]
    _pack_ = 1  # Ensure the struct is packed (no padding between fields)


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
    args = parser.parse_args()
    logger.info(f'Arguments are: {args}')
    port = args.port
    br = args.br

    ser = serial.Serial(port, br, timeout=0)

    x = 0.0
    y = 0.0

    try:
        while True:
            inp = input(
                "Input 's' 'p 3.33 4.44' "
                "'v 0.50 0.3 3.14' "
                "'r' 'd' 'a' 'o'\r\n")

            if 's' in inp:
                logger.info("You pressed 's'. 'Stop' cmd will be send")
                s = ctypes.c_ubyte(4).value.to_bytes(1, byteorder='little')
                logger.info(s)
                ser.write('TR'.encode())
                ser.write(s)
                ser.write('stop'.encode())
            elif 'p' in inp:
                logger.info(
                    f"You pressed 'p'. Point will be send. "
                    f"Input: '{inp}'")
                _, x, y = inp.split(' ')
                p = Point2D(x=float(x), y=float(y))
                size_p = ctypes.sizeof(p)
                # Convert the struct to bytes
                p_bytes = ctypes.string_at(
                    ctypes.byref(p), size_p)
                ser.write("LF".encode())
                ser.write(
                    ctypes.c_ubyte(size_p).value.to_bytes(
                        1, byteorder='little'))
                ser.write(p_bytes)
            elif 'v' in inp:
                logger.info(
                    f"You pressed 'v'. Vehicles position will be send. "
                    f"Input: '{inp}'")
                _, x, y, phi = inp.split(' ')
                p = Point2D(x=float(x), y=float(y))
                vp = Vehicle2DPosition(
                    p=p, phi=float(phi))
                # Convert the struct to bytes
                size_vp = ctypes.sizeof(vp)
                vp_bytes = ctypes.string_at(
                    ctypes.byref(vp), size_vp)
                ser.write("PV".encode())
                ser.write(vp_bytes)
            elif 'd' in inp:
                logger.info("You pressed 'd'. 'done' cmd will be send")
                ser.write('CMdone'.encode())
            elif 'a' in inp:
                logger.info("You pressed 'a'. 'abort' cmd will be send")
                ser.write('CMabort'.encode())
            elif 'o' in inp:
                logger.info("You pressed 'o'. 'ok' cmd will be send")
                ser.write('CMok'.encode())
            elif 'r' in inp:
                while True:
                    data = ser.read()
                    if data:
                        logger.info(f'Data: {data}, len: {len(data)} {type(data)}')
                    else:
                        break
    except KeyboardInterrupt:
        logger.info("Exiting program.")

    finally:
        # Close the serial connection
        ser.close()


if __name__ == "__main__":
    main()
