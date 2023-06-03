from turtle import title
import numpy as np
import matplotlib.pyplot as plt

from math import *
from inioutput import add_iniMsg

is_simulation = 1

if(is_simulation):
    f = open("data_simu.txt",mode = "r" ,encoding = "utf-8")
else:
    f = open("data_real.txt",mode = "r" ,encoding = "utf-8")

raw_datas = [line.strip('\n').split() for line in f.readlines()]

#depend convert data type 
for i in range(0,len(raw_datas)):
    for j in range(0,len(raw_datas[i])):
        if(j == 2):
            raw_datas[i][j] = int(raw_datas[i][j])
        else:
            raw_datas[i][j] = float(raw_datas[i][j])
f.close()

#print(raw_datas)

#data is string.format("%d",lastpower) " " string.format("%.2f",maxballvel) " " receiver_number " " "\n"
#need to usually modify
car_number = 16
is_flat = 1
is_chip = 0

#inicialize values
A_flat = [[0,1,0]]*car_number
A_chip = [[0,1,0]]*car_number
print(A_flat)