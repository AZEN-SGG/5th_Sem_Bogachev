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

mkdir -p -- logs || exit 1
log_file="logs/run_parallel_task${task}_$(date +%Y-%m-%d_%H-%M-%S)_$$.log"
printf 'Log: %s\n' "$log_file"
exec > "$log_file" 2>&1 || exit 1

program="./a.out"
m=60
r=${2:-6}

run_case ()
{
  printf '\n'
  printf '%q ' "$program" "$@"
  printf '\n'
  "$program" "$@"
  printf 'Exit code: %d\n' "$?"
}

printf 'Task: %d\n' "$task"

for p in 10 20
do
  for n in "${sizes[@]}"
  do
    run_case "$n" "$m" "$p" "$r" "$s"
  done
done
