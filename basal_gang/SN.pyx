# distutils: language = c++
import logging
from codes cimport TERMINATED

cdef extern from "stdlib.h":  
    double drand48()  
    void srand48(long int seedval)
logger = logging.getLogger("SN")

from simple_worker cimport Rectangle


cpdef SubstantiaNigra(Q):

    rec_ptr = new Rectangle(1, 2, 3, 4)
    rec_area = rec_ptr.getArea()

    for i in range(10):
        # Compute
        for k in range(10):
            packet =(f'iter {i}',f'packet {k}', <int>(10*drand48()))
            Q.send(packet)

        # Receive
        item = Q.recv()

    Q.send(TERMINATED)

    while True:
        item = Q.recv()
        if item == TERMINATED:
            break
    
