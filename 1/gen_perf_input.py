import random

input_file = open("input.txt", "r")
lines = input_file.read().split()
input_file.close()

output_file = open("perf_input.txt", "w")
for i in range(int(1e6)):
    output_file.write(random.choice(lines))
    output_file.write("\n")

output_file.close()
