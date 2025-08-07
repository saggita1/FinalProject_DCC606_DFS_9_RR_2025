import matplotlib.pyplot as plt
import pandas as pd

df = pd.read_csv("benchmark.csv")

df = df.sort_values("Threads")

tempo_sequencial = df[df["Threads"] == 1]["TempoTotal_ms"].values[0]
df["Speedup"] = tempo_sequencial / df["TempoTotal_ms"]

plt.figure(figsize=(8, 6))
plt.plot(df["Threads"], df["Speedup"], marker='o', linestyle='-', color='blue', label="Speedup")

plt.plot(df["Threads"], df["Threads"], linestyle='--', color='gray', label="Speedup Ideal")

plt.title("Speedup vs Número de Threads")
plt.xlabel("Número de Threads")
plt.ylabel("Speedup")
plt.xticks(df["Threads"])
plt.grid(True)
plt.legend()
plt.tight_layout()

plt.savefig("grafico_speedup.png", dpi=300)
plt.show()
