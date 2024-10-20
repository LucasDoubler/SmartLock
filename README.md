# SmartLock
用于学习嵌入式开发的智能门锁项目
项目描述：该项目是一个基于STM32H743的智能门锁系统，旨在通过物联网技术实现远程监控和控制。系统集成了FreeRTOS进行任务调度，使用MQTT协议连接阿里云平台，实现远程控制与监控功能。为了确保通信和数据存储的安全性，采用了Mbed TLS的AES加密方案，防止密码泄露和远程指令被拦截。

开发平台：硬件：STM32H743 操作系统：FreeRTOS 文件系统：FatFs 云平台：阿里云 UI界面：LVGL

具体信息：该智能门锁系统基于STM32H743平台，利用FreeRTOS进行多任务调度。通过ESP8266与MQTT协议连接阿里云，实现远程监控与操作功能。系统使用STM32H743的RNG模块生成随机数，结合Mbed TLS库进行AES加密，采用PKCS#7填充机制，确保通信的安全性。密码和密钥信息存储在SD卡中，通过FatFs文件系统进行读写操作。此外，使用LVGL生成交互式密码输入界面，替代传统物理按键，并通过舵机模拟门锁开关操作。同时，超声波传感器实时监测门的开关状态，确保系统的安全性与稳定性。

额外说明：1.由于我手里只有一个360度旋转SG90的舵机，这种舵机接收的PWM信号的占空比，只会影响到他转动的角度和方向，所以无法控制精确控制舵机转动的角度

2.将密码存储在SD卡上其实也是很不安全的，实际项目应选用更为安全的存储介质。我的弥补措施是将AES加密后的密码A存储在SD卡上，但密钥不进行存储，不过这个在存储密码时使用的密钥A是固定的，并且是一个局部变量，这样可以保证即使SD卡上没有存储密钥A，程序也可以解密加密后的密码A，同时密钥才程序中的存在时长非常短，防止被读取

3.原本想设计一个远程修改密码的功能，但是因为我的ESP8266的Flash非常小，只有1MB，无法烧录带有TLS协议和阿里云证书的MQTT固件，所以暂时放弃了这个功能，因为网络的传输数据如果不进行加密，传输密码这种重要数据是非常危险的，将会被很容易地嗅探到。这个项目中的方案是，当用户想要修改密码时，阿里云平台会向设备发送一条修改密码的消息，这条消息仅仅是告诉设备用户将要修改密码了，程序将切换到修改密码任务，用户通过操作设备的按键（用lvgl的按钮矩阵代替数字键，确认键，删除键）实现修改密码


