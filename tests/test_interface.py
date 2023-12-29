from basal_gang.builder import SpikingNetwork

def test_yaml_builder():
    spikenet = SpikingNetwork.from_yaml("spiking.yaml")

def test_run():
    spikenet = SpikingNetwork.from_yaml("spiking.yaml")
    spikenet.interface.run(dt=0.1, time=10)


