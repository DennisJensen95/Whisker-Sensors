from scipy import signal

num = [0.5, 0]
den = [1, 0.001]
fs = 8190
Ts = 1/fs
Gs = signal.TransferFunction(num, den, dt=Ts)
Gz = signal.cont2discrete([num, den], Ts, method='tustin')
print(Gz)
# print(f'Cutoff Frequency: {w}')


# Bandpass [50 80]
print("Badnpass [50 80]")
num = [0.0805*1.0e-06, 0, -0.2414*1.0e-06, 0, 0.2414*1.0e-06, 0, -0.0805*1.0e-06]
den = [1.0000, -5.9817, 14.9096, -19.8215, 14.8230, -5.9130, 0.9828]
# fs = 14816/5
fs = 20900/5
Ts = 1/fs
Gs = signal.TransferFunction(num, den, dt=Ts)
Gz = signal.cont2discrete([num, den], Ts, method='tustin')
print(Gz)

print("Badnpass [60 70]")
num = [1.0e-08 * 0.3001, 0, 1.0e-08 *(-0.9003), 0, 1.0e-08 *0.9003, 0, 1.0e-08 *(-0.3001)]
den = [1.0000, -5.9932, 14.9670, -19.9361, 14.9382, -5.9702, 0.9942]
# fs = 14816/5
fs = 20900/5
Ts = 1/fs
Gs = signal.TransferFunction(num, den, dt=Ts)
Gz = signal.cont2discrete([num, den], Ts, method='tustin')
print(Gz)


print("Badnpass [10 1000]")
num = [0.0022, 0, -0.0067, 0, 0.0067, 0, -0.0022]
den = [ 1.0000, -5.4280, 12.2944, -14.8785, 10.1494, -3.7008, 0.5635]
# fs = 14816/5
fs = 20900/5
Ts = 1/fs
Gs = signal.TransferFunction(num, den, dt=Ts)
Gz = signal.cont2discrete([num, den], Ts, method='tustin')
print(Gz)

print("Bandpass [100 1000]")
num = [0.0017, 0, -0.0052, 0, 0.0052, 0, -0.0017]
den = [1.0000, -5.4589, 12.4510, -15.1924, 10.4614, -3.8550, 0.5939]
# fs = 14816/5
fs = 20900/5
Ts = 1/fs
Gs = signal.TransferFunction(num, den, dt=Ts)
Gz = signal.cont2discrete([num, den], Ts, method='tustin')
print(Gz)

print("Bandpass [10 10000]")
num = [0.7720,         0,   -2.3160,         0,    2.3160,         0,   -0.7720]
den = [1.0000,   -0.5033,   -2.3665,    0.7511,    2.0147,   -0.3000,   -0.5960]
# fs = 14816/5
fs = 20900/5
Ts = 1/fs
Gs = signal.TransferFunction(num, den, dt=Ts)
Gz = signal.cont2discrete([num, den], Ts, method='tustin')
print(Gz)

print("Bandpass [1000 10000]")
num = [0.5765,         0,   -1.7294,         0,    1.7294,         0,   -0.57650]
den = [1.0000,    0.0671,   -1.9217,   -0.0708,    1.3550,    0.0224,   -0.3321]
# fs = 14816/5
fs = 20900/5
Ts = 1/fs
Gs = signal.TransferFunction(num, den, dt=Ts)
Gz = signal.cont2discrete([num, den], Ts, method='tustin')
print(Gz)

print("Bandpass [5 5000]")
num = [0.1362,         0,   -0.4087,         0,    0.4087,         0,   -0.1362]
den = [1.0000,   -3.2325,    4.0484,   -2.7685,    1.3439,   -0.4129,    0.0218]
# fs = 14816/5
fs = 20900/5
Ts = 1/fs
Gs = signal.TransferFunction(num, den, dt=Ts)
Gz = signal.cont2discrete([num, den], Ts, method='tustin')
print(Gz)

print("Bandpass [10 600]")
num = [0.0005,         0,   -0.0016,         0,    0.0016,         0,   -0.0005]
den = [1.0000,   -5.6584,   13.3491,  -16.8080,   11.9133,   -4.5071,    0.7110]
# fs = 14816/5
fs = 20900/5
Ts = 1/fs
Gs = signal.TransferFunction(num, den, dt=Ts)
Gz = signal.cont2discrete([num, den], Ts, method='tustin')
print(Gz)

print("Bandpass [1000 10000]")
num = [0.5765,         0,   -1.7294,         0,    1.7294,         0,   -0.5765]
den = [1.0000,    0.0671,   -1.9217,   -0.0708,    1.3550,    0.0224,   -0.3321]
# fs = 14816/5
fs = 20900/5
Ts = 1/fs
Gs = signal.TransferFunction(num, den, dt=Ts)
Gz = signal.cont2discrete([num, den], Ts, method='tustin')
print(Gz)

