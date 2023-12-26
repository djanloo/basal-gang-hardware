from basal_gang.bin import neur

net = neur.PySpikingNetwork("albert")

pop1 = neur.PyPopulation(1000, 'aqif', net)
pop2 = neur.PyPopulation(1000, 'aqif', net)

projector = neur.RandomProjector(inh_fraction=0.05, exc_fraction=0.1, 
                                 max_exc=0.2, max_inh=0.1,
                                 min_delay=0.2, max_delay=0.6)

projection12 = projector.get_projection(pop1, pop2)
pop1.project(projection12, pop2)

projection21 = projector.get_projection(pop1, pop2)
pop2.project(projection21, pop1)

net.run(dt=0.1, time=2)




