import os
import time
import matplotlib.pyplot as plt
import matplotlib.animation as animation
from matplotlib.lines import Line2D
import numpy as np
import signal
import sys

class WhiskerScope():
    def __init__(self, fig, ax, maxt=2, dt=0.02, ylim=[], update_ylim=True, file='RawData.txt'):
        self.delete_all_stream_files()
        self.fig = fig
        self.file = file
        self.update_ylim = update_ylim
        self.dt = dt
        self.maxt = maxt
        self.ax = ax
        self.tdata = [0]
        self.ydata = [0]
        self.line = Line2D(self.tdata, self.ydata)
        self.ax.add_line(self.line)
        self.ylim = ylim
        self.ax.set_xlim(0, self.maxt)
        self.ax.set_ylim(ylim[0], ylim[1])
        self.ax.figure.canvas.draw()

    def update(self, y):
        lastt = self.tdata[-1]
        if lastt > self.tdata[0] + self.maxt:
            self.save_data(self.tdata, self.ydata, self.file)
            self.tdata = [self.tdata[-1]]
            self.ydata = [self.ydata[-1]]
            self.ax.set_xlim(self.tdata[0], self.tdata[0] + self.maxt)
            if self.update_ylim:
                self.ylim[0], self.ylim[1] = min(self.ydata) - min(self.ydata)*0.025, max(self.ydata) + 0.025 * max(self.ydata)
                if self.ylim[0] != self.ylim[1]:
                    self.ax.set_ylim(self.ylim[0], self.ylim[1])
            self.ax.figure.canvas.draw()

        t = np.linspace(lastt, lastt + self.dt, len(y))
        if len(t) == 1:
            self.tdata.append(t[0])
        else:
            self.tdata.extend(t)

        if len(y) == 1:
            self.ydata.append(y[0])
        else:
            self.ydata.extend(y)

        lim_change = False
        min_y = min(self.ydata)
        if min_y == 0:
            if len(self.ydata) > 1:
                list1 = self.ydata.copy()
                list1.sort()
                min_y = list1[1]

        max_y = max(self.ydata)
        #print(f'{min_y} = {self.ylim[0]}')

        if self.ylim[0] < 0:
            if self.ylim[0] > min_y and min_y != 0:
                lim_change = True
                self.ylim[0] = min_y + 0.08 * min_y
        elif self.ylim[0] > 0:
            if self.ylim[0] > min_y and min_y != 0:
                #print('Change ylimit')
                lim_change = True
                self.ylim[0] = min_y - 0.08 * min_y


        if self.ylim[1] < 0:
            if self.ylim[1] < max_y:
                lim_change = True
                self.ylim[1] = max_y + (-1) * 0.08 * max_y
        elif self.ylim[1] > 0:
            if self.ylim[1] < max_y:
                lim_change = True
                self.ylim[1] = max_y + 0.08 * max_y

        if self.ylim[1] - self.ylim[0] < 10:
            self.ylim[0] = -5
            self.ylim[1] = 5
            lim_change = True

        if lim_change:
            self.ax.set_ylim(self.ylim[0], self.ylim[1])
            self.ax.figure.canvas.draw()

        self.line.set_data(self.tdata, self.ydata)

        return self.line,

    def save_data(self, t, y, file):
        if 'raw' in file.lower():
            filename = f'stream_data_raw_t{int(t[0])}_t{int(t[-1])}'
        elif 'offset' in file.lower():
            filename = f'stream_data_offset_t{int(t[0])}_t{int(t[-1])}'
        elif 'thresh' in file.lower():
            filename = f'stream_thresh_data_t{int(t[0])}_t{int(t[-1])}'
        elif 'cusum' in file.lower():
            filename = f'stream_cusum_data_t{int(t[0])}_t{int(t[-1])}'
        elif 'signal' in file.lower():
            filename = f'stream_signal_data_t{int(t[0])}_t{int(t[-1])}'

        file = open(filename, 'w+')


        for i in range(len(t)):
            file.write(f'({t[i]},{y[i]})\n')

        file.close()

    def Stream(self):
        file = open(self.file, 'r+')
        graph_data = file.read()
        file.truncate(0)
        file.close()

        lines = graph_data.split('\n')
        xs = []
        ys = []
        for line in lines:
            if len(line)>1:
                x, y = line.split(',')
                xs.append(x)
                ys.append(y)
        ys = np.array(ys).astype(np.float)
        yield ys

    def delete_all_stream_files(self):
        path = os.getcwd()
        files = os.listdir(path)

        for file in files:
            if 'stream_' in file:
                os.remove(path + f'/{file}')

