"""Compile et execute test_encoder.c sur un Cortex-M4 emule, sans carte.

Prerequis : toolchain ARM PlatformIO et paquet Python unicorn 2.1.4.
Le paquet peut etre installe localement, sans modifier Python :
python -m pip install --target .pio/encoder-test-tools unicorn==2.1.4
Execution : python tests/run_encoder_tests.py
"""

import os
from pathlib import Path
import subprocess
import sys

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / ".pio" / "encoder-test-tools"))
from unicorn import Uc, UC_ARCH_ARM, UC_MODE_MCLASS, UC_MODE_THUMB
from unicorn.arm_const import UC_ARM_REG_LR, UC_ARM_REG_PC, UC_ARM_REG_R0, UC_ARM_REG_SP


def run(*args):
    return subprocess.check_output([str(arg) for arg in args], cwd=ROOT, text=True)


def main():
    core = Path(os.environ.get("PLATFORMIO_CORE_DIR", Path.home() / ".platformio"))
    toolchain = core / "packages" / "toolchain-gccarmnoneeabi" / "bin"
    suffix = ".exe" if os.name == "nt" else ""
    gcc = toolchain / f"arm-none-eabi-gcc{suffix}"
    objcopy = toolchain / f"arm-none-eabi-objcopy{suffix}"
    nm = toolchain / f"arm-none-eabi-nm{suffix}"
    build = ROOT / ".pio" / "encoder-tests"
    build.mkdir(parents=True, exist_ok=True)
    elf = build / "tests.elf"
    binary = build / "tests.bin"
    # L'edition de liens ne doit pas introduire de syscalls ou de runtime HAL.
    run(gcc, "-std=c11", "-O2", "-Wall", "-Wextra", "-Werror", "-mcpu=cortex-m4",
        "-mthumb", "-ffreestanding", "-nostdlib", "-Isrc", "src/encoder.c",
        "tests/test_encoder.c", "-Wl,-Ttext=0x08000000,-Tdata=0x20000000,-e,main",
        "-o", elf)
    run(objcopy, "-O", "binary", elf, binary)
    symbols = {parts[2]: int(parts[0], 16) for line in run(nm, elf).splitlines()
               if len(parts := line.split()) == 3}
    cpu = Uc(UC_ARCH_ARM, UC_MODE_THUMB | UC_MODE_MCLASS)
    cpu.mem_map(0x08000000, 0x10000)
    cpu.mem_map(0x20000000, 0x10000)
    cpu.mem_write(0x08000000, binary.read_bytes())
    cpu.reg_write(UC_ARM_REG_SP, 0x2000FFF0)
    stop = 0x0800FFF0
    cpu.reg_write(UC_ARM_REG_LR, stop | 1)
    cpu.emu_start(symbols["main"] | 1, stop, count=2_000_000)
    if cpu.reg_read(UC_ARM_REG_PC) != stop:
        raise RuntimeError("Le test n'a pas termine dans le budget d'instructions")
    result = cpu.reg_read(UC_ARM_REG_R0)
    if result:
        raise AssertionError(f"Echec de CHECK a tests/test_encoder.c:{result}")
    print("PASS : rotations, resolutions 1/2/4, rebonds, inversions, transitions")
    print("invalides, resynchronisation, debordement du tick, axes et limites 0..7.")


if __name__ == "__main__":
    main()
