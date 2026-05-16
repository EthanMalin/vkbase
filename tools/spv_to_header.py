#!/usr/bin/env python3
"""Convert SPIR-V .spv files into a C uint32_t header.

Usage:
    python3 tools/spv_to_header.py -o apps/pyramid/shaders/shaders.h \
        build/shaders/pyramid/pyramid.vert.spv \
        build/shaders/pyramid/pyramid.frag.spv

Each .spv file produces two symbols in the output header:
    static const uint32_t <name>_spv[];        -- the SPIR-V words
    static const uint32_t <name>_spv_words;    -- word count

The <name> is derived from the filename by stripping the .spv extension and
replacing every '.' and '-' with '_'.  Examples:
    pyramid.vert.spv  ->  pyramid_vert
    my-effect.comp.spv -> my_effect_comp
"""

import argparse
import os
import struct
import sys


def _array_name(spv_path: str) -> str:
    base = os.path.basename(spv_path)
    if base.endswith('.spv'):
        base = base[:-4]
    return base.replace('.', '_').replace('-', '_') + '_spv'


def _spv_to_c(spv_path: str) -> str:
    data = open(spv_path, 'rb').read()
    if len(data) % 4 != 0:
        raise ValueError(f'{spv_path}: size ({len(data)}) is not a multiple of 4')
    words = struct.unpack_from('<' + 'I' * (len(data) // 4), data)
    arr  = _array_name(spv_path)
    lines = [f'/* {os.path.basename(spv_path)} */']
    lines.append(f'static const uint32_t {arr}[] = {{')
    for i in range(0, len(words), 8):
        chunk = words[i:i+8]
        lines.append('    ' + ', '.join(f'0x{w:08x}' for w in chunk) + ',')
    lines.append('};')
    lines.append(f'static const uint32_t {arr}_words = {len(words)};')
    return '\n'.join(lines)


def main(argv=None):
    parser = argparse.ArgumentParser(description=__doc__,
        formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument('spv_files', nargs='+', metavar='FILE.spv')
    parser.add_argument('-o', '--output', required=True, metavar='OUT.h')
    args = parser.parse_args(argv)

    guard = os.path.basename(args.output).upper().replace('.', '_').replace('-', '_')

    lines = [
        f'#ifndef {guard}',
        f'#define {guard}',
        '',
        '#include <stdint.h>',
        '',
    ]

    for spv in args.spv_files:
        lines.append(_spv_to_c(spv))
        lines.append('')

    lines.append(f'#endif /* {guard} */')

    with open(args.output, 'w') as fh:
        fh.write('\n'.join(lines) + '\n')

    print(f'Generated {args.output} ({len(args.spv_files)} shader(s))')


if __name__ == '__main__':
    main()
