function varargout = SurveyGUI(varargin)
%SURVEYGUI M-file for SurveyGUI.fig
%      SURVEYGUI, by itself, creates a new SURVEYGUI or raises the existing
%      singleton*.
%
%      H = SURVEYGUI returns the handle to a new SURVEYGUI or the handle to
%      the existing singleton*.
%
%      SURVEYGUI('Property','Value',...) creates a new SURVEYGUI using the
%      given property value pairs. Unrecognized properties are passed via
%      varargin to SurveyGUI_OpeningFcn.  This calling syntax produces a
%      warning when there is an existing singleton*.
%
%      SURVEYGUI('CALLBACK') and SURVEYGUI('CALLBACK',hObject,...) call the
%      local function named CALLBACK in SURVEYGUI.M with the given input
%      arguments.
%
%      *See GUI Options on GUIDE's Tools menu.  Choose "GUI allows only one
%      instance to run (singleton)".
%
% See also: GUIDE, GUIDATA, GUIHANDLES

% Edit the above text to modify the response to help SurveyGUI

% Last Modified by GUIDE v2.5 01-Nov-2009 00:31:37

% Begin initialization code - DO NOT EDIT
gui_Singleton = 1;
gui_State = struct('gui_Name',       mfilename, ...
    'gui_Singleton',  gui_Singleton, ...
    'gui_OpeningFcn', @SurveyGUI_OpeningFcn, ...
    'gui_OutputFcn',  @SurveyGUI_OutputFcn, ...
    'gui_LayoutFcn',  [], ...
    'gui_Callback',   []);
if nargin && ischar(varargin{1})
    gui_State.gui_Callback = str2func(varargin{1});
end

if nargout
    [varargout{1:nargout}] = gui_mainfcn(gui_State, varargin{:});
else
    gui_mainfcn(gui_State, varargin{:});
end
% End initialization code - DO NOT EDIT


% --- Executes just before SurveyGUI is made visible.
function SurveyGUI_OpeningFcn(hObject, eventdata, handles, varargin)
% This function has no output args, see OutputFcn.
% hObject    handle to figure
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
% varargin   unrecognized PropertyName/PropertyValue pairs from the
%            command line (see VARARGIN)

% Choose default command line output for SurveyGUI
handles.output = hObject;

handles.map = Map(44,-1);
handles.currentMeasurement = Null();
handles.revertMeasurement = Null();
handles.defaultMeasurement = Measurement();
handles.currentObject = Null();
updateResultDropDown(handles);
set(handles.mPanel,'Visible','off');
% Update handles structure
guidata(hObject, handles);

% UIWAIT makes SurveyGUI wait for user response (see UIRESUME)
% uiwait(handles.figure1);


% --- Outputs from this function are returned to the command line.
function varargout = SurveyGUI_OutputFcn(hObject, eventdata, handles)
% varargout  cell array for returning output args (see VARARGOUT);
% hObject    handle to figure
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
updateResultGraph(handles);
% Get default command line output from handles structure
varargout{1} = handles.output;


% --------------- Menu Callbacks -------------------------
function File_Callback(hObject, eventdata, handles)

function Save_Callback(hObject, eventdata, handles)
%allow the user to specify where to save the settings file
[filename,pathname] = uiputfile('default','Save the survey data');
 
if pathname == 0 %if the user pressed cancelled, then we exit this callback
    return
end
%construct the path name of the save location
saveDataName = fullfile(pathname,filename); 
 
%saves the gui data
hgsave(saveDataName);

function Load_Callback(hObject, eventdata, handles)
%allow the user to choose which settings to load
[filename, pathname] = uigetfile('*.fig', 'Choose the survey data to load');
 
%construct the path name of the file to be loaded
loadDataName = fullfile(pathname,filename);
 
%this is the gui that will be closed once we load the new settings
theCurrentGUI = gcf;  
 
%load the settings, which creates a new gui
hgload(loadDataName); 
 
%closes the old gui
close(theCurrentGUI);



% --------------- Object UI Components --------------------
function createObject_Callback(hObject, eventdata, handles)

% get reference to the namebox
namebox = handles.objectName_edit;
% get the current value in the namebox
name = get(namebox, 'String');

