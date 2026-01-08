#!/usr/bin/python3

from collections import defaultdict
import matplotlib.pyplot as plt
import numpy as np
import pandas as pd

df = pd.read_csv("data.csv", delim_whitespace=True)

print(df.columns)

X = [int(x) for x in df.nv]
Y = [float(y) for y in df.total_partition]

totals = defaultdict(lambda: np.zeros(10))
counts = defaultdict(int)
for index, row in df.iterrows():
    totals[row['nv']] += np.array(row)
    counts[row['nv']] += 1
ns = sorted(list(totals))
for n in ns:
    totals[n] /= counts[n]
dfmean = pd.DataFrame([totals[n] for n in ns], columns=df.columns)
print(dfmean)
Xmean = [int(x) for x in dfmean.nv]
Ymean = [float(y) for y in dfmean.total_partition]

parts = ['bfs', 'cotree', 'lca', 'partition', 'lca_cleanup']
# for index, row in dfmean.iterrows():
#     sum = 0;
#     for p in parts:
#         sum += row[p]
#         row[p] = sum
#
# print(dfmean)

fig, ax = plt.subplots(figsize=(8, 5))
# plt.xscale("log")
# plt.yscale("log")
# ax.plot(Xmean, Ymean, color='gray', linestyle='-', label='avg total')
print(len(ns), len(dfmean.nv))
next = np.zeros(len(ns))
curves = dict()
curves['zero'] = next
for p in parts:
    next = next + dfmean[p]
    curves[p] = next
parts = ["zero"] + parts
for i in range(len(parts)-1, 0, -1):
    p = parts[i]
    ax.plot(dfmean.nv, curves[p], color='black', linestyle='-')
    ax.fill_between(dfmean.nv, curves[p], curves[parts[i-1]], label=p, alpha=0.5)
ax.scatter(X, Y, color='red', marker='o', s=100, label='totals')
ax.set_xlabel("n")
ax.set_ylabel("time (seconds)")
ax.legend()
plt.show()
