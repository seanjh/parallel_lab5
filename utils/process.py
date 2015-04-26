import argparse
import os

import numpy as np

SAMPLES_PER_PIXEL = 3
IMAGE_PATH = os.path.join('..', 'img')


class Pixel(object):
    def __init__(self, red, blue, green):
        self._red = red
        self._blue = blue
        self._green = green

    @property
    def red(self):
        return self._red

    @property
    def blue(self):
        return self._blue

    @property
    def green(self):
        return self._green

    def multiply(self, scalar):
        return Pixel(self._red * scalar, self._blue * scalar, self._green * scalar)

    def add(self, pixel):
        return Pixel(self._red + pixel.red, self._blue + pixel.blue, self._green + pixel.green)

    def __repr__(self):
        return 'Pixel(%0.3f, %0.3f, %0.3f)' % (self.red, self.blue, self.green)

    def __str__(self):
        return self.__repr__()


def is_comment(line):
    return line.strip().startswith('#')


def is_empty(line):
    return len(line.strip()) == 0


def read_header(fh):
    file_type = None
    file_dimensions = None
    file_max_val = None

    for line in fh:
        if not is_comment(line) and not is_empty(line):
            if not file_type:
                file_type = line.strip()
                # print 'FILE TYPE %s' % file_type
            elif not file_dimensions:
                file_dimensions = tuple([int(val) for val in line.strip().split()])
                # print 'FILE DIMENSIONS ARE %s' % [v for v in file_dimensions]
            elif not file_max_val:
                file_max_val = int(line)
                # print 'FILE MAX VALUE IS %s' % file_max_val
                break
            else:
                raise Exception("WTF")

    return file_type, file_dimensions, file_max_val


def convert_ppm_line(line, cols, max_value):
    values = line.strip().split()
    assert (len(values) == SAMPLES_PER_PIXEL * cols)
    pixels = []

    for i in range(cols):
        int_vals = [int(val) for val in values[i * SAMPLES_PER_PIXEL:i * SAMPLES_PER_PIXEL + SAMPLES_PER_PIXEL]]
        assert len(int_vals) == 3
        r, g, b = [float(val) / float(max_value) for val in int_vals]
        pixel = Pixel(r, g, b)
        # print pixel
        pixels.append(pixel)

    return pixels


def convert_ppm_raster(fh, max_val):
    samples = list()
    for line in fh:
        values = line.strip().split()
        for sample in values:
            assert int(sample) <= max_val
            samples.append(int(sample))
            if len(samples) == SAMPLES_PER_PIXEL:
                sample_doubles = [normalize_sample(val, max_val) for val in samples]
                pixel = Pixel(sample_doubles[0], sample_doubles[1], sample_doubles[2])
                samples = list()
                yield pixel


def convert_ppm(filename):
    image = list()
    with open(filename, 'rb') as infile:
        file_type, dimensions, max_val = read_header(infile)
        rows, cols = dimensions
        row = list()
        for i, pixel in enumerate(convert_ppm_raster(infile, max_val)):
            if len(row) == cols:
                image.append(row)
                assert len(row) == cols
                row = list()
                # print 'Completed row %d' % len(image)
            if pixel is not None:
                row.append(pixel)
        if row:
            assert len(row) == cols
            image.append(row)
        print 'Rows in image: %d' % len(image)
        assert len(image) == rows


def convert_ppm_rgb(filename):
    # print filename
    with open(filename, 'rb') as infile:
        file_type, dimensions, max_val = read_header(infile)
        rows, cols = dimensions

        image_red = np.zeros((rows, cols))
        image_green = np.zeros((rows, cols))
        image_blue = np.zeros((rows, cols))
        for i, pixel in enumerate(convert_ppm_raster(infile, max_val)):
            row_num = i / cols
            col_num = i % cols
            # print 'Updating Pixel at row %d, col %d' % (row_num, col_num)
            image_red[row_num][col_num] = pixel.red
            image_green[row_num][col_num] = pixel.green
            image_blue[row_num][col_num] = pixel.blue

    return image_red, image_green, image_blue


def normalize_sample(sample, max_value):
    return float(sample) / float(max_value)


def apply_filter(image_ppm, stencil_pgm, iterations):
    red_array, green_array, blue_array = convert_ppm_rgb(image_ppm)
    # for each iteration
    #   preallocate work
    #   apply stencil
    # reverse normalize

if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('-i', dest='image_file', required=True)
    parser.add_argument('-s', dest='stencil_file', required=True)
    parser.add_argument('-it', dest='iterations', type=int, default=2)
    args = parser.parse_args()
    image_file = os.path.abspath(os.path.join(IMAGE_PATH, args.image_file))
    stencil_file = os.path.abspath(os.path.join(IMAGE_PATH, args.stencil_file))
    apply_filter(image_file, stencil_file, args.iterations)