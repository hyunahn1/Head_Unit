# PiRacer Head Unit — 로드맵 & 목표 아키텍처

> **진행 단계**: Phase 1 (Yocto 배포) → Phase 2 (OTA) → Phase 3 (SDV)

---

## 현재 구현 상태 (as-is)

```
┌──────────────────────────────────────────────────────────┐
│                   hu_shell (단일 프로세스)                  │
│                                                          │
│  ┌────────────────────────────────────────────────────┐  │
│  │                  ShellWindow                       │  │
│  │  ┌──────┐  ┌────────────────────────────────────┐  │  │
│  │  │TabBar│  │         QStackedWidget             │  │  │
│  │  │  ♪   │  │  ┌────────┬────────┬──────────┐   │  │  │
│  │  │  ▶   │  │  │ Media  │YouTube │Settings  │...│  │  │
│  │  │  ☎   │  │  │Widget  │Widget  │Widget    │   │  │  │
│  │  │  ⊕   │  │  └────────┴────────┴──────────┘   │  │  │
│  │  │  ★   │  └────────────────────────────────────┘  │  │
│  │  │  ⚙   │                                          │  │
│  │  └──────┘  ┌──────────────────────────────────────┐  │  │
│  │            │  StatusBar (속도/기어/IPC 상태)         │  │  │
│  │            └──────────────────────────────────────┘  │  │
│  └────────────────────────────────────────────────────┘  │
│                                                          │
│  GearStateManager ── VSomeIPClient ── MockVehicleData    │
└──────────────────────────────────────────────────────────┘
         ↕ VSomeIP (미완성)
┌─────────────────────┐
│  Instrument Cluster  │
│  (별도 프로세스/VM)   │
└─────────────────────┘
```

**현재 상태 평가:**
- ✅ 단일 프로세스 Qt: 안정적, Wayland/eglfs 호환
- ✅ QStackedWidget 탭 전환: 동작 확인
- ✅ 화면 크기 자동 적응 (primaryScreen 감지)
- ⚠️ VSomeIP: 클라이언트 코드 있으나 실제 IPC 미완성
- ❌ OTA 메커니즘 없음
- ❌ UI / 서비스 레이어 미분리

---

## Phase 1: Yocto 빌드 & Pi 배포

### 목표
`bitbake`로 ARM 바이너리를 빌드하여 Raspberry Pi에서 동작 확인

### 배포 아키텍처

```
┌──────────────────────────────────────────────────┐
│               Raspberry Pi 4                     │
│                                                  │
│  ┌────────────┐         ┌────────────────────┐   │
│  │  Yocto OS  │         │   hu_shell (ARM)   │   │
│  │  (poky +   │  실행   │   Qt5 + eglfs      │   │
│  │  meta-qt5) │ ──────► │   DSI Display      │   │
│  └────────────┘         └────────────────────┘   │
│                                                  │
│  systemd: hu-shell.service (자동 시작)            │
└──────────────────────────────────────────────────┘
```

### Yocto 레이어 구조

```
yocto/
├── poky/                     # Yocto 기반
├── meta-qt5/                 # Qt5 크로스컴파일
├── meta-raspberrypi/         # RPi4 BSP
├── meta-openembedded/        # 추가 패키지
└── meta-piracer/             # 프로젝트 전용 레이어
    ├── recipes-core/
    │   └── images/
    │       └── piracer-hu-image.bb    # 이미지 정의
    └── recipes-hu/
        └── headunit/
            └── headunit_git.bb        # externalsrc 빌드
```

### 빌드 명령

```bash
cd /home/seame/SEA_ME/Head_Unit/yocto
source poky/oe-init-build-env build-rpi

# 앱만 빌드 (소스 변경 시)
bitbake headunit

# 전체 이미지 빌드 (초기 또는 OS 변경 시)
bitbake piracer-hu-image

# SD카드에 이미지 굽기 (예: /dev/sdb)
sudo dd if=tmp/deploy/images/raspberrypi4/piracer-hu-image-raspberrypi4.wic \
         of=/dev/sdb bs=4M status=progress && sync
```

### 검증에서 발견된 수정 사항 (✅ 반영 완료)

| 파일 | 문제 | 수정 |
|---|---|---|
| `hu-shell.service` | `DISPLAY=:0` → Pi에 X11 없어 xcb 실패 | `QT_QPA_PLATFORM=eglfs` 로 교체 |
| `headunit_git.bb` | `DEPENDS`에 qtmultimedia 누락 | `qtbase qtmultimedia qtnetwork` 추가 |
| `piracer-hu-image.bb` | 런타임 멀티미디어 플러그인 없음 | `qtmultimedia-plugins` 추가 |

