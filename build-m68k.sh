docker run --rm -v $(pwd):/root \
  -v $HOME/src/Retro68/universal:/Retro68-build/toolchain/universal \
  -e INTERFACES=universal \
  -i ghcr.io/autc04/retro68 /bin/bash <<"EOF"
    export CC=m68k-apple-macos-gcc
    export PATH=/Retro68-build/toolchain/m68k-apple-macos/bin:$PATH
    export CFLAGS="-Xlinker --no-warn-rwx-segments -Wa,-mcpu=68030 -isystem $(pwd)/include -DOPT_THREADED_CODE=3"

    # Patch linker test in autoconf so it accepts empty output (our linker creates an empty dummy output, and writes to other files)
    # TODO: can we set the executable output extension to .bin instead? That causes the linker to output a single MacBinary file.
    sed -i 's/test -s conftest\$ac_exeext/test -e conftest\$ac_exeext/' /usr/share/autoconf/autoconf/autoconf.m4f

    autoconf && ./configure --host=m68k-apple-macos --disable-rubygems 'ac_cv_func_fork_works=no'
    make miniruby

    # Re-link miniruby with RetroConsole library. Need to use g++ because it's a C++ library
    m68k-apple-macos-g++ -lRetroConsole -Xlinker --no-warn-rwx-segments -Wa,-mcpu=68030 -isystem /root/include -DOPT_THREADED_CODE=3  -L. -fstack-protector-strong -fstack-protector-strong -pie  main.o dmydln.o miniinit.o dmyext.o array.o ast.o bignum.o class.o compar.o compile.o complex.o cont.o debug.o debug_counter.o dir.o dln_find.o encoding.o enum.o enumerator.o error.o eval.o file.o gc.o hash.o inits.o io.o io_buffer.o iseq.o load.o marshal.o math.o memory_view.o rjit.o rjit_c.o node.o node_dump.o numeric.o object.o pack.o parse.o parser_st.o proc.o process.o ractor.o random.o range.o rational.o re.o regcomp.o regenc.o regerror.o regexec.o regparse.o regsyntax.o ruby.o ruby_parser.o scheduler.o shape.o signal.o sprintf.o st.o strftime.o string.o struct.o symbol.o thread.o time.o transcode.o util.o variable.o version.o vm.o vm_backtrace.o vm_dump.o vm_sync.o vm_trace.o weakmap.o prism/api_node.o prism/api_pack.o prism/diagnostic.o prism/encoding.o prism/extension.o prism/node.o prism/options.o prism/pack.o prism/prettyprint.o prism/regexp.o prism/serialize.o prism/token_type.o prism/util/pm_buffer.o prism/util/pm_char.o prism/util/pm_constant_pool.o prism/util/pm_list.o prism/util/pm_memchr.o prism/util/pm_newline_list.o prism/util/pm_state_stack.o prism/util/pm_string.o prism/util/pm_string_list.o prism/util/pm_strncasecmp.o prism/util/pm_strpbrk.o prism/prism.o prism_init.o   coroutine/m68k/Context.o  enc/ascii.o enc/us_ascii.o enc/unicode.o enc/utf_8.o enc/trans/newline.o memcmp.o crypt.o flock.o setproctitle.o m68k/mman.o  -lm  -o miniruby.code.bin

    # Compile resources and output the final combined MacBinary file
    Rez -I/Retro68-build/toolchain/m68k-apple-macos/RIncludes --copy miniruby.code.bin /Retro68-build/toolchain/m68k-apple-macos/RIncludes/Retro68APPL.r -t 'APPL' -c '????' -o miniruby.bin
EOF

    # export LDFLAGS=-lRetroConsole
    # export LDD=m68k-apple-macos-g++
