import matplotlib.pyplot as plt

from decimal import Decimal

file = 'data/data7.txt'
testName = 'FIFO - 0k - not loaded'

arr = []
arrI = []

with open(file) as f:
    f.readline()
    for i in range(1, 40):
        arr.append(Decimal(f.readline()))
        arrI.append(i)


for x in arr:
    print(x)


minY = min(arr)
maxY = max(arr)

plt.xlabel('sample batch')
plt.ylabel('time in system (micro s)')

plt.title(testName)


plt.plot(arrI, arr, 'ro')
plt.axis([0, 41, minY - 3000, maxY + 1000])
plt.savefig('plots/' + testName)
plt.show()

