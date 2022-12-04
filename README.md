# [语音录制与播放]stm32+adc+dac

前言：我在我的下载里面放上了我的这个项目的代码，一直也没有时间总结，今天想着，总结一下这个项目相关的知识点，在此做一个说明。（写到最前方，求个`star`感谢各位～～。）

## 硬件选型

主控芯片我选择的是STM32F103VCT6，语音的采集使用单片机自带的adc进行语音采集，播放也是使用dac进行语音播放，采集电路，就是自己搭建的一个前置采集电路，播放其实就是一个放大电路，接上喇叭就可以了。具体原理图就不能贴在这里了，想要的可以私信我。

## 项目执行流程

我这个样例，实现很简单，就是通过按键控制，按键1按下，开始录音，因为我是采用数组存储，只能录2S，录音到2S，自动结束录音，按键2按下，开始播放录音。
整个代码很简单，传输都是通过DMA实现的，后期改进，你们可以自行改进。

## 相关知识点

### 语音方面

人的语音频率范围是300Hz~4000Hz.因此根据采样定律可知，采样频率为8000Hz,采样位数为16位。
则1s的话就可以采集8000*16=128000bit=128000bit/8=16000byte=16000byte/1024=15.625K。因为adc采集的语音数据是PCM数据，数据量比较大，我们可以使用speex进行压缩，我贴出来的代码没有使用压缩，所以具体怎么压缩可以参考官网，等我有时间把我压缩的代码也贴出来。
在这里介绍一下speex吧。
Speex是一套主要针对语音的开源免费，无专利保护的音频压缩格式。Speex工程着力于通过提供一个可以替代高性能语音编解码来降低语音应用输入门槛 。另外，相对于其它编解码器，Speex也很适合网络应用，在网络应用上有着自己独特的优势。同时，Speex还是GNU工程的一部分，在改版的BSD协议中得到了很好的支持。
Speex是基于CELP并且专门为码率在2-44kbps的语音压缩而设计的。它的特点有：
■窄带（8kHz），宽带（16kHz）和超宽带（32kHz）压缩于同一位流。
■强化立体编码
■数据包丢失隐蔽
■可变比特率（VBR）
■语音捕捉（VAD）
■非连续传输（DTX）
■定点运算
■感官回声消除（AEC）
■噪音屏蔽

因为speex的压缩率为16：1，1s无压缩的时候为1S 15.625K，使用SPEEX压缩15.625K/16=0.9765625K=1000byte,200K/1000byte=200S，这个的好处，你们看到了吧。

我这有个代码没有使用Flash进行存储，而是采用的数组，只是为了调试用的，建议你们使用的时候，改成存储到Flash，这样才能录制更多的语音。

### 单片机

我这里采集频率的控制，采用的是定时器控制，具体设置参考代码如下：

```c
/* TIME1 configuration */
		TIM_DeInit(TIM3);
		TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
		/* TIM3 used for timing, the timing period depends on the sample rate */
		TIM_TimeBaseStructure.TIM_Prescaler = 72 - 1;    /* TIM3CLK = 72 MHz */
		TIM_TimeBaseStructure.TIM_Period = 124;             //¶ÔÓ¦8K²ÉÑù
		TIM_TimeBaseStructure.TIM_ClockDivision = 0x00;
		TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
		TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
123456789
```

其他方面我觉得看代码都能懂了，也就不在细说了。



## 总结

写到最前方，求个`star`感谢各位～～。

总有人发私信来找我要原理图和代码，我一直也没时间回复，所以就把代码放到github吧，更方面大家的下载。
