import sys

##START read config & set parameters
config_multi_file = 'config_mt.ini'

if len(sys.argv)==2 :
	config_multi_file = sys.argv[1]

parameters = {}
for line in open(config_multi_file) :
	lines = line.strip().split()
	#print lines
	if len(lines) == 0: pass
	elif len(lines) == 1 : parameters[lines[0][1:-1]]='default'
	else : parameters[lines[0][1:-1]]=lines[1]
else : print 'parameters have readen\n'

changables = ['alpha','alpha_df','beta','beta_df','beta_bg','gamma_bg','gamma_df','window_size','increamentMaxIter','sampleMaxIterEnt','sampleMaxIterIndic','indicatorMaxIter','selectMode','eta','null_one','hasDefault','menCxt','noUnlabel']

changables = map(lambda x:parameters[x].strip().split('|'),changables)

paras2index = dict()
paras = list()

para = [['']*len(changables)]
_th = [[0]*len(changables)]
ceng = 0
while True:
	if ceng == len(changables):
		paras2index[tuple(para)] = len(paras)
		paras.append(tuple(para))
		ceng -= 1
		if ceng == -1 :
			break
		_th[ceng] += 1
	else:
		if _th[ceng] == len(changables[ceng]):
			_th[ceng] = 0
			ceng -= 1
			if ceng == -1 :
				break
			_th[ceng] += 1
		else :
				para[ceng] = changeables[ceng][_th[ceng]]
				ceng += 1
print paras

# alphas = parameters[alpha].strip().split('|')
# alpha_dfs = parameters[alpha_df].strip().split('|')
# betas = parameters[beta].strip().split('|')
# beta_dfs = parameters[beta_df].strip().split('|')
# beta_bgs = parameters[beta_bg].strip().split('|')
# gamma_bgs = parameters[gamma_bg].strip().split('|')
# gamma_dfs = parameters[gamma_df].strip().split('|')
# window_sizes = parameters[window_size].strip().split('|')
# increamentMaxIters = parameters[increamentMaxIter].strip().split('|')
# sampleMaxIterEnts = parameters[sampleMaxIterEnt].strip().split('|')
# sampleMaxIterIndics = parameters[sampleMaxIterIndic].strip().split('|')
# indicatorMaxIters = parameters[indicatorMaxIter].strip().split('|')
# selectModes = parameters[selectMode].strip().split('|')
# etas = parameters[eta].strip().split('|')
# null_ones = parameters[null_one].strip().split('|')
# hasDefaults = parameters[hasDefault].strip().split('|')
# menCxts = parameters[menCxt].strip().split('|')
# noUnlabels = parameters[noUnlabel].strip().split('|')
 
####END read config & set parameters

# print '[code_folder]\t' + parameters[code_folder]
# print '[techIndex_folder]\t' + parameters[techIndex_folder]
# print '[intCorpus_]\t' + parameters[intCorpus_]
# print '[intTestCorpus]\t' + parameters[intTestCorpus]
# print '[iter_result_folder]\t' + parameters[iter_result_folder]
# print '[test_result_folder]\t' + parameters[test_result_folder]
# print '[workspace]\t' + parameters[workspace]
# print '[iter_result_folder]\t' + parameters[iter_result_folder]
# print '[iter_result_folder]\t' + parameters[iter_result_folder]
# print '[alpha]\t' + alpha 
# print '[alpha_df]\t' + alpha_df 
# print '[beta]\t' + beta 
# print '[beta_df]\t' + beta_df 
# print '[beta_bg]\t' + beta_bg 
# print '[gamma_bg]\t' + gamma_bg 
# print '[gamma_df]\t' + gamma_df 
# print '[window_size]\t' + window_size 
# print '[increamentMaxIter]\t' + increamentMaxIter 
# print '[sampleMaxIterEnt]\t' + sampleMaxIterEnt 
# print '[sampleMaxIterIndic]\t' + sampleMaxIterIndic 
# print '[indicatorMaxIter]\t' + indicatorMaxIter 
# print '[selectMode]\t' + selectMode 
# print '[eta]\t' + eta 
# print '[null_one]\t' + null_one 
# print '[hasDefault]\t' + hasDefault 
# print '[noUnlabel]\t' + noUnlabel 
# print '[menCxt]\t' + menCxt 