% if the namebox is not empty AND an object with name doesnt already exist
if(~(isempty(name) || isa(handles.map.getObject(name),'Object')))
    
    % check if the current measurement should be saved
    questionMeasurementSave(handles, eventdata);
    
    % *** Add a new Object ***
    handles.map.addObject(name);
    % set the namebox value to an empty string
    set(namebox, 'String', '');
    % update the editObject dropdown menu to reflect the new object
    updateObjectDropDown(handles);
    
    
    % *** Set Dropdown Value ***
    % get the list of items in the drop down
    stringList = get(handles.editObjectSelector,'String');
    n = size(stringList);
    % for each item in the stringList
    for i = 1:n(1)
        % if the item is equal to name
        if(strcmp(stringList{i},name))
            % set the editObjectSelector to the current value
            set(handles.editObjectSelector, 'Value', i)
            break;
        end
    end
    
    
    % *** Set currentObject reference ***
    handles.currentObject = handles.map.getObject(name);
    
    
    % *** Update measurement drop-down ***
    updateMeasurementDropDown(handles);
    % clear the current measurement
    handles.currentMeasurement = Null();
    handles.revertMeasurement = Null();
    % updateMeasurementTextBoxes(handles);
    updateMeasurementTextBoxes(handles);
    
end
guidata(hObject, handles);

function removeObject_Callback(hObject, eventdata, handles)
if isa(handles.currentObject, 'Object')
    handles.map.removeObject(handles.currentObject.name);
    
    updateObjectDropDown(handles);
    set(handles.editObjectSelector, 'Value', 1);
    editObjectSelector_Callback(handles.editObjectSelector, eventdata, handles);
    % NO GUIDATA !!!
end

function objectName_edit_KeyPressFcn(hObject, eventdata, handles)
if strcmp(eventdata.Key,'return');
    createObject_Callback(handles.createObject,eventdata,handles);
end

function objectName_edit_Callback(hObject, eventdata, handles)

function objectName_edit_CreateFcn(hObject, eventdata, handles)
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end

function editObjectSelector_Callback(hObject, eventdata, handles)
% get the list
selectorList = get(handles.editObjectSelector,'String');
% extract the current item
if isa(selectorList, 'char')
    currName = selectorList;
else
    currName = selectorList{get(hObject,'Value')};
end
% update the current object
if ~strcmp(currName,'Select Object');
    handles.currentObject = handles.map.getObject(currName);
else
    handles.currentObject = Null();
    handles.currentMeasurement = Null();
    handles.revertMeasurement = Null();
end
% update the measurement list
updateMeasurementDropDown(handles);
% set the measurement
set(handles.editMeasurementSelector,'Value',1);
editMeasurementSelector_Callback(handles.editMeasurementSelector, eventdata, handles);
guidata(hObject, handles);

function editObjectSelector_CreateFcn(hObject, eventdata, handles)
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end


%----------------- Measurement UI Components ---------------

function createMeasurement_Callback(hObject, eventdata, handles)
if isa(handles.currentObject, 'Object')
    % *** Create a new measurement ***
    m = Measurement();
    % set the measurement name
    m.name = strcat(getTime(), ' ( , )');
    % add the measurement to the currentObject
    handles.currentObject.addMeasurement(m);
    % make the new measurement the current measurement
    handles.currentMeasurement = m.clone();
    handles.revertMeasurement = m.clone();
    % update the measurement drop-down to reflect the new measurement
    updateMeasurementDropDown(handles);
    updateMeasurementTextBoxes(handles);
    
    % choose the correct measurement in the measurement selector
    selector = handles.editMeasurementSelector;
    stringList = get(selector,'String');
    n = size(stringList);
    for i = 1:n
        if(strcmp(stringList{i},m.name))
            % set the editMeasurementSelector to the current value
            set(handles.editMeasurementSelector, 'Value', i)
            break;
        end
    end

end
guidata(hObject, handles);

function editMeasurementSelector_Callback(hObject, eventdata, handles)
selectorList = get(handles.editMeasurementSelector,'String');
if isa(selectorList, 'char')
    currName = selectorList;
else
    currName = selectorList{get(hObject,'Value')};
end
if(~strcmp(currName,'Select Measurement'))
    m = handles.currentObject.getMeasurementByName(currName);
    handles.currentMeasurement = m.clone();
    handles.revertMeasurement = m.clone();
else
    handles.currentMeasurement = Null();
    handles.revertMeasurement = Null();
end
updateMeasurementTextBoxes(handles);
guidata(hObject,handles);

function editMeasurementSelector_CreateFcn(hObject, eventdata, handles)
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
    set(hObject,'String','Select Measurement');
