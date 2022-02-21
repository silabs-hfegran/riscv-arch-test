#
# Example flow
#

# setup environment
isetup
svsetup -xcelium

mkdir TOP
cd TOP
export TOP=$(pwd)

#
# core-v-verif
#
git clone https://github.com/<fork>/core-v-verif.git
pushd core-v-verif
    # until merged onto Master
    git checkout rm_update
popd

pushd core-v-verif/cv32e40x/sim/uvmt
    MAKEVARS="SIMULATOR=xrun TARGET=XCELIUM"
    make ${MAKEVARS} clean_all
    make ${MAKEVARS} sanity
popd

#
# compliance
#
git clone https://github.com/<fork>/riscv-compliance

pushd riscv-compliance
    export CORE_V_VERIF=${TOP}/core-v-verif
    # setup gcc compiler and PREFIX
    # PATH=${PATH}:<PATH_TO_BIN>

    export CV_SW_TOOLCHAIN=<path to toolchain>
    export CV_SW_PREFIX=riscv32-unknown-elf-
    export CV_SW_VENDOR=unknown
    export CV_SIMULATOR=xrun
    export CV_CORE=cv32e40x

    make RISCV_TARGET=cv32e40x RISCV_ISA=rv32i_m RISCV_DEVICE=I
    make RISCV_TARGET=cv32e40x RISCV_ISA=rv32i_m RISCV_DEVICE=M
    make RISCV_TARGET=cv32e40x RISCV_ISA=rv32i_m RISCV_DEVICE=C
    make RISCV_TARGET=cv32e40x RISCV_ISA=rv32i_m RISCV_DEVICE=Zifencei
    make RISCV_TARGET=cv32e40x RISCV_ISA=rv32i_m RISCV_DEVICE=privilege
popd

