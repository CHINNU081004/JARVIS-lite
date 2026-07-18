#!/usr/bin/env sh
set -eu

prefix="${PREFIX:-${HOME}/.local}"
bindir="${prefix}/bin"
script_dir=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)

binary=""
if [ -x "${script_dir}/jarvis-lite" ]; then
    binary="${script_dir}/jarvis-lite"
elif [ -x "${script_dir}/bin/jarvis-lite" ]; then
    binary="${script_dir}/bin/jarvis-lite"
else
    echo "jarvis-lite binary was not found next to this installer." >&2
    exit 1
fi

mkdir -p "${bindir}"
cp "${binary}" "${bindir}/jarvis-lite"
chmod +x "${bindir}/jarvis-lite"

echo "Installed jarvis-lite to ${bindir}/jarvis-lite"
case ":${PATH}:" in
    *":${bindir}:"*) ;;
    *)
        echo "Add this directory to PATH before running jarvis-lite:"
        echo "  ${bindir}"
        ;;
esac
