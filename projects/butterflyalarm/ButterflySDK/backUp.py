import re
import os
import sys
import shutil

BACK_UP_DIR = "../History"

def backUp(argv):
	if len(argv) == 0:
		print "Project name not given!"
		return False
	pattern = re.compile(argv[0] + '_(\d{3}).zip')
	try:
		li = os.listdir(BACK_UP_DIR)
	except:
		print "Can't find back-up directory"
		return False
	try:
		li = [pattern.search(item).groups()[0] for item in li if pattern.search(item)]
		release = int(max(li)) + 1
	except:
		release = 1
	releaseStr = str(release)
	if (release < 10):
		releaseStr = "00" + releaseStr
	elif (release < 100):
		releaseStr = "0" + releaseStr
	try:
		srcName = argv[0] + ".zip"
		destName = BACK_UP_DIR + "/" + argv[0] + "_" + releaseStr + ".zip"
		shutil.copyfile(srcName, destName)
		print "\r\nCreated: " + destName + "\r\n"
	except IOError:
		print "Failed to copy file!"
		return False
	return True

if __name__ == "__main__":
	if (backUp(sys.argv[1:]) == False):
		sys.exit(1)