import os
import yaml

from basal_gang.bin.neur import SpikingNetworkInterface, PopulationInterface, RandomProjectorInterface


class SpikingNetwork:

    def __init__(self):
        pass

    @classmethod
    def from_yaml(cls, yaml_file):
        net = cls()
        net.interface = SpikingNetworkInterface("dummy")

        net.yaml_file = yaml_file
        
        if not os.path.exists(net.yaml_file):
            raise FileNotFoundError("YAML file not found")
        
        with open(net.yaml_file, "r") as f:
            net.features_dict = yaml.safe_load(f)

        net.populations = dict()
        
        for pop in net.features_dict['populations']:
            net.populations[pop['name']] = PopulationInterface(pop['size'], pop['neuron_type'], net.interface )

        for proj in net.features_dict['projections']:
            projector = RandomProjectorInterface(**(proj['features']))
            efferent = net.populations[proj['efferent']]
            afferent = net.populations[proj['afferent']]
            efferent.project(projector.get_projection(efferent, afferent), afferent)
        
        return net