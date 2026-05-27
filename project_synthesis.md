# Edge AI Embedded System — Project Synthesis

---

## Progress

| Day | Focus | Status |
|---|---|---|
| Day 1 | Project definition, architecture, phases | ✅ Done |
| Day 2 | Environment setup (Git, Python venv, STM32CubeIDE, Renode) + CV update | ✅ Done |
| Day 3 | Phase 1 — CubeMX config, project setup, C++17 build (Milestones 1–3) | ✅ Done |
| Day 4 | Phase 1 — FreeRTOS tasks, queue, IWDG kick, first running firmware | ⬜ Next |

**Current phase:** Phase 1 — Foundations  
**Next session:** Implement FreeRTOS task structure (SensorTask + UartTask + queue), implement `vApplicationStackOverflowHook`, verify scheduler runs clean

---

## Claude Code Instructions

When working with Claude Code, always include this at the start of every session:

> Do not write my implementation code for me — I want to write it myself to learn. You can guide me, explain what to write and why, and show short snippets to illustrate concepts. However, writing tests, correcting my code, and generating boilerplate I don't need to learn from (like CubeMX-generated structures) is fine.

---

## What This Project Is

A dual-node embedded AI system made of two cooperating devices: an STM32 microcontroller and a Raspberry Pi 4. The STM32 handles hard real-time sensing and runs a tiny AI model directly on-chip to detect anomalies in motion and vibration data. The Pi handles heavier AI inference using a camera and a microphone. Both devices communicate over UART, and physical actuators (servo/relay) respond to AI decisions.

The goal is to learn AI from the ground up — mathematically and practically — while applying it to a constrained, real-time embedded system.

---

## Phases

### Phase 1 — Foundations (Week 1–2)
**Renode: YES | Real hardware: NO**

1. Configure project in **STM32CubeMX standalone** (not inside CubeIDE — see Gotchas), generate code targeting STM32CubeIDE, import into CubeIDE
2. Configure UART2 in CubeMX (virtual COM port used for STM32 ↔ PC communication)
3. Configure IWDG watchdog timer in CubeMX — resets MCU automatically if firmware hangs
4. Enable FreeRTOS stack overflow detection (`configCHECK_FOR_STACK_OVERFLOW = 2`)
5. Design FreeRTOS task structure: one task for sensor acquisition, one for UART transmission
6. Implement DMA-driven UART transmission — CPU does not block waiting for bytes to send
7. Write mock IMU data generation — simulates realistic MPU-6050 accelerometer/gyroscope readings
8. Add IMU sanity checks — reject physically impossible values before they reach any downstream logic
9. Register UART error callbacks (framing error, overrun, noise) — log error type, never silently ignore
10. Define a UART packet protocol: fixed header, typed payload, checksum — both sides agree on the format
11. Write Python script on PC (acting as Pi): receive packets, validate format, parse payload
12. Add UART timeout handling in Python — if no packet arrives within N ms, log a warning and handle gracefully
13. Set up Python `logging` module — timestamped logs for every received packet, every error, every event
14. Load compiled firmware into Renode, connect Python script via virtual UART socket
15. **Test — normal flow:** verify mock IMU data flows from emulated STM32 to Python parser correctly
16. **Test — UART errors:** trigger framing/overrun errors in Renode, verify callbacks fire and are logged
17. **Test — bad IMU data:** inject out-of-range values, verify sanity check rejects them before transmission

**→ End of Phase 1: watch 3Blue1Brown series + implement numpy neural network (see Learning section)**

---

### Phase 2 — AI on the Pi (Week 3–4)
**Renode: YES (STM32 still emulated) | Real hardware: ideally arriving**

1. Collect or download an image classification dataset (start simple: 2–3 classes)
2. Train a first CNN from scratch in Keras — understand each layer: convolutional, pooling, dense
3. Export the trained model to TensorFlow Lite (.tflite)
4. Run TFLite inference in Python, measure and log latency on every run
5. Validate inference inputs before feeding the model — check shape, dtype, value range
6. Test edge case inputs: all zeros, max values, NaN — pipeline must not crash on any of them
7. Fine-tune a pretrained MobileNetV2 on custom data — understand what transfer learning does and why it works
8. Build the audio classification pipeline: record audio → generate mel spectrogram → train CNN → run inference
9. Connect the full Pi-side inference pipeline to the emulated STM32 over virtual UART
10. **Test — vision pipeline:** verify correct classifications on held-out images, measure accuracy and latency
11. **Test — audio pipeline:** verify correct sound classifications, measure latency
12. **Test — full communication:** Pi detects an event → sends UART message → emulated STM32 receives and acknowledges it

