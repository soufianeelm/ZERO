# Edge AI Embedded System — Project Synthesis

---

## Progress

| Day | Focus | Status |
|---|---|---|
| Day 1 | Project definition, architecture, phases | ✅ Done |
| Day 2 | Environment setup (Git, Python venv, STM32CubeIDE, Renode) + CV update | ✅ Done |
| Day 3 | Phase 1 — STM32 firmware, mock IMU data, UART | ⬜ Next |

**Current phase:** Phase 1 — Foundations  
**Next session:** Start writing STM32 firmware, read mock IMU data, send over UART to PC

---

## What This Project Is

A dual-node embedded AI system made of two cooperating devices: an STM32 microcontroller and a Raspberry Pi 4. The STM32 handles hard real-time sensing and runs a tiny AI model directly on-chip to detect anomalies in motion and vibration data. The Pi handles heavier AI inference using a camera and a microphone. Both devices communicate over UART, and physical actuators (servo/relay) respond to AI decisions.

The goal is to learn AI from the ground up — mathematically and practically — while applying it to a constrained, real-time embedded system.

---

## Phases

### Phase 1 — Foundations (Week 1–2)
**Renode: YES | Real hardware: NO**

- Set up the full development environment (STM32CubeIDE, Python, Renode)
- Write basic STM32 firmware: read mock IMU data, send over UART
- Write basic Python script on PC (acting as Pi): receive and parse UART data
- Establish working communication between the emulated STM32 and the PC
- Understand FreeRTOS basics: tasks, priorities, scheduling
- Understand DMA and interrupt-driven sensor acquisition

**End of Phase 1 → Watch 3Blue1Brown + implement numpy neural network (see Learning section below)**

---

### Phase 2 — AI on the Pi (Week 3–4)
**Renode: YES (STM32 still emulated) | Real hardware: ideally arriving**

- Train a first CNN for image classification using your webcam and a dataset on PC
- Export the trained model to TensorFlow Lite (.tflite)
- Run inference in Python, measure latency
- Build an audio classification pipeline: record audio → generate spectrogram → train CNN → infer
- Understand transfer learning: fine-tune a pretrained MobileNetV2 on custom data
- Connect the Pi-side Python inference to the emulated STM32 over virtual UART

---

### Phase 3 — TinyML on the STM32 (Week 5–6)
**Renode: YES (mock IMU data is an advantage here) | Real hardware: recommended by end**

- Train an autoencoder on IMU data for unsupervised anomaly detection
- Quantize the model from float32 to int8 (model quantization)
- Port the quantized model to TensorFlow Lite Micro on the STM32
- Fight memory constraints: model must fit in Flash, inference must fit in RAM
- Guarantee inference completes within a real-time deadline
- Renode lets you script fake IMU data to test edge cases without real hardware

---

### Phase 4 — Integration (Week 7–8)
**Renode: NO | Real hardware: REQUIRED**

- Connect physical STM32 and Raspberry Pi 4 via UART wire
- Real IMU data, real camera feed, real microphone input
- Full closed loop: sensor anomaly or visual/audio detection → UART message → servo/relay response
- Measure end-to-end latency
- Document and demonstrate the working system
- *Optional at this stage*: begin introducing automotive-relevant practices (MISRA C, CAN bus consideration)

---

## Tech Stack

### STM32 Side
| Tool | Purpose |
|---|---|
| C++17 | Primary language (avoid: new/delete, exceptions, RTTI, dynamic STL) |
| STM32CubeIDE | IDE + compiler (arm-none-eabi-gcc) |
| STM32 HAL | Hardware abstraction (UART, I2C, DMA, timers) |
| FreeRTOS | Real-time operating system |
| TensorFlow Lite Micro | On-chip AI inference |
| Renode | STM32 emulation (Phases 1–3) |

### Raspberry Pi / PC Side
| Tool | Purpose |
|---|---|
| Python | Primary language |
| TensorFlow / Keras | Model training |
| TensorFlow Lite | Inference on Pi |
| OpenCV | Camera input and image processing |
| librosa | Audio processing and spectrogram generation |
| PyAudio | Microphone input |
| pyserial | UART communication |
| NumPy | Math, data manipulation |

### General
| Tool | Purpose |
|---|---|
| Git | Version control |
| VS Code | Python development |
| PC (your laptop) | Acts as Pi during emulation phases |

---

## Shopping List (~€150–160)

| Component | Purpose | Approx. Price |
|---|---|---|
| Raspberry Pi 4 (2GB) | Main compute node, runs vision + audio AI | €50 |
| STM32 Nucleo-F446RE | Microcontroller node, real-time + TinyML | €20 |
| Pi Camera Module 3 | Visual input | €25 |
| USB Microphone | Audio input | €15 |
| MPU-6050 IMU | Vibration + motion sensing (I2C) | €5 |
| Servo motor (SG90) | Physical actuator, responds to AI decisions | €5 |
| Relay module | Optional second actuator | €5 |
| SD card (32GB+) | Pi OS + storage | €10 |
| Official Pi power supply | Stable 5V/3A for Pi 4 | €10 |
| Breadboard + jumper wires | Prototyping | €10 |
| **Total** | | **~€155** |

> Order early. Hardware should arrive before the end of Phase 2.

---

## Learning Strategy — 3Blue1Brown Neural Networks Series

**When:** At the end of Phase 1, before starting Phase 2.  
**Why then:** You'll have seen what training looks like from the outside. The theory will land on context.

**The series:** [youtube.com/@3blue1brown](https://www.youtube.com/@3blue1brown) → *Neural Networks* playlist (4 videos, ~1 hour total)

### How to watch it (not passively)

1. **Watch one video at a time** — do not binge
2. **After each video:** close it, open a blank document, and write everything you just learned in your own words without looking back. This is not note-taking during — it is reconstruction after. It forces actual retrieval.
3. **After all four videos:** implement a tiny neural network from scratch using only NumPy. No TensorFlow, no Keras. One hidden layer, trained on a simple dataset (XOR problem or MNIST). This should take 2–3 hours and will be the most valuable thing you do before Phase 2.

```python
# What you'll be implementing — forward pass of one layer
layer_output = inputs @ weights + bias
# That's it. A neural network is matrix multiplications stacked together.
```

Once you've written this by hand, every TensorFlow abstraction will mean something.

---

## Key Concepts You Will Learn

| Concept | Where you encounter it |
|---|---|
| Neural networks (math) | 3Blue1Brown + numpy exercise |
| CNNs + transfer learning | Phase 2, image classification |
| Spectrograms + audio CNNs | Phase 2, sound classification |
| Model quantization (float32 → int8) | Phase 3, fitting model on STM32 |
| TensorFlow Lite Micro | Phase 3, on-chip inference |
| Autoencoders + anomaly detection | Phase 3, IMU data |
| Hard real-time constraints | Phase 1 + 3, FreeRTOS + deadlines |
| DMA + interrupt-driven design | Phase 1, STM32 firmware |
| Edge AI architecture | Throughout — where to put the intelligence |
