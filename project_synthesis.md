# Edge AI Embedded System — Project Synthesis

---

## Progress

| Session | Focus | Status |
|---|---|---|
| Session 1 | Project definition, architecture, phases | ✅ Done |
| Session 2 | Environment setup (Git, Python venv, STM32CubeIDE, Renode) + CV update | ✅ Done |
| Session 3 | Phase 1 — CubeMX config, project setup, C++17 build (Milestones 1–3) | ✅ Done |
| Session 4 | Phase 1 — FreeRTOS tasks, queue, IWDG kick, Renode validation (Milestone 4) | ✅ Done |
| Session 5 | Phase 1 — Mock IMU data, sanity checks, DMA UART TX with semaphore | ✅ Done |
| Session 6 | Phase 0 — V-model setup, docs/ folder structure, Doxygen, begin SRS | ⬜ Next |

**Current phase:** Phase 0 — Specification & Design
**Next session:** Create docs/ folder, set up Doxygen, begin writing SRS

---

## Claude Code Instructions

When working with Claude Code, always include this at the start of every session:

> Do not write my implementation code for me — I want to write it myself to learn. You can guide me, explain what to write and why, and show short snippets to illustrate concepts. However, writing tests, correcting my code, and generating boilerplate I don't need to learn from (like CubeMX-generated structures) is fine.

---

## What This Project Is

A dual-node embedded AI system made of two cooperating devices: an STM32 microcontroller and a Raspberry Pi 4. The STM32 handles hard real-time sensing and runs a tiny AI model directly on-chip to detect anomalies in motion and vibration data. The Pi handles heavier AI inference using a camera and a microphone. Both devices communicate over UART, and physical actuators (servo/relay) respond to AI decisions.

The project follows the **V-model development cycle** — specification and design precede all implementation, and each test level validates its corresponding design level.

---

## Development Cycle — V-Model

```
SRS (System Requirements)           →        Acceptance Testing
    SAD (Architecture + FSMs)       →    Integration Testing
        DDD (Detailed Design)       →  Unit Testing
                    Implementation
```

No module is implemented before its detailed design is written and reviewed. No test is written before its test specification exists. Code references the requirement it satisfies via inline tags (`// REQ: SYS-001`).

### Document hierarchy

| Document | Abbreviation | Purpose |
|---|---|---|
| System Requirements Specification | SRS | What the system must do — measurable, numbered, testable |
| Software Architecture Document | SAD | Module decomposition, FSMs, data flows, memory/timing budgets |
| Interface Control Document | ICD | Exact UART packet format, timing, error handling contract |
| Detailed Design Document | DDD | Per-module function signatures, data structures, timing diagrams |
| Test Specification | TEST_SPEC | One test case per SRS requirement, unit/integration/acceptance |
| Changelog | CHANGELOG | Formal record of changes tied to git history |

### Repository structure

```
ZERO/
├── docs/
│   ├── SRS.md
│   ├── SAD.md
│   ├── ICD.md
│   ├── DDD.md
│   ├── TEST_SPEC.md
│   └── CHANGELOG.md
├── STM32/
│   └── zero-firmware/
├── pi/
└── Doxyfile
```

Each document carries a header: version, date, author, status (DRAFT / REVIEW / APPROVED).

---

## Formal FSMs

Defined in the SAD. Implemented in code. Tested against the TEST_SPEC.

### STM32 System FSM

| State | Description | Transitions |
|---|---|---|
| `INIT` | Hardware and RTOS initialisation | → `SELF_TEST` on completion |
| `SELF_TEST` | IMU reachability check, UART loopback | → `RUNNING` on pass / → `FAULT` on fail |
| `RUNNING` | Normal operation — sampling, inferring, transmitting | → `ANOMALY_DETECTED` on model flag |
| `ANOMALY_DETECTED` | Anomaly confirmed by model | → `ALERT_SENT` after UART transmission |
| `ALERT_SENT` | Alert transmitted to Pi | → `RUNNING` after acknowledgement timeout |
| `FAULT` | Unrecoverable error | Loop until IWDG reset |

### UART TX FSM (STM32)

| State | Transitions |
|---|---|
| `IDLE` | → `TRANSMITTING` on queue data received |
| `TRANSMITTING` | → `AWAITING_COMPLETION` on DMA transfer start |
| `AWAITING_COMPLETION` | → `IDLE` on semaphore release from callback / → `TX_ERROR` on timeout |
| `TX_ERROR` | Log error → `IDLE` |

### UART Protocol FSM (both sides — packet reception)

