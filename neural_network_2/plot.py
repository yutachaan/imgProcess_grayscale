# 第一引数にエラー変移のファイルを指定

import sys
import csv
import matplotlib.pyplot as plt

with open(sys.argv[1]) as f:
  reader = csv.reader(f)
  l = [row for row in reader]
x = [float(row[0]) for row in l]
y = [float(row[1]) for row in l]

plt.plot(x, y, c='red', marker='x', markersize=3)
plt.xlabel('Number of training')
plt.ylabel('Error')
plt.show()
