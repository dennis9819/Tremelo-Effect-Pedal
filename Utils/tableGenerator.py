import math 

num_points = 256
max_val = 255
line_length = 16
points_bump = []

for ix in range(0,num_points):
    val = math.ceil(math.sin(math.pi * (ix/num_points)) * max_val)
    points_bump.append(val)

c = 0
for val in points_bump:
    print (val, end=', ')
    c += 1
    if c > line_length -1:
        print('')
        c = 0