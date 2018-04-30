function [] = saveMediumFile()
    systemName = 'O2OSimulation';
    setName = {'small_set', 'medium_set', 'larger_set', 'largest_set'};
    methods = {'replan_sampling_evaluation', 'replan_sampling_random', ... 
        'replan_no_sampling', 'no_replan_sampling_evaluation'};
    path = fullfile('./data', systemName, char(setName(1)), char(methods(1)));
    saveResultInADir(path);
end

function saveResultInADir(path)
    % ����һ���ļ����µ����ݣ�������:ĳ������-->ĳ��Config������
    % path: �ļ��е�·�����Ե�ǰ�ļ�Ϊ��Ŀ¼
    files = dir(fullfile(path,'xml', '*.xml'));
    for i = 1:2
        filename = fullfile(path, 'xml', files(i).name);
        [routeSet, rejectCustomer, dynamicCustomer] = readxml(filename);
        % save(strcat(path,'result.mat'), 'routeSet', 'rejectCustomer', 'dynamicCustomer');
    end
end