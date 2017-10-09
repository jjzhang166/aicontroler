# 智能语音控制中心
<p>基于百度语音识别、语音合成和图灵机器人的智能语音控制中心。程序自动适配环境音量，取适当阀值当说话后才开始录音。加入偶发性声音检测机制，不会对突然性的声音做出处理。此程序能够在Nanopi Neo/Neo2、OrangePi Zero Plus(H5)上面正常运行。但不建议在树莓派上面运行，经过测试，虽然此程序在树莓派上面也可以运行，但是树莓派麦克风声音实在太小了，语音识别结果很不理想。<br />
程序依赖于curl、mpg123和asound。但是多安装了一个mp3播放器，作为百度TTS MP3数据播放的第二种方式。
其中GPIO的控制方式为wiringPi，鉴于很多官方系统已经预装了wiringPi，所以在程序附带的库中，并未放入。如果需要，请自行安装wiringPi。<br />
搭建步骤和演示视频戳：https://www.geeiot.net/thread-1151-1-1.html
</p>
<p>重要！使用前请先修改ai_neo2/src/config.h中的百度API和图灵机器人API为自己的API，毕竟我也是用的免费的[\笑哭]。在各自官网创建项目之后即可免费获取。</p>
百度语音识别官网：http://yuyin.baidu.com/<br />
图灵机器人官网：http://www.tuling123.com/<br />
申请API方法也会在搭建步骤中详细描述。<br />
<br />
 **未来计划** <br />
1、开发基于2.4G模块或者433模块的远程控制开关。<br />
2、接入智能家居控制中心<br />
<br />
 **脚本说明** <br />
1、在第一次运行程序之前，务必执行 <br />
sudo ./build.sh<br />
来配置系统环境、编译相关库、编译AI可执行程序。<br />
2、当编译完成之后，可以执行<br />
sudo ./run.sh<br />
来运行AI程序了。除了第一次运行而外，以后均不需要再进行第一步操作（再用uninstall.sh除外）<br />
3、卸载安装<br />
sudo ./uninstall.sh<br />
运行这个程序会解除编译，并删除系统中的库，当解除编译后重新运行程序需要进行第一步。<br />
4、重新编译程序<br />
sudo ./only_rebuild_ai.sh<br />
运行这个脚本会重新编译AI控制程序。前提是已经运行了第一步，并且没有执行卸载脚本。<br />
<br />
 **目录结构说明** <br />
1、文件夹aicontroler<br />
文件中的src为程序代码，build为存放编译后的可执行文件（测试版本还存放配置文件）。<br />
2、文件夹lib<br />
程序运行所需要的库。有alsa-lib、libmpg123、curl以及tools中的npi-config、和Nanopi NEO2、OrangePi Zero Plus各自的WinrgPi。<br />
3、文件夹test<br />
测试版程序，不需要WiringPi就能运行，所以能够在开发板和电脑上面运行。配置文件在build文件夹中。<br />
4、脚本build.sh<br />
自动编译安装脚本。会自动安装相关包、编译安装依赖库和aicontroler程序。一键编译，方便快捷。只有第一次运行或者运行了uninstall.sh才需要运行这个脚本，运行这个脚本会花较长时间，且磁盘空间必须要有2G的剩余。<br />
5、脚本only_rebuild_ai.sh<br />
只重新编译aicontroler程序，用于修改了内置配置文件之后重新编译程序。<br />
6、脚本run.sh<br />
运行程序。可以到aicontroler/build中直接执行程序。<br />
7、脚本uninstall.sh<br />
卸载程序但不会卸载已经安装的软件包。<br />
<br />
 **运行环境** <br />
原则上是支持任何linux系统的，只要安装好相关的库。经过实际测试，Ubuntu、Debian只要安装了相关的库，都可以正常运行，Centos暂未测试。不支持windows，需要的自行移植。<br />
<br />
引用的一些代码、库和资料：<br />
https://curl.haxx.se/<br />
http://www.alsa-project.org/main/index.php/Main_Page<br />
http://www.mpg123.de/<br />
http://blog.csdn.net/ownWell/article/details/8114121<br />
http://blog.csdn.net/freeze_z/article/details/44310245<br />
（还有一些参考资料未列出）