# samp-node

Forked from [`AmyrAhmady/samp-node`](https://github.com/AmyrAhmady/samp-node)

## Changes in this fork

- Only can work with [infernus](https://github.com/dockfries/infernus).
- Update `Node.js` to **v22.22.3**.
- Both `ESModule` and `CommonJS` supported, depending on the type field of package.json and the bundler output format.
- Only `entry_file` is used, `resource` config are removed.
- Removed `samp.fire` to avoid crashes.
- Building based on `ubuntu-latest` means you may need a higher version of glibc.
- Updated github workflows.

## Api

[check here](./api.md)

## libnode (Node.js shared library)

This project depends on `libnode` — Node.js built as a shared library.

Pre-built binaries are downloaded automatically from
[github.com/dockfries/libnode/releases](https://github.com/dockfries/libnode/releases)
during CMake configuration.

See that repository for instructions on building libnode from source for a
specific Node.js version.

### Local paths

If you prefer to build libnode yourself, place the files here:

| Arch | Windows | Linux |
|------|---------|-------|
| x86 (default) | `deps/node/lib/Release/win/libnode.lib` + `libnode.dll` | `deps/node/lib/Release/linux/libnode.so.127` |
| x64 | `deps/node/lib/Release/win64/libnode.lib` + `libnode.dll` | `deps/node/lib/Release/linux64/libnode.so.127` |

Set `-D__deps_check_enabled=false` when running cmake to skip the download.

## How to build samp-node

### linux with docker

```sh
git clone https://github.com/dockfries/samp-node
# or ssh
# git clone git@github.com:dockfries/samp-node.git
cd samp-node
git submodule update --init

chmod +x ./build.sh
./build.sh 22.22.3 # version
```

### linux with cmake

```sh
cd samp-node
mkdir build && cd build

# for 32-bit:
cmake -DCMAKE_BUILD_TYPE=Release ..
# for 64-bit:
# cmake -DPLUGIN_ARCH=x64 -DCMAKE_BUILD_TYPE=Release ..

make -j$(nproc)
```

### windows with Visual Studio

```pwsh
cd samp-node
mkdir build, releases -ErrorAction SilentlyContinue
cd build

# for 32-bit:
cmake .. -A Win32
# for 64-bit:
# cmake .. -A x64 -DPLUGIN_ARCH=x64

cmake --build . --config Release
cpack

cd ..
Move-Item -Path "build/cpack/*" -Destination "releases/" -Force
```

### github actions

fork and run on github actions.

## Credits

- [Damo](https://github.com/damopewpew) for his [samp.js project](https://github.com/damopewpew/samp.js).
- [Hual](https://github.com/Hual/) for some v8 tips
- [Graber](https://github.com/AGraber) for a few suggestions and helpful advices
- [pkfln (peek)](https://github.com/pkfln) for fixing and adding some samp callbacks/events in samp-node, and making [@sa-mp/node](https://github.com/samp-dev/node)
- [polygxn](https://github.com/polygxn) for his changes in README.md which don't exist anymore
- [JustMichael (ADRFranklin)](https://github.com/ADRFranklin) for his contributions, including fixes, features, and any others he's going to do in future
- [Alexander Plutalov (plutalov)](https://github.com/plutalov) for fixing long time crash issues and not having context running properly
- [iAmir (AmyrAhmady)](https://github.com/AmyrAhmady) samp-node and omp-node developer
