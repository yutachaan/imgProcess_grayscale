# 第一引数に分類前のファイル，第二引数に分類後のファイルを指定

import sys
import csv
import matplotlib.pyplot as plt

# 分類前のプロット
with open(sys.argv[1]) as f1:
  reader = csv.reader(f1)
  l1 = [row for row in reader]

for row in l1:
  plt.plot(float(row[0]), float(row[1]), marker='.', c='blue')
plt.show()

# 分類後のプロット
with open(sys.argv[2]) as f2:
  reader = csv.reader(f2)
  l2 = [row for row in reader]

for row in l2:
  if float(row[2]) == 0:
    plt.plot(float(row[0]), float(row[1]), marker='.', c='blue')
  elif float(row[2]) == 1:
    plt.plot(float(row[0]), float(row[1]), marker='.', c='red')
plt.show()
