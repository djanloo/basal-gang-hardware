from basal_gang.builder import SpikingNetwork

def test_yaml_builder():
    SpikingNetwork.from_yaml("spiking.yaml")

def test_run():
    spikenet = SpikingNetwork.from_yaml("spiking.yaml")
    spikenet.interface.run(dt=0.1, time=10)

"""
Input/Output devices
"""
def test_spike_monitor():
    spikenet = SpikingNetwork.from_yaml("spiking.yaml")
    spikenet.populations['Albert'].monitorize_spikes()

def test_state_monitor():
    spikenet = SpikingNetwork.from_yaml("spiking.yaml")
    spikenet.populations['Albert'].monitorize_states()

def test_injector():
    spikenet = SpikingNetwork.from_yaml("spiking.yaml")
    spikenet.populations["Albert"].add_injector(0.5, 2)

if __name__=="__main__":
    
    import numpy as np
    import matplotlib.pyplot as plt 

    spikenet = SpikingNetwork.from_yaml("spiking.yaml")
    spikenet.populations["Albert"].add_injector(0.5, 10)
    spikenet.populations["Albert"].monitorize_spikes()
    spikenet.populations["Albert"].monitorize_states()
    spikenet.populations["MonaLisa"].monitorize_spikes()
    spikenet.populations["MonaLisa"].monitorize_states()

    spikenet.interface.run(dt=0.01, time=10)

    plt.plot(spikenet.populations['Albert'].get_data()["spikes"])
    plt.plot(spikenet.populations['MonaLisa'].get_data()["spikes"])

    states = np.array(spikenet.populations['Albert'].get_data()['states'])
    print(states.shape)
    plt.figure(2)
    for i in range(20):
        plt.plot(states[:, i, 0],marker=".")
        plt.title("V")
    plt.figure(3)
    for i in range(5):
        plt.plot(states[:, i, 1],marker=".")
        plt.title("gsyn_exc")

    plt.figure(4)
    for i in range(5):
        plt.plot(states[:, i, 2],marker=".")
        plt.title("gsyn_inh")

    plt.figure(5)
    for i in range(5):
        plt.plot(states[:, i, 3],marker=".")
        plt.title("u")

    states = np.array(spikenet.populations['MonaLisa'].get_data()['states'])

    plt.figure(6)
    for idx, feat in enumerate(["V", "gsyne", "gsyni", "u"]):
        plt.plot(states[:, 0, idx], label=feat)
    plt.title("MonaLisa")
    plt.legend()


    plt.show()
