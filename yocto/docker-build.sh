#!/bin/bash
# =============================================================================
# PiRacer Head Unit - Yocto 빌드 스크립트 (Docker/Ubuntu 22.04)
# 사용법:
#   ./docker-build.sh [TARGET]
#
# TARGET 예시:
#   piracer-hu-image        (기본값) Head Unit 이미지
#   piracer-cluster-image   Instrument Cluster 이미지
#   headunit                headunit 패키지만 빌드
#
# 환경 변수:
#   BB_NUMBER_THREADS   BitBake 병렬 스레드 수 (기본: 호스트 코어 수)
#   SKIP_DOCKER_BUILD   1로 설정하면 Docker 이미지 재빌드 생략
# =============================================================================
set -e

# ── 경로 설정 ────────────────────────────────────────────────────────────────
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO_ROOT="$(dirname "${SCRIPT_DIR}")"
BUILD_DIR="${SCRIPT_DIR}/build-rpi"
DOCKER_IMAGE="piracer-yocto-builder:ubuntu22"

TARGET="${1:-piracer-hu-image}"

# ── 색상 출력 ────────────────────────────────────────────────────────────────
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m'

log_info()  { echo -e "${BLUE}[INFO]${NC}  $*"; }
log_ok()    { echo -e "${GREEN}[OK]${NC}    $*"; }
log_warn()  { echo -e "${YELLOW}[WARN]${NC}  $*"; }
log_error() { echo -e "${RED}[ERROR]${NC} $*"; }

# ── 사전 검사 ────────────────────────────────────────────────────────────────
echo "============================================================"
echo "  PiRacer Yocto Build  |  Target: ${TARGET}"
echo "============================================================"

# Docker 설치 확인
if ! command -v docker &>/dev/null; then
    log_error "Docker가 설치되어 있지 않습니다."
    exit 1
fi

# 디스크 공간 검사 (최소 80GB 필요)
AVAIL_GB=$(df -BG "${REPO_ROOT}" | awk 'NR==2{gsub(/G/,"",$4); print $4}')
if [ "${AVAIL_GB}" -lt 80 ]; then
    log_warn "디스크 여유 공간이 ${AVAIL_GB}GB입니다. Yocto 빌드는 ~100GB 필요합니다."
fi
log_info "사용 가능한 디스크: ${AVAIL_GB}GB"

# bblayers.conf 경로 검사 (하드코딩된 절대 경로 확인)
BBLAYERS="${BUILD_DIR}/conf/bblayers.conf"
if grep -q "/home/seame/SEA_ME/Head_Unit" "${BBLAYERS}" 2>/dev/null; then
    if [ "${REPO_ROOT}" != "/home/seame/SEA_ME/Head_Unit" ]; then
        log_warn "bblayers.conf의 경로가 현재 위치와 다릅니다!"
        log_warn "  bblayers.conf 내부: /home/seame/SEA_ME/Head_Unit"
        log_warn "  현재 REPO_ROOT:    ${REPO_ROOT}"
        log_warn "Docker 컨테이너에서 동일 경로로 마운트하여 해결합니다."
    fi
fi

# ── Docker 이미지 빌드 ───────────────────────────────────────────────────────
if [ "${SKIP_DOCKER_BUILD}" != "1" ]; then
    log_info "Docker 이미지 빌드 중: ${DOCKER_IMAGE}"
    docker build \
        --build-arg UID="$(id -u)" \
        --build-arg GID="$(id -g)" \
        --build-arg USERNAME="yocto" \
        -t "${DOCKER_IMAGE}" \
        "${SCRIPT_DIR}"
    log_ok "Docker 이미지 준비 완료"
else
    log_info "Docker 이미지 빌드 생략 (SKIP_DOCKER_BUILD=1)"
fi

# ── BitBake 빌드 명령 ────────────────────────────────────────────────────────
# bblayers.conf가 /home/seame/SEA_ME/Head_Unit를 참조하므로
# 컨테이너 내부에서 동일 경로로 마운트 필요
MOUNT_TARGET="/home/seame/SEA_ME/Head_Unit"

log_info "Docker 컨테이너 내부에서 BitBake 실행"
log_info "  빌드 대상: ${TARGET}"
log_info "  마운트:    ${REPO_ROOT} → ${MOUNT_TARGET}"

docker run --rm \
    --name "piracer-yocto-build-$$" \
    -v "${REPO_ROOT}:${MOUNT_TARGET}" \
    -e HOME="/home/yocto" \
    -e USER="yocto" \
    -e BB_ENV_PASSTHROUGH_ADDITIONS="BB_NUMBER_THREADS PARALLEL_MAKE" \
    -e BB_NUMBER_THREADS="${BB_NUMBER_THREADS:-$(nproc)}" \
    -e PARALLEL_MAKE="-j${BB_NUMBER_THREADS:-$(nproc)}" \
    "${DOCKER_IMAGE}" \
    bash -c "
        set -e
        cd ${MOUNT_TARGET}/yocto

        echo '[Docker] oe-init-build-env 초기화 중...'
        source poky/oe-init-build-env build-rpi

        # BB_NUMBER_THREADS 환경 변수로 재정의 가능
        if [ -n \"\$BB_NUMBER_THREADS\" ]; then
            export BB_NUMBER_THREADS
            echo '[Docker] BB_NUMBER_THREADS=${BB_NUMBER_THREADS}'
        fi

        echo '[Docker] BitBake 시작: ${TARGET}'
        bitbake ${TARGET}
        echo '[Docker] 빌드 완료!'
    "

# ── 빌드 결과 확인 ───────────────────────────────────────────────────────────
DEPLOY_DIR="${BUILD_DIR}/tmp/deploy/images/raspberrypi4-64"
if [ -d "${DEPLOY_DIR}" ]; then
    log_ok "빌드 완료! 이미지 파일:"
    ls -lh "${DEPLOY_DIR}"/*.wic* 2>/dev/null || \
    ls -lh "${DEPLOY_DIR}"/*.rootfs.* 2>/dev/null || \
    ls -lh "${DEPLOY_DIR}"/ | head -20
    echo ""
    log_ok "SD 카드 플래시: ./flash.sh <SD카드 장치>"
    log_ok "예시: ./flash.sh /dev/sdb"
else
    log_warn "deploy 디렉토리를 찾을 수 없습니다: ${DEPLOY_DIR}"
fi
