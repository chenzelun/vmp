
in_path = r'classes.dex'
out_path = r'insns.txt'

start = 0x123c3c
size = 0x4b

with open(in_path, "rb")as f:
    buf = f.read()[start: start + size*2]

with open(out_path, "w")as f:
    s = '{'
    for b in buf:
        s+= '0x'
        d,r = divmod(b, 16)
        s+= str(hex(d))[2:] + str(hex(r))[2:] + ','

    s = s[:-1] + '}'
    f.write(s)

print('ok')

