# V1.0 — 巡线小车 (Line-Following Car)

## 项目概述

基于 MSPM0G3507 的七路灰度巡线小车，集成：
- **TB6612** 双路电机驱动 (PWM + 编码器测速)
- **MPU6050** 六轴 IMU (DMP 姿态解算, 100Hz)
- **SSD1306 OLED** 128×64 显示 (软件SPI)
- **七路灰度传感器** (数字GPIO, 加权位置计算)
- **PID 控制器** (位置PID转向 + 速度PID + 陀螺仪偏航稳定)

## 硬件引脚分配

### 电机控制 (TB6612)
| 功能 | 引脚 | 说明 |
|------|------|------|
| PWMA | PB2 | 电机A PWM, TIMA1, 8kHz |
| AIN1 | PA13 | 电机A 方向1 |
| AIN2 | PA14 | 电机A 方向2 |
| PWMB | PA21 | 电机B PWM, TIMA0, 8kHz |
| BIN1 | PA16 | 电机B 方向1 |
| BIN2 | PA17 | 电机B 方向2 |

### 编码器 (500PPR, 20:1减速比)
| 功能 | 引脚 | 说明 |
|------|------|------|
| A相 | PA26 | 电机A, 双边沿中断 |
| B相 | PA25 | 电机A, 方向判断 |
| A相 | PB24 | 电机B, 双边沿中断 |
| B相 | PB20 | 电机B, 方向判断 |

### IMU (MPU6050, I2C0)
| 功能 | 引脚 |
|------|------|
| SDA | PA0 |
| SCL | PA1 |

### OLED (SSD1306, 4线软件SPI)
| 功能 | 引脚 |
|------|------|
| SCL | PA28 |
| SDA | PA31 |
| RST | PB14 |
| DC | PB15 |
| CS | PA22 |

### 灰度传感器 (七路)
| 通道 | 引脚 | 位置权重 |
|------|------|----------|
| S1 | PB8 | -3 (最左) |
| S2 | PA12 | -2 |
| S3 | PA18 | -1 |
| S4 | PB3 | 0 (中心) |
| S5 | PB9 | +1 |
| S6 | PA9 | +2 |
| S7 | PB19 | +3 (最右) |

### 调试串口 (UART1, 115200)
| 功能 | 引脚 |
|------|------|
| TX | PB6 |
| RX | PB7 |

## 文件结构

```
V1.0/
├── empty.c              # 主程序 (巡线控制)
├── empty.syscfg         # SysConfig 外设配置
├── BSP/
│   ├── pid.c/h          # PID 控制器
│   ├── line_sensor.c/h  # 灰度传感器接口
│   ├── bsp_tb6612.c/h   # TB6612 电机驱动
│   ├── bsp_encoder.c/h  # 正交编码器测速
│   ├── bsp_imu.c/h      # IMU 姿态解算抽象层
│   ├── bsp_siic.c/h     # I2C 硬件适配层
│   ├── inv_mpu.c/h      # MPU6050 I2C 驱动 (InvenSense)
│   ├── inv_mpu_dmp_motion_driver.c/h  # DMP 固件接口
│   ├── dmpKey.h         # DMP 配置密钥
│   ├── dmpmap.h         # DMP 内存映射
│   ├── bsp_oled.c/h     # SSD1306 OLED 驱动
│   ├── oledfont.h       # ASCII/中文 字库
│   ├── bsp_uart.c/h     # UART 调试打印
│   ├── bsp_delay.c/h    # 毫秒延时 (CPU周期)
│   ├── delay.c/h        # 毫秒延时 (IMU库依赖)
│   └── bsp_systick.h    # SysTick 声明
└── Debug/               # 构建输出 (CCS自动生成)
```

## 构建与烧录

1. 用 **Code Composer Studio Theia** 打开 `E:\TI\TI\Project\V1.0`
2. 首次构建会自动运行 SysConfig 生成 `ti_msp_dl_config.c/h`
3. 构建: Project → Build Project (或 Ctrl+B)
4. 烧录/调试: 使用 SEGGER J-Link (SWD: PA19/SWDIO, PA20/SWCLK)

**编译器:** TI ARM Clang Compiler (TICLANG 4.0.4.LTS)
**SDK:** MSPM0 SDK v2.10.00.04

## 控制算法

### 主循环 (100Hz / 10ms)

1. **读取灰度传感器** → 加权位置 (-3.0 ~ +3.0)
2. **读取 IMU** → 偏航角 (用于直线稳定)
3. **读取编码器** → 左右轮速度 (RPM)
4. **位置PID** → 转向修正量
5. **速度PID** → 加减速修正量
6. **偏航角 P 控制** → 偏航修正量
7. **合成电机输出**:
   - 左轮 = BASE_DUTY - steering - yaw_correction + speed_correction
   - 右轮 = BASE_DUTY + steering + yaw_correction + speed_correction
8. **限幅 & 死区** → 施加到电机
9. **OLED 更新** (每 10 次循环 = 100ms)

### PID 参数 (可在线调整)

| 参数 | 默认值 | 说明 |
|------|--------|------|
| KP_LINE | 200.0 | 位置比例增益 |
| KI_LINE | 5.0 | 位置积分增益 |
| KD_LINE | 6.0 | 位置微分增益 |
| KP_SPEED | 10.0 | 速度比例增益 |
| KI_SPEED | 1.0 | 速度积分增益 |
| KD_SPEED | 5.0 | 速度微分增益 |
| BASE_DUTY | 1600 | 基础占空比 (40%) |
| YAW_KP | 15.0 | 偏航角修正增益 |
| TARGET_SPEED_RPM | 80.0 | 目标速度 |

### PID 调参指南

1. **先调 KP_LINE**: 从 100 开始, 逐步增加直到小车能沿线行驶但略有摆动
2. **再加 KD_LINE**: 增加微分以抑制摆动, 从小值 (2-5) 开始
3. **最后加 KI_LINE**: 小幅积分消除稳态偏移, 注意不要过大
4. **偏航角**: 如果直线行驶时车身摇晃, 适当增加 YAW_KP
5. **速度**: 调整 BASE_DUTY 和 TARGET_SPEED_RPM 控制行驶速度

### 方向极性校准

如果电机转向反了, 修改 `bsp_tb6612.h`:
```c
#define MOTOR_A_DIR_INVERT  0   // 改为1 反转电机A
#define MOTOR_B_DIR_INVERT  1   // 改为0 反转电机B
```

如果编码器正负反了, 修改 `bsp_encoder.h`:
```c
#define ENCODER_A_INVERT    0   // 改为1 反转编码器A
#define ENCODER_B_INVERT    1   // 改为0 反转编码器B
```

## 异常处理

- **丢线 (<500ms)**: 保持最后已知位置继续行驶
- **丢线 (>500ms)**: 停车, OLED 显示 "LINE LOST"
- **全黑 (十字路口)**: 直行通过
- **IMU 初始化失败**: OLED 显示错误码, 停止
