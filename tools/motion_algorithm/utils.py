from pathlib import Path
import yaml


def get_config(conf_path):
    with open(conf_path, 'r') as file:
        conf = yaml.safe_load(file)
        return conf

def get_wheels_base(path):
    return get_config(path)['wheels_base']

def get_wheel_radius(path):
    return get_config(path)['wheel_radius']

def get_max_n(path):
    return get_config(path)['max_n']

def get_timestep_sec(path):
    return get_config(path)['timestep_sec']

def get_scene_margin_m(path):
    return get_config(path)['scene_margin_m']
