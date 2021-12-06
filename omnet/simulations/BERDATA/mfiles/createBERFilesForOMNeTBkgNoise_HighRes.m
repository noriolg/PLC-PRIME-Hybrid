function createBERFilesForOMNeTBkgNoise_HighRes
%%
load per_bkg_new
load per_bkg_high_res
figure

simBer_med=median(demodBer_all(:,:,1),3);

semilogy(awgn_db',simBer_med','-*')
hold on
simBer_fec_med = median(simBer_fec_all(:,:,1),3);
semilogy(awgn_db',simBer_fec_med','-o')
grid on

figure
semilogy(awgn_db(1,:),squeeze(demodBer_all(1,:,:))','-*b')
hold on
semilogy(awgn_db(1,:),squeeze(simBer_fec_all(1,:,:))','-ob')

semilogy(awgn_db(1,:),squeeze(demodBer_all(2,:,:))','-*g')
semilogy(awgn_db(1,:),squeeze(simBer_fec_all(2,:,:))','-og')


semilogy(awgn_db(1,:),squeeze(demodBer_all(3,:,:))','-*r')
semilogy(awgn_db(1,:),squeeze(simBer_fec_all(3,:,:))','-or')

grid on
title(['Performance with Background Noise '])
xlabel('SNR(dB)')
ylabel('Bit Error Rate')
legHandler=legend('DBPSK-FECOFF','DBPSK-FECON','DQPSK-FECOFF','DQPSK-FECON','D8PSK-FECOFF','D8PSK-FECON','Location','Best');
legString = get(legHandler,'String');

for i = 1:3
    % first without FEC
    writeToFile(legString{(i*2)-1},false, awgn_db(1,:)',simBer_med(i,:)');
end

for i = 1:3
    % then with FEC
    writeToFile(legString{(i*2)},true, awgn_db(1,:)',simBer_fec_med(i,:)');
end

%%
function writeToFile(legString,FEC,SNR, BER)

fid = fopen(strcat('../',legString,'_HighRes.txt'),'w');

guion = find(legString=='-');
% identif = 

fprintf(fid,'%s - %s %s\n',legString(1:guion-1),legString(guion+1:guion+3),legString(guion+4:end));

%Print if it has FEC
if legString(end)=='F'
    fprintf(fid,'false\n');
else
    fprintf(fid,'true\n');
end

%Print Modulation
ind = find(legString=='-');
fprintf(fid,'%s\n',legString(1:ind-1));

%print BER|SNR
for i = 1:length(BER)
    fprintf(fid,'%f|%f\n',BER(i),SNR(i));
end
fprintf(fid,'\n');

fclose(fid);

