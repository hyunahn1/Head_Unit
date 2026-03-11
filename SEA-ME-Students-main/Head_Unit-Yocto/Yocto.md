# Yocto Project & OpenEmbedded

임베디드 리눅스 시스템 구축을 위한 Yocto Project의 개념, 아키텍처, 빌드 파이프라인 및 실무 활용법을 체계적으로 정리한 가이드입니다. (참고: [Jeonghun (James) Lee: YOCTO 기본사용방법](https://ahyuo79.blogspot.com/2015/10/yocto.html))

---

## 1. Yocto Project 개요 및 아키텍처

Yocto Project는 단순한 '소프트웨어'가 아니라, 사용자 맞춤형 임베디드 리눅스를 만들기 위한 **표준, 문서, 도구들의 집합(개념)**입니다. 실제 빌드를 수행하는 핵심 엔진은 **BitBake**입니다.

### 1.1 핵심 구성 요소

| 구성요소 | 역할 및 비유 |
|----------|-------------|
| **Yocto Project** | 임베디드 리눅스 배포판 생성을 위한 거대한 프레임워크 (자동차 제조 표준) |
| **BitBake** | Python 기반의 강력한 태스크 스케줄러 및 빌드 엔진 (실제 공장 기계) |
| **OE-Core** | OpenEmbedded-Core. 공통 레시피와 핵심 라이브러리 모음 (공통 재료 창고) |
| **Poky** | OE-Core + BitBake + 기준 설정 묶음을 포함하는 **레퍼런스 배포판** |
| **Layer** | 기능별로 분리된 레시피와 메타데이터 모음 (설계도와 부품 목록) |

Poky 저장소를 클론(`git clone git://git.yoctoproject.org/poky`)하면 빌드에 필요한 기본 뼈대(OE-Core, BitBake, 기본 BSP 등)가 모두 포함되어 있습니다.

### 1.2 Open Embedded Architecture Workflow

Open Embedded / Yocto Project 빌드 프로세스의 전체 흐름입니다. 소스 수집부터 패키징, 최종 이미지 생성까지의 아키텍처를 보여줍니다.

![Open Embedded Architecture Workflow](./open-embedded-architecture-workflow.png)

1. **Upstream Source**: 원본 소스 코드, SCM(Git), 로컬 프로젝트 파일 등
2. **Metadata/Inputs**: User Configuration(`local.conf`), 메타데이터(`.bb`, 패치), Machine BSP 등
3. **Build System (BitBake)**: Source Fetching → Patch → Config/Compile → Package 분리 단계 수행
4. **Output Packages**: `.deb`, `.rpm`, `.ipk` 형태로 패키지 생성 (QA 테스트 포함)
5. **Output Image Data**: 생성된 패키지들을 조합하여 최종 Image(rootfs) 및 SDK 생성

---

## 2. 전체 디렉터리 구성 (Source vs Build)

Yocto 환경은 크게 **Source 영역**과 **Build 영역**으로 완벽히 분리됩니다.

### 2.1 Source 영역 (설계도 + 재료 목록)
Source 영역은 레시피(`.bb`)와 메타데이터가 존재하는 곳으로, **직접 수정하지 않는 것(Read-only)**이 원칙입니다. 수정이 필요하다면 새로운 Custom Layer를 만들어 오버라이드해야 합니다.

```text
Source/ (예: poky/)
├── meta/               ← OE-Core (Yocto 기본 베이스)
├── meta-poky/          ← Poky 배포판 설정
├── meta-yocto-bsp/     ← 레퍼런스 BSP
└── meta-openembedded/  ← 추가 범용 패키지 (Qt, Python 등)
```

### 2.2 Build 영역 (실제 작업 공간)
실제 컴파일이 이루어지고 결과물이 저장되는 작업 공간입니다. `oe-init-build-env` 스크립트를 통해 생성 및 진입합니다.

```bash
# Build Directory를 'build'로 생성 및 환경 변수 활성화
$ source poky/oe-init-build-env build
```
> **주의**: 새 터미널을 열 때마다 위 명령어를 실행하여 빌드 환경(BitBake 명령어 등)을 활성화해야 합니다.

```text
build/
├── conf/               ← ★ 사용자가 유일하게 직접 수정하는 설정 폴더
│   ├── local.conf      ← 빌드 전체 환경 설정 (머신, 스레드, 패키지 타입 등)
│   └── bblayers.conf   ← 빌드에 포함할 Layer 경로 목록
├── downloads/          ← 외부에서 Fetch한 소스 압축 파일들 (캐시 역할)
├── sstate-cache/       ← 컴파일 시간을 단축해주는 빌드 상태 해시 캐시
└── tmp/                ← 실제 빌드 작업 디렉터리 및 최종 결과물 (아래 상세 설명)
```

### 2.3 User Configuration 핵심 설정 (`local.conf` & `bblayers.conf`)

**`conf/local.conf`**: 전체 환경 설정값
| 변수 | 설명 |
|------|------|
| `MACHINE` | 타겟 보드 설정 (예: `raspberrypi4`, `qemux86-64`) |
| `DISTRO` | 배포판 설정 (기본값: `poky`) |
| `DL_DIR` | 다운로드된 소스 저장소. 여러 빌드 디렉터리가 공유하도록 외부 경로로 빼는 것을 권장 |
| `SSTATE_DIR` | Shared State 캐시 경로. 빌드 속도 최적화의 핵심 |
| `BB_NUMBER_THREADS` / `PARALLEL_MAKE` | BitBake 파싱 스레드 및 `make -j` 옵션 (CPU 코어 수에 맞게 설정) |
| `PACKAGE_CLASSES` | 생성할 패키지 포맷 (`package_rpm`, `package_ipk`, `package_deb`) |
| `EXTRA_IMAGE_FEATURES` | 이미지에 추가할 기능 (예: `debug-tweaks`, `ssh-server-openssh`) |

**`conf/bblayers.conf`**: `BBLAYERS` 변수에 빌드에 참여시킬 레이어의 절대 경로를 리스트업합니다.

---

## 3. 레이어 (Layer) 아키텍처

레이어는 **특정 목적의 빌드 레시피 모음 폴더**입니다. Yocto의 가장 강력한 특징으로, 레고 블록처럼 필요한 레이어만 조합해 이미지를 구성합니다.

- **분리 이유**: 하드웨어가 변경되어도 BSP 레이어만 교체하면 됨, 타 프로젝트와 재사용 용이, 의존성 및 충돌 관리 용이

### 3.1 Yocto Layer의 3가지 종류

![Yocto Layer 종류 및 BitBake 흐름](./yocto-layer-types.png)

| Configuration | Layer 종류 | 역할 |
|---------------|------------|------|
| **Policy Config** | **Distro Layer** | 배포판 정책 설정 (`conf/distro/*.conf`), `classes/`, 공통 규칙 담당 |
| **Machine Config** | **BSP Layer** | 특정 보드(Target Arch)를 위한 커널 설정, 디바이스 트리, 부트로더 |
| **Metadata** | **Software Layer** | 어플리케이션, 미들웨어, 오픈소스 패키지 레시피 제공 (`meta-qt6` 등) |

> **전문가 팁**: IVI(In-Vehicle Infotainment) 등 대규모 프로젝트에서는 앱마다 레이어를 만들지 않고, 카테고리 단위(예: `meta-ivi/recipes-apps/`)로 구조화하는 것이 모범 사례입니다.

### 3.2 BSP (Board Support Package) 레이어

특정 하드웨어를 구동하기 위한 '설명서'입니다.
- `conf/machine/<machine_name>.conf`: CPU 아키텍처, 튜닝 파라미터, 사용할 커널/부트로더 Provider 지정.
- `recipes-bsp/`: 부트로더 (u-boot 등)
- `recipes-kernel/`: 리눅스 커널 소스 및 `defconfig`, `.dts` 파일

### 3.3 레이어 생성 및 관리 도구

```bash
# 1. 새 레이어 생성 (이름은 'meta-'로 시작하는 것이 관례)
$ bitbake-layers create-layer meta-mylayer

# 2. 빌드 환경(bblayers.conf)에 레이어 추가
$ bitbake-layers add-layer /path/to/meta-mylayer
```
**레이어 설정 저장 및 복원**: 빌드 환경을 팀원과 공유할 때 `setup-layers` 기능을 사용합니다. (npm의 `package.json`과 유사)
```bash
$ bitbake-layers create-layers-setup /저장경로/
```

---

## 4. 레시피 (Recipe, `.bb`)와 문법

레시피는 BitBake에게 **"어디서 소스를 가져와서, 어떻게 컴파일하고, 어떻게 패키징할 것인가"**를 지시하는 메타데이터입니다.

### 4.1 레시피 파일 네이밍 규칙
`패키지명_버전.bb` (예: `wget_1.21.3.bb`)

### 4.2 주요 환경 변수 (전문가 레벨)
| 변수 | 설명 |
|------|------|
| `SRC_URI` | 소스 코드 위치 (Git, HTTP, 로컬 파일 등) 및 패치 파일(`.patch`) |
| `DEPENDS` | **빌드 타임(Build-time) 의존성**. 해당 패키지를 컴파일할 때 필요한 헤더/라이브러리 (recipe-sysroot에 설치됨) |
| `RDEPENDS:${PN}` | **런타임(Run-time) 의존성**. 타겟 기기에서 실행될 때 필요한 패키지 (RootFS에 같이 설치됨) |
| `S` | 압축 해제된 소스 코드가 위치하는 경로 (`${WORKDIR}/${PN}-${PV}`) |
| `B` | 실제 빌드(컴파일)가 이루어지는 작업 경로 (보통 `S`와 같지만 외부 빌드 시 다름) |
| `D` | `do_install` 태스크가 결과물을 복사할 임시 대상 경로 (Destination) |

### 4.3 상속(Inherit)
공통된 빌드 과정을 캡슐화한 클래스(`.bbclass`)를 상속받아 코드 중복을 줄입니다.
```python
inherit autotools    # ./configure, make, make install 로직 자동 처리
inherit cmake        # CMake 빌드 로직 자동 처리
inherit systemd      # Systemd 서비스 등록 로직 자동 처리
```

### 4.4 `.bbappend`를 활용한 오버라이드
기존 레시피 원본(`.bb`)을 수정하지 않고, **변경/추가할 내용만 작성**하는 파일입니다. 원본 레시피와 이름/버전이 일치해야 하며, 버전 무관 적용 시 `%` 와일드카드를 사용합니다 (`someapp_%.bbappend`).

### 4.5 오버라이드 구문 (Overrides)
> **중요**: Yocto 버전에 따라 문법이 다릅니다. 최근 Yocto(Honister 3.4 이후)에서는 언더스코어(`_`) 대신 콜론(`:`)을 사용합니다.

```python
DEPENDS = "foo"                # 기본값
DEPENDS:raspberrypi4 = "bar"   # MACHINE이 raspberrypi4일 때만 오버라이드

CFLAGS:append = " -Wall"       # 값 뒤에 공백과 함께 추가
CFLAGS:prepend = "-O2 "        # 값 앞에 추가
CFLAGS:remove = "-O2"          # 특정 문자열 제거
```

---

## 5. BitBake 빌드 엔진과 동작 원리

BitBake는 태스크 파이프라인을 구성하고 의존성에 따라 병렬 실행하는 스케줄러입니다.

### 5.1 DAG (Directed Acyclic Graph) & 병렬 실행
BitBake는 모든 레시피를 파싱하여 의존성 그래프(DAG)를 생성합니다. 상호 의존성이 없는 태스크(`do_compile` 등)는 `BB_NUMBER_THREADS`에 설정된 수만큼 병렬로 실행하여 빌드 시간을 극대화합니다.

### 5.2 Sstate-Cache (Shared State Cache)
Yocto 빌드 속도를 획기적으로 줄여주는 일등 공신입니다.
- 각 태스크의 **입력값(소스코드, 레시피, 환경변수, 의존성 해시)**을 묶어 고유 해시(Signature)를 생성합니다.
- 해시값이 이전과 동일하면 재빌드하지 않고 `sstate-cache/`에 저장된 이전 컴파일 산출물을 그대로 복원합니다.
- 소스나 변수가 단 1바이트라도 바뀌면 해시가 변경되어 해당 태스크부터 연쇄적으로 재실행됩니다.

### 5.3 BitBake 명령어
```bash
$ bitbake <recipe_name>           # 전체 태스크 실행 (예: bitbake core-image-minimal)
$ bitbake <recipe> -c <task>      # 특정 태스크만 실행 (예: bitbake wget -c compile)
$ bitbake <recipe> -c <task> -f   # 캐시를 무시하고 태스크 강제 실행 (Force)
$ bitbake <recipe> -g             # 의존성 그래프(task-depends.dot) 생성
$ bitbake -e <recipe>             # 레시피에 적용된 최종 환경변수 파싱 결과 출력 (디버깅용)
$ bitbake -c cleansstate <recipe> # 빌드 임시 파일 및 sstate 캐시 완벽 삭제
```

### 5.4 Yocto Build Workflow (전체 빌드 흐름)

![Yocto Build Workflow - Poky 구성](./yocto-poky-build-workflow.png)

1. 호스트 시스템 준비 (필수 패키지 설치)
2. Poky 및 메타 레이어(BSP 등) 클론
3. 빌드 환경 초기화 (`source oe-init-build-env`)
4. Configuration 수정 (`local.conf`, `bblayers.conf`)
5. BitBake 빌드 실행 (`bitbake <image_name>`)

---

## 6. Yocto 레시피 빌드 파이프라인 (Task 흐름)

단일 레시피가 처리되는 상세 흐름입니다.

![Yocto Recipe Workflow](./yocto-recipe-workflow.png)

### 6.1 Source Fetch & Unpack & Patch

![Source Fetch 및 Patch Application](./bitbake-source-fetch-patch.png)

- **`do_fetch`**: `SRC_URI`를 분석하여 Mirrors(원격/로컬 캐시)나 Upstream에서 소스를 `downloads/` 디렉터리로 가져옵니다.
- **`do_unpack`**: 다운로드한 아카이브를 Build Directory의 `${WORKDIR}`로 압축 해제합니다.
- **`do_patch`**: `SRC_URI`에 포함된 `*.patch` 파일들을 소스에 적용합니다.

### 6.2 Configuration & Compile

![Config 및 Compile 흐름](./bitbake-config-compile-install.png)

- **`do_configure`**: 소스 트리의 빌드 시스템(Autotools, CMake 등)을 구성합니다. (생략 가능)
- **`do_compile`**: 실제 크로스 컴파일을 수행합니다. (생략 가능)
- **`do_install`**: 컴파일된 바이너리, 라이브러리, 헤더 등을 가상의 대상 디렉터리(`${D}`)에 설치합니다. 이 시점에서는 아직 패키징이 되지 않은 통짜 결과물입니다.

### 6.3 Package Splitting (용량 최적화의 핵심)

Yocto가 임베디드 디바이스의 **용량 최적화**를 달성하는 핵심 단계입니다. `${D}` 경로에 설치된 통짜 결과물을 용도별로 잘게 쪼갭니다.

- 타겟 기기: 런타임 라이브러리와 바이너리만 필요
- 개발 PC: 헤더 파일(`/usr/include`) 필요
- 기타: 문서 파일(`/usr/share/doc`) 등 불필요한 요소 격리

**동작 과정**:
1. `D`의 내용을 `PKGD` 공간으로 복사하여 준비.
2. **`do_packagedata`**: 패키지 내 라이브러리 의존성을 분석하여 메타데이터를 생성하고 `PKGDATA_DIR`에 저장합니다. ("A 패키지는 libQt6.so가 필요하다"는 식의 정보를 기록)
3. **`do_package`**: 레시피의 `PACKAGES` 및 `FILES:*` 변수 규칙에 따라 `PKGDEST`(packages-split/) 폴더로 파일들을 분배합니다.
4. **`do_package_write_*`**: 쪼개진 디렉터리들을 최종 `.rpm`, `.deb`, `.ipk` 형태의 아카이브로 압축 패키징합니다.
5. 결과물은 **Package Feeds** (`tmp/deploy/rpm` 등)에 마트 진열대처럼 진열됩니다.

```python
# 레시피 내 패키지 스플릿 제어 예시
PACKAGES = "${PN} ${PN}-dev ${PN}-dbg ${PN}-doc"
FILES:${PN}     = "${bindir}/* ${libdir}/*.so.*"
FILES:${PN}-dev = "${includedir}/* ${libdir}/*.so"
```

### 6.4 Image Generation (`do_rootfs`)

개별 패키징이 끝난 후, 타겟 보드에 구워질 **최종 이미지**를 만드는 단계입니다.

![Image Generation 흐름](./yocto-image-generation.png)

**Package Feeds**(진열대)에서 `IMAGE_INSTALL` 변수에 정의된 패키지들을 픽업하여 실제 타겟의 파일시스템(rootfs) 형태를 조립합니다.

**주요 환경변수 (Image Recipe 내부)**
| 변수 | 설명 |
|------|------|
| `IMAGE_INSTALL` | RootFS에 설치할 필수 패키지 목록 |
| `IMAGE_FEATURES` | 패키지 그룹/기능 단위 추가 (예: `ssh-server-dropbear`) |
| `IMAGE_FSTYPES` | 생성할 이미지 포맷 (예: `ext4`, `wic.gz`, `tar.bz2`) |
| `ROOTFS_POSTPROCESS_COMMAND` | RootFS 폴더가 구성된 직후, 이미지 생성(압축) 직전에 실행할 커스텀 쉘 스크립트 지정 (설정 파일 튜닝, 권한 변경 등) |

빌드 완료 시 `tmp/deploy/images/<machine>/` 아래에 커널(`zImage`/`bzImage`), 디바이스 트리(`.dtb`), RootFS 이미지(`.ext4` 등)가 생성됩니다.

---

## 7. SDK (Software Development Kit) 생성

Yocto로 만든 임베디드 OS 위에서 동작할 애플리케이션을 개발하려면 크로스 컴파일 툴체인과 헤더/라이브러리 모음(Sysroot)이 별도로 필요합니다.

- **역할 분리**: Yocto = 타겟 OS 이미지 생성 / SDK = 해당 OS용 앱 개발 툴체인 배포
- **SDK 생성 명령어**:
  ```bash
  $ bitbake <image_name> -c populate_sdk
  ```
- 빌드 완료 후 `tmp/deploy/sdk/` 폴더에 설치 스크립트(`.sh`)가 생성됩니다. 이를 애플리케이션 개발자 PC에서 실행하여 툴체인을 설치합니다.
- 설치 후 터미널에서 SDK 환경을 활성화(`source environment-setup-...`)하면, PC의 기본 GCC 대신 크로스 컴파일러가 매핑되어 어플리케이션을 타겟 플랫폼용으로 손쉽게 컴파일할 수 있습니다.

---

## 8. 레이어 및 레시피 관리 모범 사례

1. **원본 레시피 수정 절대 금지**: OE-Core나 Poky의 원본 파일(`/meta/` 등)을 직접 수정하지 마세요. 반드시 Custom Layer를 만들고 `.bbappend`를 활용하여 덮어쓰거나 패치를 적용해야 합니다.
2. **머신별 설정 분리**: 하드웨어 종속적인 설정은 `conf/machine/` 안에 두고, 소프트웨어적인 추가 사항은 Image 레시피나 `local.conf`에 분리하는 것이 좋습니다.
3. **오버라이드 문법 준수**: 최신 Yocto 버전의 권장에 따라 환경 변수 재정의 시 언더스코어(`_`) 대신 콜론(`:`) 문법을 사용하세요 (`FILES:${PN}`).
4. **다운로드/Sstate 디렉터리 외부 분리**: `local.conf`에서 `DL_DIR`과 `SSTATE_DIR`을 빌드 폴더 외부의 절대 경로로 지정하세요. 빌드 폴더를 날리고 새로 구성하더라도 다운로드와 캐시를 재활용할 수 있어 막대한 시간을 절약할 수 있습니다.

---

## 9. 실전 문제 해결 (Troubleshooting)

### 9.1 Yocto 메타 레이어 Git 연동 시 "embedded git repository" 경고

Yocto 환경 세팅 시 `poky`, `meta-openembedded` 등 메타 레이어들을 `git clone`으로 다운로드 받은 후 메인 프로젝트에서 `git add .`를 하면 아래와 같은 경고가 발생할 수 있습니다.

```text
warning: adding embedded git repository: yocto/meta-openembedded
hint: You've added another git repository inside your current repository.
```

**원인:**
현재 작업 중인 메인 Git 저장소 안에 또 다른 독립적인 Git 저장소(`.git` 폴더를 포함한 `poky` 등)가 통째로 복사되어 들어갔기 때문에 발생합니다. 이대로 커밋하면 다른 PC에서 클론할 때 해당 폴더들이 빈 폴더로 받아집니다.

**해결 방법:**

**1단계: 잘못 add된 캐시 강제 삭제**
이미 임시 저장소(staging area)에 올라간 내역을 `-f` (force) 옵션으로 삭제합니다. (실제 파일은 지워지지 않으니 안심하셔도 됩니다.)
```bash
git rm -f --cached yocto/meta-openembedded
git rm -f --cached yocto/meta-qt5
git rm -f --cached yocto/meta-raspberrypi
git rm -f --cached yocto/poky
```

**2단계: 관리 방향 선택**

*   **방법 A: 용량 큰 Yocto 폴더는 깃허브에 안 올리고 내 컴퓨터에만 두기 (추천)**
    `.gitignore` 파일의 맨 끝에 아래 내용을 추가하여 Git 추적에서 완전히 제외시킵니다.
    ```text
    yocto/meta-openembedded/
    yocto/meta-qt5/
    yocto/meta-raspberrypi/
    yocto/poky/
    ```

*   **방법 B: 서브모듈(Submodule)로 깔끔하게 링크만 걸어서 관리하기 (정석)**
    기존에 다운받은 폴더들을 완전히 삭제한 후, `git submodule add <url> <경로>` 명령어를 사용하여 서브모듈로 다시 등록합니다. 이렇게 하면 깃허브에는 코드가 아닌 해당 레이어의 '특정 버전(커밋)' 정보만 깔끔하게 저장됩니다.

---

## 10. 우리 실습 로그 (Head_Unit 프로젝트 실제 진행 기록)

이 섹션은 "이론"이 아니라, 현재 저장소에서 우리가 실제로 실행한 명령과 결과를 기록하는 학습 노트입니다.  
앞으로 진행할 때마다 아래 형식으로 계속 추가합니다.

### 10.1 오늘까지 진행한 내용 (2026-03-06)

#### A) 환경 복구 및 기본 검증

1. AppArmor 제한 완화 (BitBake 실행 가능 상태로 전환)
```bash
sudo sysctl -w kernel.apparmor_restrict_unprivileged_userns=0
```
- 왜 했는가: Ubuntu 24.04 보안 정책 때문에 BitBake 파싱이 막히는 상황을 해제하기 위함.
- 실제 확인 결과: `kernel.apparmor_restrict_unprivileged_userns = 0`

2. Yocto build 환경 활성화
```bash
source yocto/poky/oe-init-build-env yocto/build
```
- 왜 했는가: 현재 셸에 BitBake/Yocto 환경변수를 로드하기 위함.
- 의미: 이 명령을 실행한 터미널에서만 `bitbake`, `bitbake-layers`를 정상 사용 가능.

3. 레이어 등록 상태 확인
```bash
bitbake-layers show-layers
```
- 실제 결과: `meta-piracer`, `meta-raspberrypi`, `meta-qt5` 포함 레이어 인식 성공.
- 의미: `bblayers.conf` 구성은 정상.

#### B) 실제 오류 해결 (노트북 변경 이슈)

1. 발생한 오류
```text
Unable to get checksum for headunit SRC_URI entry Head_Unit_jun: file could not be found
... searched: /home/seame2026/piracer/Head_Unit_jun
```
- 원인: 이전 노트북의 절대경로가 레시피에 하드코딩되어 있었음.

2. 수정한 핵심 포인트 (`meta-piracer/recipes-hu/headunit/headunit_git.bb`)
- 기존: `/home/seame2026/...` 절대경로 사용
- 변경: 개발 단계에서는 `externalsrc`로 로컬 워크트리를 직접 참조

```bitbake
SRC_URI = "file://hu-shell.service"
inherit cmake_qt5 pkgconfig systemd externalsrc
EXTERNALSRC = "${TOPDIR}/../../Head_Unit_jun"
S = "${EXTERNALSRC}"
B = "${WORKDIR}/build"
```

- 왜 좋은가: 노트북이 바뀌어도 저장소 구조만 같으면 재사용 가능.
- 왜 추가로 `cmake_qt5`를 썼는가: Qt5 CMake 타겟(`Qt5::Core`) 인식에 필요한 Yocto 변수(`OE_QMAKE_PATH_*`)를 자동 주입하기 위함.

#### C) 레시피 인식/소스 수집 성공

1. 레시피 조회
```bash
bitbake-layers show-recipes headunit
```
- 실제 결과: `headunit: meta-piracer git` 매칭 성공.

2. fetch 단계 실행
```bash
bitbake headunit -c fetch
```
- 실제 결과: `Tasks Summary ... all succeeded.`
- 의미: `do_fetch` 성공, 즉 소스 경로 문제 해결 완료.

#### D) RPi 타겟 build 디렉터리 분리 시작

1. 새 빌드 디렉터리 생성/진입
```bash
source yocto/poky/oe-init-build-env yocto/build-rpi
```

2. `build-rpi`에 프로젝트 레이어 등록
- `meta-oe`, `meta-python`, `meta-multimedia`, `meta-networking`, `meta-raspberrypi`, `meta-qt5`, `meta-piracer`

3. 현재 상태
- `bitbake-layers show-layers`에서 레이어 목록 정상 확인.
- 다음 확인 포인트: `conf/local.conf`의 `MACHINE` 값을 `raspberrypi4`로 고정했는지 점검.

#### E) `do_configure` 실패 원인 분석 및 해결 (RPi 컨텍스트)

1. 첫 실패 로그 핵심
```text
.../Head_Unit_jun does not appear to contain CMakeLists.txt
```
- 원인: `file://` 디렉터리 fetch 결과가 레시피가 기대한 소스 루트와 달라 `S` 경로가 비어 있는 디렉터리를 가리킴.

2. 수정 후 두 번째 실패 로그 핵심
```text
Target "hu_core" links to Qt5::Core but the target was not found
```
- 원인: 일반 `cmake` 클래스만 사용하면 meta-qt5의 CMake 통합 변수 주입이 부족할 수 있음.

3. 최종 해결
- `inherit cmake` -> `inherit cmake_qt5` 변경
- 결과:
```bash
bitbake headunit -c configure
```
`Tasks Summary ... all succeeded.` 로 통과 확인.

#### F) `do_compile` 성공 (RPi 컨텍스트)

1. 실행 명령
```bash
bitbake headunit -c compile
```

2. 핵심 로그
```text
NOTE: headunit: compiling from external source tree .../Head_Unit_jun
NOTE: Tasks Summary: ... all succeeded.
```

3. 의미
- `externalsrc`로 연결된 실제 프로젝트 소스를 기준으로 컴파일 태스크가 정상 수행됨.
- 현재 레시피는 `raspberrypi4` 타겟에서 `fetch -> configure -> compile` 단계까지 검증 완료.

#### G) `do_install` 실패 원인 분석 및 해결

1. 실패 로그 핵심
```text
file INSTALL cannot find ".../Head_Unit_jun/run_gamepad_service.sh"
```
- 원인: `CMakeLists.txt`에서 `run_gamepad_service.sh`를 필수 설치 대상으로 지정했지만, 저장소에 해당 파일이 없음.

2. 조치 내용 (`Head_Unit_jun/CMakeLists.txt`)
- 기존:
```cmake
install(PROGRAMS run_gamepad_service.sh DESTINATION bin)
```
- 변경:
```cmake
if(EXISTS "${CMAKE_SOURCE_DIR}/run_gamepad_service.sh")
    install(PROGRAMS "${CMAKE_SOURCE_DIR}/run_gamepad_service.sh" DESTINATION bin)
else()
    message(STATUS "run_gamepad_service.sh not found - skip install")
endif()
```

3. 재검증 결과
```bash
bitbake headunit -c install -f
```
- `do_install: Succeeded` 확인.
- 의미: 설치 단계에서 누락 파일로 중단되던 문제가 해소됨.

#### H) 레거시 게임패드 서비스 스크립트 정리

1. 현재 기준 정리
- `run_gamepad_service.sh`는 현재 `Head_Unit_jun`에 존재하지 않음.
- 게임패드 입력/모터 제어는 `instrument_cluster/python/piracer_bridge.py`로 통합됨.
- 실행 시작점은 `Head_Unit_jun/run_cluster.sh`이며, 클러스터 실행 중 `piracer_bridge.py`가 자동 기동됨.

2. 코드 정리 내용
- `Head_Unit_jun/CMakeLists.txt`의 레거시 설치 흔적 제거:
```cmake
install(PROGRAMS run_gamepad_service.sh DESTINATION bin)
```
- 이유: 더 이상 사용하지 않는 파일에 대한 설치 의존을 제거해 혼동/빌드 리스크를 방지.

#### I) `bitbake headunit` 최종 성공 (RPi 타겟)

1. 실행 명령
```bash
bitbake headunit
```

2. 핵심 로그
```text
MACHINE = "raspberrypi4"
NOTE: headunit: compiling from external source tree .../Head_Unit_jun
NOTE: Tasks Summary: Attempted 2593 tasks ... all succeeded.
```

3. 경고 메시지 해석
```text
... do_install is tainted from a forced run
```
- 의미: 이전에 `-f`(강제 실행)로 태스크를 돌린 이력이 있다는 알림.
- 치명 오류가 아니며, 이번 빌드 성공 여부와는 별개.

#### J) `externalsrc` checksum 경고(oe-logs/oe-workdir) 정리

1. 발생 경고
```text
Unable to get checksum for headunit SRC_URI entry oe-logs ...
Unable to get checksum for headunit SRC_URI entry oe-workdir ...
```

2. 원인
- `externalsrc` 사용 시 내부 helper symlink(`oe-workdir`, `oe-logs`)를 체크섬 계산 과정에서 참조하며, 소스 트리에 해당 이름이 없으면 경고가 출력될 수 있음.
- 빌드 자체 실패 원인은 아니었고, 실제 `Tasks Summary`는 성공 상태였음.

3. 조치
`headunit_git.bb`에 아래 설정 추가:
```bitbake
EXTERNALSRC_SYMLINKS = ""
```

4. 재검증
```bash
bitbake headunit
```
- 경고 없이 `do_package_write_rpm` 포함 전체 태스크 성공 확인.

#### K) 완전 캐시 재빌드 확인 + RPM 산출물 확인

1. 캐시 상태 재검증
```bash
bitbake headunit
```
핵심 로그:
```text
Sstate summary: Wanted 0 ... Missed 0 ... Current 938 (100% complete)
Tasks Summary: Attempted 2593 tasks of which 2593 didn't need to be rerun and all succeeded.
```
- 의미: 현재 변경사항 기준으로 재빌드가 필요 없고, 캐시 상태가 매우 안정적임.

2. RPM 산출물 확인
```text
yocto/build-rpi/tmp/deploy/rpm/cortexa7t2hf_neon_vfpv4/
  headunit-git-r0.cortexa7t2hf_neon_vfpv4.rpm
  headunit-dev-git-r0.cortexa7t2hf_neon_vfpv4.rpm
  headunit-dbg-git-r0.cortexa7t2hf_neon_vfpv4.rpm
  headunit-src-git-r0.cortexa7t2hf_neon_vfpv4.rpm
```
- 의미: 레시피 결과물이 패키지 피드(rpm)까지 정상 배포됨.

#### L) pkgdata 등록 검증 완료

1. 실행 명령
```bash
oe-pkgdata-util list-pkgs | grep headunit
```

2. 실제 출력
```text
headunit
headunit-dbg
headunit-dev
headunit-src
```

3. 의미
- 패키지 산출물뿐 아니라 pkgdata 인덱스에도 `headunit` 계열이 정상 등록됨.
- 이후 이미지 레시피에서 `IMAGE_INSTALL += "headunit"` 형태로 포함할 준비가 끝남.

#### M) Xen 지향 분리 구조 파일 추가 (Dom0 / DomU-HU / DomU-Cluster)

1. 추가한 앱 레시피
- `yocto/meta-piracer/recipes-cluster/instrument-cluster/instrument-cluster_git.bb`
- `yocto/meta-piracer/recipes-cluster/instrument-cluster/files/piracer-cluster.service`

핵심 포인트:
- `instrument_cluster`를 `externalsrc`로 연결
- `cmake_qt5` + `systemd` 상속
- 설치 대상: `PiRacerDashboard`, `python/*.py`, `config/*.json`, `piracer-cluster.service`

2. 추가한 이미지 레시피
- `yocto/meta-piracer/recipes-core/images/piracer-dom0-image.bb`
- `yocto/meta-piracer/recipes-core/images/piracer-hu-image.bb`
- `yocto/meta-piracer/recipes-core/images/piracer-cluster-image.bb`

3. 레시피 인식 검증
```bash
bitbake-layers show-recipes instrument-cluster piracer-hu-image piracer-cluster-image piracer-dom0-image
```
실제 결과:
- `instrument-cluster` 매칭 성공
- `piracer-dom0-image`, `piracer-hu-image`, `piracer-cluster-image` 매칭 성공

4. 의미
- Xen 목표 구조(도메인별 이미지 분리)를 Yocto 레이어에 반영할 기본 뼈대를 완료.
- 다음 단계는 각 이미지에 필요한 패키지(예: xen 관련, vsomeip, 정책 패키지)를 도메인별로 구체화하는 작업.

#### N) 이미지 빌드 의존성 에러 해결 (restricted license)

1. 발생 에러
```text
Nothing RPROVIDES 'linux-firmware-rpidistro-bcm43455'
... skipped: Has a restricted license 'synaptics-killswitch'
which is not listed in your LICENSE_FLAGS_ACCEPTED.
```

2. 원인
- `meta-raspberrypi`의 firmware 패키지가 제한 라이선스 플래그를 요구함.
- `piracer-hu-image` / `piracer-cluster-image`가 `packagegroup-base-extended` 경로로 해당 firmware에 의존.

3. 조치 (`build-rpi/conf/local.conf`)
```conf
LICENSE_FLAGS_ACCEPTED:append = " synaptics-killswitch"
```

4. 결과
- 이전의 `Nothing RPROVIDES linux-firmware-rpidistro-bcm43455` 의존성 해석 에러는 해소됨.

#### O) 커널 빌드 중 `PermissionError: /proc/self/uid_map` 에러

1. 발생 에러
```text
PermissionError: [Errno 1] Operation not permitted
... bb.utils.disable_network(...)
... with open("/proc/self/uid_map", "w")
```

2. 원인
- BitBake worker가 네트워크 격리를 위해 user namespace를 사용할 때, 호스트 OS 보안 정책(AppArmor + userns 제한)에 의해 차단됨.
- 증상상 `linux-raspberrypi:do_compile`에서 터졌지만, 실제 본질은 커널 레시피 자체 문제가 아니라 "호스트 권한/보안 정책" 문제.

3. 해결 절차 (런타임/영구)
```bash
# 1) 즉시 해제 (재부팅 전까지 유효)
sudo sysctl -w kernel.apparmor_restrict_unprivileged_userns=0

# 2) 확인
sysctl kernel.apparmor_restrict_unprivileged_userns
```

필요 시 영구 설정:
```bash
echo 'kernel.apparmor_restrict_unprivileged_userns = 0' | sudo tee /etc/sysctl.d/99-yocto-userns.conf
sudo sysctl --system
```

4. 재시도
```bash
source yocto/poky/oe-init-build-env yocto/build-rpi
bitbake piracer-hu-image
bitbake piracer-cluster-image
```

#### P) 재시도 최종 성공 + 이미지 산출물 확인

1. 재시도 로그 결과
- `bitbake piracer-hu-image`: `Tasks Summary ... all succeeded.`
- `bitbake piracer-cluster-image`: `Tasks Summary ... all succeeded.`

2. 참고할 점
- `source: no such file or directory: yocto/poky/oe-init-build-env` 메시지는 "현재 디렉터리 기준 상대경로" 문제였음.
- 직후 `bitbake` 명령이 정상 실행되었고, 결과도 성공이므로 최종 상태에는 영향 없음.

3. 실제 이미지 산출물 (raspberrypi4)
```text
yocto/build-rpi/tmp/deploy/images/raspberrypi4/
  piracer-hu-image-raspberrypi4.rootfs.wic.bz2
  piracer-hu-image-raspberrypi4.rootfs.ext3
  piracer-hu-image-raspberrypi4.rootfs.tar.bz2
  piracer-cluster-image-raspberrypi4.rootfs.wic.bz2
  piracer-cluster-image-raspberrypi4.rootfs.ext3
  piracer-cluster-image-raspberrypi4.rootfs.tar.bz2
  ... (manifest, spdx, bmap 등 부가 산출물 포함)
```

#### Q) 타겟 실행 시 Qt platform plugin 초기화 실패 대응

1. 현상
```text
no Qt platform plugin could be initialized
... aborted
```

2. 원인 추정
- 앱 바이너리는 존재하지만, 이미지에 Qt 플랫폼 플러그인 런타임 패키지가 빠져 있을 때 주로 발생.

3. 패키지 피드 확인
- 빌드 산출물에 `qtbase-plugins-...rpm` 패키지가 존재함을 확인.

4. 조치
- 이미지 레시피에 `qtbase-plugins`를 명시적으로 추가:
  - `piracer-hu-image.bb`
  - `piracer-cluster-image.bb`

5. 후속
- 이미지 재빌드/재플래시 후 `QT_QPA_PLATFORM=eglfs` 또는 `QT_QPA_PLATFORM=linuxfb`로 실행 재확인.

---

### 10.2 지금까지 배운 핵심 (실습 기반)

1. **Yocto 에러는 대부분 "경로/환경/레이어" 문제부터 본다.**
2. **노트북 교체 시 절대경로 하드코딩은 거의 100% 문제를 만든다.**
3. **`show-layers` -> `show-recipes` -> `-c fetch` 순서가 가장 안전한 디버깅 루틴이다.**
4. **QEMU용(`qemux86-64`)과 RPi용(`raspberrypi4`)은 목적이 다르다.**
   - QEMU: 빠른 개발/검증
   - RPi: 실제 배포 이미지

---

### 10.3 다음에 바로 할 작업 (실습 체크리스트)

1. `kernel.apparmor_restrict_unprivileged_userns` 값 확인/적용
2. `bitbake piracer-hu-image` / `bitbake piracer-cluster-image` 재실행
3. 이미지 산출물 확인 (`tmp/deploy/images/<machine>/`)
4. Dom0 이미지에 Xen/백엔드 패키지 정책 추가

---

### 10.4 실습 로그 템플릿 (앞으로 계속 추가)

```md
#### [YYYY-MM-DD] Step N - 작업 제목
- 목표:
- 명령어:
  - `...`
- 결과:
  - 성공/실패
  - 핵심 로그
- 배운 점:
- 다음 액션:
```