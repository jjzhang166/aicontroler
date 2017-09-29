# 智能语音控制中心
<p>基于百度语音识别、语音合成和图灵机器人的智能语音控制中心。程序自动适配环境音量，取适当阀值当说话后才开始录音。加入偶发性声音检测机制，不会对突然性的声音做出处理。此程序能够在Nanopi Neo/Neo2、OrangePi Zero Plus(H5)上面正常运行。但不建议在树莓派上面运行，经过测试，虽然此程序在树莓派上面也可以运行，但是树莓派麦克风声音实在太小了，语音识别结果很不理想。<br />
程序依赖于curl、mpg123和asound。但是多安装了一个mp3播放器，作为百度TTS MP3数据播放的第二种方式。
其中GPIO的控制方式为wiringPi，鉴于很多官方系统已经预装了wiringPi，所以在程序附带的库中，并未放入。如果需要，请自行安装wiringPi。<br />
搭建步骤和演示视频戳：https://www.geeiot.net/thread-1151-1-1.html
</p>
<br />
<p>重要！使用前请先修改ai_neo2/src/config.h中的百度API和图灵机器人API为自己的API，毕竟我也是用的免费的[\笑哭]。创建项目之后即可免费获取。</p>
百度语音识别官网：http://yuyin.baidu.com/<br />
图灵机器人官网：http://www.tuling123.com/<br />
申请API方法也会在搭建步骤中详细描述。<br />
<br />
 **脚本说明** <br />
1、在第一次运行程序之前，务必执行 <br />
sudo ./build.sh<br />
来配置系统环境、编译相关库、编译AI可执行程序。<br />
2、当编译完成之后，可以执行<br />
sudo ./run.sh<br />
来运行AI程序了。除了第一次运行而外，以后均不需要再进行第一步操作（再用uninstall.sh除外）<br />
3、解除安装
sudo ./uninstall.sh<br />
运行这个程序会解除编译，并删除系统中的库，当解除编译后重新运行程序需要进行第一步。<br />
4、重新编译程序
sudo ./only_rebuild_ai.sh<br />
运行这个脚本会重新编译AI程序。前提是已经运行了第一步，并且没有执行删除脚本。<br />
<br />
 **目录结构说明** <br />
lib中为需要的库。<br />
aicontroler中为代码<br />
<br />
 **运行环境** <br />
原则上是支持任何linux系统的，只要安装好相关的库。不过我编译和运行环境都是ubuntu16.04，其他的需要自行测试。不支持windows，需要的自行移植。<br />
<br />
引用的一些代码、库和资料：<br />
https://curl.haxx.se/<br />
http://www.alsa-project.org/main/index.php/Main_Page<br />
http://www.mpg123.de/<br />
http://blog.csdn.net/ownWell/article/details/8114121<br />
http://blog.csdn.net/freeze_z/article/details/44310245<br />
