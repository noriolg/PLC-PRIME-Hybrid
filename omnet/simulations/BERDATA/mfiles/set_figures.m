
function set_figures

h=get(0,'Children');

for iii=1:length(h)
    set(get(h(iii),'CurrentAxes'),'YLim',[1e-005 1])
    set(get(h(iii),'CurrentAxes'),'XLim',[-5 55])
    a=get(get(h(iii),'CurrentAxes'),'Children');
    for i=1:length(a)
        set(a(i),'LineWidth',3);
    end
end