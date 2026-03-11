# Yocto Project & OpenEmbedded

This guide systematically explains the concepts, architecture, build pipeline, and practical usage of the Yocto Project for building embedded Linux systems. (Reference: [Jeonghun (James) Lee: YOCTO basic usage](https://ahyuo79.blogspot.com/2015/10/yocto.html))

---

## 1. Yocto Project Overview and Architecture

The Yocto Project is not just a single piece of "software." It is a **set of standards, documentation, and tools (a framework concept)** for creating custom embedded Linux distributions. The core engine that actually performs builds is **BitBake**.

### 1.1 Core Components

| Component | Role and Analogy |
|----------|------------------|
| **Yocto Project** | Large framework for building embedded Linux distributions (automotive manufacturing standards) |
| **BitBake** | Powerful Python-based task scheduler and build engine (actual factory machinery) |
| **OE-Core** | OpenEmbedded-Core. Common recipes and core libraries (shared materials warehouse) |
| **Poky** | **Reference distribution** bundle including OE-Core + BitBake + baseline configuration |
| **Layer** | Collection of recipes and metadata separated by purpose (blueprints and parts list) |

If you clone the Poky repository (`git clone git://git.yoctoproject.org/poky`), it already includes the basic skeleton required for builds (OE-Core, BitBake, default BSP, etc.).

### 1.2 Open Embedded Architecture Workflow

This is the overall OpenEmbedded / Yocto Project build flow, from source collection to packaging and final image generation.

![Open Embedded Architecture Workflow](./open-embedded-architecture-workflow.png)

1. **Upstream Source**: Original source code, SCM (Git), local project files, etc.
2. **Metadata/Inputs**: User configuration (`local.conf`), metadata (`.bb`, patches), machine BSP, etc.
3. **Build System (BitBake)**: Performs Source Fetching -> Patch -> Config/Compile -> Package stages
4. **Output Packages**: Generates `.deb`, `.rpm`, `.ipk` packages (including QA checks)
5. **Output Image Data**: Combines generated packages to create final image (rootfs) and SDK

---

## 2. Full Directory Structure (Source vs Build)

The Yocto environment is clearly separated into **Source** and **Build** areas.

### 2.1 Source Area (Blueprints + Material List)
The Source area contains recipes (`.bb`) and metadata, and should generally be treated as **read-only**. If changes are required, create a custom layer and override there instead of directly editing upstream files.

```text
Source/ (e.g., poky/)
├── meta/               <- OE-Core (Yocto base)
├── meta-poky/          <- Poky distribution config
├── meta-yocto-bsp/     <- Reference BSP
└── meta-openembedded/  <- Additional generic packages (Qt, Python, etc.)
```

### 2.2 Build Area (Actual Workspace)
This is the workspace where compilation happens and build artifacts are stored. It is created and entered through `oe-init-build-env`.

```bash
# Create Build Directory as 'build' and activate environment variables
$ source poky/oe-init-build-env build
```
> **Note**: Every time you open a new terminal, run the command above to activate the build environment (BitBake commands, etc.).

```text
build/
├── conf/               <- * The only folder users directly edit
|   ├── local.conf      <- Global build settings (machine, threads, package type, etc.)
|   └── bblayers.conf   <- List of layer paths to include in the build
├── downloads/          <- Source archives fetched externally (cache role)
├── sstate-cache/       <- Build state hash cache that reduces compile time
└── tmp/                <- Actual build work dir and final artifacts (details below)
```

### 2.3 Key User Configuration (`local.conf` & `bblayers.conf`)

**`conf/local.conf`**: Global environment settings
| Variable | Description |
|----------|-------------|
| `MACHINE` | Target board (e.g., `raspberrypi4`, `qemux86-64`) |
| `DISTRO` | Distribution setting (default: `poky`) |
| `DL_DIR` | Downloaded source store. Recommended to move to external path shared by multiple build directories |
| `SSTATE_DIR` | Shared state cache path. Essential for build speed optimization |
| `BB_NUMBER_THREADS` / `PARALLEL_MAKE` | BitBake parsing threads and `make -j` option (set based on CPU cores) |
| `PACKAGE_CLASSES` | Package formats to generate (`package_rpm`, `package_ipk`, `package_deb`) |
| `EXTRA_IMAGE_FEATURES` | Extra features added to image (e.g., `debug-tweaks`, `ssh-server-openssh`) |

**`conf/bblayers.conf`**: Lists absolute paths of participating layers in the `BBLAYERS` variable.

---

## 3. Layer Architecture

A layer is a **folder containing build recipes for a specific purpose**. This is one of Yocto's most powerful features: you assemble images by combining only the layers you need, like Lego blocks.

- **Why separate layers**: swap only BSP layer when hardware changes, reuse across projects, easier dependency/conflict management

### 3.1 Three Types of Yocto Layers

![Yocto Layer types and BitBake flow](./yocto-layer-types.png)

| Configuration | Layer Type | Role |
|---------------|------------|------|
| **Policy Config** | **Distro Layer** | Distribution policy (`conf/distro/*.conf`), `classes/`, common rules |
| **Machine Config** | **BSP Layer** | Kernel config, device tree, bootloader for specific board/target arch |
| **Metadata** | **Software Layer** | Recipes for apps, middleware, open-source packages (`meta-qt6`, etc.) |

> **Expert tip**: In large projects such as IVI (In-Vehicle Infotainment), avoid creating one layer per app. A category-based structure (e.g., `meta-ivi/recipes-apps/`) is considered best practice.

### 3.2 BSP (Board Support Package) Layer

This is the "manual" needed to run specific hardware.
- `conf/machine/<machine_name>.conf`: CPU architecture, tuning parameters, and kernel/bootloader provider selection
- `recipes-bsp/`: Bootloader (e.g., u-boot)
- `recipes-kernel/`: Linux kernel source, `defconfig`, `.dts` files

### 3.3 Layer Creation and Management Tools

```bash
# 1. Create a new layer (convention: name starts with 'meta-')
$ bitbake-layers create-layer meta-mylayer

# 2. Add layer to the build environment (bblayers.conf)
$ bitbake-layers add-layer /path/to/meta-mylayer
```
**Save and restore layer settings**: Use `setup-layers` when sharing build environments with teammates (similar to `package.json` in npm).
```bash
$ bitbake-layers create-layers-setup /save/path/
```

---

## 4. Recipe (`.bb`) and Syntax

A recipe is metadata that tells BitBake **"where to fetch source, how to compile, and how to package it."**

### 4.1 Recipe Filename Convention
`package-name_version.bb` (example: `wget_1.21.3.bb`)

### 4.2 Important Variables (Expert Level)
| Variable | Description |
|----------|-------------|
| `SRC_URI` | Source location (Git, HTTP, local file, etc.) and patch files (`.patch`) |
| `DEPENDS` | **Build-time dependency**. Headers/libraries needed while compiling (installed into recipe-sysroot) |
| `RDEPENDS:${PN}` | **Run-time dependency**. Packages required when running on target device (installed into RootFS) |
| `S` | Path to unpacked source (`${WORKDIR}/${PN}-${PV}`) |
| `B` | Actual build (compile) path (usually same as `S`, but differs for out-of-tree builds) |
| `D` | Temporary destination path where `do_install` copies outputs |

### 4.3 Inherit
Reduce duplication by inheriting classes (`.bbclass`) that encapsulate common build logic.
```python
inherit autotools    # Auto-handles ./configure, make, make install
inherit cmake        # Auto-handles CMake build logic
inherit systemd      # Auto-handles Systemd service registration
```

### 4.4 Override with `.bbappend`
Use `.bbappend` to write **only changes/additions** without editing the original recipe (`.bb`). The filename/version must match the original recipe; use `%` wildcard for version-agnostic matching (`someapp_%.bbappend`).

### 4.5 Override Syntax (Overrides)
> **Important**: Syntax differs by Yocto version. In recent Yocto (after Honister 3.4), colons (`:`) are used instead of underscores (`_`).

```python
DEPENDS = "foo"                # default
DEPENDS:raspberrypi4 = "bar"   # override only when MACHINE is raspberrypi4

CFLAGS:append = " -Wall"       # append with leading space
CFLAGS:prepend = "-O2 "        # prepend
CFLAGS:remove = "-O2"          # remove specific substring
```

---

## 5. BitBake Build Engine and How It Works

BitBake is a scheduler that constructs task pipelines and runs them in parallel according to dependencies.

### 5.1 DAG (Directed Acyclic Graph) and Parallel Execution
BitBake parses all recipes and creates a dependency graph (DAG). Independent tasks (such as `do_compile`) are executed in parallel up to `BB_NUMBER_THREADS`, maximizing build throughput.

### 5.2 Sstate-Cache (Shared State Cache)
This is the key feature that dramatically reduces Yocto build time.
- Generates a unique signature hash for each task using **inputs (source code, recipe, env vars, dependency hashes)**.
- If the hash matches a previous build, BitBake skips rebuilding and restores artifacts directly from `sstate-cache/`.
- If source or variables change by even one byte, the hash changes and affected tasks are rerun transitively.

### 5.3 BitBake Commands
```bash
$ bitbake <recipe_name>           # run full task pipeline (e.g., bitbake core-image-minimal)
$ bitbake <recipe> -c <task>      # run specific task only (e.g., bitbake wget -c compile)
$ bitbake <recipe> -c <task> -f   # force task run, ignoring cache
$ bitbake <recipe> -g             # generate dependency graph (task-depends.dot)
$ bitbake -e <recipe>             # print final parsed env vars for recipe (debugging)
$ bitbake -c cleansstate <recipe> # remove build temp files and sstate cache for recipe
```

### 5.4 Yocto Build Workflow (End-to-End Flow)

![Yocto Build Workflow - Poky layout](./yocto-poky-build-workflow.png)

1. Prepare host system (install required packages)
2. Clone Poky and meta layers (BSP, etc.)
3. Initialize build environment (`source oe-init-build-env`)
4. Modify configuration (`local.conf`, `bblayers.conf`)
5. Run BitBake build (`bitbake <image_name>`)

---

## 6. Yocto Recipe Build Pipeline (Task Flow)

Detailed flow for processing a single recipe.

![Yocto Recipe Workflow](./yocto-recipe-workflow.png)

### 6.1 Source Fetch, Unpack, and Patch

![Source Fetch and Patch Application](./bitbake-source-fetch-patch.png)

- **`do_fetch`**: Parses `SRC_URI` and fetches source into `downloads/` from mirrors (remote/local cache) or upstream.
- **`do_unpack`**: Extracts downloaded archive into `${WORKDIR}` in the Build Directory.
- **`do_patch`**: Applies `*.patch` files listed in `SRC_URI` to the source.

### 6.2 Configuration and Compile

![Config and Compile flow](./bitbake-config-compile-install.png)

- **`do_configure`**: Configures source build system (Autotools, CMake, etc.). (optional)
- **`do_compile`**: Performs actual cross-compilation. (optional)
- **`do_install`**: Installs compiled binaries/libraries/headers into virtual destination directory (`${D}`). At this point, it is still one unsplit install tree, not yet packaged.

### 6.3 Package Splitting (Core of Size Optimization)

This is the key step where Yocto achieves **size optimization** for embedded devices by splitting the monolithic install tree under `${D}` by purpose.

- Target device: needs runtime libraries and binaries only
- Developer PC: needs header files (`/usr/include`)
- Others: isolate unnecessary items such as docs (`/usr/share/doc`)

**Process**:
1. Copy contents from `D` into `PKGD` staging area.
2. **`do_packagedata`**: Analyzes library dependencies and stores metadata in `PKGDATA_DIR` (e.g., "Package A requires libQt6.so").
3. **`do_package`**: Distributes files into `PKGDEST` (`packages-split/`) based on recipe `PACKAGES` and `FILES:*` rules.
4. **`do_package_write_*`**: Archives split directories into final `.rpm`, `.deb`, `.ipk` package formats.
5. Artifacts are placed into **Package Feeds** (`tmp/deploy/rpm`, etc.) like products on shelves.

```python
# Example: package split control in a recipe
PACKAGES = "${PN} ${PN}-dev ${PN}-dbg ${PN}-doc"
FILES:${PN}     = "${bindir}/* ${libdir}/*.so.*"
FILES:${PN}-dev = "${includedir}/* ${libdir}/*.so"
```

### 6.4 Image Generation (`do_rootfs`)

After individual packaging, Yocto builds the **final image** to be flashed on the target board.

![Image Generation flow](./yocto-image-generation.png)

It picks packages listed in `IMAGE_INSTALL` from **Package Feeds** and assembles the actual target root filesystem.

**Main variables (inside image recipe)**
| Variable | Description |
|----------|-------------|
| `IMAGE_INSTALL` | Required package list to install into RootFS |
| `IMAGE_FEATURES` | Add package groups/features (e.g., `ssh-server-dropbear`) |
| `IMAGE_FSTYPES` | Output image formats (e.g., `ext4`, `wic.gz`, `tar.bz2`) |
| `ROOTFS_POSTPROCESS_COMMAND` | Custom shell hook right after rootfs is assembled and before image creation/compression (config tuning, permission fixes, etc.) |

When build completes, `tmp/deploy/images/<machine>/` contains kernel (`zImage`/`bzImage`), device tree (`.dtb`), and rootfs images (`.ext4`, etc.).

---

## 7. SDK (Software Development Kit) Generation

To develop applications for your Yocto-built embedded OS, you need a separate cross-toolchain and sysroot (headers/libraries).

- **Role separation**: Yocto = builds target OS images / SDK = distributes app development toolchain for that OS
- **SDK command**:
  ```bash
  $ bitbake <image_name> -c populate_sdk
  ```
- After build, an installer script (`.sh`) is generated in `tmp/deploy/sdk/`. Run it on developers' PCs to install the toolchain.
- After installation, source the SDK environment (`source environment-setup-...`) to map cross-compilers in place of host GCC, enabling straightforward target-platform builds.

---

## 8. Layer and Recipe Management Best Practices

1. **Never edit upstream recipes directly**: Do not modify OE-Core or Poky source files (such as under `/meta/`) directly. Always create a custom layer and use `.bbappend` or patches.
2. **Separate machine-specific settings**: Keep hardware-specific settings under `conf/machine/`, and software additions in image recipes or `local.conf`.
3. **Follow override syntax**: Use colon (`:`) syntax for variable overrides in modern Yocto versions (e.g., `FILES:${PN}`), not underscore (`_`).
4. **Externalize download/sstate directories**: In `local.conf`, set `DL_DIR` and `SSTATE_DIR` to absolute paths outside the build folder. This saves major time by reusing downloads/cache even after recreating build directories.

---

## 9. Practical Troubleshooting

### 9.1 "embedded git repository" warning when integrating Yocto meta-layers

When setting up Yocto and then running `git add .` in your main project after cloning layers like `poky`, `meta-openembedded`, you may see:

```text
warning: adding embedded git repository: yocto/meta-openembedded
hint: You've added another git repository inside your current repository.
```

**Cause:**
Another independent Git repository (e.g., `poky` with its own `.git`) was copied inside your main repository. If committed as-is, those folders may appear empty when cloned elsewhere.

**Fix:**

**Step 1: Remove wrongly staged entries from cache (force)**
Use `-f` to remove staged entries. (Actual files are not deleted.)
```bash
git rm -f --cached yocto/meta-openembedded
git rm -f --cached yocto/meta-qt5
git rm -f --cached yocto/meta-raspberrypi
git rm -f --cached yocto/poky
```

**Step 2: Choose management strategy**

*   **Method A: Keep large Yocto folders local only (recommended)**
    Add this at the end of `.gitignore` to fully exclude from tracking:
    ```text
    yocto/meta-openembedded/
    yocto/meta-qt5/
    yocto/meta-raspberrypi/
    yocto/poky/
    ```

*   **Method B: Manage cleanly with submodules (canonical)**
    Delete previously downloaded folders and re-register them with `git submodule add <url> <path>`. GitHub then stores only the layer's specific commit reference, not full source copies.

---

## 10. Our Practice Log (Actual Head_Unit Project Progress)

This section is not theory. It is a learning log of real commands and results executed in the current repository.  
Keep appending new entries in the same format as work progresses.

### 10.1 Progress so far (2026-03-06)

#### A) Environment recovery and baseline verification

1. Relax AppArmor restriction (enable BitBake execution)
```bash
sudo sysctl -w kernel.apparmor_restrict_unprivileged_userns=0
```
- Why: Ubuntu 24.04 security policy blocked BitBake parsing.
- Actual check result: `kernel.apparmor_restrict_unprivileged_userns = 0`

2. Activate Yocto build environment
```bash
source yocto/poky/oe-init-build-env yocto/build
```
- Why: Load BitBake/Yocto environment variables into current shell.
- Meaning: `bitbake` and `bitbake-layers` are available only in that terminal session.

3. Verify layer registration
```bash
bitbake-layers show-layers
```
- Actual result: successful detection of layers including `meta-piracer`, `meta-raspberrypi`, and `meta-qt5`.
- Meaning: `bblayers.conf` is configured correctly.

#### B) Real issue resolution (laptop migration issue)

1. Error encountered
```text
Unable to get checksum for headunit SRC_URI entry Head_Unit_jun: file could not be found
... searched: /home/seame2026/piracer/Head_Unit_jun
```
- Cause: Absolute path from previous laptop was hardcoded in recipe.

2. Key fix (`meta-piracer/recipes-hu/headunit/headunit_git.bb`)
- Before: hardcoded absolute path `/home/seame2026/...`
- After: use `externalsrc` to reference local worktree directly during development

```bitbake
SRC_URI = "file://hu-shell.service"
inherit cmake_qt5 pkgconfig systemd externalsrc
EXTERNALSRC = "${TOPDIR}/../../Head_Unit_jun"
S = "${EXTERNALSRC}"
B = "${WORKDIR}/build"
```

- Why better: reusable across laptops if repository layout stays consistent.
- Why `cmake_qt5`: automatically injects Yocto variables (`OE_QMAKE_PATH_*`) required for Qt5 CMake targets like `Qt5::Core`.

#### C) Recipe detection/source fetch success

1. Query recipe
```bash
bitbake-layers show-recipes headunit
```
- Actual result: `headunit: meta-piracer git` matched successfully.

2. Run fetch stage
```bash
bitbake headunit -c fetch
```
- Actual result: `Tasks Summary ... all succeeded.`
- Meaning: `do_fetch` succeeded; source path issue resolved.

#### D) Start splitting RPi target build directory

1. Create/enter new build directory
```bash
source yocto/poky/oe-init-build-env yocto/build-rpi
```

2. Register project layers in `build-rpi`
- `meta-oe`, `meta-python`, `meta-multimedia`, `meta-networking`, `meta-raspberrypi`, `meta-qt5`, `meta-piracer`

3. Current status
- `bitbake-layers show-layers` confirms layer list is normal.
- Next check point: ensure `MACHINE` in `conf/local.conf` is fixed to `raspberrypi4`.

#### E) Root cause and fix for `do_configure` failure (RPi context)

1. First failure log
```text
.../Head_Unit_jun does not appear to contain CMakeLists.txt
```
- Cause: `file://` directory fetch output did not match expected source root, so `S` pointed to an empty directory.

2. Second failure log after first fix
```text
Target "hu_core" links to Qt5::Core but the target was not found
```
- Cause: Using generic `cmake` class may miss meta-qt5 CMake integration variable injection.

3. Final fix
- Change `inherit cmake` -> `inherit cmake_qt5`
- Result:
```bash
bitbake headunit -c configure
```
Passed with `Tasks Summary ... all succeeded.`

#### F) `do_compile` success (RPi context)

1. Command
```bash
bitbake headunit -c compile
```

2. Key logs
```text
NOTE: headunit: compiling from external source tree .../Head_Unit_jun
NOTE: Tasks Summary: ... all succeeded.
```

3. Meaning
- Compile task works correctly against real project source linked via `externalsrc`.
- Current recipe is verified through `fetch -> configure -> compile` for `raspberrypi4`.

#### G) Root cause and fix for `do_install` failure

1. Key failure log
```text
file INSTALL cannot find ".../Head_Unit_jun/run_gamepad_service.sh"
```
- Cause: `CMakeLists.txt` required installing `run_gamepad_service.sh`, but file did not exist in repository.

2. Action taken (`Head_Unit_jun/CMakeLists.txt`)
- Before:
```cmake
install(PROGRAMS run_gamepad_service.sh DESTINATION bin)
```
- After:
```cmake
if(EXISTS "${CMAKE_SOURCE_DIR}/run_gamepad_service.sh")
    install(PROGRAMS "${CMAKE_SOURCE_DIR}/run_gamepad_service.sh" DESTINATION bin)
else()
    message(STATUS "run_gamepad_service.sh not found - skip install")
endif()
```

3. Recheck result
```bash
bitbake headunit -c install -f
```
- Confirmed `do_install: Succeeded`.
- Meaning: install-stage failure due to missing file is resolved.

#### H) Legacy gamepad service script cleanup

1. Current baseline
- `run_gamepad_service.sh` no longer exists in `Head_Unit_jun`.
- Gamepad input/motor control is integrated in `instrument_cluster/python/piracer_bridge.py`.
- Entry point is `Head_Unit_jun/run_cluster.sh`, which auto-starts `piracer_bridge.py` during cluster run.

2. Code cleanup
- Removed legacy install trace from `Head_Unit_jun/CMakeLists.txt`:
```cmake
install(PROGRAMS run_gamepad_service.sh DESTINATION bin)
```
- Reason: remove install dependency on unused file to reduce confusion/build risk.

#### I) Final `bitbake headunit` success (RPi target)

1. Command
```bash
bitbake headunit
```

2. Key logs
```text
MACHINE = "raspberrypi4"
NOTE: headunit: compiling from external source tree .../Head_Unit_jun
NOTE: Tasks Summary: Attempted 2593 tasks ... all succeeded.
```

3. Warning interpretation
```text
... do_install is tainted from a forced run
```
- Meaning: an earlier `-f` (forced task run) history exists.
- Not a fatal error and independent of current build success.

#### J) `externalsrc` checksum warnings (`oe-logs/oe-workdir`) cleanup

1. Warning
```text
Unable to get checksum for headunit SRC_URI entry oe-logs ...
Unable to get checksum for headunit SRC_URI entry oe-workdir ...
```

2. Cause
- With `externalsrc`, internal helper symlinks (`oe-workdir`, `oe-logs`) can be referenced during checksum calculation. If those names are absent in source tree, warnings may appear.
- Not the reason of build failure; actual `Tasks Summary` was success.

3. Action
Add below in `headunit_git.bb`:
```bitbake
EXTERNALSRC_SYMLINKS = ""
```

4. Recheck
```bash
bitbake headunit
```
- Confirmed all tasks succeeded including `do_package_write_rpm`, with no warning.

#### K) Clean cache rebuild check + RPM artifact verification

1. Re-verify cache status
```bash
bitbake headunit
```
Key logs:
```text
Sstate summary: Wanted 0 ... Missed 0 ... Current 938 (100% complete)
Tasks Summary: Attempted 2593 tasks of which 2593 didn't need to be rerun and all succeeded.
```
- Meaning: no rebuild required for current changes; cache state is very stable.

2. RPM artifacts
```text
yocto/build-rpi/tmp/deploy/rpm/cortexa7t2hf_neon_vfpv4/
  headunit-git-r0.cortexa7t2hf_neon_vfpv4.rpm
  headunit-dev-git-r0.cortexa7t2hf_neon_vfpv4.rpm
  headunit-dbg-git-r0.cortexa7t2hf_neon_vfpv4.rpm
  headunit-src-git-r0.cortexa7t2hf_neon_vfpv4.rpm
```
- Meaning: recipe outputs are correctly deployed to package feed (rpm).

#### L) pkgdata registration verification complete

1. Command
```bash
oe-pkgdata-util list-pkgs | grep headunit
```

2. Actual output
```text
headunit
headunit-dbg
headunit-dev
headunit-src
```

3. Meaning
- `headunit` package family is correctly registered not only as artifacts but also in pkgdata index.
- Ready to include from image recipe using `IMAGE_INSTALL += "headunit"`.

#### M) Added Xen-oriented split structure files (Dom0 / DomU-HU / DomU-Cluster)

1. Added app recipes
- `yocto/meta-piracer/recipes-cluster/instrument-cluster/instrument-cluster_git.bb`
- `yocto/meta-piracer/recipes-cluster/instrument-cluster/files/piracer-cluster.service`

Key points:
- Link `instrument_cluster` via `externalsrc`
- Inherit `cmake_qt5` + `systemd`
- Install targets: `PiRacerDashboard`, `python/*.py`, `config/*.json`, `piracer-cluster.service`

2. Added image recipes
- `yocto/meta-piracer/recipes-core/images/piracer-dom0-image.bb`
- `yocto/meta-piracer/recipes-core/images/piracer-hu-image.bb`
- `yocto/meta-piracer/recipes-core/images/piracer-cluster-image.bb`

3. Recipe recognition check
```bash
bitbake-layers show-recipes instrument-cluster piracer-hu-image piracer-cluster-image piracer-dom0-image
```
Actual result:
- `instrument-cluster` matched
- `piracer-dom0-image`, `piracer-hu-image`, `piracer-cluster-image` matched

4. Meaning
- Base skeleton reflecting Xen target architecture (domain-separated images) is now integrated into Yocto layer.
- Next step is to refine per-domain package sets (e.g., Xen-related, vsomeip, policy packages).

#### N) Resolved image-build dependency error (restricted license)

1. Error
```text
Nothing RPROVIDES 'linux-firmware-rpidistro-bcm43455'
... skipped: Has a restricted license 'synaptics-killswitch'
which is not listed in your LICENSE_FLAGS_ACCEPTED.
```

2. Cause
- Firmware package in `meta-raspberrypi` requires restricted license flags.
- `piracer-hu-image` / `piracer-cluster-image` depend on it through `packagegroup-base-extended`.

3. Action (`build-rpi/conf/local.conf`)
```conf
LICENSE_FLAGS_ACCEPTED:append = " synaptics-killswitch"
```

4. Result
- Previous dependency resolution error for `linux-firmware-rpidistro-bcm43455` is resolved.

#### O) `PermissionError: /proc/self/uid_map` during kernel build

1. Error
```text
PermissionError: [Errno 1] Operation not permitted
... bb.utils.disable_network(...)
... with open("/proc/self/uid_map", "w")
```

2. Cause
- BitBake worker uses user namespace for network isolation, but host security policy (AppArmor + userns restriction) blocks it.
- Although observed at `linux-raspberrypi:do_compile`, root issue is host privilege/security policy, not kernel recipe itself.

3. Resolution procedure (runtime/permanent)
```bash
# 1) Immediate apply (valid until reboot)
sudo sysctl -w kernel.apparmor_restrict_unprivileged_userns=0

# 2) Verify
sysctl kernel.apparmor_restrict_unprivileged_userns
```

Permanent setting if needed:
```bash
echo 'kernel.apparmor_restrict_unprivileged_userns = 0' | sudo tee /etc/sysctl.d/99-yocto-userns.conf
sudo sysctl --system
```

4. Retry
```bash
source yocto/poky/oe-init-build-env yocto/build-rpi
bitbake piracer-hu-image
bitbake piracer-cluster-image
```

---

### 10.2 Key Lessons Learned So Far (Practice-Based)

1. **Most Yocto errors should first be checked as path/environment/layer issues.**
2. **Hardcoded absolute paths almost always break after laptop migration.**
3. **`show-layers` -> `show-recipes` -> `-c fetch` is the safest debugging routine.**
4. **QEMU (`qemux86-64`) and RPi (`raspberrypi4`) serve different goals.**
   - QEMU: fast development/verification
   - RPi: actual deployment image

---

### 10.3 Next Immediate Tasks (Practice Checklist)

1. Verify/apply `kernel.apparmor_restrict_unprivileged_userns`
2. Re-run `bitbake piracer-hu-image` / `bitbake piracer-cluster-image`
3. Check image artifacts (`tmp/deploy/images/<machine>/`)
4. Add Xen/backend package policy to Dom0 image

---

### 10.4 Practice Log Template (Keep Appending)

```md
#### [YYYY-MM-DD] Step N - Task Title
- Goal:
- Commands:
  - `...`
- Result:
  - Success/Failure
  - Key logs
- Lessons learned:
- Next action:
```
