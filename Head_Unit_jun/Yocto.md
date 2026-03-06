# Yocto

## Open Embedded Architecture Workflow

Open Embedded / Yocto Project 빌드 프로세스의 전체 흐름을 나타낸 다이어그램입니다.

![Open Embedded Architecture Workflow](./open-embedded-architecture-workflow.png)

### 다이어그램 범례
| 색상 | 구분 | 설명 |
|------|------|------|
| 노란색 | Upstream Source | 원본 소스 코드 및 자료 |
| 회색 | Metadata/Inputs | 빌드에 필요한 메타데이터·설정 |
| 연한 파란색 | Build System | 빌드 작업 수행 환경 |
| 녹색 | Output Packages | 생성된 패키지 출력 |
| 진한 파란색 | Output Image Data | 최종 이미지·SDK |
| 흰색 | Process Steps | 개별 작업 단계 |

### 흐름 요약
1. **소스 자료**: Upstream Project Releases, Local Projects, SCM에서 소스 수집
2. **입력**: User Configuration, Meta(.bb + patches), Machine BSP, Policy Configuration
3. **빌드 단계**: Source Fetching → Patch Application → Config/Compile → 패키지 분석 → .deb/.rpm/.ipk 생성 → QA Tests
4. **출력**: Package Feeds → Image Generation / SDK Generation → Images, Application Development SDK

---

## Yocto Build Workflow (빌드 워크플로)

Yocto 프로젝트로 임베디드 리눅스 이미지를 빌드하는 단계별 워크플로입니다.

![Yocto Build Workflow](./yocto-build-workflow.png)

### 워크플로 7단계
| 단계 | 내용 | 관련 도구/파일 |
|------|------|----------------|
| 1 | **호스트 시스템 준비** | Set Up Your Host Development System |
| 2 | **소스 클론** | meta-intel 등 Git 저장소 클론 (git.yoctoproject.org) |
| 3 | **BSP 레이어 생성** | `bitbake-layers create-layer` |
| 4 | **설정 변경** | `bitbake-layers`, `bsp.conf`, `bblayers.conf` |
| 5 | **레시피 변경** | recipes-bsp, recipes-graphics, recipes-core, recipes-kernel |
| 6 | **빌드 준비** | `source oe-init-build-env`, `local.conf`, `bblayers.conf` |
| 7 | **이미지 빌드** | `bitbake target` |

---

## Yocto Recipe Workflow (레시피 워크플로)

레시피를 통해 소프트웨어를 빌드·패키징하는 12단계 흐름입니다.

![Yocto Recipe Workflow](./yocto-recipe-workflow.png)

### 12단계 요약
| 단계 | 내용 |
|------|------|
| 1 | **Establish the Recipe** - 레시피 정의 |
| 2 | **Fetch Source Files** - 소스 다운로드 |
| 3 | **Unpack Source Files** - 압축 해제 |
| 4 | **Patching Source Files** - 패치 적용 |
| 5 | **Add Licensing Information** - 라이선스 정보 추가 |
| 6 | **Add Configurations** - 빌드 설정 적용 |
| 7 | **Compilation** - 컴파일 |
| 8 | **Autotools/CMake?** - Yes → 다음 단계 / No → `do_install` 직접 작성 |
| 9 | **Need Supporting Services?** - Yes → 서비스 파일 설치 / No → 자체 설치 제공 |
| 10 | **Packaging** - 패키징 |
| 11 | **Provide Post-Installation Scripts** - 설치 후 스크립트 |
| 12 | **Perform Runtime Testing** - 런타임 테스트 |

> **참고**: 2~12단계는 **반복적**이다. `bitbake <basename>`으로 레시피를 반복 실행하며 정보를 확인·검증해야 한다.

---

## 1. Yocto Project 전체 구조

Yocto Project는 **소프트웨어가 아니라** 표준 + 문서 + 도구들의 **집합(개념)**이다. 실제로 빌드를 실행하는 것은 **BitBake**이다.

### 핵심 비유
| 개념 | 비유 |
|------|------|
| Yocto | 자동차 제조 표준(ISO 규격) |
| BitBake | 실제 공장 기계 |
| Layer | 설계도와 부품 목록 |

