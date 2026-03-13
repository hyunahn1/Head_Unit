#!/bin/bash
# Yocto 환경 자동 구성 스크립트
# 사용법: bash setup.sh

set -e
SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
cd "$SCRIPT_DIR"

echo "=== Yocto Scarthgap (5.0) 환경 구성 시작 ==="

# poky (Yocto 빌드 엔진)
if [ ! -d "poky/.git" ]; then
    echo "[1/4] poky 클론 중..."
    git clone git://git.yoctoproject.org/poky
    git -C poky checkout 7d50718f90
else
    echo "[1/4] poky 이미 존재 (skip)"
fi

# meta-qt5
if [ ! -d "meta-qt5/.git" ]; then
    echo "[2/4] meta-qt5 클론 중..."
    git clone https://github.com/meta-qt5/meta-qt5.git
    git -C meta-qt5 checkout 310a5459
else
    echo "[2/4] meta-qt5 이미 존재 (skip)"
fi

# meta-raspberrypi
if [ ! -d "meta-raspberrypi/.git" ]; then
    echo "[3/4] meta-raspberrypi 클론 중..."
    git clone git://git.yoctoproject.org/meta-raspberrypi
    git -C meta-raspberrypi checkout 2c646d29912d
else
    echo "[3/4] meta-raspberrypi 이미 존재 (skip)"
fi

# meta-openembedded
if [ ! -d "meta-openembedded/.git" ]; then
    echo "[4/4] meta-openembedded 클론 중..."
    git clone git://git.openembedded.org/meta-openembedded
    git -C meta-openembedded checkout 4d3e2639dec5
else
    echo "[4/4] meta-openembedded 이미 존재 (skip)"
fi

echo ""
echo "=== 완료! 다음 명령으로 빌드 시작 ==="
echo ""
echo "  # Docker 환경에서 실행 (Ubuntu 22.04 필요)"
echo "  docker run -it --rm \\"
echo "    -v \$(pwd):/home/seame/SEA_ME/Head_Unit/yocto \\"
echo "    -w /home/seame/SEA_ME/Head_Unit/yocto \\"
echo "    yocto-builder:ubuntu22"
echo ""
echo "  # 컨테이너 안에서"
echo "  source poky/oe-init-build-env build-rpi"
echo "  bitbake headunit"
