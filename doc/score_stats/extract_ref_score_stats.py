# AYMO - Accelerated YaMaha Operator
# Copyright (c) 2023-2024 Andrea Zoppi.
#
# This file is part of AYMO.
#
# AYMO is free software: you can redistribute it and/or modify it under the
# terms of the GNU Lesser General Public License as published by the Free
# Software Foundation, either version 2.1 of the License, or (at your option)
# any later version.
#
# AYMO is distributed in the hope that it will be useful, but WITHOUT ANY
# WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for
# more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with AYMO. If not, see <https://www.gnu.org/licenses/>.

# This script extracts some statistics from AdPlug reference scores.
# These stats can help identifying the most stimulating scores for testing.

import glob
import os

if __name__ == '__main__':
    paths = glob.glob(os.path.join('..', '..', 'contrib', 'adplug', 'test', 'testref', '*.ref'))
    print('len(filenames)', len(paths))

    for path in paths:
        print(path)

        with open(path, 'rt') as stream:
            lines = stream.readlines()

        address_hit = [0] * 0x200
        bit_hit_0 = [0] * (0x200 * 8)
        bit_hit_1 = [0] * (0x200 * 8)
        waveform_hit = [0] * 8
        output_hit = [0] * 4
        fourop_hit = [0] * 6
        rhythm_hit = [0] * 5
        has_new = False
        has_rhythm = False

        addrlo = 0
        addrhi = 0
        value = 0

        for line in lines:
            line = line.strip()

            if line.startswith('setchip('):
                addrhi = int(line[8:line.find(')')]) & 1

            elif '<-' in line:
                addrlo, value = [(int(token.strip(), 16) & 0xFF) for token in line.split('<-')]
                address = (addrhi << 8) | addrlo
                address_hit[address] += 1
                base = address * 8
                for i in range(8):
                    if (value >> i) & 1:
                        bit_hit_1[base + i] += 1
                    else:
                        bit_hit_0[base + i] += 1

                if address == 0x104:
                    for i in range(6):
                        fourop_hit[i] += (value >> i) & 1

                elif address == 0x105:
                    if value & 1:
                        has_new = True

                elif address == 0xBD:
                    if value & 0x20:
                        has_rhythm = True
                        for i in range(5):
                            rhythm_hit[i] += (value >> i) & 1

                elif 0xC0 <= addrlo <= 0xDF:
                    for i in range(4):
                        output_hit[i] += (value >> (4 + i)) & 1

                elif 0xE0 <= addrlo <= 0xFF:
                    waveform_hit[value & 0x07] += 1

        with open(os.path.basename(path) + '.csv', 'wt') as stream:
            sep = ','
            stream.write(f'filename{sep}{os.path.basename(path)!r}\n')
            stream.write(f'has_new{sep}{has_new}\n')
            stream.write(f'has_rhythm{sep}{has_rhythm}\n')
            stream.write(f'#outputs_hit{sep}' + sep.join(f'{x}' for x in output_hit) + '\n')
            stream.write(f'#waveform_hit{sep}' + sep.join(f'{x}' for x in waveform_hit) + '\n')
            stream.write(f'#fourop_hit{sep}' + sep.join(f'{x}' for x in fourop_hit) + '\n')
            stream.write(f'#rhythm_hit{sep}' + sep.join(f'{x}' for x in rhythm_hit) + '\n')
            stream.write('\n')
            stream.write(f'address{sep}' + sep.join(f'#bit{i&7}={i>>3}' for i in reversed(range(16))) + '\n')
            for address in range(0x200):
                base = address * 8
                hit_1 = sep.join(f'{bit_hit_1[base + i] or ""}' for i in reversed(range(8)))
                hit_0 = sep.join(f'{bit_hit_0[base + i] or ""}' for i in reversed(range(8)))
                stream.write(f'{address:04X}h{sep}{hit_1}{sep}{hit_0}\n')
