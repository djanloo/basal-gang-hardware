from basal_gang.build import SpikingBuilder

spikingbuilder = SpikingBuilder("spiking.yaml")
spikingbuilder.build()
spikingbuilder.network.run(dt=0.1, time=10)


