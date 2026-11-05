#!/usr/bin/env python3
"""Check selected lines, counts, CLI format, pattern errors and file errors."""

import argparse
import os
from pathlib import Path
import random
import re
import subprocess
import tempfile

ROOT = Path(__file__).resolve().parents[1]

# These expectations are explicit and do not use the regex reference.
CASES = {
    1: [
        ('^ab', ['ab', 'xab', 'abc', '', 'a'], ['ab', 'abc']),
        ('^', ['', 'ab', '^'], ['', 'ab', '^']),
        (r'^\a', ['ba', 'a', 'ab'], ['a', 'ab']),
        (r'\^ab', ['^ab', 'ab', 'x^ab'], ['^ab', 'x^ab']),
        ('a^b', ['xa^by', 'ab'], ['xa^by']),
        (r'\\', ['a\\b', 'ab'], ['a\\b']),
    ],
    2: [
        ('ab$', ['ab', 'zb', 'zab', 'abc', ''], ['ab', 'zab']),
        ('$', ['', 'ab'], ['', 'ab']),
        (r'\$', ['a$', '$a', 'ab'], ['a$', '$a']),
        ('a$b', ['xa$by', 'ab'], ['xa$by']),
        (r'a\b$', ['ab', 'xab', 'ba'], ['ab', 'xab']),
    ],
    3: [
        (r'\<ab', ['ab', 'x ab', 'x\tabc', 'xab', '(ab', ''], ['ab', 'x ab', 'x\tabc']),
        (r'\<', ['', ' \t', 'ab', '\tab'], ['ab', '\tab']),
        (r'x \<ab', ['x ab', 'xx abc', 'x xab'], ['x ab', 'xx abc']),
        (r'\\<ab', ['x\\<ab', '<ab'], ['x\\<ab']),
        ('ab', ['xab', 'abc', ''], ['xab', 'abc']),
    ],
    4: [
        (r'ab\>', ['ab', 'zab', 'ab x', 'zab\tx', 'abc', 'ab!'], ['ab', 'zab', 'ab x', 'zab\tx']),
        (r'\>', ['', ' \t', 'ab', 'ab\t'], ['ab', 'ab\t']),
        (r'ab\> x', ['ab x', 'zab xx', 'abc x'], ['ab x', 'zab xx']),
        (r'\\>', ['a\\>', 'a>'], ['a\\>']),
        ('ab', ['xab', 'abc'], ['xab', 'abc']),
    ],
    5: [
        ('a.b', ['xacby', 'xa by', 'a\tb', 'ab', 'a.b'], ['xacby', 'xa by', 'a\tb', 'a.b']),
        ('.', ['', ' ', 'x'], [' ', 'x']),
        (r'a\.b', ['xa.by', 'axb'], ['xa.by']),
        (r'\.\\', ['x.\\y', '.a'], ['x.\\y']),
        ('abc', ['xabcx', 'abc', 'ab'], ['xabcx', 'abc']),
    ],
    6: [
        ('ab?c', ['xacy', 'xabcy', 'abbc', 'ab'], ['xacy', 'xabcy']),
        ('a?', ['', 'bbb', 'aaa'], ['', 'bbb', 'aaa']),
        ('a?b?c', ['c', 'ac', 'bc', 'abc', 'ab'], ['c', 'ac', 'bc', 'abc']),
        ('a??b', ['b', 'ab', 'aab', 'c'], ['b', 'ab', 'aab']),
        ('?ab', ['x?aby', 'ab'], ['x?aby']),
        (r'a\?b', ['xa?by', 'ab'], ['xa?by']),
        (r'a\??b', ['ab', 'a?b', 'a??b'], ['ab', 'a?b']),
    ],
    7: [
        ('a+b', ['xabz', 'xaaabz', 'b', 'acb'], ['xabz', 'xaaabz']),
        ('a+ab', ['xaabz', 'xaaabz', 'ab'], ['xaabz', 'xaaabz']),
        ('a++b', ['ab', 'aab', 'b'], ['ab', 'aab']),
        ('+ab', ['x+aby', 'ab'], ['x+aby']),
        ('++ab', ['x+aby', 'x+++aby', 'ab'], ['x+aby', 'x+++aby']),
        (r'a\+b', ['xa+by', 'aab'], ['xa+by']),
        (r'a\++b', ['a+b', 'a+++b', 'ab'], ['a+b', 'a+++b']),
    ],
    8: [
        ('a*b', ['xbz', 'xabz', 'xaaabz', 'acb'], ['xbz', 'xabz', 'xaaabz', 'acb']),
        ('a*b*', ['', 'ccc', 'aaa'], ['', 'ccc', 'aaa']),
        ('ab*c*', ['a', 'xa', 'xab', 'xabc'], ['a', 'xa', 'xab', 'xabc']),
        ('a*ab', ['ab', 'aab', 'aaab', 'b'], ['ab', 'aab', 'aaab']),
        ('*ab', ['x*aby', 'ab'], ['x*aby']),
        ('a****b', ['b', 'ab', 'aab'], ['b', 'ab', 'aab']),
        (r'a\*b', ['xa*by', 'ab'], ['xa*by']),
        (r'a\**b', ['ab', 'a*b', 'a***b'], ['ab', 'a*b', 'a***b']),
    ],
    9: [
        ('x[a-c]y', ['xay', 'zxcyr', 'xdy', 'xy'], ['xay', 'zxcyr']),
        ('[a-a]', ['a', 'za', 'b', ''], ['a', 'za']),
        ('[0-9][a-c]', ['2b', 'z9cy', '4d', 'b2'], ['2b', 'z9cy']),
        (r'\[a-c\]', ['x[a-c]y', 'b'], ['x[a-c]y']),
        (r'[\[-\]]', ['[', '\\', ']', '^'], ['[', '\\', ']']),
        ('[--0]', ['-', '.', '/', '0', '1'], ['-', '.', '/', '0']),
    ],
}