### 구성요소
| 구성요소 | 역할 |
|----------|------|
| **Yocto Project** | 표준, 문서, 도구 모음의 집합 (개념) |
| **BitBake** | 실제 빌드 엔진 (소프트웨어) |
| **OE-Core** | 공통 레시피 라이브러리 (공통 재료 창고) |
| **Poky** | OE-Core + 기준 설정 묶음 (레퍼런스 배포판) |
| **Layer** | 기능별로 분리된 레시피 모음 디렉터리 |

### Poky 클론 시 포함 내용
```bash
git clone https://git.yoctoproject.org/poky
```
```
poky/
├── meta/              ← OE-Core
├── meta-poky/         ← Poky 설정
├── meta-yocto-bsp/    ← 레퍼런스 BSP
└── bitbake/           ← BitBake
```

---

## 2. 레이어(Layer)란?

레이어는 **특정 목적의 빌드 레시피 모음 폴더**(디렉터리 자체)다. 레고 블록처럼 필요한 것만 조합해서 쓰는 구조다.

> **한 줄 정의**: Layer = 특정 목적의 메타데이터(레시피, 설정 파일)를 담고 있는 디렉터리

### 2.1 레이어를 나누는 이유
- 보드 바꿀 때 전부 뜯어고치지 않아도 됨
- 다른 프로젝트에서 재사용 가능
- 충돌 관리가 쉬움

### 2.2 레이어 구조 예시
```
meta/                  ← OE-Core (기본 Linux 툴)
meta-poky/             ← Poky 배포판
meta-raspberrypi/      ← BSP 레이어 (보드 전용)
meta-qt6/              ← Qt 프레임워크
meta-myapp/            ← 내 앱 레이어
```

이 레이어들을 `bblayers.conf`에 등록하면, BitBake가 전부 합쳐서 하나의 이미지를 만든다. **나중 레이어가 앞 레이어의 내용을 덮어쓸 수 있어서** 진짜 층층이 쌓이는 구조다.

### 2.3 IVI 앱 레이어 구성 예시
앱 하나하나마다 레이어를 만들지 않는다. **기능 카테고리 단위**로 나눈다:
```
meta-bsp/              ← 하드웨어 (보드, 드라이버)
meta-qt/               ← Qt 프레임워크
meta-ivi/              ← IVI 앱 전체 (앰비언트, 콜, 맵, 유튜브 등)
└── recipes-apps/
    ├── ambient-light/ambient-light.bb
    ├── call/call.bb
    ├── map/map.bb
    └── youtube/youtube.bb
```

---

## 3. BSP (Board Support Package) 레이어

BSP는 **'이 하드웨어에서 Linux 켜는 법'**을 담은 레이어다.

> **레고 비유**: BSP = 특정 보드 전용 설명서 + 부품 주문 목록. 실제 블록(부품)은 BitBake가 빌드할 때 가져온다.

### 3.1 BSP 레이어 디렉터리 구조
```
meta-mybsp/
├── conf/
│   ├── layer.conf           ← 레이어 선언
│   └── machine/
│       └── myboard.conf     ← ★ 핵심: 하드웨어 스펙 정의
├── recipes-bsp/
│   └── u-boot/              ← 부트로더
├── recipes-kernel/
│   └── linux/               ← 커널 설정
└── recipes-core/            ← 보드 전용 패키지
```

| 구성요소 | 역할 |
|----------|------|
| `conf/machine/myboard.conf` | CPU 아키텍처, 이미지 포맷 등 보드 스펙 정의 (핵심) |
| `recipes-bsp/` | 부트로더 (어떻게 켤지) |
| `recipes-kernel/` | 커널 (어떤 드라이버 쓸지) |

---

## 4. 레이어 생성 및 활성화

### 4.1 레이어 생성
```bash
bitbake-layers create-layer meta-mylayer
```
이 명령 한 줄로 레이어 뼈대가 자동 생성된다. 이름 앞에 `meta-` 붙이는 게 규칙이다.

**자동 생성 구조:**
```
meta-mylayer/
├── conf/
│   └── layer.conf           ← 레이어 설정 파일
├── recipes-example/
│   └── example/
│       └── example.bb
├── COPYING.MIT
└── README
```

