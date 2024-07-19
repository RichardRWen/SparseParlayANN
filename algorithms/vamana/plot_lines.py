import matplotlib.pyplot as plt

file_id = 2

def read_data(filename):
    """
    Read data from a given filename. Each line in the file should contain
    space-separated values, each representing the y-value at subsequent
    20-unit intervals.
    """
    data = []
    with open(filename, 'r') as file:
        for line in file:
            # Convert each line to a list of floats
            line_data = list(map(float, line.split()))
            data.append(line_data)
    return data

def plot_data(data):
    """
    Plot each line from the data. Each line's x values are calculated
    based on the index of the y values multiplied by 20.
    """
    plt.figure(figsize=(10, 6))
    for n in range(len(data)):
        for line in data[n]:
            x_values = [i * 20 for i in range(len(line))]
            if len(data) > 1:
                plt.plot(x_values, line, color=line_color(n), linewidth=0.5, alpha=0.5)  # thin and slightly transparent lines
            else:
                plt.plot(x_values, line, linewidth=0.5, alpha=0.5)  # thin and slightly transparent lines

    plt.title(plot_title(file_id))
    plt.xlabel('Time')

    plt.savefig(file_name(file_id) + '.png', format='png', dpi=300)
    plt.close()

def line_color(select):
    if select == 0:
        return 'green'
    elif select == 1:
        return 'blue'
    else:
        return 'red'

def plot_title(select):
    if select == 0:
        return 'Distance comparisons'
    elif select == 1:
        return 'Beam min and max mips'
    elif select == 2:
        return 'Unvisited max mips'
    elif select == 3:
        return 'Number of visited nodes'
    else:
        return ''

def file_name(select):
    if select == 0:
        return 'num_dist_cmps'
    elif select == 1:
        return 'beam_profile'
    elif select == 2:
        return 'unvisited_profile'
    elif select == 3:
        return 'num_visited'
    else:
        return ''

if __name__ == "__main__":
    if file_id == 1:
        data1 = read_data('beam_min_profile.csv')
        data2 = read_data('beam_avg_profile.csv')
        data3 = read_data('beam_max_profile.csv')
        plot_data([data1, data2, data3])
    else:
        data = read_data(file_name(file_id) + '.csv')
        plot_data([data])
