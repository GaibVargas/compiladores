
with open("gramatica_site.txt") as arquivo:

    linhas = arquivo.readlines()

with open("first.txt") as first:
    first = first.readlines()
    

conjunto_first = {}
for f in first:
    index = f.find('\t')
    
    primeiros = f[index:].split(',')

    conjunto_first[f[:index].strip()] = [g.strip() for g in primeiros] 

with open("follow.txt") as follow:
    follow = follow.readlines()
    

conjunto_follow = {}
for f in follow:
    index = f.find('\t')
    primeiros = f[index:].split(',')
    conjunto_follow[f[:index].strip()] = [g.strip() for g in primeiros] 
print(conjunto_follow)


try_table = {}
for linha in linhas:
    head,body = linha.split("->")    
    head = head.strip()
    # print(body)
    body = body.split("|")
    # print(body)
    for prod in body:
        # print(b)
        prod = prod.strip()
        chars = prod.split(' ')
        if chars[0] == 'EPSILON':
            for x in conjunto_follow[head]:
                if head in try_table:
                    if x in try_table[head]:
                        raise RuntimeError(f"DEU BOSTA COM {head}")
                    else:
                        try_table[head].append(x)
                else:
                    try_table[head] = list()
                    try_table[head].append(x)
            continue
        i=0
        while True:
            algo = chars[i]
            old = i
            for x in conjunto_first[algo]:
                if x == 'ε':
                    i+=1
                if head in try_table:
                    if x in try_table[head]:
                        raise RuntimeError("DEU BOSTA NO FIRST")
                    else:
                        try_table[head].append(x)
                else:
                    try_table[head] = list()
                    try_table[head].append(x)
            if i >= len(chars) or i==old:
                break
# print(try_table)




    