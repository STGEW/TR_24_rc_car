import json
import matplotlib.pyplot as plt
import json
import argparse
import logging
from pathlib import Path


logging.basicConfig(
    level=logging.DEBUG,
    format='%(asctime)s - %(name)s - %(levelname)s - %(message)s',
    handlers=[logging.StreamHandler()]
)


logger = logging.getLogger(__name__)


def load_json(file_path):
    with open(file_path, 'r') as file:
        return json.load(file)


def plot_data(file1_data, file2_data, name):
    # Extract data from the first file
    x1 = file1_data['x']
    y1 = file1_data['y']
    
    # Extract data from the second file
    x2 = file2_data['x']
    y2 = file2_data['y']

    plt.rcParams.update({'font.size': 20})

    plt.style.use('dark_background')
    
    # Create a plot
    plt.figure(figsize=(30, 18), num=name)
    
    # Plot data from the first file
    plt.plot(
        x1, y1,
        label='Original path',
        marker='o',
        linestyle='-',
        color='cyan',
        linewidth=3)
    
    # Plot data from the second file
    plt.plot(
        x2, y2,
        label='Restored path',
        marker='x',
        linestyle='--',
        color='magenta',
        linewidth=3)
    
    # Add labels and title
    plt.xlabel('X')
    plt.ylabel('Y')
    plt.title('Plot two pathes on XY plane')
    plt.legend()
    
    # Show the plot
    plt.grid(True)
    plt.show()



def main():
    logger.info('Parsing arguments')
    parser = argparse.ArgumentParser()
    parser.add_argument(
        '--path1',
        required=True,
        type=Path,
        help='Path 1')
    parser.add_argument(
        '--path2',
        required=True,
        type=Path,
        help='Path 2')
    parser.add_argument(
        '--name',
        required=True,
        type=str,
        help='Name for the window to differentiate')
    args = parser.parse_args()
    logger.info(f'Arguments are: {args}')

    # Plot the data
    plot_data(
        load_json(args.path1),
        load_json(args.path2),
        args.name)

if __name__ == '__main__':
    main()