print("Bandpass [5000 10000]")
num = [0.1365,         0,   -0.4096,         0,    0.4096,         0,   -0.1365]
den = [1.0000,    2.4110,    2.3549,    1.5319,    0.9004,    0.3075,    0.0215]
# fs = 14816/5
fs = 20900/5
Ts = 1/fs
Gs = signal.TransferFunction(num, den, dt=Ts)
Gz = signal.cont2discrete([num, den], Ts, method='tustin')
print(Gz)


print("highpass")
num = [0.9997,   -2.9991,    2.9991,   -0.9997]
den = [1.0000,   -2.9994,    2.9988,   -0.9994]
# fs = 14816/5
fs = 20900/5
Ts = 1/fs
Gs = signal.TransferFunction(num, den, dt=Ts)
Gz = signal.cont2discrete([num, den], Ts, method='tustin')
print(Gz)

print("highpass, 9000")
num = [0.0528,   -0.1056,    0.0528]
den = [1.0000,    1.2531,    0.4643]
# fs = 14816/5
fs = 20900/5
Ts = 1/fs
Gs = signal.TransferFunction(num, den, dt=Ts)
Gz = signal.cont2discrete([num, den], Ts, method='tustin')
print(Gz)

print("highpass, 7000")
num = [0.1727,   -0.3454,    0.1727]
den = [1.0000,    0.5312,    0.2219]
# fs = 14816/5
fs = 20900/5
Ts = 1/fs
Gs = signal.TransferFunction(num, den, dt=Ts)
Gz = signal.cont2discrete([num, den], Ts, method='tustin')
print(Gz)

print("highpass, 6000")
num = [0.2472,   -0.4945,    0.2472]
den = [1.0000,    0.1889,    0.1779]
# fs = 14816/5
fs = 20900/5
Ts = 1/fs
Gs = signal.TransferFunction(num, den, dt=Ts)
Gz = signal.cont2discrete([num, den], Ts, method='tustin')
print(Gz)

print("highpass, 9500")
num = [0.0309,   -0.0618,    0.0309]
den = [1.0000,    1.4451,    0.5686]
# fs = 14816/5
fs = 20900/5
Ts = 1/fs
Gs = signal.TransferFunction(num, den, dt=Ts)
Gz = signal.cont2discrete([num, den], Ts, method='tustin')
print(Gz)

print("highpass, 9500")
num = [0.1684,   -0.1684]
den = [1.0000,    0.6633]
# fs = 14816/5
fs = 20900/5
Ts = 1/fs
Gs = signal.TransferFunction(num, den, dt=Ts)
Gz = signal.cont2discrete([num, den], Ts, method='tustin')
print(Gz)

print("lowpass, 1000")
num = [ 0.0172,    0.0344,    0.0172]
den = [1.0000,   -1.5960,    0.6649]
# fs = 14816/5
fs = 20900/5
Ts = 1/fs
Gs = signal.TransferFunction(num, den, dt=Ts)
Gz = signal.cont2discrete([num, den], Ts, method='tustin')
print(Gz)

print("Reject band, [0.1 100]")
num = [0.9792,   -3.9168,    5.8752,   -3.9168,    0.9792]
den = [1.0000,   -3.9592,    5.8785,   -3.8793,    0.9600]
# fs = 14816/5
fs = 20900/5
Ts = 1/fs
Gs = signal.TransferFunction(num, den, dt=Ts)
Gz = signal.cont2discrete([num, den], Ts, method='tustin')
print(Gz)

print("Reject band, [0.1 1000]")
num = [0.8152,   -3.2610,    4.8914,   -3.2610,   0.8152]
den = [1.0000,   -3.5960,    4.8570,   -2.9259,    0.6649]
# fs = 14816/5
fs = 20900/5
Ts = 1/fs
Gs = signal.TransferFunction(num, den, dt=Ts)
Gz = signal.cont2discrete([num, den], Ts, method='tustin')
print(Gz)

print("Bandpass, [0.1 1000]")
num = [0.0172,         0,   -0.0344,         0,    0.0172]
den = [1.0000,   -3.5960,    4.8570,   -2.9259,    0.6649]
# fs = 14816/5
fs = 20900/5
Ts = 1/fs
Gs = signal.TransferFunction(num, den, dt=Ts)
Gz = signal.cont2discrete([num, den], Ts, method='tustin')
print(Gz)



#
# num1 = [1, 0]
# den1 = [2, 1, 1]
# Gs1 = signal.TransferFunction(num1, den1, dt=Ts)
# Gz1 = signal.cont2discrete([num1, den1], Ts, method='tustin')
# print(Gz1)