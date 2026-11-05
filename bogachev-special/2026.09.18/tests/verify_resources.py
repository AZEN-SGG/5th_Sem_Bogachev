from pathlib import Path
import subprocess
import tempfile

ROOT = Path(__file__).resolve().parents[1]

HARNESS = r'''#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>

int t@TASK@_solve(const char*, const char*, const char*);

static int descriptors(void)
{
    DIR* dir = opendir("/proc/self/fd");
    int result = 0;
    if (dir == NULL) return -1;
    while (readdir(dir) != NULL) ++result;
    closedir(dir);
    return result;
}

int main(int argc, char** argv)
{
    int before;
    int i;
    if (argc != 5) return 1;
    before = descriptors();
    for (i = 0; i < 100; ++i)
    {
        if (t@TASK@_solve(argv[1], argv[2], "abc") != 1) return 2;
        if (t@TASK@_solve(argv[1], argv[3], "abc") != -1) return 3;
        if (t@TASK@_solve(argv[4], argv[2], "abc") != -1) return 4;
        if (t@TASK@_solve(argv[1], argv[2], "abc\\") != -3) return 5;
        if (t@TASK@_solve(argv[3], argv[2], "abc") != -2) return 6;
        if (t@TASK@_solve(argv[1], "/dev/full", "abc") != -4) return 7;
        if (t@TASK@_solve(argv[1], argv[2], NULL) != -3) return 8;
        if (descriptors() != before) return 9;
    }
    return 0;
}
'''

with tempfile.TemporaryDirectory(prefix='dz2-memory-') as temp:
    work = Path(temp)
    src = work / 'input.txt'
    dest = work / 'output.txt'
    src.write_text('abc\n')
    runs = 0
    for task in range(1, 10):
        folder = ROOT / f'{task:02}Ex'
        harness = work / 'harness.cpp'
        harness.write_text(HARNESS.replace('@TASK@', str(task)))
        checker = work / 'checker'
        subprocess.run(['g++', '-std=c++14', '-O1', '-g', str(harness),
                        str(folder / 'solve.cpp'),
                        '-o', str(checker)], check=True)
        command = ['valgrind', '--quiet', '--leak-check=full', '--show-leak-kinds=all',
                   '--errors-for-leak-kinds=all', '--error-exitcode=99', str(checker),
                   str(src), str(dest), str(work), str(work / 'missing')]
        result = subprocess.run(command, capture_output=True, text=True, timeout=45)
        if result.returncode or result.stderr:
            raise AssertionError(f'Task {task}: exit={result.returncode}\n{result.stderr}')
        runs += 700
        print(f'Task {task}: Valgrind clean; 700 calls; descriptor count unchanged', flush=True)
    print(f'PASS: {runs} direct function calls; no memory errors, leaks or file descriptor leaks')
