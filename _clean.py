# _clean.py
# Commandline clean utility.

import pathlib
import argparse

def main():
    parser = argparse.ArgumentParser()
    parser.parse_args()

    recursive_unlink("*.exe")
    recursive_unlink("*.obj")

def recursive_unlink(pattern : str):
    for f in pathlib.Path(".").rglob(pattern):
        f.unlink()

if __name__ == "__main__":
    main()