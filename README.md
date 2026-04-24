# PiRacer Head Unit, Instrument Cluster, and Yocto Platform

An embedded automotive HMI platform for the PiRacer vehicle, combining a Qt-based Head Unit, a real-time Instrument Cluster, vehicle communication through CAN and vSomeIP, and a reproducible Yocto Linux image for Raspberry Pi 4.

The project is organized as a complete in-vehicle software stack rather than a single application. It covers user-facing HMI design, multi-process Qt architecture, inter-process communication, CAN-based vehicle data handling, camera integration, deployment automation, and target validation on embedded Linux.

## Executive Summary

This system implements a dual-display cockpit experience for the PiRacer platform:

| Area | Description |
|------|-------------|
| Head Unit | Driver-facing infotainment and control surface with media, navigation, ambient lighting, gear control, settings, reverse camera, and PDC overlay. |
| Instrument Cluster | Real-time dashboard for speed, RPM, battery, direction, drive time, and session telemetry. |
| Vehicle Communication | CAN for low-level vehicle/sensor data and vSomeIP for application-level Head Unit to Cluster synchronization. |
| Embedded Linux | Custom Yocto image targeting Raspberry Pi 4, with Qt, Wayland, GStreamer, vSomeIP, CAN utilities, and project services integrated. |
| Deployment | Docker-based Yocto build environment, BitBake recipes, image generation, SD flashing, and fast binary replacement for iterative testing. |

## System Goals

- Build a coherent automotive-style cockpit using separate Head Unit and Instrument Cluster applications.
- Run on Raspberry Pi 4 with reproducible Yocto images rather than an ad-hoc desktop Linux setup.
- Keep vehicle-facing logic behind interfaces so mock implementations and hardware implementations can be swapped cleanly.
- Support multi-process UI composition for better isolation between infotainment modules.
- Synchronize vehicle state across displays using a defined IPC layer.
- Validate the system on real target hardware, including CAN traffic and deployed binaries.

## High-Level Architecture

```text
                            PiRacer Vehicle
                                  |
                     CAN: speed / sensors / vehicle data
                                  |
                                  v
┌──────────────────────────────── Raspberry Pi 4 ───────────────────────────────┐
│                                                                                │
│  ┌────────────────────────────── Yocto Linux ────────────────────────────────┐ │
│  │ Qt5 / QtWayland / EGLFS / GStreamer / vSomeIP / SocketCAN / can-utils    │ │
│  └───────────────────────────────────────────────────────────────────────────┘ │
│                                                                                │
│  ┌──────────────────────────── Head Unit Shell ─────────────────────────────┐  │
│  │ hu_shell                                                                 │  │
│  │ - QtWayland compositor                                                    │  │
│  │ - Tab bar, status bar, shell lifecycle                                    │  │
│  │ - Reverse camera and PDC overlay                                          │  │
│  │ - Launches feature modules                                                │  │
│  └───────────────┬──────────────────────────────────────────────────────────┘  │
│                  │ Wayland surfaces + local module IPC                         │
│                  v                                                             │
│  ┌──────────────────────────────────────────────────────────────────────────┐  │
│  │ Head Unit Modules                                                        │  │
│  │ media | youtube | call | navigation | ambient | settings                 │  │
│  └──────────────────────────────────────────────────────────────────────────┘  │
│                                                                                │
│                         vSomeIP service/event channel                           │
│                                                                                │
│  ┌────────────────────────── Instrument Cluster ────────────────────────────┐  │
│  │ PiRacerDashboard                                                         │  │
│  │ - Speedometer, RPM, battery, drive time                                  │  │
│  │ - CAN speed input                                                        │  │
│  │ - Gear/direction synchronization                                         │  │
│  └──────────────────────────────────────────────────────────────────────────┘  │
│                                                                                │
└────────────────────────────────────────────────────────────────────────────────┘
```

## Repository Layout

