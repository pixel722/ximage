#!/usr/bin/python3
import sys

if len(sys.argv) != 4:
    print("Using: ./obj2c.py <input> <output> <name>")
    sys.exit()

name = sys.argv[3]

with open(sys.argv[1], 'r') as file:
    lines = file.readlines()

vertices = []
normales = []
textures = []
faces = []

for line in lines:
    items = line.split()

    if len(items) == 0:
        continue
    if items[0] == "#":
        continue
    if items[0] == "o":
        continue
    if items[0] == "v":
        v = (float(items[1]), float(items[2]), float(items[3]))
        vertices.append(v)
    if items[0] == "vn":
        vn = (float(items[1]), float(items[2]), float(items[3]))
        normales.append(vn)
    if items[0] == "vt":
        vt = (float(items[1]), float(items[2]))
        textures.append(vt)
    if items[0] == "f":
        f1 = items[1].split("/")
        f2 = items[2].split("/")
        f3 = items[3].split("/")
        f = (
            (int(f1[0])-1, int(f1[1])-1, int(f1[2])-1),
            (int(f2[0])-1, int(f2[1])-1, int(f2[2])-1),
            (int(f3[0])-1, int(f3[1])-1, int(f3[2])-1),
        )
        faces.append(f)


c = open(f"{sys.argv[2]}", 'w')

#typedef struct
#{
#    int vertices_count;
#    int normales_count;
#    int textures_count;
#    int faces_count;
#
#    v3f *vertices;
#    v3f *normales;
#    v2f *textures;
#    int *faces;
#} model_t;

c.write(f"static v3f {name}_vertices[] = {{\n")
for v in vertices:
    c.write(f"    {{ {v[0]}f, {v[1]}f, {v[2]}f }},\n")
c.write("};\n")

c.write(f"static v3f {name}_normales[] = {{\n")
for vn in normales:
    c.write(f"    {{ {vn[0]}f, {vn[1]}f, {vn[2]}f }},\n")
c.write("};\n")

c.write(f"static v2f {name}_textures[] = {{\n")
for vt in textures:
    c.write(f"    {{ {vt[0]}f, {vt[1]}f }},\n")
c.write("};\n")

c.write(f"static int {name}_faces[] = {{\n")
for f in faces:
    #             v1         vn1        vt1        v2         vn2        vt2        v3         vn3        vt3
    c.write(f"    {f[0][0]}, {f[0][1]}, {f[0][2]}, {f[1][0]}, {f[1][1]}, {f[1][2]}, {f[2][0]}, {f[2][1]}, {f[2][2]},\n")
c.write("};\n")

c.write(f"model_t {name} = {{\n")
c.write(f"    .vertices_count = {int(len(vertices))},\n")
c.write(f"    .normales_count = {int(len(normales))},\n")
c.write(f"    .textures_count = {int(len(textures))},\n")
c.write(f"    .faces_count = {len(faces) * 9},\n")
c.write(f"    .vertices = {name}_vertices,\n")
c.write(f"    .normales = {name}_normales,\n")
c.write(f"    .textures = {name}_textures,\n")
c.write(f"    .faces = {name}_faces,\n")
c.write("};\n")

c.close()

print(f"Generated \"{name}.c\"")
