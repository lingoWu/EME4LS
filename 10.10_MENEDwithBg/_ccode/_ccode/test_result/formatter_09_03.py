# usage:
# the first arg is config file
# or the first arg is '-r' and the other args are resFileNames


import sys
import re

#START read config & set parameters
config_file = 'config.ini'

resNames = list()
if len(sys.argv)>=2 :
	if sys.argv[1] != '-r':
		config_file = sys.argv[1]
	else :
		resNames = map(lambda i:sys.argv[i],xrange(2,len(sys.argv)))

parameters = {}
for line in open(config_file) :
	lines = line.strip().split()
	#print lines
	if len(lines) == 0: pass
	elif len(lines) == 1 : parameters[lines[0][1:-1]]='default'
	else : parameters[lines[0][1:-1]]=lines[1]
else : print 'parameters have readen\n'

if(len(resNames) == 0):
	resNames = parameters['resFiles'].strip().split(',')
testFiles = parameters['testFiles'].strip().split(',')

sys.path.append(parameters['preprocessDir'])
import word_list

###END read config & set parameters

# \/ parse the readLine then,return a list 
## like [1, ' || china:0.10000 || china^,:0.00100']
def getRes(resLine ):
	res = map(lambda x:x.strip(),resLine.strip().split('||'))
	if len(res) < 2:
		return [-3,'\n']
	res = [ int(res[1]) , reduce ( lambda x,y:x+y,
		map(
		lambda x:' || '+word_list.getEnWord(int(x[0:x.find(':')]))+x[x.find(':'):]
			,res[2:])) + '\n']
	if(parameters.get('debug',-1)>=0): print res[0],word_list.getCnWord(res[0])
	return res

# \/ trans the res files
for f in xrange(0,len(testFiles)):
	resFile = open(resNames[f],'r')
	resLine = resFile.readline()
	while (resLine.strip()[0] == '#'): 
		# Corpus remark: 110+20K,test20fl_wa,5~20000,lq3,Exc3more,NotExcEnStop,withTest_nist5,withExtCn_dict,NOTRANS=-1,null_mention+ & time: 2015-09-03_16.28.12
		if 'Corpus' in resLine :
			match = re.compile(r'time\s*:(.*)').search(resLine)
			print 'Bingo'
			if match:
			 	strDate = match.group(1).strip()
				word_list.readDict( strDate , bool(int(parameters['ExtCnWord'])))
		resLine = resFile.readline()
	thisRes = getRes(resLine) 
	outFile = open(resNames[f]+'_','w')
	for testLine in open(testFiles[f],'r'):
		outFile.write(testLine)
		if '</seg>' in testLine:
			outFile.write('</BgLDAmodel>\n')
			sourseWords = testLine[testLine.find('>')+1:testLine.rfind('<')].strip().split()
			i = 0
			for sourseWord in sourseWords:
				if word_list.getCnWord(thisRes[0]) == sourseWord:
					outFile.write('M || '+str(i)+thisRes[1])
					resLine = resFile.readline()
					thisRes = getRes(resLine) 
				i += 1
			outFile.write('</BgLDAmodel>\n')
