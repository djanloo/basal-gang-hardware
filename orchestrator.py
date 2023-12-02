from basal_gang.bin.SN import SubstantiaNigra
from multiprocessing import Process
from multiprocessing import Pipe
import logging
from rich.logging import RichHandler
from basal_gang.bin.codes import Py_TERMINATE

logging.basicConfig(
    level="DEBUG",
    format="%(name)s - %(message)s",
    datefmt="[%X]",
    handlers=[RichHandler(rich_tracebacks=True)]
)

logger = logging.getLogger('ORCH')

SNin, SNout = Pipe()
p = Process(target=SubstantiaNigra, args=(SNout,))
p.start()

while True:
    logger.info("Polling..")
    if SNin.poll(timeout=0.1):
        item = SNin.recv()
        logger.info(f"Received {item}")
        SNin.send("Yessir")
        logger.info(f"Sent yassir signal")
        
        if item == Py_TERMINATE:
            break
    else:
        logger.info("No messages")

logger.info("Terminating")
SNin.send(Py_TERMINATE)

p.join()
