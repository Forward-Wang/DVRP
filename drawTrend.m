function [] = drawTrend()
    txtFilename = 'pr06.txt';
    [index, currentCost, globalCost] = textread(txtFilename, '%f %f %f');
    % xaxis = 1000:1000:15000;
    % xaxis = [13, xaxis];
    xaxis = [13:15000];
    plot(index(xaxis), currentCost(xaxis), 'b-', 'LineWidth', 1);
    hold on
    plot(index(xaxis), globalCost(xaxis), 'r-', 'LineWidth', 2);
    % plot(index(1:12), 6000*ones(1,12), 'gx', 'MarkerSize', 16)
    axis([0 14999 3000 6000])
    legend('��ǰ���ܵĽ�', '��ǰ���Ž�')
    xlabel('��������')
    ylabel('·������')
    hold off
end