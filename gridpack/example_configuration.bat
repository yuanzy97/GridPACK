
set path=%path%;c:\cygwin64\bin
set prefix="C:\GridPACK"

set CFLAGS="/D _ITERATOR_DEBUG_LEVEL=0"
set CXXFLAGS="/D _ITERATOR_DEBUG_LEVEL=0"

cmake -Wdev --debug-trycompile ^
    -G "Visual Studio 10 2010 Win64" ^
    -D USE_PROGRESS_RANKS:BOOL=OFF ^
    -D BOOST_ROOT:PATH=C:\GridPACK ^
    -D Boost_USE_STATIC_LIBS:BOOL=ON ^
    -D BOOST_INCLUDEDIR=C:\GridPACK\include\boost-1_61 ^
    -D PETSC_DIR:PATH="C:\GridPACK\src\petsc-3.6.4" ^
    -D PETSC_ARCH:STRING='mswin-cxx-complex-opt' ^
    -D PARMETIS_DIR:PATH=C:\GridPACK ^
    -D MPIEXEC_MAX_NUMPROCS:STRING="2" ^
    -D GRIDPACK_TEST_TIMEOUT:STRING=10 ^
    ..

  
