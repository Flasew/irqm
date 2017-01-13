import numpy as np
from ctypes import *

lib = CDLL(read.so)

class event_record(Structure):

	_fields_ = [

		('irq_user',c_int),
		('t_user',c_int),
		('msg_user',c_char_p),
		('msg_len',c_int),

		('irq_isr',c_int),
		('t_isr',c_int),

		('irq_dispatch',c_int),
		('t_dispatch', c_int),
		('msg_dispatch',c_char_p),
		('t_flush',c_int),
		('bytes_sent',c_int)

	]

class log(object):

	def __init__(self,fname):

		self.irq_user = np.array([],dtype='int64')
		t_user = np.array([],dtype='int64')
		msg_user = []
		msg_len = np.array([],dtype='int64')

		irq_isr = np.array([],dtype='int64')
		t_isr = np.array([],dtype='int64')

		irq_dispatch = np.array([],dtype='int64')
		t_dispatch = np.array([],dtype='int64')
		msg_dispatch = []
		t_flush = np.array([],dtype='int64')
		bytes_sent = np.array([],dtype='int64')

		self.len = lib.log_len
		self.c_fname = create_string_buffer(fname)
		

	def fetch(self):

		irq_user = []
		t_user = []
		msg_user = []
		msg_len = []

		irq_isr = []
		t_isr = []

		irq_dispatch = []
		t_dispatch = []
		msg_dispatch = []
		t_flush = []
		bytes_sent = []

		buf = lib.get(self.c_fname)

		for e in self.buf:

			irq_user.append(int(self.buf.irq_user))
			t_user.append(int(self.buf.t_user))
			msg_user.append(str(self.buf.msg_user))
			msg_len.append(int(self.buf.msg_len))

			irq_isr.append(int(self.buf.irq_isr))
			t_isr.append(int(self.buf.t_isr))

			irq_dispatch.append(int(self.buf.irq_dispatch))
			t_dispatch.append(int(self.buf.t_dispatch))
			msg_dispatch.append(str(self.buf.msg_dispatch))
			t_flush.append(int(self.buf.t_flush))
			bytes_sent.append(int(self.buf.bytes_sent))

