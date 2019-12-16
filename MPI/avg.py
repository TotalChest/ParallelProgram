dst_file = open('avg_results','w')

for i in ['mini','small','medium','large','extralarge']:
	dst_file.write('\n'+i.upper()+'\n')
	for j in [2,4,8,16,32,64]:
		c = 0
		for k in range(1,4):
			try:
				src = open(i.upper()+'/'+i+'_'+str(j)+'_'+str(k))
				for line in f:
					if 'Time in seconds' in line:
						c += float(line.split('= ')[1])
				src.close()
			except:
				pass
		dst_file.write(str(j)+' process\n')
		dst_file.write(str(c/3)+'\n')
dst_file.close()
