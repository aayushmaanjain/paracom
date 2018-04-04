# import os

import os
from random import randint
# random.randint(1,101)
def direxplore(root):
	for dirname, dirnames, filenames in os.walk(root):
		# print path to all subdirectories first.
		for subdirname in dirnames:
			print(os.path.join(dirname, subdirname))

		# print path to all filenames.
		for filename in filenames:
			print(os.path.join(dirname, filename))

		# Advanced usage:
		# editing the 'dirnames' list will stop os.walk() from recursing into there.
		if '.git' in dirnames:
			# don't go into any .git directories.
			dirnames.remove('.git')

def createdir(root, avgbranch, remdepth, words, fnum):
	fwordslen = 100
	if remdepth != 0 :
		for branch in range(avgbranch):
			dirname = root + str(branch)
			os.mkdir(dirname)
		subdirs = [x for x in os.listdir(root) if os.path.isdir(os.path.join(root,x))]
		for subdir in subdirs:
			createdir(root+subdir+"/", avgbranch, remdepth-1, words, fnum)
	else:
	# if random.randint(0,1) == 1:
		numfile = randint(1,5)
		fnum[0] = fnum[0] + numfile
		for i in range(numfile):
			fname = str(i)+".txt"
			# print(root+fname)
			fwords = [words[randint(0,int(len(words)/100))] for i in range(100)]
			with open(root+fname,'w') as outfile:
				for fword in fwords:
					outfile.write("%s\n" % fword)

def main():
	with open("words_alpha.txt") as wordfile:
		words = wordfile.readlines()
	words = [x.strip() for x in words]
	wlen = len(words)
	print(wlen)
	root = input("Input root directory name:")
	dirname = root + "/"
	# print(root)
	maxdepth = int(input("Max Depth: "))
	avgdepth = int(input("Avg Depth: "))
	avgbranch = int(input("Branch factor: "))
	fnum = [0]
	os.mkdir(root)
	createdir(dirname, avgbranch, avgdepth, words, fnum)
	print("files: "+str(fnum[0]))
	direxplore(root)
	# for depth in range(avgdepth):
	# 	# dirname = dirname + str(depth) + "/"
	# 	if depth != avgdepth - 1: 
	# 		for branch in range(avgbranch):
	# 			mkdir(dirname)
	# 	# print(dirname)

if __name__ == '__main__':
	main()