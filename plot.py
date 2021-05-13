import matplotlib.pyplot as plt

from decimal import Decimal

file = 'data-affinity/data9.txt'

arr = []
arrI = []

with open(file) as f:

    f.readline()
    for i in range(1, 41):
        arr.append(Decimal(f.readline()))
        arrI.append(i)
    testName = f.readline().rstrip('\n')


for x in arr:
    print(x)

minY = min(arr)
maxY = max(arr)

plt.xlabel('sample batch')
plt.ylabel('time in system (micro s)')

plt.title(testName)


plt.plot(arrI, arr, 'ro')
plt.axis([0, 41, minY - 3000, maxY + 1000])
plt.savefig('plots/affinity/' + testName)
plt.show()

