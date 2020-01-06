f_avg = 345
f_raw = 8482

t_const_avg = 4000
t_const_raw = 100000

avg_first_val = 600*t_const_avg
avg_second_val = 620*t_const_avg
diff = (avg_second_val - avg_first_val)
time_to_correct_avg = diff/f_avg

raw_first_val = 600*t_const_raw
raw_second_val = 620*t_const_raw
diff = (raw_second_val - raw_first_val)
time_to_correct_raw = diff/f_raw

print(f'Time to correct for average filter: {round(time_to_correct_avg, 2)} seconds')
print(f'Time to correct for raw: {round(time_to_correct_raw,2)} seconds')