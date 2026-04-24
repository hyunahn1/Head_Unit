# PiRacer Park Distance Control

Park Distance Control (PDC) integration for the PiRacer head unit.  
The system augments the reverse camera view with distance-aware visual guidance using ultrasonic sensor data received from the vehicle CAN bus.

This project was built as a complete feature slice: sensor input, distance validation, warning-level classification, reverse-camera overlay rendering, audible warning control, Yocto deployment, and on-target verification on Raspberry Pi.

## Project Summary

When the driver selects reverse gear, the head unit opens the rear camera view. The PDC feature then reads rear obstacle distance data from CAN, computes the current warning state, and renders parking guide lines directly over the camera feed.

The resulting behavior is similar to production parking assist systems:

- green guidance for safe/near range
- yellow guidance for caution range
- red guidance for critical range
- sensor-sector feedback for obstacle direction
- stale-data suppression to avoid misleading warnings
- audible feedback based on proximity level

## System Context

The PDC feature is integrated into the existing PiRacer head unit software stack.

```text
PiRacer vehicle
  |
  |  Ultrasonic / speed data
  v
CAN bus
  |
  v
Raspberry Pi 4
  |
  +-- Head Unit Shell (Qt)
  |     |
  |     +-- ReverseCameraWindow
  |     +-- PdcController
  |     +-- SocketCanPdcProvider
  |     +-- PdcOverlayPainter
  |     +-- PdcBeepController
  |
  +-- Yocto Linux image
```

The feature is activated by the existing gear flow. When `GearState::R` is selected, the reverse camera window is shown and the PDC overlay becomes active.

## Key Features

| Feature | Description |
|---------|-------------|
| Reverse-camera overlay | Draws PDC guide lines on top of the rear camera feed or fallback placeholder. |
| CAN-based distance input | Reads live CAN frames from `can0` through SocketCAN. |
| Mock sensor support | Provides a development path for UI and logic testing without hardware. |
| Warning-level classification | Converts nearest obstacle distance into Far, Near, Caution, or Critical. |
| Stale-data handling | Suppresses warnings when CAN data stops updating. |
| Sensor-sector visualization | Shows which rear area is closest to an obstacle. |
| Audible warning controller | Maps proximity level to beep cadence. |
| Yocto deployment | Built into the Raspberry Pi head unit image and deployed to `/usr/bin/hu_shell`. |

## Architecture

The PDC implementation is divided into five main responsibilities.

```text
CAN / Mock Input
      |
      v
IPdcSensorProvider
      |
      v
PdcController
      |
      +--------------------------+
      |                          |
      v                          v
ReverseCameraWindow        PdcBeepController
      |
      v
PdcOverlayPainter
```

### 1. Sensor Provider Layer

The sensor provider abstracts the source of distance data.

| Component | Role |
|-----------|------|
| `IPdcSensorProvider` | Common interface for all PDC distance sources. |
| `SocketCanPdcProvider` | Reads PDC data from Linux SocketCAN on `can0`. |
| `MockPdcSensorProvider` | Generates synthetic sensor values for local UI testing. |

This separation keeps the rest of the PDC system independent from the transport layer. The UI and controller do not need to know whether data came from live CAN traffic or a mock provider.

### 2. PDC State Model

The shared PDC state lives in `PdcTypes`.

Core model types:

| Type | Description |
|------|-------------|
| `PdcSensorReading` | One sensor reading: name, distance in centimeters, validity, timestamp. |
| `PdcState` | Aggregated PDC state: rear sensors, nearest distance, warning level, active/stale flags. |
| `PdcWarningLevel` | Warning enum: Off, Far, Near, Caution, Critical. |

The model is intentionally small and explicit. It carries only the data needed by the controller, overlay painter, and beep controller.

### 3. PDC Controller

`PdcController` receives raw sensor readings and turns them into a stable UI/audio state.

Responsibilities:

