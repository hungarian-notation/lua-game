luagame.window:resize(200, 100)

vector = luagame.vector
matrix = luagame.matrix

a = vector(0, 0, 0)

transform = matrix()

transform:rotate(math.pi / 2, 0, 0, 1)
transform:translate(10, 0, 0)

inverse = transform:get_inverse()

print(transform);
print(inverse);

b = transform * a

print( b )

c = inverse * b

print ( c )

print( inverse * transform )
