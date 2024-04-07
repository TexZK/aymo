import json
import os
import sys
from contextlib import redirect_stdout

__thin__ = '-' * 80

if __name__ == '__main__':
    inpath = sys.argv[1]
    outpath = sys.argv[2]

    with open(inpath, 'rt') as infile:
        lines = infile.readlines()

    durations = {}
    cpuexts = set()

    for index, line in enumerate(lines):
        print(__thin__)
        print(f'Entry:      {1+index:3d} / {len(lines):3d}')

        info = json.loads(line)
        name = info['name']
        if not name.startswith('tda8425_process_'):
            continue

        cmdline = info['command']
        cpuext = cmdline[cmdline.index('--cpu-ext')+1]
        exit_code = info['returncode']

        print(f'CPU-ext:    {cpuext}')
        print(f'Command:    {cmdline}')
        print(f'Exit-code:  {exit_code}')
        assert not exit_code

        cpuexts.add(cpuext)
        stdout = info['stdout']
        rtidx = stdout.index('Render time:')
        sidx = stdout.index('seconds', rtidx)
        duration = float(stdout[rtidx+12:sidx])
        print(f'Duration:   {duration} seconds')

        durations[cpuext] = durations.get(cpuext, 0) + duration

    cpuexts.remove('dummy')
    cpuexts.remove('none')
    cpuexts = ['dummy', 'none'] + list(sorted(cpuexts))

    with open(outpath, 'wt') as outfile:
        outfile.write(f'TOTAL')
        for cpuext in cpuexts:
            outfile.write(f',{cpuext}')
        outfile.write('\n')

        for cpuext in cpuexts:
            duration = durations[cpuext]
            outfile.write(f',{duration:.6f}')
        outfile.write('\n')
