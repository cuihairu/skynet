#!/bin/bash
# Run the standalone skynet test services through test/autotest_helper.lua.
#
# Each test runs as the only lua service of a fresh single-node skynet
# instance (harbor 0). The helper launches the test service, waits until
# that service dies, then aborts the node so the process exits cleanly.
#
# A test passes when:
#   - the node exits 0
#   - the helper reports the test as finished ( it did not have to force
#     abort it on timeout )
#   - the helper did not fail to launch it
#   - the log holds no lua error trace, unless the test deliberately
#     exercises an error path ( see EXPECT_ERROR_LOG below )
#
# Usage: test/run_autotest.sh [test names...]   (default: the full list)
set -u
cd "$(dirname "$0")/.." || exit 1

TESTS=(
  testcrypt
  testbson
  testsha
  testcoroutine
  testtimer
  testtimeout
  testdatacenter
  testsharetable
  testdatasheet
  teststm
  testsm
  testqueue
  testping
  testecho
  testselect
  testresponse
  testtobeclosed
  testdeadcall
  testudp
  testsocket
  testnetpack
  testoverload
  testmemlimit
  testmulticast
  testmulticast2
)
[ $# -gt 0 ] && TESTS=("$@")

# Not gate material, deliberately excluded:
#   testhandle      spawns and kills about a million services before its own
#                   loop can terminate, it needs minutes per run
#   testdeadloop, testendless           designed to hang
#   testhttp, testredis, testredis2, testrediscluster, testmysql,
#   testmongodb, testdns, testpipeline, testterm, testharborlink
#                   need a live external service or a second node

# Tests that exercise error paths on purpose: the lua error text in their log
# is the thing they are verifying, so a bare "stack traceback" is not a
# failure for them. The helper's finished marker and the absence of a force
# abort remain the success signals.
#   testtimer        skynet.trace_timeout(true) makes skynet.task() dump
#                   coroutine traces
#   testping         pingserver throws "ERROR" by design
#   testselect       the service under test answers error "ERROR" by design
#   testtobeclosed   deliberately raises on the close / dispatch_error path
#   testmulticast2   the bundled console service dies reading a closed socket
EXPECT_ERROR_LOG=" testtimer testping testselect testtobeclosed testmulticast2 "

unexpected_error() {
	case "$EXPECT_ERROR_LOG" in
		*" $1 "*) return 1 ;;
	esac
	grep -q "stack traceback" "$2"
}

[ -x ./skynet ] || { echo "skynet binary missing; run: make linux"; exit 1; }

ROOT_DIR="$PWD"
WORK="$(mktemp -d)"
trap 'rm -rf "$WORK"' EXIT

pass=0
fail=0
failed=()

for t in "${TESTS[@]}"; do
  cfg="$WORK/$t.config"
  log="$WORK/$t.log"
  {
    echo "include \"$ROOT_DIR/examples/config.path\""
    echo "thread = 2"
    echo "logger = nil"
    echo "logpath = \".\""
    echo "harbor = 0"
    echo "start = \"autotest_helper\""
    echo "bootstrap = \"snlua bootstrap\""
    echo "cpath = root..\"cservice/?.so\""
    echo "autotest_name = \"$t\""
    echo "autotest_timeout = 30"
  } > "$cfg"

  timeout 60 ./skynet "$cfg" > "$log" 2>&1
  rc=$?

  if [ $rc -eq 0 ] \
     && grep -q "Autotest: finished[[:space:]]*$t" "$log" \
     && ! grep -q "Autotest: timeout" "$log" \
     && ! grep -q "Autotest: failed to launch" "$log" \
     && ! unexpected_error "$t" "$log"; then
    pass=$((pass + 1))
    echo "PASS $t"
  else
    fail=$((fail + 1))
    failed+=("$t")
    echo "FAIL $t (rc=$rc, log: $log)"
  fi
done

echo
echo "autotest: pass=$pass fail=$fail total=${#TESTS[@]}"
[ $fail -eq 0 ] || { printf 'failed: %s\n' "${failed[*]}"; exit 1; }
