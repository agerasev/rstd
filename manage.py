#!/usr/bin/env python3

import os
import subprocess
from argparse import ArgumentParser
from pathlib import Path
from dataclasses import dataclass
from shutil import rmtree

core_dir = Path(__file__).resolve().parent


def run(cmd, env={}, cwd=None):
    print(f"\nRunning {cmd}" + (f" at '{cwd}'" if cwd is not None else "") + "\n", flush=True)
    subprocess.run(cmd, env={**os.environ, **env}, cwd=cwd, check=True)


@dataclass
class Core:
    build_dir: Path
    compiler: str
    clean: bool

    def _compiler_env(self):
        if self.compiler == "gcc":
            return {"CC": "gcc-10", "CXX": "g++-10"}
        elif self.compiler == "clang":
            return {"CC": "clang-11", "CXX": "clang++-11"}
        else:
            raise RuntimeError(f"Unknown compiler: {self.compiler}")

    def prepare(self):
        if self.build_dir.exists():
            if self.clean:
                rmtree(self.build_dir)

        if not self.build_dir.exists():
            self.build_dir.mkdir()

            env = self._compiler_env()
            profile = self.build_dir / "profile.txt"
            run(["conan", "profile", "new", profile, "--detect"], env=env)
            run(["conan", "profile", "update", "settings.compiler.libcxx=libstdc++11", profile], env=env)
            run(["conan", "install", core_dir / "test", "--profile", profile, "--build=missing"], env=env, cwd=self.build_dir)

            run(["cmake", core_dir / "test"], env=env, cwd=self.build_dir)

    def build(self):
        self.prepare()

        run(["cmake", "--build", self.build_dir, "--parallel", str(os.cpu_count())])

    def test(self):
        self.build()

        run([self.build_dir / "core_test"])


actions = [
    ("prepare", Core.prepare),
    ("build", Core.build),
    ("test", Core.test),
]


parser = ArgumentParser()
parser.add_argument("build_dir", type=Path, help="build directory path")
parser.add_argument("action", type=str, choices=[a[0] for a in actions], help="action")
parser.add_argument("--compiler", type=str, choices=["gcc", "clang"], default="gcc")
parser.add_argument("--clean", action="store_true", help="recreate build directory")


args = parser.parse_args()

core = Core(
    build_dir=args.build_dir.resolve(),
    compiler=args.compiler,
    clean=args.clean,
)

{k: v for k, v in actions}[args.action](core)
