# SPDX-License-Identifier: GPL-2.0
# Copyright (C) 2024 hanwckf

PKG_NAME="HD2312"
PKG_VERSION="59c4dbfdc504c1230948f63fe5b0a47ca6c71bf8"
PKG_SHA256="4e72adcd55687db975920d21c600d7f981dc9941d01a64516113bd336f434d8d"
PKG_LICENSE="GPL"
PKG_SITE="https://github.com/hanwckf/hd2312"
PKG_URL="https://github.com/hanwckf/hd2312/archive/${PKG_VERSION}.tar.gz"
PKG_LONGDESC="DVB driver for LeTV DTMB USB Stick (CY68013A+HDIC HD2312)"
PKG_IS_KERNEL_PKG="yes"

make_target() {
  kernel_make KDIR=$(kernel_path) -C ${PKG_BUILD}
}

makeinstall_target() {
  mkdir -p ${INSTALL}/$(get_full_module_dir)/${PKG_NAME}
    cp *.ko ${INSTALL}/$(get_full_module_dir)/${PKG_NAME}
}