### 4.2 layer.conf 핵심 변수
| 변수 | 역할 |
|------|------|
| `BBPATH` | 이 레이어 디렉터리를 BitBake 검색 경로에 추가 |
| `BBFILES` | 이 레이어 안의 레시피 파일 위치 정의 |
| `BBFILE_COLLECTIONS` | 이 레이어의 고유 식별자 이름 |
| `BBFILE_PRIORITY` | 레이어 우선순위 (높을수록 우선) |
| `LAYERVERSION` | 레이어 버전 번호 |
| `LAYERDEPENDS` | 이 레이어가 필요로 하는 다른 레이어 |
| `LAYERSERIES_COMPAT` | 호환되는 Yocto 버전 선언 |

### 4.3 레이어 활성화 (bblayers.conf)
레이어 만들어도 `bblayers.conf`에 등록 안 하면 BitBake가 존재 자체를 모른다.

```python
BBLAYERS ?= " \
  /home/user/poky/meta \
  /home/user/poky/meta-poky \
  /home/user/mystuff/meta-mylayer \
"
```

**명령어로도 가능:**
```bash
bitbake-layers add-layer /home/user/mystuff/meta-mylayer
```

BitBake는 `bblayers.conf`에 지정된 **순서대로** 위에서 아래로 레이어를 파싱한다.

---

## 5. 레시피 (.bb 파일)

BitBake한테 **'이 소프트웨어를 어떻게 빌드할지'** 알려주는 파일이다.

### 5.1 파일 이름 규칙
```
wget_1.21.3.bb
└── 패키지이름_버전
```

### 5.2 레시피 구성요소

**① 변수 - 재료와 설정**
```python
DESCRIPTION = "wget은 HTTP/FTP 다운로드 툴"
SRC_URI = "https://ftp.gnu.org/wget-${PV}.tar.gz"   # 소스 위치
DEPENDS = "openssl"   # 빌드 타임 의존성
RDEPENDS = "libc"    # 런타임 의존성
```

**② 태스크 함수 - 조리 순서**
```python
do_configure() {
    ./configure --prefix=/usr
}
do_compile() {
    oe_runmake
}
do_install() {
    install -m 0755 wget ${D}${bindir}/wget
}
```

**③ 상속 - 공통 템플릿**
```python
inherit autotools    # configure, make, make install 자동 처리
inherit pkgconfig
```

### 5.3 .bbappend - 원본 건드리지 않고 수정
원본 레시피를 복사하거나 수정하지 않고, **내가 원하는 내용만 얹는** 파일이다.

> **핵심 원칙**: 원본 .bb는 절대 수정하지 않는다. .bbappend로 변경할 부분만 덮어쓴다.

```python
# meta-myboard/recipes-core/wget/wget_1.21.3.bbappend
SRC_URI += "file://my-fix.patch"
```

BitBake가 빌드할 때 .bb와 .bbappend를 **합쳐서** 읽는다.

**파일 이름은 원본과 버전까지 일치**해야 한다. 버전 무관하게 적용하려면 `%` 와일드카드:
- `someapp_6.%.bbappend` ← 6.x 버전 전부 적용
- `someapp_%.bbappend` ← 모든 버전 적용

---

## 6. BitBake 빌드 엔진

BitBake는 레시피(.bb 파일)들을 읽고 **실제로 빌드를 실행하는** 프로그램이다.

> **악보 비유**: BSP = 악보(선언), BitBake = 연주자(실행)

### 6.1 태스크 파이프라인
| 태스크 | 역할 |
|--------|------|
| `do_fetch` | SRC_URI 기반 소스 다운로드 |
| `do_unpack` | 압축 해제 |
| `do_patch` | 패치 적용 |
| `do_configure` | configure 스크립트 실행 |
| `do_compile` | make 실행 |
| `do_install` | 임시 디렉터리(${D})에 설치 |
| `do_package` | .ipk/.rpm/.deb 생성 |
| `do_image` | 최종 이미지 생성 |

