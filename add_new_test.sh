testname=$1
if [ "$testname" = "" ]; then
    echo "usage: $0 <test_name>"
    exit -1
fi
echo "add test [$testname]: src/tests/test_${testname}.cc "
cp src/tests/template_t.cc src/tests/test_${testname}.cc
