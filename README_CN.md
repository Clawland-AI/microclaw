# MicroClaw

**基于 C/Rust 构建的传感器级微型 AI 智能体（Agent）。内存占用 <1MB，运行于 $2-5 极低成本 MCU 硬件。**

[English](README.md) | [简体中文](README_CN.md)

> [Clawland](https://github.com/Clawland-AI) 开源生态核心成员。

---

## 概述 (Overview)

MicroClaw 是 Claw 家族中体积极致精简的成员 —— 一款直接运行在成本低至 2 美元微控制器（MCU）上的裸机（Bare-metal）AI 智能体。它负责毫秒级实时传感器数据采集、本地边缘决策判断，并将结构化事件上报至上游 PicClaw 或 NanoClaw 节点。

## 核心特性 (Key Features)

- **裸机运行 (Bare Metal)** — 无需操作系统（RTOS/OS），直接在 MCU 裸机上高效执行
- **超低功耗 (Ultra-Low Power)** — 深度睡眠模式、事件唤醒机制，极致电池续航友好
- **实时传感 (Real-Time Sensing)** — 亚毫秒级传感器轮询与即时闭环响应
- **本地规则引擎 (Local Rules Engine)** — 离线支持 if-then-else 条件与阈值规则判断，脱网自主运行
- **网状互联 (Mesh Communication)** — 支持 ESP-NOW、LoRa、Zigbee 或 UART 直连上游智能体
- **固件 OTA (OTA Updates)** — 通过 PicClaw 支持安全可靠的固件空中无线升级

## 支持的硬件平台 (Supported Hardware)

| 微控制器 (MCU) | 运行内存 (RAM) | 闪存 (Flash) | 参考单价 | 特性说明 |
|---|---|---|---|---|
| **ESP32-C3** | 400KB | 4MB | ~$2 | Wi-Fi + BLE 双模，性价比首选 |
| **ESP32-S3** | 512KB | 8MB | ~$3 | 向量 AI 指令加速，支持摄像头扩展 |
| **STM32F103** | 20KB | 64KB | ~$1.5 | 极低成本工业级核心，基础精简环境 |
| **RP2040** | 264KB | 2MB | ~$1 | 双核 Cortex-M0+，支持可编程 I/O (PIO) |
| **nRF52840** | 256KB | 1MB | ~$4 | BLE Mesh 网状网络，超低功耗射频 |

## 系统架构 (Architecture)

```
┌───────────────────────────┐
│       MicroClaw (MCU)     │
│  ┌────────┐ ┌───────────┐ │
│  │Sensors │ │RulesEngine│ │
│  │  ADC   │ │ 阈值控制  │ │
│  │  GPIO  │ │ 状态机FSM │ │
│  └────────┘ └───────────┘ │
│  ┌────────┐ ┌───────────┐ │
│  │ 通信层 │ │ 功耗管理  │ │
│  │ESP-NOW │ │ 深度睡眠  │ │
│  │ UART   │ │ 唤醒定时器│ │
│  └────────┘ └───────────┘ │
└───────────────────────────┘
        │ 上报事件与遥测数据
        ▼
   ┌─────────┐
   │ PicClaw │
   └─────────┘
```

## 项目状态 (Status)

🚧 **Pre-Alpha** — 处于架构设计与原型演进阶段。热烈欢迎社区贡献者加入！

## 参与贡献 (Contributing)

详情请参阅 [Clawland 贡献指南](https://github.com/Clawland-AI/.github/blob/main/CONTRIBUTING.md)。

**核心贡献者享有产品收入 20% 的收益分成。** 查阅 [贡献者收益分成条款](https://github.com/Clawland-AI/.github/blob/main/CONTRIBUTOR-REVENUE-SHARE.md) 了解激励细节。

## 开源协议 (License)

本项目采用 Apache License 2.0 开源协议 — 详情见 [LICENSE](LICENSE)。
