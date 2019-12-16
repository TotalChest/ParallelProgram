src1 = open('polus_results.txt')
src2 = open('bluegene_results.txt')
dst = open('avg_result.txt','w')

dst.write('----------POLUS----------\n')

text_list = []
text_list = src1.readlines()

for i in range(0, len(text_list), 4):
    dst.write(text_list[i])
    dst.write( str( round( sum( map( float, text_list[i+1:i+4] ) )/3, 6 ) ) + '\n' )

dst.write('----------BLUEGENE----------\n')

text_list = []
text_list = src2.readlines()

for i in range(0, len(text_list), 4):
    dst.write(text_list[i])
    if text_list[i+1] != 'TL\n':
        dst.write( str( round ( sum( map( float, text_list[i+1:i+4] ) )/3, 6 ) ) + '\n')
    else:
        dst.write('TL\n')

src1.close()
src2.close()
dst.close()
