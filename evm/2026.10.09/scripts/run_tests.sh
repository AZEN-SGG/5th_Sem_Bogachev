#!/usr/bin/env bash

mkdir -p -- logs || exit 1
log_file="logs/run_tests_$(date +%Y-%m-%d_%H-%M-%S)_$$.log"
printf 'Log: %s\n' "$log_file"
exec > "$log_file" 2>&1 || exit 1

script_dir=$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)
program="./a.out"
test_dir="$script_dir/../tests"
r=${1:-6}
blocks=(1 3 10 15 30)

run_case ()
{
  printf '\n'
  printf '%q ' "$program" "$@"
  printf '\n'
  "$program" "$@"
  printf 'Exit code: %d\n' "$?"
}

for file in "$test_dir"/*
do
  [[ -f "$file" ]] || continue
  n=$(awk '{ count += NF } END { print int(sqrt(count)) }' "$file")

  for m in "${blocks[@]}"
  do
    run_case "$n" "$m" "$r" 0 "$file"
  done
done

for s in 1 2 3 4
do
  for ((n = 1; n <= 30; ++n))
  do
    for m in "${blocks[@]}"
    do
      run_case "$n" "$m" "$r" "$s"
    done
  done
done
