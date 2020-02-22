# _build.py
# Commandline build utility. 

import pathlib
import argparse
import subprocess

OUT_PATH = "bin/"
RECURSIVE_EXCLUDES = ["bin", ".vscode"]

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("targets", nargs="*", type=str, help="Build Target(s)")
    parser.add_argument("--debug", "-d", action="store_true", help="Build With Debugging Symbols")
    args = parser.parse_args()

    if 0 == len(args.targets):
        # build all
        build_all(args.debug)
    else:
        # build specified targets
        for target in args.targets:
            p = pathlib.Path(target)
            if p.is_dir():
                build_recursive(target, args.debug)
            else:
                build(target, args.debug)

def build_all(debug : bool):
    build_recursive(".", debug)

def build_recursive(path : str, debug : bool):
    print(f"build recursive from root {path}")
    for e in pathlib.Path(path).iterdir():
        if e.name in RECURSIVE_EXCLUDES:
            continue

        if e.is_dir():
            build_recursive(str(e), debug)
        elif e.name.endswith(".cpp"):
            build(str(e), debug) 

def build(path : str, debug : bool):
    target_name = name(path)
    out_obj = f"{OUT_PATH}{as_obj(target_name)}"
    out_exe = f"{OUT_PATH}{as_exe(target_name)}"

    print(f"[+] Building {target_name} to {OUT_PATH}{target_name}")
    
    command = [
        "cl",
        "/EHsc",
        "/nologo", 
        "/std:c++17", 
        "/W4",
        "/IC:/Dev/ZMQ/libzmq/include", 
        "/IC:/Dev/ZMQ/cppzmq",
        f"/Fe{out_exe}",
        f"/Fo{out_obj}",
        path,
        "/link",
        "/libpath:C:/Dev/ZMQ/libzmq/build/lib/Debug",
        "libzmq-v142-mt-gd-4_3_2.lib"
        ]

    if (debug):
        command.append("/DEBUG")

    subprocess.run(command)

def name(path : str) -> str:
    return pathlib.Path(path).name

def as_exe(src_name : str) -> str:
    return src_name.split(".")[0] + ".exe"

def as_obj(src_name : str) -> str:
    return src_name.split(".")[0] + ".obj"

if __name__ == "__main__":
    main()