```text
Head_Unit/
├── Head_Unit_jun/                     # Main Qt source tree
│   ├── shell/                         # hu_shell compositor and system shell
│   ├── modules/                       # Independent Head Unit feature modules
│   ├── core/                          # Shared interfaces, IPC, models, protocol
│   ├── instrument_cluster/            # Instrument Cluster Qt dashboard
│   ├── config/                        # vSomeIP and display configuration
│   └── docs/                          # Architecture, verification, PDC docs
│
├── yocto/                             # Embedded Linux build system
│   ├── Dockerfile                     # Ubuntu 22.04 Yocto build container
│   ├── docker-build.sh                # Build automation wrapper
│   ├── flash.sh                       # SD card flashing helper
│   ├── poky/                          # Yocto Poky base
│   ├── meta-openembedded/             # OE layer collection
│   ├── meta-raspberrypi/              # Raspberry Pi BSP layer
│   ├── meta-qt5/                      # Qt5 layer
│   └── meta-piracer/                  # Custom project layer
│       ├── recipes-hu/headunit/       # Head Unit recipe and startup files
│       ├── recipes-cluster/           # Instrument Cluster recipe
│       ├── recipes-core/images/       # Project image recipes
│       ├── recipes-connectivity/      # vSomeIP recipe
│       └── recipes-python/            # PiRacer Python support
│
├── pdc.md                             # Original PDC project brief
├── YOCTO_BUILD_GUIDE.md               # Detailed build/deployment guide
├── SSH_GUIDE.md                       # Target discovery and SSH notes
└── README.md
```

## Head Unit

The Head Unit is the primary driver-facing HMI. It is built around a shell process, `hu_shell`, that owns the display, manages shared vehicle state, launches feature modules, and composes module windows through QtWayland.

### Head Unit Responsibilities

| Responsibility | Implementation |
|----------------|----------------|
| Shell lifecycle | `ShellWindow` creates the root UI, compositor, modules, status bar, splash screen, and camera window. |
| Module isolation | Each major feature runs as its own process and connects back to the shell. |
| Display composition | `HUCompositor` receives Wayland surfaces and routes them into the active content area. |
| Vehicle state | `GearStateManager`, `IVehicleDataProvider`, `VSomeIPClient`, and module bridge messages synchronize speed, gear, battery, and IPC state. |
| Reverse camera | `ReverseCameraWindow` displays a GStreamer camera feed or placeholder fallback. |
| Parking assistance | PDC reads CAN distance data and renders warning overlays on the reverse camera. |

### Head Unit Modules

| Module | Binary | Purpose |
|--------|--------|---------|
| Media | `hu_module_media` | Local audio playback, playlist handling, media UI. |
| YouTube | `hu_module_youtube` | Web-based video/content surface where supported. |
| Call | `hu_module_call` | Phone/call UI placeholder and interaction surface. |
| Navigation | `hu_module_navigation` | Navigation and map-oriented UI. |
| Ambient | `hu_module_ambient` | Interior lighting presets, color control, brightness control. |
| Settings | `hu_module_settings` | Speed unit, build information, IPC status, vehicle status. |

### Shell And Module Communication

The Head Unit uses two communication mechanisms:

| Channel | Purpose |
|---------|---------|
| Wayland | Carries module-rendered surfaces into the compositor shell. |
| Local Unix socket IPC | Carries structured messages such as gear updates, speed updates, ambient color commands, and status updates. |

This separation allows UI rendering and control messages to remain independent. A module can crash or reconnect without requiring the entire shell to be redesigned around a monolithic widget tree.

## Instrument Cluster

The Instrument Cluster is a separate Qt application focused on real-time driving information. It targets a dashboard-style display and presents vehicle telemetry in a compact, glanceable form.

### Cluster Responsibilities

| Responsibility | Implementation |
|----------------|----------------|
| Speed display | Central analog/digital speedometer. |
| RPM display | Wheel RPM visualization. |
| Battery display | Voltage and percentage with color-coded status. |
| Session data | Drive time and maximum speed record. |
| Vehicle direction | Forward/reverse direction display and synchronization. |
| CAN input | SocketCAN-based speed data path in the cluster source tree. |
| Head Unit sync | vSomeIP gear/speed communication path. |

