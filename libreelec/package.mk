# SPDX-License-Identifier: GPL-2.0
# Copyright (C) 2024 hanwckf

PKG_NAME="HD2312"
PKG_VERSION="c0ec05bdd41e514e2f8371848750488b5fb2547d"
PKG_SHA256="26568b4f09d21e4e265c4b9ef3f6c277d009a4b2f3b10ea29dea2a858fb40ff6"
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
