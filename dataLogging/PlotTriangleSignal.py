import numpy as np
import pandas as pd
import matplotlib.pyplot as plt

def float_vec(x, y):
    x = np.array(x)
    y = np.array(y)
    for i in range(len(x)):
        x[i] = float(x[i])
        y[i] = float(y[i])


    return x, y

dataframe = pd.read_csv('./../../Sensitivity/charge_discharge_whisker_sensor.csv')

xlabel = dataframe.iloc[0, 0].strip('()')
ylabel = dataframe.iloc[0, 1]
x = dataframe.iloc[1:, 0]
y = dataframe.iloc[1:, 1]
x, y = float_vec(x, y)
# print(min)
# print(first_index)
# print(second_index)



# print(min(y)-0.1*min(y))
# print(y[np.where(y <= min(y)-0.01*min(y))])
# period = x[np.where(y <= min(y)-0.01*min(y))]
mark1 = x[np.where((y == 0) & (x >-296) & (x < -290))][0]
mark2 = x[np.where((y == 0) & (x >-285) & (x < -280))][0]
Ts = mark2*10**(-6) - mark1*10**(-6)

first_index = np.where(x >= -25)[0][0]
second_index = np.where(x > 25)[0][0]
x = x[first_index:second_index]
y = y[first_index:second_index]

idx = np.where(y == 0)
# print(y[idx])
# print(x[idx])
# print(idx[0][0])
first_t = x[idx[0][76]]
# print(first_t)
second_t = x[idx[0][0]]
# first_t = -17.188
# second_t = -3.1
# x = x[idx[0][:42]]
# y = y[idx[0][:42]]
# print(idx[0][0])

print(first_t - second_t)

print(1/((first_t - second_t)*10**(-3)))
print(max(y))
print(min(y))



# print(f'{1/Ts}Hz')





plt.plot(x, y)
plt.xlabel(f'Time [{xlabel}]')
plt.ylabel(f'Voltage [{ylabel}]')
plt.show()