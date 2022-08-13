#!/usr/bin/env python3

from typing import Optional
import os
import subprocess
from argparse import ArgumentParser
from pathlib import Path
from dataclasses import dataclass
from shutil import rmtree

root = Path(__file__).resolve().parent


def run(cmd, env={}, cwd=None):
    print(f"\nRunning {cmd}" + (f" at '{cwd}'" if cwd is not None else "") + "\n", flush=True)
    subprocess.run(cmd, env={**os.environ, **env}, cwd=cwd, check=True)


@dataclass
class Compiler:
    name: str
    version: Optional[int]

    @staticmethod
    def from_str(str):
        parts = str.split("-")
        if len(parts) == 1:
            return Compiler(parts[0], None)
        elif len(parts) == 2:
            return Compiler(parts[0], int(parts[1]))
        else:
            raise RuntimeError(f"Bad compiler string: {str}. Expected 'name[-version]'")

    def binaries(self):
        ver = f"-{self.version}" if self.version is not None else ""
        if self.name == "gcc":
            return (f"gcc{ver}", f"g++{ver}")
        elif self.name == "clang":
            return (f"clang{ver}", f"clang++{ver}")
        else:
            raise RuntimeError(f"Unknown compiler name: {self.name}. Expected 'gcc' or 'clang'")

    def env(self):
        cc, cxx = self.binaries()
        return {"CC": cc, "CXX": cxx}


@dataclass
class Project:
    build_dir: Path
    compiler: Compiler
    clean: bool

    def prepare(self):
        if self.build_dir.exists():
            if self.clean:
                rmtree(self.build_dir)

        if not self.build_dir.exists():
            self.build_dir.mkdir()

            run(["cmake", root], env=self.compiler.env(), cwd=self.build_dir)

    def build(self):
        self.prepare()

        run(["cmake", "--build", self.build_dir, "--parallel", str(os.cpu_count())])

    def test(self):
        self.build()

        run([self.build_dir / "rcore_test"])


actions = [
    ("prepare", Project.prepare),
    ("build", Project.build),
    ("test", Project.test),
]


parser = ArgumentParser()
parser.add_argument("build_dir", type=Path, help="build directory path")
parser.add_argument("action", type=str, choices=[a[0] for a in actions], help="action")
parser.add_argument("--compiler", type=str, choices=["gcc", "clang"], default="gcc")
parser.add_argument("--clean", action="store_true", help="recreate build directory")


args = parser.parse_args()

core = Project(
    build_dir=args.build_dir.resolve(),
    compiler=Compiler.from_str(args.compiler),
    clean=args.clean,
)

{k: v for k, v in actions}[args.action](core)