### Phase 1 체크리스트
- [ ] `bitbake headunit` 빌드 성공
- [ ] ARM 바이너리 SD카드에 배포
- [ ] Pi 부팅 후 `hu_shell` 자동 시작 확인 (systemd)
- [ ] 터치/마우스 입력 동작 확인 (eglfs + evdev)
- [ ] 화면 해상도 자동 적응 확인 (1200×??? 디스플레이)
- [ ] VSomeIP 연결 (Instrument Cluster와 기어/속도 통신)

---

## Phase 2: OTA (Over-The-Air) 업데이트

### 목표
네트워크를 통해 Pi에 설치된 앱을 원격으로 업데이트

### OTA 아키텍처

```
┌─────────────────┐    HTTPS    ┌──────────────────────────┐
│  Update Server  │ ──────────► │    Raspberry Pi          │
│  (GitHub/S3/    │             │                          │
│   로컬 서버)    │             │  ┌────────────────────┐  │
└─────────────────┘             │  │   SWUpdate         │  │
                                │  │  (업데이트 에이전트) │  │
                                │  └─────────┬──────────┘  │
                                │            │             │
                                │  ┌─────────▼──────────┐  │
                                │  │  A/B 파티션 구조    │  │
                                │  │  Slot A (현재)     │  │
                                │  │  Slot B (업데이트) │  │
                                │  └────────────────────┘  │
                                └──────────────────────────┘
```

### 권장 OTA 방식: SWUpdate + Yocto

**Yocto에 추가할 레이어:**

```
meta-piracer/
└── recipes-core/
    └── images/
        └── piracer-hu-image.bb  ← SWUpdate 추가
```

```bitbake
# piracer-hu-image.bb에 추가
IMAGE_INSTALL:append = " \
    headunit \
    qtbase-plugins \
    swupdate \          ← 추가
    swupdate-www \      ← 웹 UI (선택)
"
```

**업데이트 패키지 구조:**

```
update.swu
├── sw-description    # 업데이트 메타데이터
├── sw-description.sig # 서명 (보안)
└── headunit.tar.gz   # 새 바이너리
```

**sw-description 예시:**

```
software =
{
    version = "2.1.0";
    hardware-compatibility = ["1.0"];

    images: (
        {
            filename = "headunit.tar.gz";
            type = "shellscript";
            installed-directly = true;
        }
    );
}
```

### A/B 파티션 전략

```
SD카드 레이아웃:
┌──────────┬──────────┬──────────┬──────────┐
│  boot    │  rootfsA │  rootfsB │  data    │
│  (FAT)   │  (현재)  │  (백업)  │  (유지)  │
│  ~128MB  │  ~512MB  │  ~512MB  │  나머지  │
└──────────┴──────────┴──────────┴──────────┘
```

- 업데이트 실패 시 Slot A로 자동 롤백
- 부트로더(U-Boot)가 파티션 전환 관리

### Phase 2 체크리스트
- [ ] Yocto 이미지에 SWUpdate 통합
- [ ] A/B 파티션 구조 구성
- [ ] 업데이트 패키지 서명 (보안)
- [ ] 원격 업데이트 트리거 메커니즘
- [ ] 롤백 시나리오 검증
- [ ] 네트워크 연결 상태 확인 로직

---

## Phase 3: SDV (Software Defined Vehicle) 아키텍처

### 목표
Xen 하이퍼바이저 기반 도메인 분리 + 서비스 지향 아키텍처

### 전체 시스템 구조

```
┌───────────────────────────────────────────────────────────────────┐
│                      Xen Hypervisor                               │
├──────────────┬──────────────────────┬────────────────────────────┤
│    Dom0      │    DomU1 (HU VM)     │    DomU2 (Cluster VM)      │
│              │                      │                             │
│  device      │  ┌────────────────┐  │  ┌─────────────────────┐  │
│  drivers     │  │  hu_shell      │  │  │  instrument_cluster │  │
│  virtio      │  │  (Qt5 UI)      │  │  │  (Qt5)              │  │
│  xenbus      │  └───────┬────────┘  │  └──────────┬──────────┘  │
│              │          │ VSomeIP   │             │ VSomeIP      │
│              │  ┌───────▼────────┐  │  ┌──────────▼──────────┐  │
│              │  │  HU Service    │  │  │  Vehicle Service     │  │
│              │  │  (비즈니스로직) │  │  │  (속도/기어/배터리)  │  │
│              │  └───────┬────────┘  │  └──────────┬──────────┘  │
└──────────────┴──────────┼───────────┴─────────────┼─────────────┘
                          │    virtio network         │
                          └───────────────────────────┘
                                    VSomeIP
```