- accept readings from an `IPdcSensorProvider`
- reject invalid distance values
- detect stale data using a timeout
- compute the nearest valid obstacle
- map nearest distance to a warning level
- suppress warnings when PDC is inactive
- suppress warnings when vehicle speed exceeds the configured active range
- emit `PdcState` updates for the camera overlay and beep controller

Default warning thresholds:

| Warning Level | Distance |
|---------------|----------|
| Far | `>= 120 cm` |
| Near | `60-119 cm` |
| Caution | `30-59 cm` |
| Critical | `< 30 cm` |
| Off | inactive, stale, invalid, or speed-gated |

### 4. Visual Overlay

`PdcOverlayPainter` renders the visual PDC information on top of `ReverseCameraWindow`.

Rendered elements:

- green guide lines for far/near range
- yellow guide line for caution range
- red guide line for critical range
- four rear sensor sector bars
- nearest-distance pill
- stale/fault indication when sensor data is unavailable

The overlay is drawn after the camera frame, so it works with both the real GStreamer camera feed and the existing placeholder image.

### 5. Audible Warning

`PdcBeepController` maps warning level to beep cadence.

| Warning Level | Beep Behavior |
|---------------|---------------|
| Off | no beep |
| Far | no beep |
| Near | slow beep |
| Caution | medium beep |
| Critical | fast beep |

The controller stops immediately when PDC becomes inactive or sensor data becomes stale.

## Data Flow

### Runtime Flow

```text
1. Gear changes to R
2. ShellWindow opens ReverseCameraWindow
3. PdcController becomes active
4. SocketCanPdcProvider reads CAN frames from can0
5. PdcController validates and classifies sensor data
6. ReverseCameraWindow receives PdcState
7. PdcOverlayPainter renders guide lines and sensor sectors
8. PdcBeepController updates audible feedback
9. Gear leaves R
10. Camera closes and PDC warnings stop
```

### CAN Input

The live PiRacer target was observed receiving CAN frames on `can0`.

Observed sample:

```text
(000.000000)  can0  123   [8]  00 49 00 00 00 00 00 00
(000.200799)  can0  123   [8]  00 48 00 00 00 00 00 00
(003.403546)  can0  123   [8]  00 4B 00 00 00 00 00 00
```

Supported decode paths:

| CAN ID | Payload | Meaning |
|--------|---------|---------|
| `0x123` | Byte 1 | Observed live frame. Byte 1 is treated as a rear distance candidate and copied to all rear sectors. |
| `0x350` | Four little-endian `uint16` values | Proposed four-sensor rear distance frame: rear-left, rear-mid-left, rear-mid-right, rear-right. |

The `0x123` mapping is intentionally isolated inside `SocketCanPdcProvider`, so it can be replaced once the final ultrasonic CAN contract is confirmed.

## Safety And Fault Handling

The PDC system favors safe suppression over misleading output.

| Fault / Condition | Handling |
|-------------------|----------|
| Gear is not reverse | PDC is inactive. |
| No CAN frame within stale timeout | Warning level becomes Off. |
| Sensor value below minimum range | Reading is invalid. |
| Sensor value above maximum range | Reading is invalid. |
| No valid sensors | No nearest distance is shown. |
| `can0` unavailable | Camera still opens; PDC fault is logged. |
| Camera unavailable | Placeholder remains visible; overlay can still render for debugging. |

## Repository Map

Important project files:

```text
Head_Unit_jun/
├── core/
│   ├── interfaces/
│   │   └── IPdcSensorProvider.h
│   ├── ipc/
│   │   ├── MockPdcSensorProvider.h/.cpp
│   │   └── SocketCanPdcProvider.h/.cpp
│   └── models/
│       └── PdcTypes.h
├── shell/
│   ├── PdcController.h/.cpp
│   ├── PdcBeepController.h/.cpp
│   ├── ShellWindow.h/.cpp
│   └── widgets/
│       ├── ReverseCameraWindow.h/.cpp
│       └── PdcOverlayPainter.h/.cpp
└── docs/
    └── pdc/
        ├── ARCHITECTURE.md
        ├── CAN_SIGNAL_CONTRACT.md
        ├── IMPLEMENTATION_PLAN.md
        ├── UI_OVERLAY_SPEC.md
        └── VERIFICATION_PLAN.md
```

