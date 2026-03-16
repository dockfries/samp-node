# samp-node

Forked from [`AmyrAhmady/samp-node`](https://github.com/AmyrAhmady/samp-node)

## Changes in this fork

- Only can work with [infernus](https://github.com/dockfries/infernus).
- Update `Node.js` to **v22.22.1**.
- Both `ESModule` and `CommonJS` supported, depending on the type field of package.json and the bundler output format.
- Only `entry_file` is used, `resource` config are removed.
- Removed `samp.fire` to avoid crashes.
- Building based on `ubuntu-latest` means you may need a higher version of glibc.
- Updated github workflows.

## Api

[check here](./api.md)

## How to pre-build x86 libnode binaries for samp-node

### NodeHeaders

> example v22.x

[download here](https://nodejs.org/download/release/latest-v22.x/).

0. delete everything under `deps/node/include`.
1. download `node-v22.22.1-headers.tar.gz`.
2. decompress and copy everything under `node/v22.22.1/include/node` to `deps/node/include`.

### Windows v22

```sh
git clone https://github.com/nodejs/node.git -b v22.x --depth 1
cd node
.\vcbuild x86 dll openssl-no-asm
cd out/Release # so you can get the libnode.dll & libnode.lib, windows need this two files.
```

### Linux v22

You need to install docker first.

Recommended to run only in a local virtual machine environment.

```sh

# Create a normal user and set up home directories and empty passwd.
sudo useradd -m -s /bin/bash YOUR_NORMAL_USERNAME
sudo passwd -d YOUR_NORMAL_USERNAME

# Add this user to the docker group.
sudo usermod -aG docker YOUR_NORMAL_USERNAME

su YOUR_NORMAL_USERNAME

rm -fr ~/Devel/unofficial-builds-home
mkdir -p ~/Devel/unofficial-builds-home
cd ~/Devel/unofficial-builds-home

git clone https://github.com/dockfries/unofficial-builds
cd ~/Devel/unofficial-builds-home/unofficial-builds
chmod +x ./**/*.sh

bin/local_build.sh -r x86_22 -v v22.22.1 # don't forget startWith 'v'

cp ~/Devel/unofficial-builds-home/staging/release/v22.22.1/node-v22.22.1-linux-x86.tar.gz /tmp

su YOUR_SUDO_USER
mv /tmp/node-v22.22.1-linux-x86.tar.gz ~
# use anything like shell/sftp download this file
# decompress it, you can see libnode.so.xxx in lib folder, that's what you need only.
# For version 22, `.so` files end with `127`
```

after that, for local build samp-node, pls put your libnode into `deps/node/lib/Release/win` and `deps/node/lib/Release/linux` respectively.

for build samp-node, see `.github/workflows/build.yml`.

## How to build x86 samp-node

### linux with docker

```sh
git clone https://github.com/dockfries/samp-node
# or ssh
# git clone git@github.com:dockfries/samp-node.git
cd samp-node
git submodule update --init

chmod +x ./build.sh
./build.sh 22.22.1 # version
```

### windows with Visual Studio

```pwsh
cd samp-node
mkdir build, releases -ErrorAction SilentlyContinue
cd build

cmake .. -A Win32
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
