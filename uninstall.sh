#!/bin/bash
# 嗯，不完整的清除，但是只能这样了
echo "===================开始执行清除脚本========================"
cd aicontroler/build
make clean
cd ../../lib
cd curl/
sudo make uninstall
make clean
cd ..
cd libmpg123/
sudo make uninstall
make clean
cd ../../test/ai/build
make clean
cd ~
echo "=======================全部完成============================"
exit #