def reference(task, pattern):
    """Translate only the feature of this task; use Python re for matching."""
    pieces = []
    i = 0
    op = {6: '?', 7: '+', 8: '*'}.get(task)
    while i < len(pattern):
        ch = pattern[i]
        if ch == '\\':
            i += 1
            if i == len(pattern):
                raise ValueError('trailing backslash')
            ch = pattern[i]
            if task == 3 and ch == '<':
                pieces.append(r'(?<![^ \t])(?=[^ \t])')
            elif task == 4 and ch == '>':
                pieces.append(r'(?<=[^ \t])(?![^ \t])')
            else:
                pieces.append(re.escape(ch))
        elif task == 1 and i == 0 and ch == '^':
            pieces.append('^')
        elif task == 2 and i == len(pattern) - 1 and ch == '$':
            pieces.append('$')
        elif task == 5 and ch == '.':
            pieces.append('.')
        elif op and ch == op and pieces:
            if not pieces[-1].endswith('(?:repeat)'):
                pieces[-1] = '(?:' + pieces[-1] + ')' + op + '(?:repeat)'
        elif task == 9 and ch == '[':
            endpoints = []
            for endpoint in range(2):
                i += 1
                if i == len(pattern):
                    raise ValueError('missing endpoint')
                if pattern[i] == '\\':
                    i += 1
                    if i == len(pattern):
                        raise ValueError('missing escaped endpoint')
                elif pattern[i] in '[]':
                    raise ValueError('unescaped bracket')
                endpoints.append(pattern[i])
                if endpoint == 0:
                    i += 1
                    if i == len(pattern) or pattern[i] != '-':
                        raise ValueError('missing dash')
            i += 1
            if i == len(pattern) or pattern[i] != ']' or ord(endpoints[0]) > ord(endpoints[1]):
                raise ValueError('invalid range')
            pieces.append('[' + re.escape(endpoints[0]) + '-' + re.escape(endpoints[1]) + ']')
        elif task == 9 and ch == ']':
            raise ValueError('unpaired bracket')
        else:
            pieces.append(re.escape(ch))
        i += 1
    return re.compile(''.join(pieces).replace('(?:repeat)', ''))


class Suite:
    def __init__(self, executables, work):
        self.executables = executables
        self.work = work
        self.checked = 0
        self.lines = 0

    def run(self, task, pattern, lines, expected=None, ending='\n', final_newline=True):
        if expected is None:
            regex = reference(task, pattern)
            expected = [line for line in lines if regex.search(line)]
        data = ending.join(lines)
        if lines and final_newline:
            data += ending
        src = self.work / 'input.txt'
        dest = self.work / 'output.txt'
        src.write_bytes(data.encode())
        dest.write_bytes(b'old output')
        result = subprocess.run([str(self.executables[task]), str(src), str(dest), pattern],
                                capture_output=True, text=True, timeout=20)
        wanted = ''.join(line + '\n' for line in expected).encode()
        actual = dest.read_bytes()
        if result.returncode != 0 or actual != wanted:
            raise AssertionError(f'Task {task}, pattern {pattern!r}\n'
                                 f'exit={result.returncode}\nstdout={result.stdout}\nstderr={result.stderr}\n'
                                 f'expected={wanted[:500]!r}\nactual={actual[:500]!r}')
        summary = re.fullmatch(r'.+ : Task = (\d+) Result = (-?\d+) Elapsed = \d+\.\d{2}\n', result.stdout)
        assert summary and int(summary[1]) == task and int(summary[2]) == len(expected), result.stdout
        assert not result.stderr, result.stderr
        self.checked += 1
        self.lines += len(lines)

    def error(self, task, pattern, code, src=None, dest=None):
        src = src or self.work / 'input.txt'
        dest = dest or self.work / 'output.txt'
        result = subprocess.run([str(self.executables[task]), str(src), str(dest), pattern],
                                capture_output=True, text=True, timeout=20)
        assert result.returncode != 0, (task, pattern, result.stdout)
        assert f'Task = {task} Result = {code} Elapsed = ' in result.stdout, result.stdout
        assert not result.stderr, result.stderr
        self.checked += 1


