#!/bin/bash
set -eux

# ${parameter:-word} uses word as the default if the parameter is unset.
LINUX_VER=${LINUX_VER:-ubuntu-18.04}
LLVM_VER=${LLVM_VER:-7.0.1}
PREFIX=${PREFIX:-${HOME}}

LLVM_DEP_URL=https://releases.llvm.org/${LLVM_VER}
TAR_NAME=clang+llvm-${LLVM_VER}-x86_64-linux-gnu-${LINUX_VER}

wget -q ${LLVM_DEP_URL}/${TAR_NAME}.tar.xz
tar -C ${PREFIX} -xf ${TAR_NAME}.tar.xz
rm ${TAR_NAME}.tar.xz
mv ${PREFIX}/${TAR_NAME} ${PREFIX}/clang+llvm

# Confusingly - sets the option and + unsets the option
set +x
echo "Please set:"
echo "export PATH=\$PREFIX/clang+llvm/bin:\$PATH"
echo "export LD_LIBRARY_PATH=\$PREFIX/clang+llvm/lib:\$LD_LIBRARY_PATH"