end

function removeMeasurement_Callback(hObject, eventdata, handles)
if isa(handles.currentMeasurement, 'Measurement')
   handles.currentObject.removeMeasurement(handles.currentMeasurement.name);
   updateMeasurementDropDown(handles);
   set(handles.editMeasurementSelector,'Value',1);
   set(handles.loadObjectLocation,'Value',1);
   editMeasurementSelector_Callback(handles.editMeasurementSelector,eventdata,handles);
end





%----------------- Measurement Edit Panel -------------------


function x_edit_Callback(hObject, eventdata, handles)
if isa(handles.currentMeasurement, 'Measurement')
    str = get(hObject, 'String');
    value = str2num(str);
    if ~isempty(str) && isreal(value)
        handles.currentMeasurement.x = value;
    else
        handles.currentMeasurement.x = Null();
        set(hObject, 'String', '');
    end
end

function x_edit_CreateFcn(hObject, eventdata, handles)
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end

function sigx_edit_Callback(hObject, eventdata, handles)
if isa(handles.currentMeasurement, 'Measurement')
    str = get(hObject, 'String');
    value = str2num(str);
    if ~isempty(str) && isreal(value)
        handles.currentMeasurement.sigx = value;
    else
        handles.currentMeasurement.sigx = Null();
        set(hObject, 'String', '');
    end
end

function sigx_edit_CreateFcn(hObject, eventdata, handles)
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end




function y_edit_Callback(hObject, eventdata, handles)
if isa(handles.currentMeasurement, 'Measurement')
    str = get(hObject, 'String');
    value = str2num(str);
    if ~isempty(str) && isreal(value)
        handles.currentMeasurement.y = value;
    else
        handles.currentMeasurement.y = Null();
        set(hObject, 'String', '');
    end
end

function y_edit_CreateFcn(hObject, eventdata, handles)
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end

function sigy_edit_Callback(hObject, eventdata, handles)
if isa(handles.currentMeasurement, 'Measurement')
    str = get(hObject, 'String');
    value = str2num(str);
    if ~isempty(str) && isreal(value)
        handles.currentMeasurement.sigy = value;
    else
        handles.currentMeasurement.sigy = Null();
        set(hObject, 'String', '');
    end
end

function sigy_edit_CreateFcn(hObject, eventdata, handles)
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end




function phi_edit_Callback(hObject, eventdata, handles)
if isa(handles.currentMeasurement, 'Measurement')
    str = get(hObject, 'String');
    value = str2num(str);
    if ~isempty(str) && isreal(value)
        handles.currentMeasurement.phi = toRad(value);
    else
        handles.currentMeasurement.phi = Null();
        set(hObject, 'String', '');
    end
end

function phi_edit_CreateFcn(hObject, eventdata, handles)
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end

function sigphi_edit_Callback(hObject, eventdata, handles)
if isa(handles.currentMeasurement, 'Measurement')
    str = get(hObject, 'String');
    value = str2num(str);
    if ~isempty(str) && isreal(value)
        handles.currentMeasurement.sigphi = toRad(value);
    else
        handles.currentMeasurement.sigphi = Null();
        set(hObject, 'String', '');
    end
end

function sigphi_edit_CreateFcn(hObject, eventdata, handles)
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end




function h_edit_Callback(hObject, eventdata, handles)
if isa(handles.currentMeasurement, 'Measurement')
    str = get(hObject, 'String');
    value = str2num(str);
    if ~isempty(str) && isreal(value)
        handles.currentMeasurement.h = value;
    else
        handles.currentMeasurement.h = Null();
        set(hObject, 'String', '');
    end
end

function h_edit_CreateFcn(hObject, eventdata, handles)
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end

function sigh_edit_Callback(hObject, eventdata, handles)
if isa(handles.currentMeasurement, 'Measurement')
    str = get(hObject, 'String');
    value = str2num(str);
    if ~isempty(str) && isreal(value)
        handles.currentMeasurement.sigh = value;
    else
        handles.currentMeasurement.sigh = Null();
        set(hObject, 'String', '');
    end
end

function sigh_edit_CreateFcn(hObject, eventdata, handles)
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end




function D_edit_Callback(hObject, eventdata, handles)
if isa(handles.currentMeasurement, 'Measurement')
    str = get(hObject, 'String');
    value = str2num(str);
    if ~isempty(str) && isreal(value)
        handles.currentMeasurement.D = value;
    else
        handles.currentMeasurement.D = Null();
        set(hObject, 'String', '');
    end