## Build

The project is built through the Yocto Docker wrapper.

Build the full head unit image:

```bash
cd yocto
SKIP_DOCKER_BUILD=1 ./docker-build.sh
```

Build only the head unit package:

```bash
cd yocto
SKIP_DOCKER_BUILD=1 ./docker-build.sh headunit
```

The head unit recipe uses `externalsrc`, so it builds from the local `Head_Unit_jun` source tree.

## Deployment

After building, the new `hu_shell` binary can be deployed directly to the Raspberry Pi for fast validation without reflashing the SD card.

```bash
scp yocto/build-rpi/tmp/work/cortexa72-poky-linux/headunit/git/image/usr/bin/hu_shell \
    root@192.168.86.35:/tmp/hu_shell.pdc

ssh root@192.168.86.35 \
    "cp /usr/bin/hu_shell /usr/bin/hu_shell.bak && \
     cp /tmp/hu_shell.pdc /usr/bin/hu_shell && \
     chmod 0755 /usr/bin/hu_shell && \
     /etc/init.d/hu-shell restart"
```

The complete image artifact is also generated under:

```text
yocto/build-rpi/tmp/deploy/images/raspberrypi4-64/
```

## Target Verification

PDC deployment was verified on the Raspberry Pi target.

### CAN Interface

```bash
ssh root@192.168.86.35 "ip link show can0"
```

Expected:

```text
can0: <NOARP,UP,LOWER_UP,ECHO>
```

### CAN Traffic

```bash
ssh root@192.168.86.35 "candump -tz -n 80 -T 3000 can0"
```

Observed:

```text
can0  123   [8]  00 49 00 00 00 00 00 00
```

### Binary Contains PDC

```bash
ssh root@192.168.86.35 "strings /usr/bin/hu_shell | grep PDC"
```

Expected:

```text
[PDC] Using SocketCAN sensor provider on can0
[PDC] SocketCAN provider listening on
```

### Runtime Log

The deployed binary confirmed SocketCAN startup:

```text
[PDC] Using SocketCAN sensor provider on can0
[PDC] SocketCAN provider listening on "can0"
```

## Validation Checklist

| Test | Expected Result |
|------|-----------------|
| Shift to reverse | Reverse camera window opens. |
| PDC provider starts | Log shows SocketCAN provider on `can0`. |
| CAN frame received | PDC state updates from `0x123` or `0x350`. |
| Safe distance | Green guide lines are visible. |
| Medium distance | Yellow guide line becomes emphasized. |
| Close obstacle | Red guide line becomes emphasized. |
| CAN data stops | PDC becomes stale and warning is suppressed. |
| Shift out of reverse | Camera closes and beep stops. |

## Documentation

Detailed PDC documentation is available in:

```text
Head_Unit_jun/docs/pdc/
```

| Document | Purpose |
|----------|---------|
| `ARCHITECTURE.md` | System architecture and component responsibilities |
| `CAN_SIGNAL_CONTRACT.md` | CAN IDs, payload assumptions, and discovery procedure |
| `UI_OVERLAY_SPEC.md` | Overlay colors, layout, and warning visualization |
| `IMPLEMENTATION_PLAN.md` | Implementation phases and file-level plan |
| `VERIFICATION_PLAN.md` | Unit, integration, UI, and hardware validation plan |

## Future Work

- Confirm the final ultrasonic CAN payload format with controlled obstacle-distance tests.
- Calibrate guide-line geometry to the actual rear camera field of view.
- Add per-sensor fault indicators to distinguish invalid, disconnected, and out-of-range sensors.
- Replace the initial `QApplication::beep()` implementation with target-specific audio or buzzer output.
- Add a standalone simulator repository for demos and threshold tuning without hardware.

## Status

Implemented and deployed to the Raspberry Pi head unit target.  
The current version supports live SocketCAN input, reverse-camera overlay rendering, stale-data handling, and warning-level classification.
