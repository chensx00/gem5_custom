import sys
import os

if __name__ == '__main__':
    if len(sys.argv) != 4 and len(sys.argv) != 3:
        print("Usage: python extract_verilator.py <verilator_makefile> <shared_lib_name> <extra_objs>")
        sys.exit(1)
    makefile_dir = os.path.dirname(sys.argv[1])
    makefile_name = os.path.basename(sys.argv[1])
    shared_lib_name = sys.argv[2]
    if len(sys.argv) == 4:
        extra_objs = sys.argv[3].split()  # extra object files split by whitespace
    else:
        extra_objs = []

    f = os.popen(f"make -C {makefile_dir} -f {makefile_name} debug-make")
    resolved_obj = extra_objs
    for line in f:
        if line.startswith("VM_GLOBAL"):
            for c in line.split()[1:]:
                resolved_obj.append(c + '.o')
        elif line.startswith("VK_OBJS"):
            for c in line.split()[1:]:
                resolved_obj.append(c)
    f.close()
    
    for obj in resolved_obj:
        # We assume that `-fpic` flag has been added by the makefile writer
        cmd = f'make -C {makefile_dir} -f {makefile_name} {obj}'
        res = os.system(cmd)
        assert res == 0, f"cmd \"{cmd}\" failed"
    # We assume that this script be called from makefile,
    # so its working dir and final shared lib should be there
    cmd = f'cd {makefile_dir}; gcc -shared -o lib{shared_lib_name}.so {" ".join(resolved_obj)}'
    res = os.system(cmd)
    assert res == 0, f"cmd \"{cmd}\" failed"