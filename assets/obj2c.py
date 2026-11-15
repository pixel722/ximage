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
        continue
    if items[0] == "vt":
        continue
    if items[0] == "f":
        f1 = items[1].split("/")
        f2 = items[2].split("/")
        f3 = items[3].split("/")
        face = (
            (int(f1[0])-1, int(f1[1])-1, int(f1[2])-1),
            (int(f2[0])-1, int(f2[1])-1, int(f2[2])-1),
            (int(f3[0])-1, int(f3[1])-1, int(f3[2])-1),
        )
        faces.append(face)


c = open(f"{sys.argv[2]}", 'w')

#typedef struct
#{
#    int vertices_count;
#    int faces_count;
#    v3f *vertices;
#    int *faces;
#} model_t;

c.write(f"static v3f {name}_vertices[] = {{\n")
for v in vertices:
    c.write(f"    {{ {v[0]}f, {v[1]}f, {v[2]}f }},\n")
c.write("};\n")

c.write(f"static int {name}_faces[] = {{\n")
for f in faces:
    c.write(f"    {f[0][0]}, {f[1][0]}, {f[2][0]},\n")
c.write("};\n")

c.write(f"model_t {name} = {{\n")
c.write(f"    .vertices_count = {int(len(vertices) / 3)},\n")
c.write(f"    .faces_count = {len(faces)},\n")
c.write(f"    .vertices = {name}_vertices,\n")
c.write(f"    .faces = {name}_faces,\n")
c.write("};\n")

c.close()

print(f"Generated \"{name}.c\"")
