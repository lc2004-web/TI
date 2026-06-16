## OLED_IIC — SSD1306 OLED 驱动 (I2C 版本)

基于 MSPM0G3507 的 0.96寸 SSD1306 OLED 显示驱动，使用软件 I2C 协议通信。

## 引脚分配

| 外设     | 引脚 | 功能         |
|----------|------|--------------|
| I2C (SCL) | PA28 | I2C 时钟线  |
| I2C (SDA) | PA31 | I2C 数据线  |
| GPIO     | PA14 | LED (active high) |
| DEBUGSS  | PA20 | SWCLK (调试) |
| DEBUGSS  | PA19 | SWDIO (调试) |

## I2C 协议说明

- **从机地址**: 0x3C (7-bit) / 0x78 (8-bit write)
- **实现方式**: 软件 I2C (位操作 GPIO)，不占用硬件 I2C 外设
- **时钟频率**: 约 200kHz (由 delay_cycles 控制)
- **写格式**: `[START] [Addr+W] [Control Byte] [Data...] [STOP]`
  - Control Byte = 0x00 → 后续字节为命令
  - Control Byte = 0x40 → 后续字节为显示数据

## 程序逻辑

上电后循环执行：
1. 清空显存 `OLED_GRAM[128][8]`
2. 在 (0,0) 显示字符串 `"CNT:"`
3. 在 (40,0) 显示 8 位递增计数器
4. 调用 `OLED_Refresh_Gram()` 批量刷新到 OLED
5. 计数器 +1，翻转 LED，延时约 1 秒

## 构建与烧录

- **IDE**: Code Composer Studio (CCS) Theia 70.5.1
- **编译器**: TI Arm Clang v4.0.4.LTS
- **SDK**: MSPM0 SDK v2.10.00.04
- **调试器**: SEGGER J-Link (SWD)

在 CCS 中打开项目，编译后通过 J-Link 烧录调试。

## 与原 SPI 版本的区别

| 特性       | SPI 版本 (OLED_project) | I2C 版本 (OLED_IIC) |
|------------|------------------------|---------------------|
| 通信协议   | 4线软件SPI             | 2线软件I2C          |
| 引脚数     | 5 (SCLK,SDA,RST,DC,CS) | 2 (SCL,SDA)         |
| 从机选择   | CS 片选                | I2C 地址            |
| 数据/命令  | DC 引脚                | Control Byte        |
| 硬件复位   | RST 引脚               | 软件命令            |
| 刷新方式   | 逐字节 SPI 写          | I2C 批量写 (每页128字节一次事务) |
