# 智能地铁列车控制系统

这是一个用于演示与教学的嵌入式+上位机项目，模拟“地铁车厢”场景，包含下位机固件（STC15 系列）与上位机（Qt 桌面应用）两部分。项目旨在展示物联网场景下的感知、决策、执行与通信闭环：温度采集、阈值管理、风扇控制、车门与安防监测、语音/音乐播放、非易失存储与串口协议。

<p align="center">
  <img src="Final Project Report/images/22-46-17.png" alt="项目整体框架图" width="400"><br>
  <span style="color:gray; font-size:14px;">项目整体框架图</span>
</p>


## 目录

- [目录结构（简要）](#目录结构简要)
- [模块概览](#模块概览)
  - [下位机模块](#下位机模块)
  - [上位机功能](#上位机功能)
- [上位机 UI 展示](#上位机-ui-展示)


## 目录结构（简要）

- `source/` - 下位机固件源码（C）
- `inc/`, `BSP_inc/` - 头文件与外设抽象
- `Latex/` - 项目报告的 XeLaTeX 源文件与图片
- `output/` - 编译输出（固件构建）
- `list/` - 构建时的中间列表文件
- `项目文档/` - 高阶设计与协议文档（项目文档的来源）
- `Qt 上位机/` - 上位机项目


## 模块概览

下位机主要模块（文件 -> 职责）：

- `core.c`：全局状态、1s/100ms 定时回调逻辑
- `main.c`：系统初始化、回调注册与主循环
- `music_module.c`：音乐播放控制
- `nv_temp_threshold.c`：温度阈值非易失存储与管理
- `my_uart1.c`：通过串口与上位机通信
- `radio_module.c`：收音机模块
- `real_time_module.c`：实时时钟模块
- `temp_controlled_fan.c`：风扇（直流电机）控制逻辑
- `temp_module.c`：温度采集与显示
- `time_module.c`：ETA 剩余到站时间计算逻辑
- `ultrasonic_module.c`：超声波模块
- `voice_module.c`：语音播报模块


上位机功能（摘要）：

- Qt 桌面应用提供多车厢实时状态表、报警日志、远程指令下发、历史数据导出与诊断页面
- 使用 `QSerialPort` 管理串口，按照 BSP 协议解析帧并更新 UI

## 上位机 UI 展示


<p align="center">
  <img src="Final Project Report/images/21-05-51.png" alt="实时监控页示意图" width="400"><br>
  <span style="color:gray; font-size:14px;">实时监控页示意图</span>
</p>

---

<p align="center">
  <img src="Final Project Report/images/21-06-37.png" alt="远程监控页示意图" width="400"><br>
  <span style="color:gray; font-size:14px;">远程监控页示意图</span>
</p>

---

<p align="center">
  <img src="Final Project Report/images/21-06-59.png" alt="温度历史曲线示意图" width="400"><br>
  <span style="color:gray; font-size:14px;">温度历史曲线示意图</span>
</p>



