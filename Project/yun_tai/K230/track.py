'''
云台追踪 — K230 视觉端 (矩形检测)
====================================
功能: Canny边缘检测 → 拟合矩形 → 取最大矩形中心 → 串口发送偏移
输出: UART2, IO11(TX)/IO12(RX), 115200, 10Hz
协议: "X:+0123,Y:+0045\r\n"  或  "X:LOST,Y:LOST\r\n"
'''

import time, os, gc
from machine import Pin, UART, FPIOA
from media.sensor import *
from media.display import *
from media.media import *
import cv_lite
import ulab.numpy as np

# ---- 画面参数 ----
FRAME_W = 640
FRAME_H = 480
CENTER_X = FRAME_W // 2
CENTER_Y = FRAME_H // 2

# ---- 串口: UART2, IO11=TX, IO12=RX, 115200 ----
fpioa = FPIOA()
fpioa.set_function(11, FPIOA.UART2_TXD)
fpioa.set_function(12, FPIOA.UART2_RXD)
uart = UART(UART.UART2, 115200)

# ---- 摄像头: RGB888, 640x480 ----
sensor = Sensor(id=2, width=1280, height=960, fps=90)
sensor.reset()
sensor.set_framesize(width=FRAME_W, height=FRAME_H)
sensor.set_pixformat(Sensor.RGB888)

Display.init(Display.ST7701, width=800, height=480, to_ide=True, quality=50)
MediaManager.init()
sensor.run()

image_shape = [FRAME_H, FRAME_W]
clock = time.clock()

# ---- 矩形检测参数 ----
CANNY_LO    = 50        # Canny 低阈值
CANNY_HI    = 150       # Canny 高阈值
APPROX_EPS  = 0.04      # 多边形拟合精度
AREA_MIN    = 0.001     # 最小面积比例
MAX_COS     = 0.5       # 最大角余弦 (越小越接近直角)
BLUR_SIZE   = 5         # 高斯模糊核

# ---- 视觉端死区 ----
VIS_DEADBAND = 10

# ---- 主循环 ----
last_send = time.ticks_ms()
while True:
    clock.tick()
    now = time.ticks_ms()

    img = sensor.snapshot()
    img_np = img.to_numpy_ref()

    rects = cv_lite.rgb888_find_rectangles(
        image_shape, img_np,
        CANNY_LO, CANNY_HI, APPROX_EPS,
        AREA_MIN, MAX_COS, BLUR_SIZE
    )

    n = len(rects) // 4
    if n > 0:
        # 取面积最大的矩形
        max_area, best = 0, 0
        for i in range(n):
            w = rects[4*i + 2]
            h = rects[4*i + 3]
            area = w * h
            if area > max_area:
                max_area = area
                best = i

        bx = rects[4*best]
        by = rects[4*best + 1]
        bw = rects[4*best + 2]
        bh = rects[4*best + 3]
        cx = bx + bw // 2
        cy = by + bh // 2
        dx = cx - CENTER_X
        dy = cy - CENTER_Y

        # 死区
        if abs(dx) < VIS_DEADBAND: dx = 0
        if abs(dy) < VIS_DEADBAND: dy = 0

        msg = "X:{:+05d},Y:{:+05d}\r\n".format(dx, dy)
        img.draw_rectangle(bx, by, bw, bh, color=(255, 0, 0), thickness=2)
    else:
        msg = "X:LOST,Y:LOST\r\n"

    # 10Hz 发送
    if now - last_send >= 100:
        uart.write(msg)
        last_send = now

    # 显示
    img.draw_string_advanced(0, 0, 24,
        '%.1ffps' % clock.fps(), color=(255, 255, 255))
    img.draw_line(CENTER_X-10, CENTER_Y, CENTER_X+10, CENTER_Y,
                  color=(0,0,255), thickness=1)
    img.draw_line(CENTER_X, CENTER_Y-10, CENTER_X, CENTER_Y+10,
                  color=(0,0,255), thickness=1)
    Display.show_image(img,
        x=round((800-FRAME_W)/2), y=round((480-FRAME_H)/2))

    gc.collect()
