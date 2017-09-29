echo =====================配置系统环境==========================
sudo apt-get update
echo ===================配置系统环境完成========================
echo ...
echo =====================安装相关软件==========================
sudo apt-get install -y make
sudo apt-get install -y xmlto libasound2-dev
sudo apt-get install -y curl
sudo apt-get install -y linux-sound-base
sudo apt-get install -y oss4-base
sudo apt-get install -y alsa-base
sudo apt-get install -y alsa-utils
sudo apt-get install -y alsa-oss
sudo apt-get install -y mpg123
sudo apt-get install -y sox
sudo apt-get install -y sox libsox-fmt-all
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

