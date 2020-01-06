import numpy as np
import matplotlib.pyplot as plt

# file_x = open('./../../data/data_results_first_prototype/moving_whisker_pin15_first_prototype.txt', 'r+')
file_x = open('./datalog_pin15.txt', 'r+')
file_y = open('./datalog_offset_pin15.txt', 'r+')
data_x = file_x.read()
data_y = file_y.read()
file_x.close()
file_y.close()

def floatVec(vec):
    vec = [float(i) for i in vec]
    return vec

def CleanOutliers(vector):
    u = np.mean(vector)
    s = np.std(vector)
    filtered = [e for e   in vector if (u - 2 * s <= e <= u + 2 * s)]
    return filtered

plt.figure(1)
data_x = data_x.split(',')
print(len(data_x))
data_x = floatVec(data_x)
data_x = np.array(data_x)
data_x = CleanOutliers(data_x)
x = np.linspace(0, 10, len(data_x))
plt.plot(x, data_x)
plt.ylim(max(data_x)+10, min(data_x)-10)
plt.xlabel('Time [s]')
plt.ylabel('Touch value')
plt.title('Direct touch value')

plt.figure(2)
plt.title('Offset values')
print(len(data_y))
data_y = data_y.split(',')
data_y = floatVec(data_y)
data_y = np.array(data_y)
data_y = CleanOutliers(data_y)
y = np.linspace(0, 10, len(data_y))
plt.plot(y, data_y)
plt.ylim(max(data_y)+10, min(data_y)-10)
plt.ylabel('Touch label')
plt.xlabel('Time [s]')
plt.show()