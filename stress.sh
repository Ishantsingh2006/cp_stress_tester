#!/bin/bash

TESTS=10
CHECK_MODE=0
MAIN_FILE=""
BRUTE_FILE=""
GEN_FILE=""
SANDBOX=".tmp_stress"

show_help() {
  echo "usage: $(basename "$0") -m <main_sol.cpp> -b <brute_or_checker.cpp> -g <generator.cpp> [OPTIONS]"
  echo "Required Arguments:"
  echo "  -m, --main         Candidate solution C++ file to be tested"
  echo "  -b, --brute        Brute force solution (or checker code if -c is used)"
  echo "  -g, --gen          Testcase generator C++ file"
  echo "Optional Arguments:"
  echo "  -t, --tests        Number of test cases to run (default: 10)"
  echo "  -c, --check_output Checker/Validator mode: uses brute file to validate candidate's output"
  echo "  -h, --help         Show help message"
}
if [[ $# -eq 0 ]]; then
  echo "Error: No arguments provided."
  echo "use '--help' for more information."
  exit 1
fi
while [[ $# -gt 0 ]]; do
  case $1 in
    -m|--main) 
      if [[ -z "$2" || "$2" == -* ]]; then echo "Error: missing filename for $1"; exit 1; fi
      MAIN_FILE="$2"; shift 2 
      ;;
    -b|--brute) 
      if [[ -z "$2" || "$2" == -* ]]; then echo "Error: missing filename for $1"; exit 1; fi
      BRUTE_FILE="$2"; shift 2 
      ;;
    -g|--gen) 
      if [[ -z "$2" || "$2" == -* ]]; then echo "Error: missing filename for $1"; exit 1; fi
      GEN_FILE="$2"; shift 2 
      ;;
    -t|--tests) 
      if [[ -z "$2" || "$2" == -* ]]; then echo "Error: missing number for $1"; exit 1; fi
      if ! [[ "$2" =~ ^[0-9]+$ ]]; then echo "Error: $1 requires an integer."; exit 1; fi
      TESTS="$2"; shift 2 
      ;;
    -c|--check_output) 
      CHECK_MODE=1; shift 1 
      ;;
    -h|--help) 
      show_help; exit 0 
      ;;
    *) 
      echo "Unknown argument: $1"
      echo "Try '--help' for more information."
      exit 1 
      ;;
  esac
done
if [[ -z "$MAIN_FILE" || -z "$BRUTE_FILE" || -z "$GEN_FILE" ]]; then
  echo "Error: missing required files."
  echo "try '--help' for more information."
  exit 1
fi

for file in "$MAIN_FILE" "$BRUTE_FILE" "$GEN_FILE"; do
  if [[ ! -f "$file" ]]; then
    echo "Error: file '$file' does not exist in the current directory."
    exit 1
  fi
done
mkdir -p "$SANDBOX"
compile() {
  local src="$1"
  local out="$2"
  if ! g++ -O3 -std=c++23 "$src" -o "$out" 2> "${out}_err.txt"; then
    echo "Compilation failed for $src. error:"
    cat "${out}_err.txt"
    rm -rf "$SANDBOX"
    exit 1
  fi
  rm -f "${out}_err.txt"
}
compile "$GEN_FILE" "$SANDBOX/gen_bin"
compile "$MAIN_FILE" "$SANDBOX/main_bin"
compile "$BRUTE_FILE" "$SANDBOX/brute_bin"

FAILED=0
FAIL_REASON=""

for ((i=1; i<=TESTS; i++)); do
  if ! ./$SANDBOX/gen_bin > $SANDBOX/input.txt 2> $SANDBOX/gen_err.txt; then
    echo "Testcase #$i: RUNTIME ERROR in Generator!"
    FAILED=1; FAIL_REASON="GEN_RE"; break
  fi
  ./$SANDBOX/main_bin < $SANDBOX/input.txt > $SANDBOX/main_out.txt 2> $SANDBOX/main_err.txt
  MAIN_EXIT=$?
  if [[ $MAIN_EXIT -ne 0 ]]; then
    echo "Testcase #$i: RUNTIME ERROR in Candidate solution"
    FAILED=1; FAIL_REASON="MAIN_RE"; break
  fi
  if [[ $CHECK_MODE -eq 1 ]]; then
    ./$SANDBOX/brute_bin $SANDBOX/input.txt $SANDBOX/main_out.txt > $SANDBOX/checker_msg.txt 2> $SANDBOX/checker_err.txt
    BRUTE_EXIT=$?
    if [[ $BRUTE_EXIT -ne 0 ]]; then
      echo "Testcase #$i: FAILED (Checker rejected output or crashed)"
      FAILED=1; FAIL_REASON="CHECKER_FAIL"; break
    fi
  else
    ./$SANDBOX/brute_bin < $SANDBOX/input.txt > $SANDBOX/brute_out.txt 2> $SANDBOX/brute_err.txt
    BRUTE_EXIT=$?
    if [[ $BRUTE_EXIT -ne 0 ]]; then
      echo "Testcase #$i: RUNTIME ERROR in Brute Force"
      FAILED=1; FAIL_REASON="BRUTE_RE"; break
    fi
    if ! diff -Z --color=always $SANDBOX/main_out.txt $SANDBOX/brute_out.txt > $SANDBOX/diff_out.txt; then
      echo "Testcase #$i: WRONG ANSWER (Outputs differs)"
      FAILED=1; FAIL_REASON="WA"; break
    fi
  fi
done

if [[ $FAILED -eq 1 ]]; then
  echo "Input used:"
  cat $SANDBOX/input.txt
  if [[ "$FAIL_REASON" == "MAIN_RE" ]]; then
    echo "Candidate output (before crashing):"
    cat $SANDBOX/main_out.txt
    echo "Runtime error details:"
    cat $SANDBOX/main_err.txt
  elif [[ "$FAIL_REASON" == "WA" ]]; then
    echo "Candidate output:"
    cat $SANDBOX/main_out.txt
    echo "Expected output:"
    cat $SANDBOX/brute_out.txt
    echo "Diff mismatch:"
    cat $SANDBOX/diff_out.txt
  elif [[ "$FAIL_REASON" == "CHECKER_FAIL" ]]; then
    echo "Candidate output:"
    cat $SANDBOX/main_out.txt
    echo "Checker output/error:"
    cat $SANDBOX/checker_msg.txt
    cat $SANDBOX/checker_err.txt
  fi
else echo "All testcases passed"
fi
rm -rf "$SANDBOX"
