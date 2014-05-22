#!/usr/bin/python
""" Implementation code for Project 2 in Randomized Algorithms
(Fall 2014).
    Authors: Martin Storgaard, Jon Bjerrum Jacobsen,
             Anders Strand-Holm Vinther
    E-mails: {201400002, 20117331, 20033980}@post.au.dk
"""

import string
import random
import sys
random.seed("Martin Storgaard and Jon Bjerrum Jacobsen and Anders Strand-Holm Vinther" + sys.argv[1] + sys.argv[2])

ascii = string.digits + " " + string.letters + string.punctuation


n = int(sys.argv[1])
ab = sys.argv[2]

with open('data' + str(n) + ab, 'w') as f:
	for i in range(10**n):
		s = ''
		for i in range(80):
			s += random.choice(ascii)
		f.write(s + "\n")
		