| State | Transitions |
|---|---|
| `WAIT_HEADER` | → `WAIT_PAYLOAD` on valid header bytes received |
| `WAIT_PAYLOAD` | → `VALIDATE` on expected byte count received |
| `VALIDATE` | → `PROCESS` on checksum match / → `DISCARD` on mismatch |
| `PROCESS` | Handle packet → `WAIT_HEADER` |
| `DISCARD` | Log bad packet → `WAIT_HEADER` |

### Pi Inference FSM

| State | Transitions |
|---|---|
| `IDLE` | → `DATA_RECEIVED` on UART packet arrival |
| `DATA_RECEIVED` | → `PREPROCESSING` immediately |
| `PREPROCESSING` | Validate + format input → `INFERRING` |
| `INFERRING` | Run model → `DECISION` |
| `DECISION` | Threshold check → `ACTUATING` or `NO_ACTION` |
| `ACTUATING` | Trigger servo/relay → `IDLE` |
| `NO_ACTION` | Log result → `IDLE` |

---

## Phases

### Phase 0 — Specification & Design
**Output: documents only | No implementation**

> This phase runs now, before continuing Phase 1 implementation. The firmware prototype built in Days 3–5 informs the specification — it is not production code yet.

1. Create `docs/` folder in repo with all document files (empty, with standard headers)
2. Set up **Doxygen** — configure `Doxyfile` targeting STM32 and Pi source folders
3. Write **SRS**: number and define all system-level requirements (SYS-001 onwards) — each must be measurable and testable
4. Write **SAD**: decompose the system into modules, define all FSMs formally (reference Formal FSMs section above), define memory and timing budgets per module
5. Write **ICD**: define the exact UART packet format byte by byte — header magic bytes, payload layout, checksum algorithm, error handling contract between STM32 and Pi
6. Write **DDD**: per-module — function signatures, data structures, task stack sizes, timing diagrams
7. Write **TEST_SPEC**: one test case per SRS requirement — defined before any test is written

---

### Phase 1 — Foundations Implementation
**Renode: YES | Real hardware: NO**

> Implementation proceeds module by module, each referencing its DDD section. Every function includes Doxygen comments and a `// REQ: SYS-XXX` tag.

**Already implemented (prototype — must be refactored to match DDD):**
- CubeMX project: UART2 + DMA, IWDG, FreeRTOS (stack overflow detection level 2)
- FreeRTOS task structure: `SensorTask`, `UartTask`, queue
- Mock IMU data generation with sanity checks
- DMA UART TX with binary semaphore

**Remaining:**
1. **Refactor first** — move implementations out of `main.cpp` into dedicated modules before writing any new code:
   - `SensorTask` → `sensor_task.cpp` / `sensor_task.hpp`
   - `UartTask` → `uart_task.cpp` / `uart_task.hpp`
   - `imu_data_valid()` + `randomNoise()` → `imu_data.cpp` (`.hpp` already exists)
   - `main.cpp` keeps only CubeMX generated code + forward declarations in USER CODE blocks
2. Refactor existing code to match DDD specifications and add Doxygen comments
2. Replace `snprintf` + `%f` UART formatting with binary packet protocol (as defined in ICD)
3. Make `buf[128]` static to move it off the task stack
4. Register UART error callbacks (framing error, overrun, noise) — log type, never silently ignore
5. Implement STM32 System FSM and UART TX FSM in firmware
6. Write Python script on PC (acting as Pi): receive packets, validate format, parse payload per ICD
7. Implement UART Protocol FSM in Python parser
8. Add UART timeout handling in Python — log warning if no packet within N ms
9. Set up Python `logging` module — timestamped logs for every packet, error, and event
10. Load compiled firmware into Renode, connect Python script via virtual UART socket
11. **Unit test — STM32:** IMU sanity checks, packet formatting, FSM transitions
12. **Unit test — Python:** packet parser, checksum validation, timeout handling
13. **Integration test — normal flow:** mock IMU data flows STM32 → Python parser correctly
14. **Integration test — UART errors:** trigger framing/overrun in Renode, verify callbacks fire
15. **Integration test — bad IMU data:** inject out-of-range values, verify rejected before transmission
16. Containerise Python environment with **Docker** (`docker/pi/Dockerfile`)
17. Containerise STM32 build environment with **Docker** (`docker/stm32/Dockerfile` — arm-none-eabi-gcc + make)
18. Set up **GitHub Actions CI** — auto generate doxygen + auto build firmware + run tests on every push, using both containers

**→ End of Phase 1: watch 3Blue1Brown series + implement numpy neural network (see Learning section)**

---

### Phase 2 — AI Pipeline Implementation
**Renode: YES (STM32 still emulated) | Real hardware: ideally arriving**

