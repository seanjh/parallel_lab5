#!/usr/bin/env python

import argparse
import os
import calendar
import subprocess
from os.path import isfile
from datetime import datetime


FILTERED_OUTPUT_DIR = 'filtered'
TIMESTAMP = str(calendar.timegm(datetime.now().timetuple()))


def extension_matches(filename, matches):
    parts = filename.split('.')
    extension = parts.pop().lower()
    return extension in matches


def is_jpeg(filename):
    return extension_matches(filename, ['jpg', 'jpeg'])


def is_png(filename):
    return extension_matches(filename, ['png'])


def is_tiff(filename):
    return extension_matches(filename, ['tif', 'tiff'])


def is_gif(filename):
    return extension_matches(filename, ['gif'])


def is_ppm(filename):
    return extension_matches(filename, ['ppm'])


def convert_to_ppm(command, infile, outfile):
    # print 'netbpm command: %s, infile: %s, outfile: %s' % (command, infile, outfile)
    with open(outfile, 'wb') as fh:
        command_args = [command, infile]
        ppm_p = subprocess.Popen(command_args, stdout=subprocess.PIPE)
        plain_p = subprocess.Popen(['pnmtopnm', '-plain'], stdin=ppm_p.stdout, stdout=fh)
        plain_p.wait()


def convert_to_original(command, infile, outfile):
    # print 'netbpm command: %s, infile: %s, outfile: %s' % (command, infile, outfile)
    with open(outfile, 'wb') as fh:
        command_args = [command, infile]
        ppm_p = subprocess.Popen(command_args, stdout=fh)
        ppm_p.wait()


def output_pnm_filename(input_file, old_extension, new_extension, directory=None):
    # print 'directory %s, input %s, extension %s' % (diretory, input_file, new_extension)
    parts = input_file.split('.')
    parts.pop()
    filename = ''.join([''.join(parts), '~~', old_extension, '~~', '.', new_extension])
    if directory is not None:
        return os.path.abspath(os.path.join(directory, filename))
    else:
        return os.path.abspath(filename)


def output_original_filename(filename, directory=None, extension=None):
    if extension is not None:
    # stuff~~jpg~~.ppm --> [stuff, jpg, .ppm]
        parts = filename.split('~~')
        # print 'PARTS: %s' % parts
        ppm_ext = parts.pop()
        original_extension = parts.pop()
        parts.insert(0, ''.join([TIMESTAMP, '-']))
        root_filename = ''.join(parts)
        full_filename = ''.join([root_filename, '.', original_extension])
    else:
        parts = filename.split('.')
        parts.pop()
        full_filename = ''.join([parts, ('.%s' % extension)])
    if directory is not None:
        return os.path.join(directory, FILTERED_OUTPUT_DIR, full_filename)
    else:
        return os.path.join(FILTERED_OUTPUT_DIR, full_filename)


def ppm_to_original(filename, directory=None, extension=None):
    file_ = filename
    filename = os.path.join(directory, file_) if directory is not None else file_
    if is_ppm(file_):
        out_filename = output_original_filename(directory, file_, extension)
        print 'NEW FILENAME: %s' % out_filename
        if is_jpeg(out_filename):
            convert_to_original('pnmtojpeg', filename, out_filename)
        elif is_png(filename):
            convert_to_original('pnmtopng', filename, out_filename)
        elif is_tiff(filename):
            convert_to_original('pamtotiff', filename, out_filename)
        elif is_gif(filename):
            convert_to_original('pamtogif', filename, out_filename)
    else:
        pass
        # print 'Skipping non-PPM file %s' % file_


def directory_to_original(directory, extension=None):
    filtered_directory = os.path.join(directory, FILTERED_OUTPUT_DIR)
    if not os.path.exists(filtered_directory):
        print 'Making directory for filtered files: %s' % os.path.abspath(filtered_directory)
        os.mkdir(filtered_directory)

    for file_ in os.listdir(directory):
        ppm_to_original(file_, directory, extension)


def file_to_ppm(filename, directory=None):
    file_ = filename
    filename = os.path.join(directory, file_) if directory is not None else file_
    if isfile(filename):
        if is_jpeg(filename):
            out_filename = output_pnm_filename(file_, 'jpg', 'ppm', directory)
            convert_to_ppm('jpegtopnm', filename, out_filename)
        elif is_png(filename):
            out_filename = output_pnm_filename(file_, 'png', 'ppm', directory)
            convert_to_ppm('pngtopnm', filename, out_filename)
        elif is_tiff(filename):
            out_filename = output_pnm_filename(file_, 'tif', 'ppm', directory)
            convert_to_ppm('tifftopnm', filename, out_filename)
        elif is_gif(filename):
            out_filename = output_pnm_filename(file_, 'gif', 'ppm', directory)
            convert_to_ppm('giftopnm', filename, out_filename)
        elif is_ppm(filename):
            pass


def directory_to_ppm(directory):
    for file_ in os.listdir(directory):
        filename = os.path.join(directory, file_)
        file_to_ppm(file_, directory)


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    groupc = parser.add_mutually_exclusive_group(required=True)
    groupc.add_argument('-d', dest='directory', help='Directory of files to convert')
    groupc.add_argument('-f', dest='file', help='Individual file to convert')
    groupc = parser.add_mutually_exclusive_group(required=True)
    groupc.add_argument('--ppm', dest='to_ppm', action='store_true', help='Convert file(s) to PPM')
    groupc.add_argument('--jpg', dest='to_jpeg', action='store_true', help='Convert file(s) to JPG')
    groupc.add_argument('--png', dest='to_png', action='store_true', help='Convert file(s) to PNG')
    # groupc.add_argument('--tif', dest='to_tif', action='store_true', help='Convert file(s) to TIFF')
    # groupc.add_argument('--gif', dest='to_tif', action='store_true', help='Convert file(s) to GIF')
    args = parser.parse_args()

    extension = None
    if args.to_ppm:
        extension = 'ppm'
    elif args.to_jpeg:
        extension = 'jpg'
    elif args.to_png:
        extension = 'png'

    if args.to_ppm:
        if args.directory:
            directory_to_ppm(args.directory)
        elif args.file:
            file_to_ppm(args.file)
    else:
        if args.directory:
            directory_to_original(args.directory, extension)
        elif args.file:
            ppm_to_original(args.file, extension)
