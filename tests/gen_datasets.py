import array
import random
import sys

LENGTH = 1000000

if __name__ == '__main__':

    random.seed(0)
    data_list = [random.gauss(0, .2) for _ in range(LENGTH)]

    data_array = array.array('f', data_list)
    if sys.byteorder != 'little':
        data_array.byteswap()

    data_view = memoryview(data_array)

    with open('gauss_f32le.dat', 'wb') as file:
        file.write(data_view)

    data_array.byteswap()
    data_view = memoryview(data_array)

    with open('gauss_f32be.dat', 'wb') as file:
        file.write(data_view)

    data_list = [int(max(-32767, min(x * 32767, +32767))) for x in data_list]

    data_array = array.array('h', data_list)
    if sys.byteorder != 'little':
        data_array.byteswap()

    data_view = memoryview(data_array)

    with open('gauss_s16le.dat', 'wb') as file:
        file.write(data_view)

    data_array.byteswap()
    data_view = memoryview(data_array)

    with open('gauss_s16be.dat', 'wb') as file:
        file.write(data_view)
