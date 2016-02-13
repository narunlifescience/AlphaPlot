# test that the code builds with different compile options The default
# (aeb) build tests the default configuration (CONFIG=release, no
# extras like Origin
find . -name Makefile -exec rm {} \;
qmake DEFINES=ORIGIN_IMPORT CONFIG=debug
make clean
make -j`grep processor /proc/cpuinfo|wc -l` 
if [ $? -ne 0 ]; then 
    echo "FAILED"
    exit 1; 
fi
echo "PASSED"
