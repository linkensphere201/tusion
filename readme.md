
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

