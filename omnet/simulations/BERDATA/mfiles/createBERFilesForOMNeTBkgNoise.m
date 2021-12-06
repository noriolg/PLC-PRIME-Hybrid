function createBERFilesForOMNeTBkgNoise
%%
 load per_bkg_new
%  load per_bkg_high_res
 figure
    
  M=squeeze(demodBer_fec(:,:,1));
 
                  semilogy(awgn_db',M','-*')
                  hold on 
         M=squeeze(simBer_fec(:,:,1));
         semilogy(awgn_db',M','-o')
 
 
    semilogy(awgn_db(1,:),squeeze(demodBer_fec(1,:,1))','-*b')
    hold on
    semilogy(awgn_db(1,:),squeeze(simBer_fec(1,:,1))','-ob')
    
    semilogy(awgn_db(1,:),squeeze(demodBer_fec(2,:,2))','-*g')
    semilogy(awgn_db(1,:),squeeze(simBer_fec(2,:,2))','-og')
    
    
    semilogy(awgn_db(1,:),squeeze(demodBer_fec(3,:,3))','-*r')
    semilogy(awgn_db(1,:),squeeze(simBer_fec(3,:,3))','-or')
    
    grid on
title(['Performance with Background Noise '])
    xlabel('SNR(dB)')
    ylabel('Bit Error Rate')
    legHandler=legend('DBPSK-FECOFF','DBPSK-FECON','DQPSK-FECOFF','DQPSK-FECON','D8PSK-FECOFF','D8PSK-FECON','Location','Best');
    legString = get(legHandler,'String'); 
    
    for i = 1:3 
        % first without FEC
%     writeToFile(legString{(i*2)-1},false, awgn_db(1,:)',simBer_med(i,:)');
    end
    
        for i = 1:3 
        % then with FEC
%     writeToFile(legString{(i*2)},true, awgn_db(1,:)',simBer_fec_med(i,:)');
    end
    
    %%
function writeToFile(legString,FEC,SNR, BER)

fid = fopen(strcat('../',legString,'.txt'),'w');

fprintf(fid,'%s\n',legString);

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
 fprintf(fid,'%f|%d\n',BER(i),SNR(i));
 end
fprintf(fid,'\n');

fclose(fid);

    