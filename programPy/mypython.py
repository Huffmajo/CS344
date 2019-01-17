#!/huffmajo/bin/env python
import sys
import string
import random

for i in range(3):
	randomLetters = ""
	f= open("file%d" % (i+1), "w+")
	for j in range(10):
		randomLetters += random.choice(string.ascii_lowercase)
	randomLetters += "\n"
	f.write(randomLetters)
	f.close()
	sys.stdout.write(randomLetters)

randomNum1 = random.randint(1, 42)
randomNum2 = random.randint(1, 42)
randomProd = randomNum1 * randomNum2
sys.stdout.write("%d\n" % randomNum1)
sys.stdout.write("%d\n" % randomNum2)
sys.stdout.write("%d\n" % randomProd)
