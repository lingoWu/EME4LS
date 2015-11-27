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
		resNames = sys.argv[2:]

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

sys.path.append(parameters['preprocessDir'])
import word_list
import utili

###END read config & set parameters

def change2en(m):
	return '|| '+ word_list.getEnWord(int(m.group(1).strip())) + ':' + m.group(2)

def change2cn(m):
	return '|| '+ word_list.getCnWord(int(m.group(1).strip())) +' ||'

for resName in resNames:
	outfile = open(resName+'_','w')
	for line in open(resName,'r'):
		# Corpus remark: 110+20K,test20fl_wa,5~20000,lq3,Exc3more,NotExcEnStop,withTest_nist5,withExtCn_dict,NOTRANS=-1,null_mention+ & time: 2015-09-03_16.28.12
		if line.strip()[0] == '#':
			if 'Corpus' in line :
				match = re.compile(r'time\s*:(.*)').search(line)
				if match:
				 	strDate = match.group(1).strip()
					word_list.readDict( strDate , bool(int(parameters['ExtCnWord'])))
		else :
			line = (re.compile(re.compile(r'\|\|([^\|]+):([^\|]+)'))).sub(change2en, line)
			line = (re.compile(r'\|\|([^\|:]+)\|\|')).sub(change2cn, line)
			outfile.write(line)
	outfile.close()

for resName in resNames:
	right = 0
	all = 0
	resfile = open(resName+'_ACR','w')
	for line in open(resName,'r'):
		# Corpus remark: 110+20K,test20fl_wa,5~20000,lq3,Exc3more,NotExcEnStop,withTest_nist5,withExtCn_dict,NOTRANS=-1,null_mention+ & time: 2015-09-03_16.28.12
		if line.strip()[0] == '#':
			#if 'Corpus' in line :
			#	match = re.compile(r'time\s*:(.*)').search(line)
			#	if match:
			#	 	strDate = match.group(1).strip()
			#		word_list.readDict( strDate , bool(int(parameters['ExtCnWord'])))
			if 'testFilename' in line :
				ansFilename = line.strip()[line.strip().rfind(utili.getSeparator())+1:] + '_ANS'
				ans_file = open(sys.path[-1] + utili.getSeparator() + strDate \
												+ utili.getSeparator() + ansFilename ,'r')
		else :
			ansLine = ans_file.readline()
			ans = ansLine.strip().split()
			cn = (re.compile(r'\|\|([^\|:]+)\|\|')).search(line).group(1).strip()
			assert(cn == ans[0])
			enM = '0'
			enP = 0
			for m in (re.compile(re.compile(r'\|\|([^\|]+):([^\|]+)'))).finditer(line):
				#print m.group(1).strip()
				if float(m.group(2).strip()) > enP:
					enP = float(m.group(2).strip())
					enM = m.group(1).strip()
			if enM == ans[1]:
				right += 1
			all += 1
			resfile.write(line + enM + '\n' + ansLine)
	resfile.write('ACrate %d / %d = %f' %(right,all,1.0*right/all))
	resfile.close()


