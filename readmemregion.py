import os
import argparse
import subprocess


def get_memory_map_by_region_name(pid):
    path = os.path.join("/proc", str(pid), "maps")
    with open(path) as mapfile:
        lines = mapfile.readlines()
    lines_parts = [line.split(" ", 5) for line in lines]
    memory_map = dict()
    for entry in lines_parts:
        start_address, end_address = [int(hex_number, 16) for hex_number in entry[0].split('-')]
        name = entry[-1].strip()
        memory_map[name] = (start_address, end_address)
    return memory_map


def parse_args():
    parser = argparse.ArgumentParser()
    parser.add_argument("-p", "--pid", type=int, help="PID of process of which to read memory")
    parser.add_argument("-r", "--regions", type=str, nargs="+", help="Names of memory regions to read")
    return parser.parse_args()


def read_memory_region(pid, start_address, region_size):
    exe_path = os.path.join(os.path.curdir, "readmem")
    cmd = [exe_path, str(pid), str(start_address), str(region_size)]
    print cmd
    return subprocess.check_output(cmd)


def print_memory_region(region):
    print region


def main():
    args = parse_args()
    memory_map = get_memory_map_by_region_name(args.pid)
    for region in args.regions:
        start_address, end_address = memory_map[region]
        region_size = end_address - start_address
        region = read_memory_region(args.pid, start_address, region_size)
        print_memory_region(region)

if __name__ == "__main__":
    main()
