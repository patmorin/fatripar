#!/usr/bin/python3

from collections import defaultdict
import matplotlib as mpl
import matplotlib.pyplot as plt
import numpy as np
import pandas as pd

mpl.use("pgf")
pgf_with_pdflatex = {
        "font.family": "serif",
    "font.size": 9,
    "text.usetex" : True,
    "pgf.rcfonts": False,
    "pgf.texsystem": "pdflatex",
    # "pgf.preamble": [
    #      r"\usepackage[utf8]{inputenc}",
    #      r"\usepackage[T1]{fontenc}",
    #      r"\usepackage{kpfonts}",
    #      ]
}
mpl.rcParams.update(pgf_with_pdflatex)


def plot_data(df, xlabel, ylabel, parts, filename, scatter=False, xlog=False, ylog=False):
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

    Xmean = [int(x) for x in dfmean.nv]
    Ymean = [float(y) for y in dfmean.total_partition]

    plt.figure(figsize=(6.8, 3.5))
    fig, ax = plt.subplots(figsize=(6.8, 3.5))
    if xlog:
        ax.set_xscale('log')
    if ylog:
        ax.set_yscale('log')
    next = np.zeros(len(ns))
    curves = dict()
    curves['zero'] = next
    for p in parts:
        next = next + dfmean[p]
        curves[p] = next
    parts2 = ["zero"] + parts
    for i in range(len(parts2)-1, 0, -1):
        p = parts2[i]
        ax.plot(dfmean.nv, curves[p], color='black', linestyle='-', linewidth=.5)
        ax.fill_between(dfmean.nv, curves[p], curves[parts2[i-1]], label=p, alpha=0.25)
    if scatter:
        ax.scatter(X, Y, color='red', marker='o', s=5, label='points')
    ax.set_xlabel(xlabel)
    ax.set_ylabel(ylabel)
    ax.legend()

    plt.savefig(filename, format='pdf', bbox_inches='tight')


if __name__ == "__main__":
    parts = ['bfs', 'cotree', 'lca', 'partition'] # 'lca_cleanup'
    df = pd.read_csv("data.csv", delim_whitespace=True)
    plot_data(df, "$n$", "time (seconds)", parts, "total.pdf", False)
    df2 = pd.DataFrame(columns=df.columns)
    for index, row in df.iterrows():
        n = row['nv']
        for p in parts:
            row[p] = row[p] / n
        df2.loc[len(df2)] = row
    print(df2)
    plot_data(df2, "$n$", "time/$n$ (seconds)", parts, "per_item.pdf", False)
