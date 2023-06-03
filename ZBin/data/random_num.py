import random
from mpl_toolkits import mplot3d
import numpy as np
import matplotlib.pyplot as plt

'''
count = 100

with open("num.txt",'w') as f:
    for i in range(count):
        a = random.random()
        b = random.random()
        c = (a+b)*2+(random.random())*0.4
        d = random.random()
        f.write("{:.2f}\t{:.2f}\t{:.2f}\t{:.2f}\n".format(a,b,c,d))

a = []
b = []
c = []

with open("num.txt",'r') as f:
    lines = f.readlines()
    for i,l in enumerate(lines):
        nums_str = l.replace('\n','').split('\t')
        assert(len(nums_str) == 4)
        nums = [float(n) for n in nums_str]
        a.append(nums[0])
        b.append(nums[1])
        c.append(nums[2])
        print(nums)
'''




'''
pack = xxxx().pack

pack.version
pack.name
for data in pack.data:
    a,b,c,d = data
{
    "pack":{
        "version": 1.1,
        "name": "components",
        "data":[
            [1,2,3,4],
            [2,1,3,4],
            [3,3,4,4,]
        ]
    }
}
'''

a = []
b = []
c = []
d = []
e = []

with open("anglevariable4000.txt",'r') as f:
    lines = f.readlines()
    for i,l in enumerate(lines):
        nums_str = l.replace('\n','').split()
        print(len(nums_str))
        assert(len(nums_str) == 5)
        nums = [float(n) for n in nums_str]
        a.append(nums[0])
        b.append(nums[1])
        c.append(nums[2])
        d.append(nums[3])
        e.append(nums[4])
        print(nums)

fig = plt.figure()
ax = plt.axes(projection='3d')
ax.scatter3D(a,b,e,c=c,cmap="cool")

plt.show()