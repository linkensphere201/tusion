### description
this is a tool collection for personal, includes:

- a benchmark tool for nebula.

- a data generator for graph database.

- a thrift-tool like tcp introspection tool working for fbthrift. (PEND)

- a redis-protocol-compatible key-value store. (PEND)

### build

to build, run these scripts like below:

```
installdir=...
includedir=...
librarydir=...
mkdir build
cmake -S . -B build                                    \
    -DCMAKE_EXPORT_COMPILE_COMMANDS=ON                 \
    -DCMAKE_INSTALL_PREFIX="${installdir}"             \
    -DCMAKE_INCLUDE_PATH="${includedir}"               \
    -DCMAKE_LIBRARY_PATH="${librarydir}"
make VERBOSE=1 -C build/
```

