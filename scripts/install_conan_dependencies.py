#!/usr/bin/env python3
"""Helper script to install dependent Conan packages.
"""

from pathlib import Path
import subprocess
from typing import Iterable
import os

import click

THIS_DIR = Path(__file__).absolute().parent


def make_build_path(build_type: str) -> str:
    """Make the path of the build directory.

    Args:
        build_type (str): Type of build.

    Returns:
        str: Path.
    """

    return str(THIS_DIR.parent / "build" / build_type)


@click.command()
@click.argument(
    "build_type",
    nargs=1,
    type=click.Choice(["Debug", "Release", "RelWithDebInfo", "MinSizeRel"]),
)
@click.argument("additional_args", nargs=-1)
def install_conan_dependencies(build_type: str, additional_args: Iterable[str]):
    """Install dependent Conan packages.

    - build_type: Type of build.

    - additional_args: Additional command line arguments.
    """

    build_path = make_build_path(build_type)

    os.makedirs(build_path, exist_ok=True)

    command = (
        [
            "conan",
            "install",
            "--build",
            "missing",
            "-s",
            "build_type=" + build_type,
            "-o",
            "requirements_for_tests=True",
        ]
        + list(additional_args)
        + [
            "../..",
        ]
    )
    print(f"> run command: {command}")
    result = subprocess.run(command, check=False, cwd=build_path)

    exit(result.returncode)


if __name__ == "__main__":
    install_conan_dependencies()