end

function D_edit_CreateFcn(hObject, eventdata, handles)
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end

function sigD_edit_Callback(hObject, eventdata, handles)
if isa(handles.currentMeasurement, 'Measurement')
    str = get(hObject, 'String');
    value = str2num(str);
    if ~isempty(str) && isreal(value)
        handles.currentMeasurement.sigD = value;
    else
        handles.currentMeasurement.sigD = Null();
        set(hObject, 'String', '');
    end
end

function sigD_edit_CreateFcn(hObject, eventdata, handles)
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end




function theta_edit_Callback(hObject, eventdata, handles)
if isa(handles.currentMeasurement, 'Measurement')
    str = get(hObject, 'String');
    value = str2num(str);
    if ~isempty(str) && isreal(value)
        handles.currentMeasurement.theta = toRad(value);
    else
        handles.currentMeasurement.theta = Null();
        set(hObject, 'String', '');
    end
end

function theta_edit_CreateFcn(hObject, eventdata, handles)
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end

function sigtheta_edit_Callback(hObject, eventdata, handles)
if isa(handles.currentMeasurement, 'Measurement')
    str = get(hObject, 'String');
    value = str2num(str);
    if ~isempty(str) && isreal(value)
        handles.currentMeasurement.sigtheta = toRad(value);
    else
        handles.currentMeasurement.sigtheta = Null();
        set(hObject, 'String', '');
    end
end

function sigtheta_edit_CreateFcn(hObject, eventdata, handles)
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end




function s_edit_Callback(hObject, eventdata, handles)
if isa(handles.currentMeasurement, 'Measurement')
    str = get(hObject, 'String');
    value = str2num(str);
    if ~isempty(str) && isreal(value)
        handles.currentMeasurement.s = value;
    else
        handles.currentMeasurement.s = Null();
        set(hObject, 'String', '');
    end
end

function s_edit_CreateFcn(hObject, eventdata, handles)
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end

function sigs_edit_Callback(hObject, eventdata, handles)
if isa(handles.currentMeasurement, 'Measurement')
    str = get(hObject, 'String');
    value = str2num(str);
    if ~isempty(str) && isreal(value)
        handles.currentMeasurement.sigs = value;
    else
        handles.currentMeasurement.sigs = Null();
        set(hObject, 'String', '');
    end
end

function sigs_edit_CreateFcn(hObject, eventdata, handles)
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end




function r_edit_Callback(hObject, eventdata, handles)
if isa(handles.currentMeasurement, 'Measurement')
    str = get(hObject, 'String');
    value = str2num(str);
    if ~isempty(str) && isreal(value)
        handles.currentMeasurement.r = value;
    else
        handles.currentMeasurement.r = Null();
        set(hObject, 'String', '');
    end
end

function r_edit_CreateFcn(hObject, eventdata, handles)
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end

function sigr_edit_Callback(hObject, eventdata, handles)
if isa(handles.currentMeasurement, 'Measurement')
    str = get(hObject, 'String');
    value = str2num(str);
    if ~isempty(str) && isreal(value)
        handles.currentMeasurement.sigr = value;
    else
        handles.currentMeasurement.sigr = Null();
        set(hObject, 'String', '');
    end
end

function sigr_edit_CreateFcn(hObject, eventdata, handles)
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end





function loadObjectLocation_Callback(hObject, eventdata, handles) %#ok<*INUSD>
if isa(handles.currentMeasurement,'Measurement')
    
    selectorList = get(handles.loadObjectLocation,'String');
    if isa(selectorList,'char')
        currName = selectorList;
    else
        currName = selectorList{get(hObject,'Value')};
    end
    if ~strcmp(currName, 'Select Object');
        handles.currentMeasurement.associatedObject = currName;
        currentObject = handles.map.getObject(currName);
        if isreal(currentObject.location.xobj)
            handles.currentMeasurement.x = currentObject.location.xobj;
        else
            handles.currentMeasurement.x = Null();
        end
        if isreal(currentObject.location.sigxobj)
            handles.currentMeasurement.sigx = currentObject.location.sigxobj;
        else
            handles.currentMeasurement.sigx = Null();
        end
        if isreal(currentObject.location.yobj)
            handles.currentMeasurement.y = currentObject.location.yobj;
        else
            handles.currentMeasurement.y = Null();
        end
        if isreal(currentObject.location.sigyobj)
            handles.currentMeasurement.sigy = currentObject.location.sigyobj;
        else
            handles.currentMeasurement.sigy = Null();
        end
    else
        handles.currentMeasurement.associatedObject = Null();
        handles.currentMeasurement.x = Null();
        handles.currentMeasurement.sigx = Null();
        handles.currentMeasurement.y = Null();
        handles.currentMeasurement.sigy = Null();
    end
    updateMeasurementTextBoxes(handles);
    guidata(hObject,handles);
