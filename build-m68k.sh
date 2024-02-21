docker run --rm -v $(pwd):/root \
  -v $HOME/src/Retro68/universal:/Retro68-build/toolchain/universal \
  -e INTERFACES=universal \
  -i ghcr.io/autc04/retro68 /bin/bash <<"EOF"
    export CC=m68k-apple-macos-gcc
    export PATH=/Retro68-build/toolchain/m68k-apple-macos/bin:$PATH
    export CFLAGS="-Xlinker --no-warn-rwx-segments -Wa,-mcpu=68030 -isystem $(pwd)/include"
    sed -i 's/test -s conftest\$ac_exeext/test -e conftest\$ac_exeext/' /usr/share/autoconf/autoconf/autoconf.m4f
    ./configure --host=m68k-apple-macos --disable-rubygems
    make miniruby
EOF
