# distutils: language = c++
import logging
from time import sleep
from rich.logging import RichHandler
from codes cimport TERMINATED

cdef extern from "stdlib.h":  
    double drand48()  
    void srand48(long int seedval)
import os
logger = logging.getLogger("SN")

from simple_worker cimport Rectangle


cpdef SubstantiaNigra(Q):

    rec_ptr = new Rectangle(1, 2, 3, 4)  # Instantiate a Rectangle object on the heap

    rec_area = rec_ptr.getArea()

    srand48(146959595959)

    for i in range(10):
        # Compute
        for k in range(10):
            packet =(f'PID {os.getpid()}', f'iter {i}',f'packet {k}', <int>(10*drand48()))
            Q.send(packet)

        # Receive
        item = Q.recv()

    Q.send(TERMINATED)

    while True:
        item = Q.recv()
        if item == TERMINATED:
            break
    