end


function loadObjectLocation_CreateFcn(hObject, eventdata, handles)
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end

function mesaurementSave_Callback(hObject, eventdata, handles) %#ok<*INUSL,*DEFNU>
if isa(handles.currentMeasurement, 'Measurement')
    c = handles.currentMeasurement;
    d = handles.defaultMeasurement;
    if isa(c.associatedObject, 'char');
        a = map.getObject(c.associatedObject);
        if ~(a.location.xobj == c.x...
                && a.location.yobj == c.y...
                && a.location.sigxobj == c.sigx...
                && a.location.sigyobj == c.sigy)
            c.associatedObject = Null();
        end
    else
        
    end
    save = handles.currentObject.getMeasurementByName(c.name);
    if isreal(c.x)
        save.x = c.x;
    else
        save.x = d.x;
    end
    if isreal(c.sigx)
        save.sigx = c.sigx;
    else
        save.sigx = d.sigx;
    end
    if isreal(c.y)
        save.y = c.y;
    else
        save.y = d.y;
    end
    if isreal(c.sigy)
        save.sigy = c.sigy;
    else
        save.sigy = d.sigy;
    end
    if isreal(c.phi)
        save.phi = c.phi;
    else
        save.phi = d.phi;
    end
    if isreal(c.sigphi)
        save.sigphi = c.sigphi;
    else
        save.sigphi = d.sigphi;
    end
    if isreal(c.D)
        save.D = c.D;
    else
        save.D = d.D;
    end
    if isreal(c.sigD)
        save.sigD = c.sigD;
    else
        save.sigD = d.sigD;
    end
    if isreal(c.h)
        save.h = c.h;
    else
        save.h = d.h;
    end
    if isreal(c.sigh)
        save.sigh = c.sigh;
    else
        save.sigh = d.sigh;
    end
    if isreal(c.theta)
        save.theta = c.theta;
    else
        save.theta = d.theta;
    end
    if isreal(c.sigtheta)
        save.sigtheta = c.sigtheta;
    else
        save.sigtheta = d.sigtheta;
    end
    if isreal(c.s)
        save.s = c.s;
    else
        save.s = d.s;
    end
    if isreal(c.sigs)
        save.sigs = c.sigs;
    else
        save.sigs = d.sigs;
    end
    if isreal(c.s)
        save.r = c.r;
    else
        save.r = d.r;
    end
    if isreal(c.s)
        save.sigr = c.sigr;
    else
        save.sigr = d.sigr;
    end
    save.isActive = c.isActive;
    handles.map.updateResults(handles.currentObject);
    resultObjectSelector_Callback(handles.resultObjectSelector,eventdata,handles);
    updateResultGraph(handles);
end

function revert_Callback(hObject, eventdata, handles)
if isa(handles.currentMeasurement, 'Measurement') && isa(handles.revertMeasurement, 'Measurement')
    handles.currentMeasurement = handles.revertMeasurement.clone();
    updateMeasurementTextBoxes(handles);
    guidata(hObject, handles);
end

function ActivateMeasurement_Callback(hObject, eventdata, handles)
status = get(hObject,'Value');
if status
    handles.currentMeasurement.isActive = 1;
    set(handles.ActivateMeasurement,'String','Deactivate');
    set(handles.ActivateMeasurement,'BackgroundColor',[.4 1 .4]);
else
    handles.currentMeasurement.isActive = 0;
    set(handles.ActivateMeasurement,'String','Activate');
    set(handles.ActivateMeasurement,'BackgroundColor',[1 .4 .4]);
end
guidata(hObject,handles);




% ----------------- Results Area ---------------------------

function resultObjectSelector_Callback(hObject, eventdata, handles)
selectorList = get(handles.resultObjectSelector, 'String');
% extract the current item
if isa(selectorList, 'char')
    currName = selectorList;
else
    currName = selectorList{get(hObject,'Value')};
