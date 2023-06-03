
import random
from matplotlib import pyplot as plt
from mpl_toolkits.mplot3d import Axes3D

fig = plt.figure()
ax1 = mpl_axes(projection='3d')


#count = 20
'''
with open("num.txt",'w') as f:
    for i in range(count):
        a = random.random()
        b = random.random()*10
        c = random.random()*0.1
        d = random.random()
        f.write("{:.2f}\t{:.2f}\t{:.2f}\t{:.2f}\n".format(a,b,c,d))
'''

with open("anglevariable3000.txt",'r') as f:
    lines = f.readlines()
    for i,l in enumerate(lines):
        nums_str = l.replace('\n','').split()
        assert(len(nums_str) == 3)
        a,b,c = [float(n) for n in nums_str]
        print(a,b,c)

'''
with open("num.txt",'r') as f:
    lines = f.readlines()
    for i,l in enumerate(lines):
        nums_str = l.replace('\n','').split('\t')
        print(len(nums_str) )
        assert(len(nums_str) == 4)
        a,b,c,d = [float(n) for n in nums_str]
        print(a,b,c,d)
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
