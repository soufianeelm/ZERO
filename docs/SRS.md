# System Requirements Specification — ZERO

| Field | Value |
|---|---|
| Version | 0.2 |
| Date | 2026-06-17 |
| Author | Soufiane El mouahid |
| Status | DRAFT |

---

## 1. Purpose

Define system requirement without going into technical details.

## 2. Scope

This system monitors vibrations, movement, visuals and audio to detect anomalies, make decisions according to data, and interact with actuators according to decisions made.

## 3. Definitions

## 4. System Requirements

### 4.1 Functional
**SYS-F001** - The system shall detect anomalies from vibrations and motion.
- Prewritten data that correspond to anomalies, output verification
- Test reference: TEST_SPEC ...

**SYS-F002** - The system shall detect visual and audio anomalies.
- Prewritten data that correspond to anomalies, output verification
- Test reference: TEST_SPEC...

**SYS-F003** - The system shall take decisions according to detected anomalies.
- Verification on decisions output, if it does take the right decision depending on the detected prewritten anomaly.
- Test reference: TEST_SPEC..

**SYS-F004** - The system shall interact with actuators according to decisions made.
- Verification of the actuators, if they receive the right command corresponding to the right decision.
- Test reference: TEST_SPEC...

**SYS-F005** - The system shall reject impossible data comming from sensors (vibrations, motion, visual, audio)
- Send impossible data and check if system rejects them
- Test reference: TEST_SPEC...

<!-- One requirement per entry. Format:
**SYS-001** — Description.
- Measurable criterion
- Test reference: TEST_SPEC §X.Y
-->
