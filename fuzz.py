import sys
import random


if len(sys.argv) != 2:
    exit(1)

n = sys.argv[1]
print(n)

for _ in range(int(n)):
    x = round(random.uniform(-1*10**6, 10**6), 4)
    y = round(random.uniform(-1*10**6, 10**6), 4)
    print(x, y)
