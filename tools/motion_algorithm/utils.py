from pathlib import Path
import yaml


def get_config(path):
    conf_path = Path('config.yml')
    with open(conf_path, 'r') as file:
        conf = yaml.safe_load(file)
        return conf

def get_wheels_base(path):
    get_config()['wheels_base']


def get_wheel_radius(path):
    get_config()['wheel_radius']


def get_max_n(path):
    get_config()['max_n']
