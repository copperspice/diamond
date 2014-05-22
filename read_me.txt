
ReadMe - Build Instructions for Windows

1   Download diamond zip or tar source and install

2   Download hunspell zip or tar source and install

3   From a bash prompt in /hunspell 

      a   ./do-config-hunspell

      b   cd build
   
      c   make install

4   From a bash prompt in /diamond

      a   ./configure CXXFLAGS="O2"

      b   make install

