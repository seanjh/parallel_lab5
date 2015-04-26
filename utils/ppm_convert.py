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


def output_pnm_filename(diretory, input_file, old_extension, new_extension):
    # print 'directory %s, input %s, extension %s' % (diretory, input_file, new_extension)
    parts = input_file.split('.')
    parts.pop()
    filename = ''.join([''.join(parts), '~~', old_extension, '~~', '.', new_extension])
    return os.path.abspath(os.path.join(diretory, filename))


def output_original_filename(directory, filename):
    # stuff~~jpg~~.ppm --> [stuff, jpg, .ppm]
    parts = filename.split('~~')
    # print 'PARTS: %s' % parts
    ppm_ext = parts.pop()
    original_extension = parts.pop()
    parts.insert(0, ''.join([TIMESTAMP, '-']))
    root_filename = ''.join(parts)
    full_filname = ''.join([root_filename, '.', original_extension])
    return os.path.join(directory, FILTERED_OUTPUT_DIR, full_filname)


def files_to_original(directory):
    filtered_directory = os.path.join(directory, FILTERED_OUTPUT_DIR)
    if not os.path.exists(filtered_directory):
        print 'Making directory for filtered files: %s' % os.path.abspath(filtered_directory)
        os.mkdir(filtered_directory)

    for file_ in os.listdir(directory):
        filename = os.path.join(directory, file_)
        if is_ppm(file_):
            out_filename = output_original_filename(directory, file_)
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


def files_to_ppm(directory):
    for file_ in os.listdir(directory):
        filename = os.path.join(directory, file_)
        if isfile(filename):
            if is_jpeg(filename):
                out_filename = output_pnm_filename(directory, file_, 'jpg', 'ppm')
                convert_to_ppm('jpegtopnm', filename, out_filename)
            elif is_png(filename):
                out_filename = output_pnm_filename(directory, file_, 'png', 'ppm')
                convert_to_ppm('pngtopnm', filename, out_filename)
            elif is_tiff(filename):
                out_filename = output_pnm_filename(directory, file_, 'tif', 'ppm')
                convert_to_ppm('tifftopnm', filename, out_filename)
            elif is_gif(filename):
                out_filename = output_pnm_filename(directory, file_, 'gif', 'ppm')
                convert_to_ppm('giftopnm', filename, out_filename)
            elif is_ppm(filename):
                pass


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('-d', dest='directory', required=True, help='Directory of files to convert')
    group = parser.add_mutually_exclusive_group(required=True)
    group.add_argument('--to', dest='to_ppm', action='store_true', help='Convert files in directory to PPM')
    group.add_argument('--from', dest='from_ppm', action='store_true', help='Convert ~~ files in directory to original')
    args = parser.parse_args()
    if args.to_ppm:
        files_to_ppm(args.directory)
    elif args.from_ppm:
        files_to_original(args.directory)
    else:
        raise Exception('WTF')