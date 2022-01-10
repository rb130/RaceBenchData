import struct
import random
import sys

dim = int(sys.argv[1])
n = int(sys.argv[2])
out = sys.argv[3]

data = [random.random() for _ in range(n*dim)]
data = [struct.pack("<f", x) for x in data]

with open(out, "wb") as f:
    f.write(b''.join(data))