> DDD for Pi modules written before implementation begins.

1. Write DDD for Pi vision module, audio module, and inference orchestrator
2. Write TEST_SPEC entries for all Phase 2 requirements
3. Collect or download an image classification dataset (start simple: 2–3 classes)
4. Train a first CNN from scratch in Keras — understand each layer: convolutional, pooling, dense
5. Export trained model to TensorFlow Lite (.tflite)
6. Run TFLite inference in Python, measure and log latency on every run
7. Validate inference inputs before feeding model — check shape, dtype, value range
8. Fine-tune pretrained MobileNetV2 on custom data — understand what transfer learning does and why
9. Build audio classification pipeline: record audio → mel spectrogram → CNN → inference
10. Implement Pi Inference FSM in orchestrator
11. Connect full Pi-side pipeline to emulated STM32 over virtual UART
12. **Unit test — vision pipeline:** correct classifications on held-out images, latency within budget
13. **Unit test — audio pipeline:** correct sound classifications, latency within budget
14. **Unit test — edge cases:** all zeros, max values, NaN — pipeline must not crash
15. **Integration test:** Pi detects event → UART message → emulated STM32 receives and acknowledges

---

### Phase 3 — TinyML Implementation
**Renode: YES | Real hardware: recommended by end of phase**

> DDD for TFLite Micro module written before implementation begins.

1. Write DDD for STM32 inference module — model architecture, memory layout, inference budget
2. Write TEST_SPEC entries for all Phase 3 requirements
3. Generate IMU dataset via Renode — script normal and anomalous sequences (drift, spike, freeze)
4. Train autoencoder on normal IMU data only — model learns what "normal" looks like
5. Validate autoencoder: reconstructs normal well, produces high error on anomalies
6. Quantize model from float32 to int8
7. Compare float32 vs int8 output on same inputs — measure and document accuracy loss
8. Port quantized model to TFLite Micro, integrate into STM32 C++17 firmware
9. Verify model fits in Flash, inference buffers fit in RAM
10. Measure worst-case inference time on emulated STM32 — must meet real-time deadline from DDD
11. **Unit test — quantization:** accuracy loss within acceptable bounds defined in TEST_SPEC
12. **Unit test — inference timing:** worst-case execution time within budget
13. **Integration test — synthetic sequences:** normal, drift, spike, freeze — verify FSM transitions correctly
14. **Integration test — UART failure:** simulate mid-session disconnect, verify STM32 recovers without hang

---

### Phase 4 — Integration & Acceptance Testing
**Renode: NO | Real hardware: REQUIRED**

1. Flash STM32 firmware to physical Nucleo-F446RE
2. Set up Raspberry Pi 4: install OS, configure Python environment, deploy pipeline
3. Connect STM32 and Pi via physical UART wire
4. Connect MPU-6050 IMU to STM32 via I2C — verify real sensor data matches ICD format
5. Connect Pi Camera Module 3 and USB microphone — verify inference pipelines run on real inputs
6. Build full closed loop: anomaly/detection event → UART → servo/relay response
7. Measure end-to-end latency: sensor event to actuator response, worst case
8. **Acceptance test — SYS requirements:** run every TEST_SPEC acceptance test case, document pass/fail
9. **Acceptance test — power cycle:** unplug each node independently, both recover without intervention
10. **Acceptance test — stress:** run full pipeline 30+ minutes, monitor for memory leaks, task starvation, latency drift
11. **Acceptance test — node failure:** disconnect one node, verify the other handles it gracefully
12. Finalise all documentation — SRS, SAD, DDD, ICD marked APPROVED, measured latency recorded
13. *Optional:* MISRA C++ audit, CAN bus as replacement for UART

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

**`vApplicationStackOverflowHook` is auto-generated by CubeMX (v2.x)**
When stack overflow detection level 2 is enabled, CubeMX generates the hook automatically — no manual implementation needed.

**In CubeMX 2.x there is no `MX_FREERTOS_Init()`**
Task creation and RTOS object initialisation happen directly in `main.cpp` inside the `USER CODE` labeled sections, not in a separate init function.

**SYS timebase must be TIM6, not SysTick**
FreeRTOS uses SysTick as its tick source. HAL also defaults to SysTick for `HAL_Delay` and `HAL_GetTick`. They conflict. Always set System Core → SYS → Timebase Source to **TIM6** in CubeMX.

**Renode 1.16.1 has no NUCLEO-F446RE or STM32F446 platform**
A custom platform was created: `STM32/renode-platform/nucleo_f446re.repl` and `nucleo_f446re.resc`, based on `stm32f4.repl` — all peripheral addresses are identical across the STM32F4 family.

