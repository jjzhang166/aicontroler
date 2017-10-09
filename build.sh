echo =====================配置系统环境==========================
sudo apt-get update
echo ===================配置系统环境完成========================
echo ...
echo =====================安装相关软件==========================
sudo apt install -y make
sudo apt install -y xmlto libasound2-dev
sudo apt install -y curl
sudo apt install -y linux-sound-base
sudo apt install -y oss4-base
sudo apt install -y alsa-base
sudo apt install -y alsa-utils
sudo apt install -y alsa-oss
sudo apt install -y mpg123
sudo apt install -y sox
sudo apt install -y sox libsox-fmt-all
echo ===================安装相关软件完成========================
echo ...
echo =====================编译相关软件==========================
cd lib/
cd alsa-lib/
./configure
make clean
make
sudo make install
cd ../curl/
./configure
make clean
make
sudo make install
cd ../libmpg123/
./configure
make clean
make
sudo make install
cd ..
echo ===================编译相关软件完成========================
echo ...
echo ===================编译语音识别程序========================
cd ..
cd aicontroler/build/
make clean
make
sudo cp /usr/local/lib/libcurl.so.4 /usr/lib/
sudo cp /usr/local/lib/libcurl.so /usr/lib/
echo =======================全部完成============================
echo 可以执行 sudo ./run.sh 来运行AI程序了.
echo ===========================================================

