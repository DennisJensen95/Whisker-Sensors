

[sample_time_1, y_1, fs_1] = get_y_sample_time('./oven_grate_signal_2');
[sample_time_2, y_2, fs_2] = get_y_sample_time('./potholder_signal');
[sample_time_3, y_3, fs_3] = get_y_sample_time('./yoga_mat_signal_2');

y_1 = y_1 - mean(y_1);
y_2 = y_2 - mean(y_2);
y_3 = y_3 - mean(y_3);
num_data = min([size(y_1, 2) size(y_2, 2) size(y_3, 2)]);

y_1 = y_1(1, [1:num_data]);
y_2 = y_2(1, [1:num_data]);
y_3 = y_3(1, [1:num_data]);

a = [1 , -0.98]; b = [1,-1];
y_1 = filtfilt(b,a,y_1);
y_2 = filtfilt(b,a,y_2);
y_3 = filtfilt(b,a,y_3);

fs_min = int16(min([fs_1, fs_2, fs_3]));

[Cxy,f] = mscohere(y_2, y_3,hamming(num_data),[],[],fs_min);
figure()
title('Coherence Estimate')
plot(f,Cxy)
grid on




[P2,f2] = periodogram(y_3,[],[],fs_min,'power');
figure()
plot(f2,P2)



function [sample_time, y_vec, fs] = get_y_sample_time(filename)
fileID = fopen(filename, 'r');
y_data = fscanf(fileID, '%s\n');
fclose(fileID);

sample_time = str2double(y_data(1, [14:16]));
nums = size(y_data(1, [16:end]));

y_new = [];

start = 16;
for j = 1:nums(2)/5
    end_num = start + 5;
    y_new = [y_new str2double(y_data(1, [start+1:end_num]))];
    start = end_num;
end
y_vec = y_new;

fs = size(y_vec,2)/sample_time;
end