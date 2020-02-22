# _clean.py
# Commandline clean utility.

import pathlib
import argparse

EXTENSIONS = ["*.exe", "*.obj", "*.pdb", "*.ilk"]

def main():
    parser = argparse.ArgumentParser()
    parser.parse_args()

    for ext in EXTENSIONS:
        recursive_unlink(ext)

def recursive_unlink(pattern : str):
    for f in pathlib.Path(".").rglob(pattern):
        f.unlink()

if __name__ == "__main__":
    main()