end
if ~strcmp(currName, 'Select Object');
    currentObject = handles.map.getObject(currName);
    if isreal(currentObject.location.xobj)
        set(handles.xObj, 'String', num2str(currentObject.location.xobj));
    else
        set(handles.xObj, 'String', '');
    end
    if isreal(currentObject.location.sigxobj)
        set(handles.sigxObj, 'String', num2str(currentObject.location.sigxobj));
    else
        set(handles.sigxObj, 'String', '');
    end
    if isreal(currentObject.location.yobj)
        set(handles.yObj, 'String', num2str(currentObject.location.yobj));
    else
        set(handles.yObj, 'String', '');
    end
    if isreal(currentObject.location.sigyobj)
        set(handles.sigyObj, 'String', num2str(currentObject.location.sigyobj));
    else
        set(handles.sigyObj, 'String', '');
    end
else
    set(handles.xObj, 'String', '');
    set(handles.sigxObj, 'String', '');
    set(handles.yObj, 'String', '');
    set(handles.sigyObj, 'String', '');
end
guidata(hObject,handles);

function resultObjectSelector_CreateFcn(hObject, eventdata, handles)
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end


function yOrigin_edit_Callback(hObject, eventdata, handles)
str = get(hObject, 'String');
value = str2double(str);
o = handles.map.objectMap('Origin');
if isreal(value)
   handles.map.originOffsetY = value;
   o.location.yobj = value;
else
    set(hObject, 'String', '');
end
updateResultGraph(handles);
guidata(hObject,handles);

function yOrigin_edit_CreateFcn(hObject, eventdata, handles)
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end

function xOrigin_edit_Callback(hObject, eventdata, handles)
str = get(hObject, 'String');
value = str2double(str);
o = handles.map.objectMap('Origin');
if isreal(value)
   handles.map.originOffsetX = value;
   o.location.xobj = value;
else
    set(hObject, 'String', '');
end
updateResultGraph(handles);
guidata(hObject,handles);

function xOrigin_edit_CreateFcn(hObject, eventdata, handles)
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end




% ---------------- Helper Functions -----------------------------


function updateObjectDropDown(handles)
objectDropDown = handles.editObjectSelector;
objMap = handles.map.getAllObjects();
k = objMap.keys();
n = 1;
stringList{1} = 'Select Object';
for i = 1:int32(objMap.Count)
    currObj = objMap(char(k(i)));
    if(~strcmp(currObj.name, 'Origin'))
        stringList{n} = currObj.name;
        n = n+1;
    end
end
set(objectDropDown,'String',stringList);
updateResultDropDown(handles);

function updateMeasurementDropDown(handles)
% get the reference to the editMeasurement drop down
mDropDown = handles.editMeasurementSelector;
% get the containers.map of measurements for the current object
if isa(handles.currentObject, 'Object')
    mMap = handles.currentObject.getAllMeasurements();
    % get the list of keys in this map
    k = mMap.keys();
    stringList{1} = 'Select Measurement';
    % for each key
    for i = 1:int32(mMap.Count)
        % get the measurement at the current key
        currMes = mMap(char(k(i)));
        % add the measurements name to the stringList
        stringList{i} = currMes.name;
    end
    % update the stringList
    set(mDropDown,'String',stringList);
else
    stringList{1} = 'Select Measurement';
    set(mDropDown,'String',stringList);
end

