# apk copying python script

# the role of this file is actually copying the apk/so files from src to dst
# based on the generated config file.
# all the apk/so files mentioned in the config file will be attempted to copied.
# at the end, it will print out stats for copying.

# therefore, controlling which apk files should be copied should be already done
# in the config file

# to understand what this does and how to use it, please check
# /doc/how_to_use_apkcopy_scripts.md




import os
from os import listdir
import re
import sys


CONFIG_FILE_NAME="apkcopy_generationlist.txt"



class bcolors:
    HEADER = '\033[95m'
    OKBLUE = '\033[94m'
    OKGREEN = '\033[92m'
    WARNING = '\033[93m'
    FAIL = '\033[91m'
    ENDC = '\033[0m'
    BOLD = '\033[1m'
    UNDERLINE = '\033[4m'



# check python verison. this code is for python 2
if sys.version_info[0] != 2:
	print "python is not verison 2.x. aborting"
	exit()






fd=open(CONFIG_FILE_NAME,'r')

readall=fd.readlines()

fd.close()

MSL_PATH=''
OUTPUT_SYSTEM_DIR=''

apklist=[]
solist=[]

for line in readall:

	# remove leading and trailing whitespaces
	line = line.strip()

	if len(line) == 0:
		continue

	# if '#' character exists at the front, then ignore this.
	if line[0]=='#':

		continue

	# split the line. the delimeter is , or =
	if '=' in line:
		items = line.split('=')
	else:
		items = line.split(',')
	

	# if the first item is..
	if items[0] == 'MSL_PATH':
		print line
		MSL_PATH=items[1]

	if items[0] == 'OUTPUT_SYSTEM_DIR':
		OUTPUT_SYSTEM_DIR=items[1]

	if items[0] == 'apk':
		apklist.append([items[1],items[2],items[3]])

	if items[0] == 'so':
		solist.append(items[1])


# ask the user if the OUTPUT_SYSTEM_DIR is correct
print "is the OUTPUT_SYSYTEM_DIR = "+ OUTPUT_SYSTEM_DIR +"? [y/n, y:default]"
userinput = raw_input()

if userinput is "":
	userinput='y'

if userinput == 'Y':
	userinput='y'
elif userinput == 'N':
	userinput='n'


if userinput == 'n':
	print "wrong OUTPUT_SYSTEM_DIR. abort"
	exit()
elif userinput is not "y":
	print "wrong user input. abort"
	exit()
else:
	print "\n"


## get info of what dirs exists in the target directory

sysappsdirlist = [ d for d in listdir(OUTPUT_SYSTEM_DIR) if os.path.isdir(OUTPUT_SYSTEM_DIR+"/"+d) ]

# filter the list so that only the aka related dirs remain. Either start with Apps or Musio
pattern = re.compile("^Apps|^Musio")
track_sysapp_dir_list = [ item for item in sysappsdirlist if pattern.match(item) ]


dstdir_len= len(track_sysapp_dir_list)



# start with copying apk list.

template = "{FILENAME:40}\t{SRC_RESULT:10}\t{DST_RESULT:10}\t{COPY_RESULT:10}"

#print template with titles
print template.format(FILENAME="FILENAME",SRC_RESULT="src apk",DST_RESULT="dst dir",COPY_RESULT="cp result")
apkfile_copypass_count=0

for item in apklist:
	
	# apklist item info
	#item[0]: buildtype
	#item[1]: output apk name
	#item[2]: system apk name

	srcfile_exist_flag=True
	dstdir_exist_flag=True

	srcfilepath=MSL_PATH+"/"+item[1]+"/build/outputs/apk/"+item[1]+"-"+item[0]+".apk"
	dstdirpath=OUTPUT_SYSTEM_DIR+"/"+item[2]
	dstfilepath=dstdirpath+"/"+item[2]+".apk"

	# check if srcfile exists
	
	if not os.path.isfile(srcfilepath):
		srcfile_exist_flag=False

	# check if dstdir exists
	if not os.path.isdir(dstdirpath):
		dstdir_exist_flag=False
	else:
		# if dstdir exists, then please pop off it from the track_sysapp_dir_list
		if item[2] in track_sysapp_dir_list:
			track_sysapp_dir_list.remove(item[2])
			
		else:
			print item[2]+" is not in track_list. this is weird!!!!"
			exit()

	# execute copy command
	cmd="cp "+srcfilepath+" "+dstfilepath+" 2> /dev/null"
	copyresult = os.system(cmd)
	cp_report=""
	

	failcolor=False
	# print report
	if srcfile_exist_flag:
		src_report="Y"
	else:
		src_report="N"

	if dstdir_exist_flag:
		dst_report="Y"
	else:
		dst_report="N"

	if copyresult == 0:
		cp_report="Y"
		apkfile_copypass_count+=1
	else:
		cp_report="N"
		if dstdir_exist_flag:
			failcolor=True
		

	title=item[1]+ "(" + item[2]+")"
	title=title[0:40]

	outputstr = template.format(FILENAME=title,SRC_RESULT=src_report,DST_RESULT=dst_report,COPY_RESULT=cp_report)
	
	if failcolor:
		outputstr = bcolors.FAIL + outputstr + bcolors.ENDC

	print outputstr


# if len(track_sysapp_dir_list) == 0:

print "** apk file copy report **"
print "number of destination AKA dirs: %s , copied apk files: %s\n" % (dstdir_len,apkfile_copypass_count)


# print "\n\nremaining tracklist: %s" % track_sysapp_dir_list

# print "destdir count: %s\n" % dstdir_len
# print "copied apk files: %s\n" % apkfile_copypass_count




	
# generate so copy script

sofile_plan_count=len(solist)
sofile_copy_count=0


for item in solist:

	splititems = item.split('/')
	filename=splititems[-1]


	srcfile_exist_flag=True
	srcfilepath=MSL_PATH+"/"+item
	destdir=OUTPUT_SYSTEM_DIR

	if not os.path.isfile(srcfilepath):
		srcfile_exist_flag=False


	# execute copy command
	cmd="cp "+srcfilepath+" "+destdir+" 2> /dev/null"
	copyresult = os.system(cmd)


	# print report
	if srcfile_exist_flag:
		src_report="Y"
	else:
		src_report="N"

	if copyresult == 0:
		cp_report="Y"
		sofile_copy_count+=1
	else:
		cp_report="N"



	block=""
	block+="cp $MSL_PATH/" + item + " $OUTPUT_SYSTEM_DIR 2> /dev/null\n"
	block+="\nif [ $? -eq 0 ]; then\n"

	title=item
	title=title[0:40]

	
	print template.format(FILENAME=title,SRC_RESULT=src_report,DST_RESULT="-",COPY_RESULT=cp_report)



print "** so file copy report **"
print "planned copy items: %s , copy success count: %s" % (sofile_plan_count,sofile_copy_count)






# print end
print "===end==="

