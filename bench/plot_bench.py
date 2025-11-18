import csv
import os
import math
import matplotlib.pyplot as plt

IMG_DIR = "imgs"

os.makedirs(IMG_DIR, exist_ok=True)

def load_csv(path):
    rows = []
    with open(path, newline="", encoding="utf-16") as f:
        reader = csv.DictReader(f)
        for r in reader:
            r["impl"] = r["impl"].strip()
            r["cap"] = int(r["cap"])
            r["P"] = int(r["P"])
            r["C"] = int(r["C"])
            r["items"] = int(r["items"])
            r["trials"] = int(r["trials"])
            r["time_avg_s"] = float(r["time_avg_s"])
            r["time_min_s"] = float(r["time_min_s"])
            r["time_max_s"] = float(r["time_max_s"])
            r["throughput_avg_ops_per_s"] = float(r["throughput_avg_ops_per_s"])
            rows.append(r)
    return rows

twolock = load_csv("csv/twolock.csv")
onelock = load_csv("csv/onelock.csv")
seq     = load_csv("csv/seq.csv")  # seq has P=C=0, one row per cap

def filter_rows(rows, cap=None):
    if cap is None:
        return rows
    return [r for r in rows if r["cap"] == cap]

# --------------------------------------------------------
# 1) Throughput vs #threads (P+C) for each implementation
# --------------------------------------------------------
def plot_throughput_vs_threads(cap=256):
    two = filter_rows(twolock, cap)
    one = filter_rows(onelock, cap)

    # Sort by total threads
    two.sort(key=lambda r: r["P"] + r["C"])
    one.sort(key=lambda r: r["P"] + r["C"])

    threads_two = [r["P"] + r["C"] for r in two]
    thr_two     = [r["throughput_avg_ops_per_s"] for r in two]

    threads_one = [r["P"] + r["C"] for r in one]
    thr_one     = [r["throughput_avg_ops_per_s"] for r in one]

    plt.figure()
    plt.plot(threads_two, thr_two, marker="o", label="twolock")
    plt.plot(threads_one, thr_one, marker="o", label="onelock")
    plt.xlabel("Total threads (P + C)")
    plt.ylabel("Throughput (ops/s)")
    plt.title(f"Throughput vs Threads (cap={cap})")
    plt.grid(True)
    plt.legend()
    plt.tight_layout()
    out = os.path.join(IMG_DIR, f"throughput_vs_threads_cap{cap}.png")
    plt.savefig(out)
    plt.close()
    print("Saved:", out)

# --------------------------------------------------------
# 2) Speedup vs baseline (twolock & onelock, cap=256, P=1,C=1)
# --------------------------------------------------------
def plot_speedup_vs_threads(cap=256):
    # twolock and onelock rows for this capacity
    two = filter_rows(twolock, cap)
    one = filter_rows(onelock, cap)

    # sequential baseline row for this capacity (P=0,C=0)
    seq_rows = [r for r in seq if r["cap"] == cap]
    if not seq_rows:
        print("No sequential row found for cap", cap)
        return
    seq_row = seq_rows[0]

    items_seq = seq_row["items"]
    thr_seq   = seq_row["throughput_avg_ops_per_s"]
    # total ops in sequential run: 2 * items_seq
    T_seq = (2.0 * items_seq) / thr_seq

    def runtime_parallel(row):
        # total ops for this parallel config: 2 * P * items
        total_ops = 2.0 * row["P"] * row["items"]
        return total_ops / row["throughput_avg_ops_per_s"]

    # Compute speedup vs sequential for twolock
    two.sort(key=lambda r: r["P"])
    Ps_two = []
    speed_two = []
    for r in two:
        P = r["P"]
        T_par = runtime_parallel(r)
        Ps_two.append(P)
        speed_two.append(T_seq / T_par if T_par > 0 else float("nan"))

    # Compute speedup vs sequential for onelock
    one.sort(key=lambda r: r["P"])
    Ps_one = []
    speed_one = []
    for r in one:
        P = r["P"]
        T_par = runtime_parallel(r)
        Ps_one.append(P)
        speed_one.append(T_seq / T_par if T_par > 0 else float("nan"))

    plt.figure()
    plt.plot(Ps_two, speed_two, marker="o", label="twolock (vs seq)")
    plt.plot(Ps_one, speed_one, marker="o", label="onelock (vs seq)")

    # optional: horizontal line at 1.0 = same speed as sequential
    plt.axhline(1.0, linestyle="--", color="gray", label="same as seq")

    plt.xlabel("P (with C = P)")
    plt.ylabel("Speedup vs sequential")
    plt.title(f"Speedup vs P (cap={cap})")
    plt.grid(True)
    plt.legend()
    plt.tight_layout()
    out = os.path.join(IMG_DIR, f"speedup_vs_seq_cap{cap}.png")
    plt.savefig(out)
    plt.close()
    print("Saved:", out)


# --------------------------------------------------------
# 3) Throughput vs capacity (cap) for fixed P=C
# --------------------------------------------------------
def plot_throughput_vs_cap(P_fixed=4):
    caps = sorted({r["cap"] for r in twolock})
    thr_two = []
    thr_one = []

    for cap in caps:
        # pick row with given P_fixed
        r_two = next((r for r in twolock if r["cap"] == cap and r["P"] == P_fixed and r["C"] == P_fixed), None)
        r_one = next((r for r in onelock if r["cap"] == cap and r["P"] == P_fixed and r["C"] == P_fixed), None)

        if r_two is not None:
            thr_two.append(r_two["throughput_avg_ops_per_s"])
        else:
            thr_two.append(float("nan"))

        if r_one is not None:
            thr_one.append(r_one["throughput_avg_ops_per_s"])
        else:
            thr_one.append(float("nan"))

    plt.figure()
    plt.plot(caps, thr_two, marker="o", label=f"twolock (P=C={P_fixed})")
    plt.plot(caps, thr_one, marker="o", label=f"onelock (P=C={P_fixed})")
    plt.xlabel("Capacity (cap)")
    plt.ylabel("Throughput (ops/s)")
    plt.title(f"Throughput vs Capacity (P=C={P_fixed})")
    plt.grid(True)
    plt.legend()
    plt.tight_layout()
    out = os.path.join(IMG_DIR, f"throughput_vs_cap_P{P_fixed}.png")
    plt.savefig(out)
    plt.close()
    print("Saved:", out)

if __name__ == "__main__":
    # Pick a representative capacity (say 256) for first two plots
    plot_throughput_vs_threads(cap=256)
    plot_speedup_vs_threads(cap=256)
    # Throughput vs cap, e.g. P=C=4
    plot_throughput_vs_cap(P_fixed=4)