function updateMeasurementTextBoxes(handles)
clearMeasurementTextBoxes(handles);
if isa(handles.currentMeasurement, 'Measurement')
    set(handles.mPanel,'Visible','on');
    if isreal(handles.currentMeasurement.x)
        set(handles.x_edit, 'String',num2str(handles.currentMeasurement.x));
    else
        set(handles.x_edit, 'String', '');
    end
    if isreal(handles.currentMeasurement.sigx)
        set(handles.sigx_edit,'String',num2str(handles.currentMeasurement.sigx));
    else
        set(handles.sigx_edit, 'String', '');
    end
    if isreal(handles.currentMeasurement.y)
        set(handles.y_edit,'String',num2str(handles.currentMeasurement.y));
    else
        set(handles.y_edit, 'String', '');
    end
    if isreal(handles.currentMeasurement.sigy)
        set(handles.sigy_edit,'String',num2str(handles.currentMeasurement.sigy));
    else
        set(handles.sigy_edit, 'String', '');
    end
    if isreal(handles.currentMeasurement.phi)
        set(handles.phi_edit,'String',num2str(toDeg(handles.currentMeasurement.phi)));
    else
        set(handles.phi_edit, 'String', '');
    end
    if isreal(handles.currentMeasurement.sigphi)
        set(handles.sigphi_edit,'String',num2str(toDeg(handles.currentMeasurement.sigphi)));
    else
        set(handles.sigphi_edit, 'String', '');
    end
    if isreal(handles.currentMeasurement.h)
        set(handles.h_edit,'String',num2str(handles.currentMeasurement.h));
    else
        set(handles.h_edit, 'String', '');
    end
    if isreal(handles.currentMeasurement.sigh)
        set(handles.sigh_edit,'String',num2str(handles.currentMeasurement.sigh));
    else
        set(handles.sigh_edit, 'String', '');
    end
    if isreal(handles.currentMeasurement.D)
        set(handles.D_edit,'String',num2str(handles.currentMeasurement.D));
    else
        set(handles.D_edit, 'String', '');
    end
    if isreal(handles.currentMeasurement.sigD)
        set(handles.sigD_edit,'String',num2str(handles.currentMeasurement.sigD));
    else
        set(handles.sigD_edit, 'String', '');
    end
    if isreal(handles.currentMeasurement.theta)
        set(handles.theta_edit,'String',num2str(toDeg(handles.currentMeasurement.theta)));
    else
        set(handles.theta_edit, 'String', '');
    end
    if isreal(handles.currentMeasurement.sigtheta)
        set(handles.sigtheta_edit,'String',num2str(toDeg(handles.currentMeasurement.sigtheta)));
    else
        set(handles.sigtheta_edit, 'String', '');
    end
    if isreal(handles.currentMeasurement.s)
        set(handles.s_edit,'String',num2str(handles.currentMeasurement.s));
    else
        set(handles.s_edit, 'String', '');
    end
    if isreal(handles.currentMeasurement.sigs)
        set(handles.sigs_edit,'String',num2str(handles.currentMeasurement.sigs));
    else
        set(handles.sigs_edit, 'String', '');
    end
    if isreal(handles.currentMeasurement.r)
        set(handles.r_edit,'String',num2str(handles.currentMeasurement.r));
    else
        set(handles.r_edit, 'String', '');
    end
    if isreal(handles.currentMeasurement.sigr)
        set(handles.sigr_edit,'String',num2str(handles.currentMeasurement.sigr));
    else
        set(handles.sigr_edit, 'String', '');
    end
    if handles.currentMeasurement.isActive
        set(handles.ActivateMeasurement,'Value',1);
        set(handles.ActivateMeasurement,'String','Deactivate');
        set(handles.ActivateMeasurement,'BackgroundColor',[.4 1 .4]);
    else
        set(handles.ActivateMeasurement,'Value',0);
        set(handles.ActivateMeasurement,'String','Activate');
        set(handles.ActivateMeasurement,'BackgroundColor',[1 .4 .4]);
    end
else
    set(handles.mPanel,'Visible','off');
end

function clearMeasurementTextBoxes(handles)
set(handles.x_edit, 'String', '');
set(handles.sigx_edit, 'String', '');
set(handles.y_edit, 'String', '');
set(handles.sigy_edit, 'String', '');
set(handles.phi_edit, 'String', '');
set(handles.sigphi_edit, 'String', '');
set(handles.h_edit, 'String', '');
set(handles.sigh_edit, 'String', '');
set(handles.D_edit, 'String', '');
set(handles.sigD_edit, 'String', '');
set(handles.theta_edit, 'String', '');
set(handles.sigtheta_edit, 'String', '');
set(handles.s_edit, 'String', '');
set(handles.sigs_edit, 'String', '');
set(handles.r_edit, 'String', '');
set(handles.sigr_edit, 'String', '');

function questionMeasurementSave(handles, eventdata)

function updateResultDropDown(handles)
selector = handles.resultObjectSelector;
loadObjectDropDown = handles.loadObjectLocation;
objMap = handles.map.getAllObjects;
k = objMap.keys();
stringList{1} = 'Select Object';
for i = 1:int32(objMap.Count)
    cObj = objMap(char(k(i)));
    stringList{i} = cObj.name;
end
set(selector, 'String', stringList);
set(loadObjectDropDown,'String',stringList);

function updateResultGraph(handles)
handles.map.plot(handles.resultGraph);
