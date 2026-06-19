# samp-node

Forked from [`AmyrAhmady/samp-node`](https://github.com/AmyrAhmady/samp-node)

## Changes in this fork

- Only can work with [infernus](https://github.com/dockfries/infernus).
- Update `Node.js` to **v24.17.0**.
- Both `ESModule` and `CommonJS` supported, depending on the type field of package.json and the bundler output format.
- Only `entry_file` is used, `resource` config are removed.
- Removed `samp.fire` to avoid crashes.
- Building based on `ubuntu-latest` means you may need a higher version of glibc.
- Updated github workflows.

## Api

[check here](./api.md)

## How to pre-build x64 libnode binaries for samp-node

### NodeHeaders

> example v24.x

[download here](https://nodejs.org/download/release/latest-v24.x/).

0. delete everything under `deps/node/include`.
1. download `node-v24.17.0-headers.tar.gz`.
2. decompress and copy everything under `node/v24.17.0/include/node` to `deps/node/include`.

### Windows v24

```sh
git clone https://github.com/nodejs/node.git -b v24.x --depth 1
cd node
.\vcbuild x64 dll openssl-no-asm
cd out/Release # libnode.dll & libnode.lib
```

### Linux v24

You need to install docker first.

Recommended to run only in a local virtual machine environment.

```sh
git clone https://github.com/nodejs/node.git -b v24.x --depth 1
cd node
./configure --shared --openssl-no-asm
make -j$(nproc)
# out/Release/libnode.so.137 is what you need
```

after that, for local build samp-node, pls put your libnode into paths below.

### Local paths for libnode

| Arch | Windows | Linux |
|------|---------|-------|
| x64 | `deps/node/lib/Release/win64/libnode.lib` + `libnode.dll` | `deps/node/lib/Release/linux64/libnode.so.137` |

for build samp-node, see `.github/workflows/build.yml`.

## How to build samp-node

### linux with docker

```sh
git clone https://github.com/dockfries/samp-node
# or ssh
# git clone git@github.com:dockfries/samp-node.git
cd samp-node
git submodule update --init

chmod +x ./build.sh
./build.sh 24.17.0 # version
```

### linux with cmake (x64 default)

```sh
cd samp-node
mkdir build && cd build

cmake -DCMAKE_BUILD_TYPE=Release ..
make -j$(nproc)
```

### windows with Visual Studio

```pwsh
cd samp-node
mkdir build, releases -ErrorAction SilentlyContinue
cd build

cmake .. -A x64
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
