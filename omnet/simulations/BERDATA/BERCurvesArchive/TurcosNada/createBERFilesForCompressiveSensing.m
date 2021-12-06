function createBERFilesForCompressiveSensing
%%
% El modo indica que tipo de curva es la que quiero sacar
modo = 'NADA';  % Sin aplicar CS
modo = 'REC';   % Aplicando CS con Reconstruccion
% modo = 'ANU';   % Aplicando CS con Anulacion

environ = 'performance_NOCOMB_BOMP_imp_RELAT_temp2';
 environ = 'performance_DS_23_BOMP_imp_temp';
load(environ);

close all
figure
theoryBer = (1/2)*exp(-(10.^(awgn_db(1,:)/10)));
theoryBer(find(theoryBer<1e-6))=NaN;
semilogy(awgn_db(1,:),theoryBer,'y','LineWidth',4,...)
    'MarkerEdgeColor','k',...
    'MarkerFaceColor','b',...
    'MarkerSize',10)
hold on

if strcmp(modo,'NADA')
    demodBer_fec = demodBer;
    simBer_fec = simBer;
elseif strcmp(modo,'REC')
    demodBer_fec = demodBer_REC;
    simBer_fec = simBer_REC;
elseif strcmp(modo,'ANU')
    demodBer_fec = demodBer_ANU;
    simBer_fec = simBer_ANU;
end

semilogy(awgn_db',demodBer_fec','-*')
hold on
grid on
title(strcat('demod BER  ',' \Gamma=',num2str(noise_param.gamma),'A=',num2str(noise_param.A)))
legend('Theoretical bkgNoise','DBPSK','DQPSK','D8PSK')



figure

semilogy(awgn_db',simBer_fec','-o')
hold on
grid on
title(strcat('Sim BER FEC  ',' \Gamma=',num2str(noise_param.gamma),'A=',num2str(noise_param.A)))
legend('DBPSK','DQPSK','D8PSK')
% set_figures

figure

semilogy(awgn_db(1,:),demodBer_fec(1,:)','-*b')
hold on
semilogy(awgn_db(1,:),simBer_fec(1,:)','-ob')

semilogy(awgn_db(2,:),demodBer_fec(2,:)','-*g')
semilogy(awgn_db(2,:),simBer_fec(2,:)','-og')


semilogy(awgn_db(3,:),demodBer_fec(3,:)','-*r')
semilogy(awgn_db(3,:),simBer_fec(3,:)','-or')

grid on
title(['Performance with Middleton Imp. Noise Model - ',' \Gamma=',num2str(noise_param.gamma),' A=',num2str(noise_param.A)])
xlabel('SNR(dB)')
ylabel('Bit Error Rate')
legHandler=legend('DBPSK-FECOFF','DBPSK-FECON','DQPSK-FECOFF','DQPSK-FECON','D8PSK-FECOFF','D8PSK-FECON','Location','Best');
legString = get(legHandler,'String');

%%
for i = 1:3
    % first without FEC
    writeToFile(legString{(i*2)-1},false,awgn_db(1,:),demodBer_fec(i,:)');
end

for i = 1:3
    % then with FEC
    writeToFile(legString{(i*2)},true, awgn_db(1,:),simBer_fec(i,:)');
end

fid = fopen('imp_noise_environment.txt','w');

fprintf(fid, '%s',environ);
fclose(fid);

function writeToFile(legString,FEC,SNR, BER)

fid = fopen(strcat('../',legString,'_imp.txt'),'w');

ind = find(legString=='-');

fprintf(fid,'%s',[legString(1:ind-1),' - ']);

indO = find(legString=='O');
fprintf(fid,'%s\n',['FEC ',legString(indO:end)]);


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
    fprintf(fid,'%1.10f|%d\n',BER(i),SNR(i));
end
fprintf(fid,'\n');

fclose(fid);

