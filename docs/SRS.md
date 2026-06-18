# System Requirements Specification — ZERO

| Field | Value |
|---|---|
| Version | 0.3 |
| Date | 2026-06-18 |
| Author | Soufiane El mouahid |
| Status | DRAFT |

---

## 1. Purpose

This document defines the system-level requirements for the ZERO embedded AI 
system. It serves as the reference against which all implementation and testing 
decisions are validated.

## 2. Scope

The system monitors motion, vibrations, hand gestures, and spoken keywords to 
detect predefined events and trigger a physical servo response. All detection 
events are logged to files accessible to the operator. This document covers 
system behaviour only — architecture, implementation, and test procedures are 
defined in separate documents.

## 3. Definitions

## 4. System Requirements

### 4.1 Functional

**SYS-F001** — The system shall detect the following motion events with an
accuracy ≥ 90%: being picked up, tipping over, falling.
- Verify against a test dataset labeled as detection or non-detection,
  containing minimum 50 non-detection samples (device stationary, device
  nudged or pushed without tipping or being picked up) and 50 detection
  samples per event type, never seen during training.
- Test reference: TEST_SPEC §...

**SYS-F002** — The system shall detect the following vibration events with an
accuracy ≥ 90%: strong knock on the surface, heavy object placed nearby,
strong impact nearby.
- Verify against a test dataset labeled as detection or non-detection,
  containing minimum 50 non-detection samples (light knocks, footsteps
  nearby, ambient vibrations) and 50 detection samples per event type,
  never seen during training.
- Test reference: TEST_SPEC §...

**SYS-F003** — The system shall detect the following hand gestures with an 
accuracy ≥ 90%: thumbs up, thumbs down, open palm.
- Verify against a test dataset labeled as detection or non-detection,
  containing minimum 50 non-detection samples (other hand gestures and no
  hand present) and 50 detection samples per gesture type, never seen
  during training.
- Test reference: TEST_SPEC §...

**SYS-F004** — The system shall detect the following English words with an 
accuracy ≥ 90%: "pause", "start", "stop".
- Verify against a test dataset labeled as detection or non-detection,
  containing minimum 50 non-detection samples (other speech, silence,
  background noise) and 50 detection samples per word, never seen during
  training.
- Test reference: TEST_SPEC §...

**SYS-F005** — The system shall activate the servo with a 45° clockwise
rotation upon any detection. The servo shall not activate when no detection
occurs.
- Verify servo rotates 45° clockwise ± 5° across 10 consecutive detection
  inputs per detection type. Verify no activation occurs across 10
  consecutive non-detection inputs.
- Test reference: TEST_SPEC §...

**SYS-F006** — The system shall log every detection event with a timestamp
and detection type to a file accessible to the operator.
- Verify log file contains correct timestamp and detection type for each
  triggered event across 10 consecutive detection inputs per detection type.
- Test reference: TEST_SPEC §...

### 4.2 Performance / Real-time
**SYS-P001** — ...

### 4.3 Reliability / Robustness
**SYS-R001** — The system shall stay active until turned off by user. 
- monitor activity for 48 hours.

### 4.4 Observability
**SYS-O001** — ...

## 5. Constraints
<!-- One requirement per entry. Format:
**SYS-001** — Description.
- Measurable criterion
- Test reference: TEST_SPEC §X.Y
-->
