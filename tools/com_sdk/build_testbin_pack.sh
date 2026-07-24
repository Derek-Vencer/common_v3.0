#!/bin/bash
# =============================================================================
# Batch-build 7 platforms (lst/map/bin + OTA bin) and copy artifacts per
# platform into ./comm_v3_sdk_test/<plat>/
#
# Platforms : 1306p / 1306 / 1502p / 1502x / 1503 / 1307p / 1307ph
# Usage     : run from the SDK root
#     ./tools/com_sdk/build_testbin_pack.sh            # build all 7 platforms
#     ./tools/com_sdk/build_testbin_pack.sh 1306p      # build a single platform
#     SKIP_BUILD=1 ./tools/com_sdk/build_testbin_pack.sh   # skip build, only copy existing artifacts
#
# Error policy: if any platform fails to build, print the key error excerpt
# and stop the whole script immediately (do NOT continue with later platforms).
# =============================================================================
rm -rf out/*
rm -rf comm_v3_sdk_test/*
set -u
set -o pipefail   # important: make `cmd | tee log` propagate cmd's exit code,
                  # otherwise tee always returns 0 and swallows make failures

# ----------------------------- Paths & basic config --------------------------
SCRIPT_PATH="$(readlink -f "$0" 2>/dev/null || echo "$0")"
SDK_ROOT="$(cd "$(dirname "$SCRIPT_PATH")/../.." && pwd)"
cd "$SDK_ROOT"

OUT_BASE="$SDK_ROOT/comm_v3_sdk_test"
BUILD_CMD="$SDK_ROOT/tools/com_sdk/build_cust_cmd.sh"
LOG_DIR="$OUT_BASE/_build_logs"

# Short platform name -> CHIPID accepted by build_cust_cmd.sh (with "best" prefix)
PLATFORMS=("1306p" "1306" "1502p" "1502x" "1503" "1307p" "1307ph")
declare -A CHIPID_MAP=(
    ["1306p"]="best1306p"
    ["1306"]="best1306"
    ["1502p"]="best1502p"
    ["1502x"]="best1502x"
    ["1503"]="best1503"
    ["1307p"]="best1307p"
    ["1307ph"]="best1307ph"
)

# ---------------------------- Toolchain sanity check ------------------------
ARM_TOOLCHAIN_DIR="/usr/bin/gcc-arm-none-eabi-10.3-2021.10/bin"
if ! command -v arm-none-eabi-gcc >/dev/null 2>&1; then
    if [ -x "$ARM_TOOLCHAIN_DIR/arm-none-eabi-gcc" ]; then
        export PATH="$ARM_TOOLCHAIN_DIR:$PATH"
        echo "[env] arm-none-eabi toolchain added to PATH: $ARM_TOOLCHAIN_DIR"
    else
        echo "[env][FATAL] arm-none-eabi-gcc not found, please install or set PATH manually"
        exit 1
    fi
fi
arm-none-eabi-gcc --version | head -1

if ! command -v make >/dev/null 2>&1; then
    echo "[env][FATAL] make not found"
    exit 1
fi

if [ ! -x "$BUILD_CMD" ]; then
    chmod +x "$BUILD_CMD" 2>/dev/null || true
fi

# Output directory must be writable; otherwise tee would silently drop logs
if ! mkdir -p "$LOG_DIR"; then
    echo "[FATAL] cannot create log dir: $LOG_DIR (check whether SDK_ROOT='$SDK_ROOT' is resolved correctly)"
    exit 1
fi
if [ ! -w "$LOG_DIR" ]; then
    echo "[FATAL] log dir is not writable: $LOG_DIR"
    exit 1
fi

# ---------------------------- Helpers ---------------------------------------
ts() { date '+%H:%M:%S'; }

# Pretty-print build error: extract the most useful error/Error/undefined-reference lines from the log
report_build_error() {
    local plat="$1"
    local stage="$2"
    local logf="$3"
    echo
    echo "################################################################"
    echo "## [BUILD ERROR] platform=$plat  stage=$stage"
    echo "## log file   : $logf"
    echo "################################################################"
    if [ -f "$logf" ]; then
        echo "---- key errors (up to 40 lines) ----"
        grep -nE "error:|Error |undefined reference|recipe for target|\*\*\* |No rule to make target|fatal error" "$logf" 2>/dev/null | tail -40 || true
        echo "---- last 30 lines of log ----"
        tail -30 "$logf"
    else
        echo "(log file does not exist: $logf)"
    fi
    echo "################################################################"
    echo "## Build failed, stopping immediately. Please fix and retry, or run manually:"
    echo "##   bash $BUILD_CMD <chipid> lst"
    echo "################################################################"
}

copy_one() {
    # copy_one <src> <dst_dir>
    local src="$1"
    local dst="$2"
    if [ -f "$src" ]; then
        cp -f "$src" "$dst/"
        echo "        [cp] $(basename "$src")"
    else
        echo "        [warn] missing: $src"
        return 1
    fi
}

# Build one platform and copy artifacts.
# NOTE: any compile/copy failure triggers exit 1 directly, stopping the whole script.
build_one() {
    local plat="$1"
    local chipid="${CHIPID_MAP[$plat]:-}"
    if [ -z "$chipid" ]; then
        echo "[$(ts)] [FATAL] unknown platform: $plat"
        exit 1
    fi

    local target_dir="$OUT_BASE/$plat"
    mkdir -p "$target_dir"

    local app_log="$LOG_DIR/${plat}_app.log"
    local ota_log="$LOG_DIR/${plat}_ota.log"

    echo
    echo "================================================================"
    echo "[$(ts)] >>> Building platform: $plat (CHIPID=$chipid)"
    echo "================================================================"

    if [ "${SKIP_BUILD:-0}" != "1" ]; then
        # 1) lst build (also produces bin/map/lst)
        echo "[$(ts)]     step 1/2: build_cust_cmd.sh $chipid lst"
        if ! bash "$BUILD_CMD" "$chipid" lst 2>&1 | tee "$app_log"; then
            report_build_error "$plat" "app(lst)" "$app_log"
            exit 1
        fi
        # Treat "exit 0 but artifact missing" as failure too
        if [ ! -f "$SDK_ROOT/out/$chipid/$chipid.bin" ]; then
            echo "[$(ts)] [FATAL] $plat: build_cust_cmd.sh exited 0 but artifact missing: out/$chipid/$chipid.bin"
            report_build_error "$plat" "app(lst)" "$app_log"
            exit 1
        fi

        # 2) OTA bin build (the lst branch in build_cust_cmd.sh does NOT build ota, trigger it separately)
        echo "[$(ts)]     step 2/2: build prod_test/ota_copy"
        if ! make T=prod_test/ota_copy CHIP="$chipid" OTA_BIN_COMPRESSED=0 DEBUG=1 -j64 2>&1 | tee "$ota_log"; then
            report_build_error "$plat" "ota_copy" "$ota_log"
            exit 1
        fi
        if [ ! -f "$SDK_ROOT/out/prod_test/ota_copy/ota_copy.bin" ]; then
            echo "[$(ts)] [FATAL] $plat: ota_copy exited 0 but artifact missing: out/prod_test/ota_copy/ota_copy.bin"
            report_build_error "$plat" "ota_copy" "$ota_log"
            exit 1
        fi
    else
        echo "[$(ts)]     SKIP_BUILD=1, skipping build, only copying existing artifacts"
    fi

    # 3) Copy artifacts (any missing file is fatal)
    echo "[$(ts)]     copy artifacts -> $target_dir"
    local out_app="$SDK_ROOT/out/$chipid"
    copy_one "$out_app/$chipid.bin" "$target_dir" || { echo "[$(ts)] [FATAL] missing app bin"; exit 1; }
    copy_one "$out_app/$chipid.map" "$target_dir" || { echo "[$(ts)] [FATAL] missing app map"; exit 1; }
    copy_one "$out_app/$chipid.lst" "$target_dir" || { echo "[$(ts)] [FATAL] missing app lst"; exit 1; }

    local ota_bin="$SDK_ROOT/out/prod_test/ota_copy/ota_copy.bin"
    if [ -f "$ota_bin" ]; then
        cp -f "$ota_bin" "$target_dir/${chipid}_ota.bin"
        echo "        [cp] ${chipid}_ota.bin (from ota_copy.bin)"
    else
        echo "[$(ts)] [FATAL] missing ota bin: $ota_bin"
        exit 1
    fi

    echo "[$(ts)] [OK] $plat done"
}

# ----------------------------- Main flow ------------------------------------
mkdir -p "$OUT_BASE"

# Allow building only the platforms passed on the command line
if [ "$#" -gt 0 ]; then
    SELECTED=("$@")
else
    SELECTED=("${PLATFORMS[@]}")
fi

declare -a OK_LIST=()

# NOTE: build_one calls exit 1 internally on any failure, stopping the whole script.
for plat in "${SELECTED[@]}"; do
    build_one "$plat"
    OK_LIST+=("$plat")
done

# ----------------------------- Summary --------------------------------------
echo
echo "================================================================"
echo "build_testbin_pack.sh summary @ $(date '+%F %T')"
echo "  output dir : $OUT_BASE"
echo "  build logs : $LOG_DIR"
echo "  OK         : ${OK_LIST[*]:-(none)}"
echo "================================================================"
echo
echo "Final layout:"
if command -v tree >/dev/null 2>&1; then
    tree -L 2 "$OUT_BASE" 2>/dev/null || ls -lR "$OUT_BASE"
else
    for d in "$OUT_BASE"/*/; do
        [ -d "$d" ] || continue
        echo "  $d"
        ls -1 "$d" | sed 's/^/      /'
    done
fi

# Reaching here means all platforms succeeded (any failure already exited via build_one)
exit 0
