#! /usr/bin/python
from os import listdir
import subprocess
from optparse import OptionParser
import sys

max_major = 1<<11

def get_majors():
	devices = listdir('/sys/dev/char')
	majors = [ int(d.split(':')[0]) for d in devices ]
	return majors

def unused_majors():
	unused = []
	majors = get_majors()
	for i in range(1,max_major):
		if not (i in majors):
			unused.append(i)
	return unused
			
def launch(major,
		irq,
		irqflags,
		target_path,
		driver_name,
		timeout_msec,
		delay_msec,
		delay_usec):
	
	major = int(major)
	irq = int(irq)
	irqflags = int(irqflags)
	
	
	cmd = 'insmod irqm.ko'
	cmd += ' major=%i irq=%i irqflags=%i target_path=%s driver_name=%s' % (major,irq,irqflags,target_path,driver_name)
	cmd += ' timeout_msec=%i delay_msec=%i delay_usec=%i' %(timeout_msec,delay_msec,delay_usec)

	print cmd
	subprocess.call(cmd,shell=True)
	
	cmd = 'mknod /dev/%s c %i 0'%(driver_name,major)
	subprocess.call(cmd,shell=True)
	print cmd
	
	cmd = 'chmod 777 /dev/%s' % (driver_name)
	subprocess.call(cmd,shell=True)
	print cmd
	
if __name__ == '__main__':

	parser = OptionParser()
	parser.add_option('--irq',dest='irq', default='0')
	parser.add_option('--irqflags',dest='irqflags', default=0x0080)
	parser.add_option('--dev',dest='target_dev', default='/dev/null')
	parser.add_option('--name',dest='name',default='acb')
	parser.add_option('--delay_usec',dest='delay_usec',default=0)
	parser.add_option('--delay_msec',dest='delay_msec',default=0)
	parser.add_option('--timeout_msec',dest='timeout_msec',default=1)

	parser.add_option('--unload',dest='unload',action='store_true',default=False)

	o,r = parser.parse_args()
	
	m = unused_majors()[0]
	
	if(o.unload):
		cmd = "rmmod irqm"
		subprocess.call(cmd,shell=True)
		print cmd
		cmd = "rm /dev/%s"%o.name
		subprocess.call(cmd,shell=True)
		print cmd
		sys.exit()
	
	launch(m, o.irq, o.irqflags,
			o.target_dev,
			o.name,
			o.timeout_msec,
			o.delay_msec,
			o.delay_usec)
			
			
	
