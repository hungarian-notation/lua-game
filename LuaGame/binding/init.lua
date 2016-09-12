luagame.window:resize(200, 100)

vector = luagame.vector
matrix = luagame.matrix

a = vector(0, 0, 0, 1)

transform = matrix()

transform:rotate(math.pi / 2, 0, 0, 1)
transform:translate(10, 0, 0)

print(transform);

print( transform * a )