**Renode startup warnings are normal**
`flash_controller` unhandled writes, `SYSCFG:EXTICR4` unimplemented register, `dma1` unhandled writes, `nvic` priority warning — these are peripheral model incompleteness in Renode's STM32F4 base and do not affect firmware execution.

**IWDG in Renode requires a reset macro in the `.resc` file**
Without it, after the watchdog fires, the CPU tries to execute from address `0x0`, finds nothing, and halts. The reset macro reloads the ELF correctly on reset.

**`osDelay` yields the CPU — it is not a busy-wait**
Equivalent to `asyncio.sleep` in Python, not `time.sleep`. The scheduler runs other tasks during the delay.

**FreeRTOS is not a real OS**
It is a scheduling library compiled into the same binary as the application. There is no memory isolation between tasks — a bug in one task can corrupt everything silently.

**Renode commands to know**
`pause` / `start`, `cpu PC`, `machine GetTimeSourceInfo`, `mach clear` (reset simulation without closing Renode), `include @path/to/script.resc`

**DMA UART TX callback requires USART2 global interrupt enabled**
`HAL_UART_TxCpltCallback` in DMA normal mode uses a two-interrupt chain: DMA1_Stream6 fires first, enables TCIE, then USART2 TC fires to call the callback. Without enabling USART2 global interrupt in CubeMX, the chain never completes and the callback never fires.

**CubeMX regeneration wipes anything outside USER CODE blocks**
Function prototypes, declarations, includes — anything outside `/* USER CODE BEGIN */` / `/* USER CODE END */` markers gets erased on regeneration. Always place your declarations inside the markers.

**`snprintf` with `%f` is stack-heavy — avoid in embedded**
Six `%f` conversions use large internal stack buffers. Exceeded the 1024-byte UartTask stack causing a system freeze caught by the IWDG. Fixed by increasing stack to 2048 bytes. Long-term fix: replace with binary packet protocol (planned in Phase 1 remaining steps).

**IWDG reset at exactly the configured timeout = system completely frozen**
If Renode resets at precisely the watchdog timeout, the firmware is not running at all — HardFault, stack overflow, or infinite loop. Use this as a reliable crash indicator.

**Renode CPU hook syntax**
`cpu AddHook <addr> "machine.Pause()"` — the hook body must be valid Python. Variable names alone are not valid hook bodies.

**`static` local buffers move data off the task stack**
A `static char buf[128]` inside a function is allocated in BSS (global memory), not on the stack. Use this for large local buffers in tasks with tight stack budgets.

---

## C++17 Embedded Restrictions

> These are **absolute rules** for all firmware code on the STM32. Read before every coding session.

| ❌ Never use | Why |
|---|---|
| `new` / `delete` | Dynamic heap allocation — heap fragmentation on a system with no memory manager causes unpredictable failures |
| `malloc` / `free` | Same reason as above |
| `try` / `catch` / `throw` | No C++ exception runtime exists on bare metal — an unhandled exception calls `std::terminate()` and does nothing useful |
| `dynamic_cast` / `typeid` | RTTI embeds metadata tables that waste Flash and has runtime overhead |
| `std::vector`, `std::string`, `std::map` and any STL container that allocates dynamically | All use heap allocation internally |
| Virtual functions in hot paths | vtable dispatch adds indirection and prevents inlining — avoid in interrupt handlers and tight loops |

> These are **enforced by the compiler** for exceptions and RTTI (`-fno-exceptions -fno-rtti` flags are set). The rest are **discipline constraints** — the compiler will not stop you, you must stop yourself.

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
| GitHub Actions | CI — auto build + test on every push |
| Docker | Reproducible build environment |
| Doxygen | API documentation generated from source comments |
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
| V-model development cycle | Throughout — spec before code, test before deploy |
| FSM design and implementation | Phase 0 (spec) + Phase 1–3 (implementation) |
| Requirements engineering | Phase 0, SRS writing |
| Neural networks (math) | 3Blue1Brown + numpy exercise |
| CNNs + transfer learning | Phase 2, image classification |
| Spectrograms + audio CNNs | Phase 2, sound classification |
| Model quantization (float32 → int8) | Phase 3, fitting model on STM32 |
| TensorFlow Lite Micro | Phase 3, on-chip inference |
| Autoencoders + anomaly detection | Phase 3, IMU data |
| Hard real-time constraints | Phase 1 + 3, FreeRTOS + deadlines |
| DMA + interrupt-driven design | Phase 1, STM32 firmware |
| Edge AI architecture | Throughout — where to put the intelligence |
