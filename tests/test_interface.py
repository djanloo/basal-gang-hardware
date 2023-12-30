from basal_gang.builder import SpikingNetwork

def test_yaml_builder():
    spikenet = SpikingNetwork.from_yaml("spiking.yaml")

def test_injector():
    spikenet = SpikingNetwork.from_yaml("spiking.yaml")
    spikenet.populations["Albert"].add_injector(10.5, 2.1)
    spikenet.populations["Albert"].monitorize()
    spikenet.populations["MonaLisa"].monitorize()
    
    spikenet.interface.run(dt=0.2, time=50)
    return spikenet

def test_run():
    spikenet = SpikingNetwork.from_yaml("spiking.yaml")
    spikenet.interface.run(dt=0.1, time=10)

if __name__=="__main__":
    import matplotlib.pyplot as plt 

    evolved_spikenet = test_injector()
    plt.plot(evolved_spikenet.populations['Albert'].get_data())
    plt.plot(evolved_spikenet.populations['MonaLisa'].get_data())
    plt.show()