### Cluster Data Flow

```text
Speed sensor / Arduino / CAN
          |
          v
Instrument Cluster data reader
          |
          v
Dashboard widgets
          |
          +---- Speedometer
          +---- RPM gauge
          +---- Battery widget
          +---- Direction indicator
```

The cluster is intentionally separate from the Head Unit. This reflects the way real vehicle cockpits divide responsibilities between infotainment surfaces and safety/driver-information displays.

## Vehicle Communication

### CAN

CAN is used for vehicle and sensor data entering the Raspberry Pi. Current use cases include speed data and PDC distance data.

Observed CAN behavior from the target:

```text
can0  123   [8]  00 49 00 00 00 00 00 00
can0  123   [8]  00 48 00 00 00 00 00 00
```

The PDC implementation currently supports:

| CAN ID | Meaning |
|--------|---------|
| `0x123` | Observed live frame. Byte 1 is treated as a rear distance candidate. |
| `0x350` | Proposed four-sensor PDC frame using four little-endian centimeter values. |

CAN access is implemented through Linux SocketCAN, which allows the application to use standard Linux networking primitives for CAN sockets and event-driven reads.

### vSomeIP

vSomeIP is used for application-level communication between Head Unit and Instrument Cluster.

| Signal | Direction | Purpose |
|--------|-----------|---------|
| Speed | Cluster to Head Unit | Display and state synchronization. |
| Gear | Bidirectional | Gear state propagation from touch/gamepad/cluster sources. |
| Battery | Cluster to Head Unit | Vehicle status display. |

The project uses a service-oriented communication model instead of directly coupling UI classes across processes. That keeps the Head Unit and Cluster independently deployable and closer to automotive middleware patterns.

## Park Distance Control Extension

PDC is integrated as a feature extension of the reverse camera flow.

```text
GearState::R
    |
    v
ReverseCameraWindow opens
    |
    v
SocketCanPdcProvider reads can0
    |
    v
PdcController computes warning state
    |
    +--> PdcOverlayPainter renders guide lines
    |
    +--> PdcBeepController updates warning cadence
```

PDC components:

| Component | Role |
|-----------|------|
| `IPdcSensorProvider` | Abstract interface for distance sources. |
| `SocketCanPdcProvider` | Reads PDC frames from `can0`. |
| `MockPdcSensorProvider` | Generates local test data without sensors. |
| `PdcController` | Validates readings, handles stale data, computes warning level. |
| `PdcOverlayPainter` | Draws green/yellow/red overlay and sensor bars. |
| `PdcBeepController` | Converts warning level into audible feedback cadence. |

Warning model:

| Level | Distance |
|-------|----------|
| Far | `>= 120 cm` |
| Near | `60-119 cm` |
| Caution | `30-59 cm` |
| Critical | `< 30 cm` |
| Off | inactive, stale, invalid, or speed-gated |

Detailed PDC documentation is available in:

```text
Head_Unit_jun/docs/pdc/
```

## Yocto Platform

The project uses Yocto to build a controlled Linux image for Raspberry Pi 4. This is a central part of the system, not just a packaging detail.

### Why Yocto

Yocto provides:

| Capability | Value for this project |
|------------|------------------------|
| Reproducible root filesystem | The same image can be rebuilt with the same layers, recipes, and configuration. |
| Target-specific dependency control | Qt, vSomeIP, GStreamer, CAN tools, and project binaries are built into the image. |
| Service integration | Head Unit and Cluster startup scripts/services are installed as part of the image. |
| Board support | Raspberry Pi 4 kernel, boot files, device support, and hardware configuration are managed through layers. |
| Deployment artifact | Produces `.wic.bz2` images that can be flashed directly to SD card. |

### Yocto Layers