---

### Phase 3 — TinyML on the STM32 (Week 5–6)
**Renode: YES | Real hardware: recommended by end of phase**

1. Generate an IMU dataset using Renode — script normal sequences and anomalous sequences (drift, spike, freeze)
2. Train an autoencoder on normal IMU data only — the model learns what "normal" looks like
3. Validate the autoencoder: verify it reconstructs normal data well and produces high error on anomalous data
4. Quantize the model from float32 to int8
5. Compare float32 vs int8 output on the same inputs — measure and document the accuracy loss
6. Port the quantized model to TFLite Micro and integrate it into the STM32 C++17 firmware
7. Fight memory constraints: model must fit entirely in Flash, inference buffers must fit in RAM
8. Measure worst-case inference time on the emulated STM32 — must complete within the real-time deadline
9. Script all synthetic IMU sequences in Renode: normal, gradual drift, sudden spike, sensor freeze
10. Verify the autoencoder's response to each sequence is correct
11. Simulate UART failure mid-session in Renode — verify STM32 recovers cleanly without hanging or corrupting state
12. **Test — full STM32 pipeline:** IMU data → FreeRTOS task → TFLite Micro inference → anomaly flag → UART output

---

### Phase 4 — Integration (Week 7–8)
**Renode: NO | Real hardware: REQUIRED**

1. Flash STM32 firmware to the physical Nucleo-F446RE board
2. Set up Raspberry Pi 4: install OS, configure Python environment, deploy inference pipeline
3. Connect STM32 and Raspberry Pi via physical UART wire
4. Connect MPU-6050 IMU to STM32 via I2C — verify real sensor data matches expected format
5. Connect Pi Camera Module 3 and USB microphone — verify inference pipelines run on real inputs
6. Build the full closed loop: anomaly or detection event → UART message → servo/relay response
7. Measure end-to-end latency: from sensor event to physical actuator response, worst case
8. **Test — power cycle:** unplug and replug each node independently — both must recover without manual intervention
9. **Test — stress:** run the full pipeline continuously for 30+ minutes — monitor for memory leaks, task starvation, latency drift
10. **Test — node failure:** disconnect one node intentionally — verify the other handles it gracefully without crashing
11. Document the complete system: architecture, latency measurements, known limitations
12. *Optional:* begin automotive-relevant practices — MISRA C++ audit, CAN bus as replacement for UART

---

## Gotchas & Discoveries

Things learned the hard way — read before starting a new session.

**STM32CubeMX is no longer inside STM32CubeIDE (v2.0+)**
CubeMX is now a standalone application only. Correct workflow: configure in CubeMX standalone → generate code → import project into CubeIDE → write application code in CubeIDE. When peripheral config needs to change, open the `.ioc` file (launches CubeMX), reconfigure, regenerate, re-import.

**CubeMX regeneration recreates `main.c`**
Every time you regenerate, CubeMX overwrites `main.c`. You must rename it to `main.cpp` again. Known friction point — do not forget.

**Renaming `main.c` to `main.cpp` is not enough to enable C++**
The G++ compiler section never appears in build settings until you explicitly right-click project → Convert to C++ Project. Without this, `.cpp` files are silently not compiled, producing `undefined reference to main` errors.

**`extern "C"` on all HAL callbacks in `.cpp` files**
Any HAL callback defined in a `.cpp` file must be wrapped with `extern "C"` so the C HAL linker can find it without C++ name mangling. Silent link-time failure if forgotten. Example:
```cpp
extern "C" void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart) { ... }
```

**`vApplicationStackOverflowHook` must be implemented**
Stack overflow detection level 2 is active. The linker requires this function to exist or it will error. Must be implemented in Day 4 before the firmware can link.

**SYS timebase must be TIM6, not SysTick**
FreeRTOS uses SysTick as its tick source. HAL also defaults to SysTick for `HAL_Delay` and `HAL_GetTick`. They conflict. Always set System Core → SYS → Timebase Source to **TIM6** in CubeMX.

---

## Tech Stack

### STM32 Side
| Tool | Purpose |
|---|---|
| C++17 | Primary language (avoid: new/delete, exceptions, RTTI, dynamic STL) |
| STM32CubeMX | Standalone peripheral configurator — owns the `.ioc` file, generates code |
| STM32CubeIDE | IDE + compiler (arm-none-eabi-gcc) — application code, build, debug, flash |
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
