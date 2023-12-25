from basal_gang.bin import neur

net = neur.PySpikingNetwork("albert")

pop1 = neur.PyPopulation(20, 'aqif', net)
pop2 = neur.PyPopulation(20, 'aqif', net)
