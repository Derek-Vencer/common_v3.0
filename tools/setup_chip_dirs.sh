#!/bin/bash
# setup_chip_dirs.sh <SRCROOT> <T> [CHIP]
#
# Point bthost/ and multimedia/algorithms/ symlinks to the correct source tree:
#   <rel>_<CHIP>/   -- chip-specific (preferred)
#   <rel>_generic/  -- generic fallback
#
# All real source directories carry an explicit suffix; the un-suffixed names
# are always symlinks and never real directories.

set -e

SRCROOT="$1"
T="$2"
CHIP="$3"

# If CHIP not given on command line, try to read it from target.mk (incl. include chain).
if [ -z "$CHIP" ]; then
    TARGET_MK="$SRCROOT/config/$T/target.mk"
    [ -f "$TARGET_MK" ] || exit 0   # no target.mk -- skip silently

    # Recursively resolve CHIP= across include chain. Visited set avoids loops.
    _find_chip() {
        local mk="$1" visited="$2" val inc inc_path
        [ -f "$mk" ] || return 0
        case ":${visited}:" in *":${mk}:"*) return 0 ;; esac
        visited="${visited}:${mk}"
        val=$(grep -m1 '^[[:space:]]*CHIP[[:space:]]*[?:]*=' "$mk" 2>/dev/null \
              | sed 's/.*=[[:space:]]*//' | tr -d ' \t\r')
        if [ -n "$val" ]; then echo "$val"; return 0; fi
        while IFS= read -r line; do
            inc=$(echo "$line" | sed -n 's/^[[:space:]]*include[[:space:]]\+\([^[:space:]#]\+\).*/\1/p')
            [ -n "$inc" ] || continue
            inc_path="$SRCROOT/$inc"
            val=$(_find_chip "$inc_path" "$visited")
            if [ -n "$val" ]; then echo "$val"; return 0; fi
        done < "$mk"
    }
    CHIP=$(_find_chip "$TARGET_MK" "")
    [ -n "$CHIP" ] || exit 0   # no CHIP anywhere -- skip silently
fi

setup_link() {
    local rel="$1"
    local link="$SRCROOT/$rel"
    local specific="$SRCROOT/${rel}_${CHIP}"
    local generic_fb="$SRCROOT/${rel}_generic"

    if [ -d "$specific" ]; then
        local dest="$specific" label="${rel}_${CHIP}"
    elif [ -d "$generic_fb" ]; then
        local dest="$generic_fb" label="${rel}_generic"
    elif [ -d "$link" ] && [ ! -L "$link" ]; then
        # Legacy repo: bthost/ or multimedia/algorithms/ is a plain real directory
        # (no chip-suffix convention). Nothing to do -- skip silently.
        return 0
    else
        echo "setup_chip_dirs.sh: neither ${rel}_${CHIP}/ nor ${rel}_generic/ found" >&2; exit 1
    fi

    # Refuse to overwrite a real directory -- rename it to <name>_generic first.
    if [ -d "$link" ] && [ ! -L "$link" ]; then
        echo "setup_chip_dirs.sh: '$link' is a real directory; rename to '${link}_generic' first" >&2
        exit 1
    fi

    # Update symlink only when needed (rm+ln avoids ln -sfn portability issues).
    if [ -L "$link" ] && [ "$(readlink "$link")" = "$dest" ]; then return 0; fi
    [ -L "$link" ] && rm "$link"
    ln -s "$dest" "$link"
    echo -e "  \033[33m[chip-dir]\033[0m $rel -> \033[32m$label\033[0m  (CHIP=$CHIP)"
}

setup_link "bthost"
setup_link "multimedia/algorithms"
