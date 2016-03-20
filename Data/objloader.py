#!/usr/bin/python3
import sys
f=open(sys.argv[1])
inp=[ x.strip() for x in f.readlines()]
ver=[]
uv=[]
norm=[]
join=[]
ind=[]
uvp=True
di=dict()

for l in inp:
        st=l.split()
        if st[0]=='v':
                ver.append((float(st[1]),float(st[2]),float(st[3])))
        elif st[0]=='vt':
                uv.append((float(st[1]),float(st[2])))
        elif st[0]=='vn':
                norm.append((float(st[1]),float(st[2]),float(st[3])))
        elif st[0]=='f':
                pind=[]
                for pnt in (st[1],st[2],st[3]):
                        p=pnt.split('/')
                        if p[1]=='':
                                p[0]=int(p[0])
                                p[2]=int(p[2])
                                uvp=False
                                if (p[0],p[2]) not in di.keys():
                                        di[(p[0],p[2])]=len(join)
                                        join.append(ver[p[0]-1]+(0,0)+norm[p[2]-1])
                                pind.append(di[(p[0],p[2])])
                        else:
                                p[0]=int(p[0])
                                p[1]=int(p[1])
                                p[2]=int(p[2])
                                if (p[0],p[1],p[2]) not in di.keys():
                                        di[(p[0],p[1],p[2])]=len(join)
                                        join.append(ver[p[0]-1]+uv[p[1]-1]+norm[p[2]-1])
                                pind.append(di[(p[0],p[1],p[2])])
                ind.append(pind)

print(len(join),len(ind))
print('')
for v in join:
        for fl in v:
                print(fl,end=' ')
        print('')
print('')
for f in ind:
        print(f[0],f[1],f[2])