### 6.2 DAG (의존성 그래프)
빌드 전에 모든 레시피의 `DEPENDS` 변수를 읽어서 **의존성 지도(DAG)**를 만든다. 이 지도로 빌드 순서를 결정하고, 의존성 없는 것들을 **병렬 실행**한다.

```
bash → glibc → linux-headers
openssl → perl   (linux-headers와 perl은 서로 의존성 없음 → 동시 실행 가능)
```

### 6.3 해시 기반 캐싱 (sstate-cache)
태스크 입력값(소스, 레시피, 환경변수, 의존성)의 **해시가 같으면** 재빌드하지 않고 캐시에서 꺼내 쓴다.

- **태스크 해시** = 해시(소스코드 + 레시피 내용 + 의존성 해시 + 환경변수)
- 소스 하나 수정하면 그 이후 태스크들만 연쇄적으로 재실행
- 캐시는 다른 프로젝트끼리도 공유 가능

### 6.4 병렬 태스크 실행
```python
# local.conf
BB_NUMBER_THREADS = "8"    # 동시에 실행할 레시피 수
PARALLEL_MAKE = "-j8"     # make -j8 과 동일
```

**DAG + 캐싱 + 병렬 실행**이 합쳐져서 Yocto 빌드가 효율적으로 동작한다.

---

## 7. 크로스 컴파일

내가 쓰는 컴퓨터(x86)와 **다른 환경(ARM)용으로** 빌드하는 것이다. Yocto는 `MACHINE` 변수 지정만 하면 크로스 컴파일러 툴체인 생성부터 이미지 패키징까지 **자동으로** 처리한다.

```python
# local.conf
MACHINE = "raspberrypi4"
```

**빌드 결과물:**
```
build/tmp/deploy/images/raspberrypi4/
├── core-image-minimal.ext4    ← rootfs
├── core-image-minimal.wic     ← SD카드 이미지
├── zImage                      ← 커널
└── bcm2711-rpi-4-b.dtb        ← Device Tree
```

---

## 8. 오버라이드 문법

특정 보드나 조건일 때만 변수를 다르게 적용하는 문법이다.

```python
DEPENDS = "foo"           # 모든 보드에 적용
DEPENDS:one = "foo"       # 'one' 보드에서만 적용

CFLAGS:append = " -Wall"   # 뒤에 추가
CFLAGS:prepend = "-O2 "    # 앞에 추가
CFLAGS:remove = "-O2"      # 제거
```

---

## 9. SDK

Yocto로 OS를 만들었다면, 그 OS 위에서 돌아갈 앱도 **ARM용으로 크로스 컴파일**해야 한다. SDK는 그걸 쉽게 해주는 도구 모음이다.

> **역할 분리**: Yocto = OS 자체를 만드는 것, SDK = 그 OS 위에서 돌아갈 앱을 만들 때 쓰는 도구

```bash
# SDK 생성
bitbake core-image-minimal -c populate_sdk

# SDK 환경 활성화
source /opt/poky/4.0/environment-setup-armv8a-poky-linux

# 이후 일반 make하면 자동으로 ARM용으로 컴파일됨
$CC hello.c -o hello
```

| 방법 | 용도 |
|------|------|
| **SDK로 앱 개발** | 이미지 건드리지 않고 앱만 따로 올림 (개발/테스트) |
| **레시피로 이미지에 포함** | 이미지 자체에 앱이 내장됨 (배포용) |

---

## 10. 레이어 관리 모범 사례

- 원본 레시피 절대 복사 금지 → **.bbappend 사용**
- 머신별 조건부 설정 → **:머신명 오버라이드** 사용
- 머신별 파일은 머신별 폴더에 분리 배치
- 레이어 이름은 **meta-** 접두사 사용
- `layer.conf`의 전역 설정은 조건부로 적용

### 10.1 레이어 설정 저장 및 복원
빌드 환경을 팀원과 공유하거나 다른 시스템에서 재현할 때 사용한다. (npm의 package.json + npm install과 동일한 개념)

```bash
# 저장
bitbake-layers create-layers-setup /내경로/
→ setup-layers.json (레이어 목록, 버전, 출처 기록)
→ setup-layers (복원 스크립트)

# 복원
meta-mylayer/setup-layers
```