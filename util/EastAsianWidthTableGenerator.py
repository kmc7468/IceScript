import os
import sys
import urllib.request

if not os.path.exists("../src/Encoding"):
	os.mkdir("../src/Encoding")
elif os.path.exists("../src/Encoding/EastAsianWidthTable.txt"):
	sys.exit(0)

res = urllib.request.urlopen("http://www.unicode.org/Public/UCD/latest/ucd/EastAsianWidth.txt")

if res.getcode() == 200:
	string = res.read().decode("utf8")
	stringLines = string.splitlines()

	data = list()

	for line in stringLines:
		if len(line) == 0 or line[0] == '#':
			continue

		temp = line.split(' ')[0].split(';')

		table = temp[0]
		tableBegin = 0
		tableEnd = 0

		if ".." in table:
			tableSplited = table.split("..")
			tableBegin = int(tableSplited[0], 16)
			tableEnd = int(tableSplited[1], 16)
		else:
			tableBegin = int(table, 16)
			tableEnd = tableBegin

		if temp[1] == 'W':
			dataLen = len(data)
			if dataLen != 0 and data[-1][1] == 'F':
				data[-1] = ((data[-1][0][0], tableEnd), 'F')
			else:
				data.append(((tableBegin, tableEnd), 'F'))
		else:
			dataLen = len(data)
			if dataLen != 0 and data[-1][1] == 'H':
				data[-1] = ((data[-1][0][0], tableEnd), 'H')
			else:
				data.append(((tableBegin, tableEnd), 'H'))

	file = open("../src/Encoding/EastAsianWidthTable.txt", "w")
	
	for element in data:
		file.write("{{ {{ 0x{}, 0x{} }}, '{}' }},\n".format(element[0][0], element[0][1], element[1]))

	file.close()
else:
	sys.exit(1)