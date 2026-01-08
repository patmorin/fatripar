#!/usr/bin/python3

import subprocess

ns = [3**i for i in range(2,16)]
reps = 20

with open("data.csv", "w") as fp:
    fp.write("nv nf read verify bfs cotree lca partition lca_cleanup total_partition\n")
    fp.flush()
    for n in ns:
        for i in range(reps):
            print(f"Creating input of size {n} ({i+1} of {reps})")
            with open("input.txt", "w") as ifp:
                subprocess.run(["./inputs/makeinput", str(n)], stdout=ifp, check=True)
            print(f"Running on input of size {n} ({i+1} of {reps})")
            subprocess.run(["./tripod_demo", "input.txt"], stdout=fp, check=True)
