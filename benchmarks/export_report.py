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

        cmdline = info['command']
        score = os.path.basename(cmdline[-1])
        cpuext = cmdline[cmdline.index('--cpu-ext')+1]
        exit_code = info['returncode']

        print(f'Score:      {score!r}')
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

        if score not in durations:
            durations[score] = {}
        durations[score][cpuext] = duration

    cpuexts.remove('dummy')
    cpuexts.remove('none')
    cpuexts = ['dummy', 'none'] + list(sorted(cpuexts))
    totals = {cpuext: 0 for cpuext in cpuexts}

    durations = {score: durations[score] for score in sorted(durations, key=lambda x: x.lower())}

    with open(outpath, 'wt') as outfile:
        outfile.write(f'SCORE')
        for cpuext in cpuexts:
            outfile.write(f',{cpuext}')
        outfile.write('\n')

        for score, values in durations.items():
            score = score.replace('"', '\\"')
            outfile.write(f'"{score}"')
            for cpuext in cpuexts:
                duration = values[cpuext]
                outfile.write(f',{duration:.6f}')
                totals[cpuext] += duration
            outfile.write('\n')
