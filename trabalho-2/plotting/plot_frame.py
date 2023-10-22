import matplotlib.pyplot as plt
# n_frames fifo lru opt
falhas_por_frame = [[], [], [], []]
with open("output.txt") as file:

    for line in file:
        line = line.split()
        for i in range(len(line)):
            falhas_por_frame[i].append(int(line[i]))


start = 2
end = 100

falhas_por_frame = [falhas[start-1:end-1] for falhas in falhas_por_frame]

fig, ax = plt.subplots(figsize=(20, 20))

ax.xaxis.get_major_locator().set_params(integer=True)
ax.set_xlabel("Número de Molduras")
ax.set_ylabel("Faltas")
ax.xaxis.set_ticks([start, *range(5, end+1, 5)])

# Add each algorithm times with captions to their colors
ax.plot(falhas_por_frame[0], falhas_por_frame[1], label="FIFO")
ax.plot(falhas_por_frame[0], falhas_por_frame[2], label="LRU")
ax.plot(falhas_por_frame[0], falhas_por_frame[3], label="OPT")

plt.legend(loc="upper right", frameon=False)



plt.savefig("benchmark.png", bbox_inches="tight")