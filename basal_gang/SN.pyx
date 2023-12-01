import logging
from time import sleep
from rich.logging import RichHandler

cdef extern from "stdlib.h":
    double drand48()
    void srand48(long int seedval)

logger = logging.getLogger("SN")

cpdef SubstantiaNigra(Q):

    srand48(146959595959)
    for i in range(10):
        # Compute
        for k in range(10):
            Q.send(<int>(10*drand48()))

        # Send
        Q.send("a")

        # Receive
        if Q.poll(1):
            while True:
                item = Q.recv()
                if item is None:
                    break
                else:
                    logger.info(f"received {item}")
        else:
            logger.warning(f"time {i}: No incoming MSGS")
    Q.send(None)