def close_down_event_handler(sig, frame):
    if isinstance(figures, list):
        for i in range(len(figures)):
            plt.close(figures[i])
    else:
        plt.close(figures)
    print('\nYou closed the plotting.')
    sys.exit(0)

signal_true = 'all'

if 'signal' in signal_true:
    fig, ax = plt.subplots()
    figures = fig
    signal.signal(signal.SIGINT, close_down_event_handler)
    ax.set_title('Signal')
    scope = WhiskerScope(fig, ax, 5, 0.025, [-5, 5], file='signal.txt')
    ani = animation.FuncAnimation(fig, scope.update, scope.Stream, interval=50,
                                  blit=True)

elif 'three' in signal_true:
    fig, ax = plt.subplots()
    fig2, ax2 = plt.subplots()
    fig3, ax3 = plt.subplots()
    figures = [fig, fig2, fig3]
    signal.signal(signal.SIGINT, close_down_event_handler)
    ax.set_title('Raw touch data')
    ax2.set_title('Offset Value')
    ax3.set_title('threshold')

    scope = WhiskerScope(fig, ax, 5, 0.025, [800, 950], file='RawData.txt')
    scope2 = WhiskerScope(fig2, ax2, 5, 0.025, [-0.3, 0.3], update_ylim=False, file='OffsetData.txt')
    scope3 = WhiskerScope(fig3, ax3, 5, 0.025, [-2, 2], file='thresh.txt')

    ani = animation.FuncAnimation(fig, scope.update, scope.Stream, interval=50,
                                  blit=True)
    ani2 = animation.FuncAnimation(fig2, scope2.update, scope2.Stream, interval=50,
                                   blit=True)
    ani3 = animation.FuncAnimation(fig3, scope3.update, scope3.Stream, interval=50,
                                   blit=True)

else:
    fig, ax = plt.subplots()
    fig2, ax2 = plt.subplots()
    fig3, ax3 = plt.subplots()
    fig4, ax4 = plt.subplots()
    figures = [fig, fig2, fig3, fig4]
    signal.signal(signal.SIGINT, close_down_event_handler)
    ax.set_title('Raw touch data')
    ax2.set_title('Offset Value')
    ax3.set_title('threshold')
    ax4.set_title('cusum')

    scope = WhiskerScope(fig, ax, 5, 0.025, [800, 950], file='RawData.txt')
    scope2 = WhiskerScope(fig2, ax2, 5, 0.025, [-5, 5], update_ylim=True, file='OffsetData.txt')
    scope3 = WhiskerScope(fig3, ax3, 5, 0.025, [-2, 2], file='thresh.txt')
    scope4 = WhiskerScope(fig4, ax4, 5, 0.025, [-5, 5], update_ylim=True, file='cusum.txt')

    ani = animation.FuncAnimation(fig, scope.update, scope.Stream, interval=50,
                              blit=True)
    ani2 = animation.FuncAnimation(fig2, scope2.update, scope2.Stream, interval=50,
                                blit=True)
    ani3 = animation.FuncAnimation(fig3, scope3.update, scope3.Stream, interval=50,
                                   blit=True)

    ani4 = animation.FuncAnimation(fig4, scope4.update, scope4.Stream, interval=50,
                                   blit=True)

plt.show()
