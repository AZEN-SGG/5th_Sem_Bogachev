#!/usr/bin/env bash

task=${1:-}

case "$task" in
  1|3|6|7)
    s=1
    sizes=(5400 10800 21600 43200)
    ;;
  2|4|8)
    s=1
    sizes=(3240 6480 12960 25920)
    ;;
  5)
    s=1
    sizes=(5760 11520 23040 46080)
    ;;
  9|10|11|15|16|17|21|23)
    s=3
    sizes=(5400 10800 21600 43200)
    ;;
  12|13|14|18|19|20|22|24)
    s=3
    sizes=(3240 6480 12960 25920)
    ;;
  *)
    printf 'Usage: %s task [r], task = 1..24\n' "$0" >&2
    exit 1
    ;;
esac


for utility in python3 taskset readelf ldd mkdir date
do
  if ! command -v "$utility" > /dev/null 2>&1
  then
    printf 'Required utility not found: %s\n' "$utility" >&2
    exit 1
  fi
done

program="./a.out"
m=60
r=${2:-6}

if [[ ! -x "$program" ]]
then
  printf 'Executable not found: %s\n' "$program" >&2
  exit 1
fi

mkdir -p -- logs || exit 1
log_file="logs/run_large_task${task}_$(date +%Y-%m-%d_%H-%M-%S)_$$.log"
printf 'Log: %s\n' "$log_file"
exec > "$log_file" 2>&1 || exit 1

if ! elf_info=$(LC_ALL=C readelf -l "$program" 2>&1)
then
  printf '%s\n' "$elf_info" >&2
  exit 1
fi

if [[ "$elf_info" == *"Requesting program interpreter: "* ]]
then
  interpreter=${elf_info#*"Requesting program interpreter: "}
  interpreter=${interpreter%%]*}
  if [[ ! -x "$interpreter" ]]
  then
    printf 'Program interpreter not found: %s\n' "$interpreter" >&2
    exit 1
  fi

  if ! libraries=$(LC_ALL=C ldd -r "$program" 2>&1)
  then
    printf '%s\n' "$libraries" >&2
    exit 1
  fi
  printf '%s\n' "$libraries"
  case "$libraries" in
    *"not found"*|*"undefined symbol:"*|*"cannot be preloaded"*)
      printf 'Shared library check failed\n' >&2
      exit 1
      ;;
  esac
else
  printf 'Static executable: no shared-library interpreter\n'
fi

cpu=$(python3 - "${CPU:-auto}" <<'PY'
import os
import sys
import time
from pathlib import Path


def read_stats():
    stats = {}
    for line in Path('/proc/stat').read_text().splitlines():
        fields = line.split()
        if fields and fields[0].startswith('cpu') and fields[0][3:].isdigit():
            values = list(map(int, fields[1:9]))
            stats[int(fields[0][3:])] = (sum(values), values[3] + values[4])
    return stats


def siblings(cpu):
    path = Path(f'/sys/devices/system/cpu/cpu{cpu}/topology/thread_siblings_list')
    if not path.is_file():
        return [cpu]
    result = []
    for item in path.read_text().strip().split(','):
        bounds = item.split('-')
        result.extend(range(int(bounds[0]), int(bounds[-1]) + 1))
    return result


try:
    allowed = os.sched_getaffinity(0)
    requested = sys.argv[1]
    if requested != 'auto':
        if not requested.isascii() or not requested.isdecimal() or int(requested) not in allowed:
            sys.exit('CPU must be one of: ' + ', '.join(map(str, sorted(allowed))))
        cpu = int(requested)
        print(f'CPU: {cpu} (manual)', file=sys.stderr)
    else:
        before = read_stats()
        time.sleep(1)
        after = read_stats()
        load = {}
        for cpu in before.keys() & after.keys():
            total = after[cpu][0] - before[cpu][0]
            idle = after[cpu][1] - before[cpu][1]
            load[cpu] = max(0.0, min(1.0, (total - idle) / total)) if total > 0 else 1.0
        allowed &= os.sched_getaffinity(0) & load.keys()
        if not allowed:
            sys.exit('No available CPUs in /proc/stat')
        cpu = min(allowed, key=lambda c: (sum(load.get(s, 1.0) for s in siblings(c)), load[c], c))
        print(f'CPU: {cpu}, sampled load: {load[cpu]:.1%}', file=sys.stderr)
    print(cpu)
except (OSError, ValueError) as error:
    sys.exit(str(error))
PY
) || exit 1

result=0

run_case ()
{
  printf '\n'
  printf '%q ' taskset -c "$cpu" "$program" "$@"
  printf '\n'
  taskset -c "$cpu" "$program" "$@"
  local code=$?
  printf 'Exit code: %d\n' "$code"
  if ((code != 0))
  then
    result=1
  fi
}

printf 'Task: %d\n' "$task"

for n in "${sizes[@]}"
do
  run_case "$n" "$m" "$r" "$s"
done

exit "$result"
