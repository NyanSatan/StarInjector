import os, sys, struct, subprocess, datetime

linabuild = '/Volumes/Data/Projects/linabuild/linabuild2.py'
linabuild_db = '/usr/local/share/linabuild/starinject.db'

if __name__ == '__main__':
	if len(sys.argv) != 2:
		exit(-1)

	path = sys.argv[1]

	try:
		binary = open(path, 'r+')
	except Exception as e:
		print 'failed to open binary: %s' % (str(e))
		exit(-1)

	binary_buf = binary.read(0x320)
	length = os.path.getsize(path)


	build_tag_placeholder = 'BUILD_TAG_STRING'
	build_tag_addr = binary_buf.find(build_tag_placeholder)

	if build_tag_addr < 0:
		print 'failed to find build tag placeholder'
		exit(-1)

	build_tag = str()

	if os.path.exists(linabuild):
		build_tag = subprocess.check_output(['python3', linabuild, "StarInjector", linabuild_db, './src'])[:-1]
	else:
		build_tag = 'localbuild...%s...%s' % (os.environ.get('USER'), datetime.datetime.now())

	zeroes = bytearray(len(build_tag_placeholder))

	binary.seek(build_tag_addr)
	binary.write(zeroes)
	binary.seek(build_tag_addr)
	binary.write(build_tag)
