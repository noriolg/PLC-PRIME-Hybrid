function createBERFilesForOMNeT
%%
simulacion = 5;
load(strcat('per_hardly',num2str(simulacion)));
    
    figure
    for i=1:size(simBer_all,1)
        for j=1:size(simBer_all,2)
            simBer_med(i,j)=median(simBer_all(i,j,:));
            simBer_fec_med(i,j)=median(simBer_fec_all(i,j,:));
        end
    end
    
    semilogy(awgn_db_nofec(1,:)',simBer_med(1,:)','-*b')
    hold on
    semilogy(awgn_db_fec(1,:)',simBer_fec_med(1,:)','-ob')
    
    semilogy(awgn_db_nofec(1,:)',simBer_med(2,:)','-*g')
    semilogy(awgn_db_fec(1,:)',simBer_fec_med(2,:)','-og')
    
    
    semilogy(awgn_db_nofec(1,:)',simBer_med(3,:)','-*r')
    semilogy(awgn_db_fec(1,:)',simBer_fec_med(3,:)','-or')
    
    grid on
    title('BER Vs SNR- media iteraciones para heavily')
    xlabel('SNR(dB)')
    ylabel('BER')
    legHandler=legend('DBPSK-FECOFF','DBPSK-FECON','DQPSK-FECOFF','DQPSK-FECON','D8PSK-FECOFF','D8PSK-FECON','Location','Best');
    legString = get(legHandler,'String'); 
    %%
    for i = 1:3 
        % first without FEC
    writeToFile(legString{(i*2)-1},false, awgn_db_nofec(1,:)',simBer_med(i,:)');
    end
    
        for i = 1:3 
        % then with FEC
    writeToFile(legString{(i*2)},true, awgn_db_nofec(1,:)',simBer_fec_med(i,:)');
    end
    
    
function writeToFile(legString,FEC,SNR, BER)

fid = fopen(strcat('../',legString,'_imp.txt'),'w');

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

    