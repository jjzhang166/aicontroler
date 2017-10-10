#!/bin/bash
#这个脚本的作用是自动安装相关的软件、安装相关的库、自动化编译程序。
#需要root权限来运行这个脚本

IS_RASPI="n"
IS_TEST="n"
echo "=====================配置系统环境=========================="
echo "是否在树莓派上面搭建？"
echo "如果在树莓派上面搭建，将会对程序做出适应树莓派的配置(y or n)："
read IS_RASPI
echo "是否编译测试版程序？"
echo "测试版本不带有wiringPi，所有只有基本的语音识别功能(y or n):"
read IS_TEST

if [ "$IS_RASPI"x = "y"x ] && [ "$IS_TEST"x = "y"x ]; then
	# 树莓派内置配置文件
	cp lib/config/config.h.raspi aicontroler/src/
	rm -rf aicontroler/src/config.h
	mv aicontroler/src/config.h.raspi aicontroler/src/config.h
	# 树莓派ini配置文件 测试版程序
	cp lib/config/config.ini.raspi test/ai/build
	rm -rf test/ai/build/config.ini
	mv test/ai/build/config.ini.raspi test/ai/build/config.ini
	echo "搭建方式：树莓派  编译测试代码"
elif [ "$IS_RASPI"x = "y"x ] && [ "$IS_TEST"x = "n"x ]; then
	# 树莓派内置配置文件
	cp lib/config/config.h.raspi aicontroler/src/
	rm -rf aicontroler/src/config.h
	mv aicontroler/src/config.h.raspi aicontroler/src/config.h
	# 树莓派ini配置文件 测试版程序
	cp lib/config/config.ini.raspi test/ai/build
	rm -rf test/ai/build/config.ini
	mv test/ai/build/config.ini.raspi test/ai/build/config.ini
	echo "搭建方式：树莓派  不编译测试代码"
elif [ "$IS_RASPI"x = "n"x ] && [ "$IS_TEST"x = "y"x ]; then
	# 树莓派内置配置文件
	cp lib/config/config.h.nml aicontroler/src/
	rm -rf aicontroler/src/config.h
	mv aicontroler/src/config.h.nml aicontroler/src/config.h
	# 树莓派ini配置文件 测试版程序
	cp lib/config/config.ini.nml test/ai/build
	rm -rf test/ai/build/config.ini
	mv test/ai/build/config.ini.nml test/ai/build/config.ini
	echo "搭建方式：常规  编译测试代码"
elif [ "$IS_RASPI"x = "n"x ] && [ "$IS_TEST"x = "n"x ]; then
	# 树莓派内置配置文件
	cp lib/config/config.h.nml aicontroler/src/
	rm -rf aicontroler/src/config.h
	mv aicontroler/src/config.h.nml aicontroler/src/config.h
	# 树莓派ini配置文件 测试版程序
	cp lib/config/config.ini.nml test/ai/build
	rm -rf test/ai/build/config.ini
	mv test/ai/build/config.ini.nml test/ai/build/config.ini
	echo "搭建方式：常规  不编译测试代码"
fi
echo "===================配置系统环境完成========================"
echo "..."
echo "=====================安装相关软件=========================="
sudo apt-get update
sudo apt-get install -y make
sudo apt-get install -y xmlto libasound2-dev
sudo apt-get install -y curl
sudo apt-get install -y linux-sound-base
sudo apt-get install -y alsa-base
sudo apt-get install -y alsa-utils
sudo apt-get install -y alsa-oss
sudo apt-get install -y mpg123
sudo apt-get install -y sox
sudo apt-get install -y sox libsox-fmt-all
echo "===================安装相关软件完成========================"
echo "..."
echo "=====================编译相关软件=========================="
echo "开始编译安装alsa-lib..."
cd lib/
cd alsa-lib/
./configure
make clean
make
sudo make install
echo "开始编译安装curl..."
cd ../curl/
./configure
make clean
make
sudo make install
echo "开始编译安装mpg123..."
cd ../libmpg123/
./configure
make clean
make
sudo make install
cd ../..
sudo cp /usr/local/lib/libcurl.so.4 /usr/lib/
sudo cp /usr/local/lib/libcurl.so /usr/lib/
echo "===================编译相关软件完成========================"
echo "..."
echo "===================编译语音识别程序========================"
if [ "$IS_TEST"x = "y"x ];then
echo "开始编译测试代码..."
cd test/ai/build
make clean
make
echo "测试代码编译完成！"
cd ../../..
fi
echo "开始编译智能语音控制中心程序..."
cd aicontroler/build/
make clean
make
cd ~
echo "=======================全部完成============================"
echo "可以执行 sudo ./run.sh 来运行AI程序了."
echo "========================================================="
exit #