### 권장 HU 내부 레이어 구조

```
hu_shell (DomU1 내부)
│
├── UI 레이어 (Qt Widgets/QML)
│   ├── ShellWindow
│   ├── MediaWidget
│   ├── SettingsWidget
│   └── ...
│       ↕ Qt Signal/Slot
│
├── Service 레이어 (비즈니스 로직)
│   ├── VehicleService      ← 속도/기어 상태 관리
│   ├── MediaService        ← 미디어 재생 로직
│   ├── AmbientService      ← LED 제어 로직
│   └── SettingsService     ← 설정 관리
│       ↕ VSomeIP / DDS
│
└── IPC 레이어
    ├── VSomeIPClient       ← DomU 간 통신
    └── VehicleDataProvider ← 차량 데이터 추상화
```

### 현재 코드 → SDV 전환 방향

```cpp
// ❌ 현재: UI가 직접 비즈니스 로직 참조
class MediaWindow : public QWidget {
    MediaPlayer *m_player;  // UI가 직접 플레이어 제어
};

// ✅ 권장: UI는 서비스 인터페이스만 사용
class IMediaService {
    virtual void play() = 0;
    virtual void pause() = 0;
    virtual void setSource(const QUrl &url) = 0;
signals:
    virtual void stateChanged(bool playing) = 0;
};

class MediaWindow : public QWidget {
    IMediaService *m_service;  // 인터페이스만 참조
};
```

### VSomeIP 서비스 정의 (확장)

| Service ID | 이름 | Method/Event | 방향 | 용도 |
|---|---|---|---|---|
| 0x1234 | VehicleData | speed (0x01) | Cluster→HU | 속도 |
| 0x1234 | VehicleData | gear (0x02) | Bidirectional | 기어 상태 |
| 0x1234 | VehicleData | battery (0x03) | Cluster→HU | 배터리 |
| 0x1235 | MediaControl | play (0x01) | HU내부 | 미디어 재생 |
| 0x1236 | AmbientCtrl | setColor (0x01) | HU→LED | LED 색상 |
| 0x1237 | OTAControl | checkUpdate (0x01) | Server→HU | 업데이트 확인 |

### Phase 3 체크리스트
- [ ] Xen Dom0/DomU 이미지 분리 (`piracer-dom0-image.bb` 완성)
- [ ] virtio network 설정 (DomU 간 통신)
- [ ] UI / Service 레이어 코드 분리
- [ ] VSomeIP 서비스 인터페이스 정의
- [ ] AUTOSAR Adaptive 호환 고려 (장기)
- [ ] 보안: 코드 서명, 도메인 간 접근 제어

---

## 단계별 우선순위 요약

```
지금 해야 할 것:
  Phase 1 ──► bitbake headunit
              SD카드 배포 및 Pi 동작 확인
              VSomeIP IPC 완성

  Phase 2 ──► SWUpdate Yocto 레이어 추가
              A/B 파티션 구성
              OTA 테스트

  Phase 3 ──► Xen Dom0/DomU 분리 완성
              UI/Service 레이어 분리
              VSomeIP 서비스 확장
```

---

## 아키텍처 검증 체크리스트

### 설계 원칙 준수 여부

| 원칙 | 현재 | Phase 1 | Phase 2 | Phase 3 |
|---|---|---|---|---|
| 단일 책임 (SRP) | ⚠️ 부분 | ⚠️ | ⚠️ | ✅ |
| 의존성 역전 (DIP) | ❌ | ❌ | ⚠️ | ✅ |
| 인터페이스 분리 | ⚠️ 부분 | ⚠️ | ⚠️ | ✅ |
| 도메인 격리 | ❌ | ❌ | ❌ | ✅ |
| 무중단 업데이트 | ❌ | ❌ | ✅ | ✅ |

### 성능 기준

| 항목 | 목표 | 측정 방법 |
|---|---|---|
| 앱 시작 시간 | < 3초 | systemd 부팅 → 화면 표시 |
| 탭 전환 | < 100ms | QElapsedTimer |
| OTA 적용 시간 | < 2분 | SWUpdate 로그 |
| 메모리 사용 | < 256MB | `/proc/meminfo` |

---

*최종 수정: 2026-03-11*
*현재 단계: Phase 1 진행 중*
