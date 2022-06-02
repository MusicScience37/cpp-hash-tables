FROM musicscience37/sphinx-doxygen:clang14-v5.0

RUN apt-get update && \
    apt-get install -y --no-install-recommends \
    openssh-client gnupg2 \
    vim nano && \
    apt-get autoremove && \
    apt-get autoclean && \
    ln -s "$(command -v clangd-14)" /usr/local/bin/clangd

WORKDIR /root
RUN git clone https://github.com/include-what-you-use/include-what-you-use.git -b clang_14 && \
    cd include-what-you-use && \
    mkdir build && \
    cd build && \
    # Needed for configuration.
    pyenv global 3.9.12 && \
    cmake -G Ninja -DCMAKE_PREFIX_PATH=/usr/lib/llvm-14 .. && \
    cmake --build . && \
    cmake --build . --target install

WORKDIR /root
RUN git clone https://github.com/gperftools/gperftools.git -b gperftools-2.9.1 && \
    cd gperftools && \
    ./autogen.sh && \
    ./configure CC=clang CXX=clang++ CXXFLAGS=-stdlib=libc++ --disable-libunwind --enable-frame-pointers && \
    make -j && \
    make install