def random_pattern(task, rng):
    alphabet = 'abc ^$<>.?+*[]-\\\t'
    tokens = []
    for _ in range(rng.randrange(0, 7)):
        if task == 9 and rng.random() < .3:
            tokens.append(rng.choice(['[a-c]', '[0-9]', '[a-a]', r'[\[-\]]']))
            continue
        ch = rng.choice(alphabet)
        if ch == '\\' or ch in '[]' and task == 9 or rng.random() < .3:
            ch = '\\' + ch
        tokens.append(ch)
        if task in (6, 7, 8) and rng.random() < .35:
            tokens.append({6:'?',7:'+',8:'*'}[task] * rng.randint(1, 4))
    return ''.join(tokens)


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--executables', type=Path, help='directory containing a01.out ... a09.out')
    parser.add_argument('--random-count', type=int, default=160)
    args = parser.parse_args()
    exes = {task: ((args.executables / f'a{task:02}.out') if args.executables else
                   ROOT / f'{task:02}Ex' / ('a.exe' if os.name == 'nt' else 'a.out')).resolve() for task in range(1, 10)}
    rng = random.Random(20261007)
    alphabet = 'abc ^$<>.?+*[]-\\\t0129'
    corpus = ['', ' ', '\t', 'abc', 'xabcx', 'a', 'b', 'c', 'aaaaab', 'a b', 'a\tb',
              '^ab', 'ab$', '?ab', '+ab', '*ab', 'a\\b', '[a-c]']
    corpus += [''.join(rng.choice(alphabet) for _ in range(rng.randrange(0, 24))) for _ in range(100)]
    with tempfile.TemporaryDirectory(prefix='dz2-tests-') as temp:
        suite = Suite(exes, Path(temp))
        for task in range(1, 10):
            before = suite.checked
            for pattern, lines, expected in CASES[task]:
                suite.run(task, pattern, lines, expected)
            suite.run(task, '', ['', 'a', 'abc'], ['', 'a', 'abc'])
            suite.run(task, '', [], [])
            suite.run(task, 'abc', ['zabc', 'ab', '', 'abc'], ['zabc', 'abc'], final_newline=False)
            suite.run(task, 'abc', ['abc', 'abcx', 'xabc', 'ab'], ['abc', 'abcx', 'xabc'], ending='\r\n')
            # Input lines fit in the original LEN=1234 buffer by assumption.
            line = 'x' * 1229 + 'abc'
            suite.run(task, 'abc', [line, 'abc', 'miss'], [line, 'abc'])
            suite.run(task, 'abc', ['x' + line], ['x' + line], final_newline=False)
            # The decoded pattern can fill all LEN - 1 character positions.
            boundary_pattern = 'a' * 1233
            suite.run(task, boundary_pattern, [boundary_pattern],
                      [boundary_pattern], final_newline=False)
            suite.run(task, boundary_pattern, ['a' * 1232, 'b'], [])
            suite.error(task, 'a' * 1232 + '\\', -3)
            for _ in range(args.random_count):
                suite.run(task, random_pattern(task, rng), corpus)
            suite.run(task, 'abc', ['abc'] * 2500, ['abc'] * 2500)
            invalid = ['\\', 'ab\\', 'ab\\\\\\']
            if task == 9:
                invalid += ['[', ']', '[]', '[ab]', '[a-]', '[z-a]', '[a-b', '[a-b]]', '[a-b-c]', '[a-\\']
            for pattern in invalid:
                suite.error(task, pattern, -3)
            suite.error(task, 'abc', -1, src=suite.work / 'missing.txt')
            suite.error(task, 'abc', -1, dest=suite.work)
            if os.name == 'posix':
                suite.error(task, 'abc', -2, src=suite.work)
                suite.error(task, 'abc', -4, dest=Path('/dev/full'))
            usage = subprocess.run([str(exes[task])], capture_output=True, text=True, timeout=5)
            assert usage.returncode != 0 and usage.stdout.startswith('Usage:'), usage.stdout
            suite.checked += 1
            print(f'Task {task}: {suite.checked - before} checks passed', flush=True)
        print(f'PASS: {suite.checked} program runs; {suite.lines} line comparisons')


if __name__ == '__main__':
    main()
