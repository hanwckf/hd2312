# Linux DVB driver for NERC DTV (CY68013A+HDIC HD2312) DTMB/DMB-TH USB adapter

#### 使用DKMS本地编译

```shell
# 安装内核头文件和dkms工具，以Ubuntu/Debian为例
sudo apt install dkms linux-headers-$(uname -r)

# 下载源码到/usr/src/hd2312-1.0
git clone https://github.com/hanwckf/hd2312.git /usr/src/hd2312-1.0

# 使用dkms构建并安装内核模块
dkms add -m hd2312/1.0
dkms build -m hd2312/1.0
dkms install -m hd2312/1.0
```

#### 使用Linux源码树编译

```shell
# 配置内核源码树，打开CONFIG_DVB_USB选项
# make menuconfig
# make modules_prepare

# 下载源码
git clone https://github.com/hanwckf/hd2312.git 

# 设定目标KDIR并开始编译
cd hd2312
make KDIR=<path/to/kernelsource>
```
