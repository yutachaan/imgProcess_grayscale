# 第一引数に分類前のファイル，第二引数に分類後のファイル，第三引数にカテゴリ数を指定

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

cmap = plt.get_cmap("tab10")
for i in range(int(sys.argv[3])):
  for row in l2:
    if int(row[2]) == i + 1:
      plt.plot(float(row[0]), float(row[1]), marker='.', c=cmap(i))
plt.show()
