import struct
import os
import sys


def fetch(fname):
	
	fd = os.open(fname, os.O_WRONLY)
	
	buff = read(fd,nbytes)
	
	
	close(fd)
	
	return buff
	
def parse(buff):
	
	
