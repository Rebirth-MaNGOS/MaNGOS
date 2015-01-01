#!/bin/csh
setenv TBB22_INSTALL_DIR "/home/andkem/Dokument/Programmering/Rebirth/root/dep/tbb" #
setenv tbb_bin "${TBB22_INSTALL_DIR}/build/libs_release" #
if (! $?CPATH) then #
    setenv CPATH "${TBB22_INSTALL_DIR}/include" #
else #
    setenv CPATH "${TBB22_INSTALL_DIR}/include:$CPATH" #
endif #
if (! $?LIBRARY_PATH) then #
    setenv LIBRARY_PATH "${tbb_bin}" #
else #
    setenv LIBRARY_PATH "${tbb_bin}:$LIBRARY_PATH" #
endif #
if (! $?LD_LIBRARY_PATH) then #
    setenv LD_LIBRARY_PATH "${tbb_bin}" #
else #
    setenv LD_LIBRARY_PATH "${tbb_bin}:$LD_LIBRARY_PATH" #
endif #
 #
