import yaml
import os

from basal_gang.bin import neur


class SpikingBuilder:
    def __init__(self, yaml_file):
        self.yaml_file = yaml_file
        
        if not os.path.exists(self.yaml_file):
            raise FileNotFoundError("YAML file not found")
        
        with open(self.yaml_file, "r") as f:
            self.features_dict = yaml.safe_load(f)

        self.network = neur.PySpikingNetwork("aaa")
        self.populations = dict()
        
    def build(self):
        for pop in self.features_dict['populations']:
            self.populations[pop['name']] = neur.PyPopulation(pop['size'], pop['neuron_type'], self.network)

        for proj in self.features_dict['projections']:
            projector = neur.RandomProjector(**(proj['features']))
            efferent = self.populations[proj['efferent']]
            afferent = self.populations[proj['afferent']]
            efferent.project(projector.get_projection(efferent, afferent), afferent)

