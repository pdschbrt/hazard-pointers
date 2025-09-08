# Hazard Pointers

Exemplary usages of hazard pointers and more.


## How to build?

```
$ mkdir build-dbg
$ CC=clang CXX=clang++ cmake -G "Ninja" -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DCMAKE_BUILD_TYPE=Debug ..
$ ninja
```

Feel free to run the example programs through `valgrind`.

### Building with sanitizers

You can also build with sanitizers.
You can use the address and undefined behavior sanitizers _or_ the thread sanitizer.
The thread sanitizer is not compatible with the former two sanitizers.
To obtain full sanitizer support, you will need to build the folly library with sanitizers, too.

```
# Address and undefined behavior sanitizers
$ CC=clang CXX=clang++ cmake -G "Ninja" -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DCMAKE_BUILD_TYPE=Debug -DUSE_SANITIZER=address,undefined ..
# Or use thread sanitizer
$ CC=clang CXX=clang++ cmake -G "Ninja" -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DCMAKE_BUILD_TYPE=Debug -DUSE_SANITIZER=thread ..
```
