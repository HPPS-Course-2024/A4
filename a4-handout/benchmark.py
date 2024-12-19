import subprocess
import re
import matplotlib.pyplot as plt
import numpy as np

# Commands
GEN_NAIVE_CMD_PLACEHOLDER = (r"./random_ids data/20000_samples.tsv | head -n {n} > benchmark/{n}_ids")
GEN_COORD_CMD_PLACEHOLDER = (r"./random_coords data/20000_samples.tsv | head -n {n} > benchmark/{n}_coords")
CLEAN_CMD = r"rm benchmark/*"
MAKE_CMD = r"make"
ID_NAIVE_CMD_PLACEHOLDER = (r"./id_query_naive data/20000_samples.tsv < benchmark/{n}_ids")
ID_INDEX_CMD_PLACEHOLDER = (r"./id_query_index data/20000_samples.tsv < benchmark/{n}_ids")
ID_BIN_SORT_CMD_PLACEHOLDER = (r"./id_query_bin_sort data/20000_samples.tsv < benchmark/{n}_ids")
COORD_NAIVE_CMD_PLACEHOLDER = (r"./coord_query_naive data/20000_samples.tsv < benchmark/{n}_coords")

def clean_time_str(s):
    try:
        return float(re.sub(r"[a-zA-Z]", "", s.replace("\t", "")))
    except ValueError:
        return None

def compute_time_for_input_size(program_cmd, input_size, generator=GEN_NAIVE_CMD_PLACEHOLDER):
    cmd = generator.format(n=input_size)
    subprocess.run(
        cmd, shell=True, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL
    )
    time_cmd = f"bash -c 'time {program_cmd}' 2>&1"
    result = subprocess.run(
        time_cmd,
        shell=True,
        capture_output=True,
    )
    time = list(map(clean_time_str, result.stdout.decode("utf-8").splitlines()[-3:]))
    subprocess.run(CLEAN_CMD, shell=True)
    return (*time,)

result = subprocess.run(MAKE_CMD, shell=True)
if result.returncode != 0:
    print("Make compilation failed")
    exit(1)

logarithmic = False
input_sizes = None

if logarithmic:
    input_sizes = np.logspace(0, 6, num=50, dtype=int)
else: 
    MAX_INPUT_SIZE = 2000
    STEP_SIZE = 1
    input_sizes = list(range(1, MAX_INPUT_SIZE + 1, STEP_SIZE))

print(input_sizes)

query_types = {
    "Naive Query": {
        "cmd": ID_NAIVE_CMD_PLACEHOLDER, "real": [], "user": [], "color": 'blue'
    },
    "Indexed Query": {
        "cmd": ID_INDEX_CMD_PLACEHOLDER, "real": [], "user": [], "color": 'green'
    },
    "Binsort Query": {
        "cmd": ID_BIN_SORT_CMD_PLACEHOLDER, "real": [], "user": [], "color": 'orange'
    },
    "Coord Query": {
        "cmd": COORD_NAIVE_CMD_PLACEHOLDER, "real": [], "user": [], 
        "generator": GEN_COORD_CMD_PLACEHOLDER, "color": 'red'
    }
}

for i, input_size in enumerate(input_sizes):
    for query_name, query_info in query_types.items():
        cmd = query_info["cmd"].format(n=input_size)
        generator = query_info.get("generator", GEN_NAIVE_CMD_PLACEHOLDER)
        real_time, user_time, _ = compute_time_for_input_size(cmd, input_size, generator) 
        if real_time is None or user_time is None:
            print(f"Failed to compute time for {query_name} query with input size {input_size}")
            continue
        query_info["real"].append(real_time)
        query_info["user"].append(user_time)
    if i % 5 == 0:
        print(f"Processed {input_size}/{input_sizes[-1]} inputs")
plt.figure(figsize=(10, 6)) 


for query_name, query_info in query_types.items():
    plt.plot(input_sizes, query_info['real'], label=f"{query_name} (Real)", color=query_info['color'], alpha=0.7, linestyle='-')
    plt.plot(input_sizes, query_info['user'], label=f"{query_name} (User)", color=query_info['color'], alpha=0.5, linestyle='--')


if logarithmic:
    plt.title("Execution Time (Real vs User) vs Input Size (Log-Log Scale)")
    plt.xlabel("Input Size (Log Scale)")
else:
    plt.title("Execution Time (Real vs User) vs Input Size")
    plt.xlabel("Input Size (n)")

plt.ylabel("Execution Time (Log Scale, $\mu$, in nanoseconds)")
plt.xscale("log" if logarithmic else "linear")
plt.yscale("log")

plt.legend(bbox_to_anchor=(1.03, 1), loc='upper left', framealpha=0.8)
plt.grid()
plt.tight_layout() 

plt.savefig("benchmark.png")