"""
Script that should be called at the beginning of a GDB session. This can be configured
in your GDB init file:

    $ cat ~/.gdbinit
    source gdb_startup.py

then, call "arm-none-eabi-gdb-py" when in this folder.
"""
import gdb
import os


def execute_gdb_command(cmd_string, verbose=True):
    if verbose:
        print("(gdb) {}".format(cmd_string))
    gdb.execute(cmd_string)


def choose_file(list_of_files):
    print("\nFiles:")
    for ind, f in enumerate(list_of_files):
        short_f = "/".join(f.split("/")[-3:])
        short_f = "(...)/" + short_f
        print("\t{}: {}".format(ind, short_f))
    while True:
        try:
            choice = int(raw_input("Which file do you choose? "))
        except Exception:
            print("Invalid choice.")
            continue

        if choice < len(list_of_files):
            return list_of_files[choice]
        else:
            print("Invalid index.")


def find_elf_files(directory):
    elf_files = []
    for root, dirs, files in os.walk(directory):
        for f in files:
            if ".elf" in f:
                filepath = os.path.join(root, f)
                print filepath
                elf_files.append(filepath)
    return elf_files


if __name__ == '__main__':
    f = choose_file(find_elf_files(os.path.abspath("../OvenTemp/build")))
    execute_gdb_command("target remote localhost:3333")
    execute_gdb_command("monitor reset")
    execute_gdb_command("file {}".format(f))
    execute_gdb_command("continue")
