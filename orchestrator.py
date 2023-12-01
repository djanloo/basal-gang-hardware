from basal_gang.SN import SubstantiaNigra
from multiprocessing import Process
from multiprocessing import Pipe
import logging
from rich.logging import RichHandler

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
    item = SNin.recv()
    logger.info(f"Received {item}")
    SNin.send("Yessir")
    SNin.send(None)
    if item is None:
        break

p.join()