| Layer | Purpose |
|-------|---------|
| `poky` | Yocto reference distribution and OpenEmbedded Core. |
| `meta-openembedded` | Additional common recipes and libraries. |
| `meta-raspberrypi` | Raspberry Pi BSP support. |
| `meta-qt5` | Qt5 recipes used by the HMI applications. |
| `meta-piracer` | Project-specific image recipes, application recipes, services, and configuration. |

### Custom Recipes

| Recipe | Purpose |
|--------|---------|
| `headunit_git.bb` | Builds and installs `hu_shell` and Head Unit modules. |
| `instrument-cluster_git.bb` | Builds and installs the Instrument Cluster dashboard. |
| `piracer-hu-image.bb` | Creates the Head Unit target image. |
| `piracer-cluster-image.bb` | Creates the Instrument Cluster target image. |
| `vsomeip3_3.4.10.bb` | Provides vSomeIP middleware. |
| `python3-piracer_0.1.1.bb` | Provides PiRacer Python support. |

### Build Environment

The Yocto build runs inside a Docker container based on Ubuntu 22.04. This avoids host distribution issues and keeps the build environment stable.

```text
Host machine
    |
    v
Docker build container
    |
    v
BitBake / Yocto build
    |
    v
Raspberry Pi 4 image
```

### Build Commands

Build the full Head Unit image:

```bash
cd yocto
SKIP_DOCKER_BUILD=1 ./docker-build.sh piracer-hu-image
```

Build only the Head Unit package:

```bash
cd yocto
SKIP_DOCKER_BUILD=1 ./docker-build.sh headunit
```

Build the Instrument Cluster image:

```bash
cd yocto
SKIP_DOCKER_BUILD=1 ./docker-build.sh piracer-cluster-image
```

The build output is generated under:

```text
yocto/build-rpi/tmp/deploy/images/raspberrypi4-64/
```

Example artifact:

```text
piracer-hu-image-raspberrypi4-64.rootfs.wic.bz2
```

## Deployment

### Full Image Deployment

The full image can be flashed to an SD card:

```bash
cd yocto
./flash.sh /dev/sdX
```

The exact device should be verified with `lsblk` before flashing.

### Fast Application Deployment

During development, rebuilding and reflashing the entire image is unnecessary for most application changes. A faster workflow is to build the package, copy the updated binary to the Raspberry Pi, and restart the service.

Example for `hu_shell`:

```bash
scp yocto/build-rpi/tmp/work/cortexa72-poky-linux/headunit/git/image/usr/bin/hu_shell \
    root@192.168.86.35:/tmp/hu_shell.new

ssh root@192.168.86.35 \
    "cp /usr/bin/hu_shell /usr/bin/hu_shell.bak && \
     cp /tmp/hu_shell.new /usr/bin/hu_shell && \
     chmod 0755 /usr/bin/hu_shell && \
     /etc/init.d/hu-shell restart"
```

## Runtime On Target

On the Raspberry Pi target, the Head Unit runs as:

```text
/usr/bin/hu_shell
```

The shell launches module binaries:

```text
/usr/bin/hu_module_media
/usr/bin/hu_module_youtube
/usr/bin/hu_module_call
/usr/bin/hu_module_navigation
/usr/bin/hu_module_ambient
/usr/bin/hu_module_settings
```

Startup configuration is installed through the Yocto recipe:

```text
/etc/init.d/hu-shell
/usr/bin/config/vsomeip_headunit.json
/usr/bin/config/kms_headunit.json
```

Relevant runtime environment:

| Variable | Purpose |
|----------|---------|
| `QT_QPA_PLATFORM=eglfs` | Runs the shell directly on the display without desktop window manager. |
| `QT_QPA_EGLFS_KMS_CONFIG` | Selects KMS/display configuration. |
| `XDG_RUNTIME_DIR=/run/user/0` | Runtime directory for Wayland socket and Qt platform data. |
| `VSOMEIP_CONFIGURATION` | Points to Head Unit vSomeIP configuration. |
| `HU_REAR_CAMERA_DEVICE` | Camera device selection for reverse camera. |

## Verification

### Build Verification

Successful Yocto build summary:

```text
Tasks Summary: Attempted 7832 tasks ... all succeeded.
```

Image output:

```text
piracer-hu-image-raspberrypi4-64.rootfs.wic.bz2
piracer-hu-image-raspberrypi4-64.rootfs.wic.bmap
```

### Target Process Verification

```bash
ssh root@192.168.86.35 "ps | grep -E 'hu_shell|hu_module|PiRacerDashboard'"
```

Expected Head Unit processes:

```text
/usr/bin/hu_shell
/usr/bin/hu_module_media
/usr/bin/hu_module_youtube
/usr/bin/hu_module_call
/usr/bin/hu_module_navigation
/usr/bin/hu_module_ambient
/usr/bin/hu_module_settings
```

### CAN Verification

```bash
ssh root@192.168.86.35 "ip link show can0"
ssh root@192.168.86.35 "candump -tz -n 80 -T 3000 can0"
```

Expected:

```text
can0: <NOARP,UP,LOWER_UP,ECHO>
```

### PDC Runtime Verification

```bash
ssh root@192.168.86.35 "grep -E 'PDC|SocketCAN|can0' /tmp/hu_shell.log"
```

Observed:

```text
[PDC] Using SocketCAN sensor provider on can0
[PDC] SocketCAN provider listening on "can0"
```

## Engineering Highlights

### Multi-Process HMI

The Head Unit shell and feature modules are separated into multiple processes. This mirrors a more resilient cockpit architecture than a single monolithic UI process. Rendering surfaces are integrated through QtWayland, while control messages use local IPC.

### Hardware Abstraction

Vehicle data, PDC sensor data, and LED control are accessed through interfaces. This keeps UI code independent from mock, CAN, vSomeIP, or hardware-specific implementations.

### Embedded Build Reproducibility

Application code is not manually copied into a Raspberry Pi filesystem as the primary deployment path. It is represented through Yocto recipes and image definitions, which makes the system reproducible and reviewable.

### Target-Oriented Validation

The project validates beyond desktop compilation. It includes Raspberry Pi deployment, process checks, CAN interface checks, binary verification through `strings`, and live runtime log inspection.

## Current Status

| Area | Status |
|------|--------|
| Head Unit shell | Implemented and running on target. |
| Head Unit modules | Implemented as separate processes. |
| Instrument Cluster | Implemented as separate dashboard application. |
| Yocto image | Builds successfully for Raspberry Pi 4. |
| vSomeIP integration | Implemented; routing availability depends on runtime service configuration. |
| CAN interface | `can0` verified on target. |
| Reverse camera | Integrated with GStreamer pipeline and placeholder fallback. |
| PDC overlay | Implemented, built into Yocto image, deployed to target. |

## Documentation Index

| Document | Purpose |
|----------|---------|
| `YOCTO_BUILD_GUIDE.md` | Full Yocto build and deployment guide. |
| `SSH_GUIDE.md` | Raspberry Pi discovery and SSH connection notes. |
| `Head_Unit_jun/docs/ARCHITECTURE.md` | Head Unit architecture summary. |
| `Head_Unit_jun/docs/VERIFICATION.md` | Head Unit verification plan. |
| `Head_Unit_jun/instrument_cluster/docs/SPECIFICATION.md` | Instrument Cluster specification. |
| `Head_Unit_jun/instrument_cluster/docs/VERIFICATION_PLAN.md` | Instrument Cluster verification plan. |
| `Head_Unit_jun/docs/pdc/` | PDC architecture, CAN contract, UI spec, implementation plan, and verification plan. |

## Future Work

- Confirm final ultrasonic CAN payload contract with controlled obstacle-distance measurements.
- Improve vSomeIP startup orchestration so routing manager availability is deterministic.
- Replace placeholder call/YouTube/navigation behavior with production-ready services where required.
- Add automated integration tests for module IPC and vSomeIP message paths.
- Add screenshots and recorded demos for Head Unit, Cluster, reverse camera, and PDC operation.
- Consolidate fast deployment scripts for safe binary replacement on the Raspberry Pi.
