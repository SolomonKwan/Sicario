import sys
import re

def compare(name: str):
	expected = open("./output/" + name + ".txt", 'r').readlines()
	actual = open("./output/" + name + ".tmp", 'r').readlines()
	i, j = 0, 0
	while True:
		if expected[i].startswith("regex info"):
			i, j = regexCompare(i, j, expected, actual)
			if i == False and j == False: return False
		elif expected[i].startswith("regex "):
			pattern = re.compile(expected[i].split("regex ")[1])
			if not pattern.match(actual[j]): return False
			i += 1
			j += 1
		elif not expected[i] == actual[j]:
			return False
		else:
			i += 1
			j += 1
		if i >= len(expected) or j >= len(actual): break
	return j == len(actual) and i == len(expected)

def regexCompare(i, j, expected, actual):
	pattern = re.compile(expected[i].split("regex ")[1])
	counts = 0
	while True:
		if j >= len(actual):
			return False, False
		elif pattern.match(actual[j]):
			j += 1
		else:
			i += 1
			break
		counts += 1
	if counts == 0: return False, False
	return i, j

if __name__ == "__main__":
	if compare(sys.argv[1]):
		exit(0)
	else:
		exit(1)