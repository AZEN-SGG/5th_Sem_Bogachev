from pathlib import Path
import subprocess
import sys


def run(arguments, expected):
    result = subprocess.run(arguments, capture_output=True, text=True)
    if result.returncode or result.stderr != expected:
        print('Failed:', ' '.join(arguments))
        print('Exit code:', result.returncode)
        print('stdout:', repr(result.stdout))
        print('stderr:', repr(result.stderr))
        return None
    return result.stdout


def main():
    if len(sys.argv) != 3:
        print('Test executable and timer probe are required', file=sys.stderr)
        return 1
    executable = str(Path(sys.argv[1]).resolve())
    probe = str(Path(sys.argv[2]).resolve())
    expected = 'Cannot allocate shared pointer state\n'
    expected += 'Cannot allocate object copy\n' * 2
    output = run([executable], expected)
    if output is None:
        return 1
    print(output, end='')
    cases = (
        ('quiet', ''),
        ('invalid', 'Invalid warning interval\n'),
        ('default', '1'),
        ('lifetime', '1'),
        ('assign', '11'),
        ('self', '1'),
        ('inactive', 'Invalid warning interval\n'),
        ('reuse', '1'),
    )
    for mode, expected in cases:
        output = run([probe, mode], expected)
        if output is None or output:
            return 1
    print(f'Elapsed warning cases: {len(cases)}, errors: 0')
    return 0


if __name__ == '__main__':
    sys.exit(main())
