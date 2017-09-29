
echo ===================开始执行清除脚本========================
cd ai_neo2/build
make clean
cd ../lib
cd alsa-lib/
sudo make uninstall
make clean
cd ..
cd curl/
sudo make uninstall
make clean
cd ..
cd libmpg123/
sudo make uninstall
make clean
cd ../..
echo =======================全部完成============================

