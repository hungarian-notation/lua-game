luagame.window:resize(200, 100)

vec = luagame.vector
mat = luagame.matrix

a = vec(1, 2, 3)

b = a * 2

c = b:get_homogenized()

print ("a: ", a)
print ("b: ", b)
print ("c: ", c)