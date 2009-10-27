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

% Last Modified by GUIDE v2.5 23-Oct-2009 14:49:38

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

handles.map = Map(0,0);
handles.currentMeasurement = Null();
handles.revertMeasurement = Null();
handles.currentObject = Null();
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

% Get default command line output from handles structure
varargout{1} = handles.output;

% --- Executes on button press in createObject.
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

function objectName_edit_Callback(hObject, eventdata, handles)

% --- Executes during object creation, after setting all properties.
function objectName_edit_CreateFcn(hObject, eventdata, handles)
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end

function x_edit_Callback(hObject, eventdata, handles)
if isa(handles.currentMeasurement, 'Measurement')
    str = get(hObject, 'String');
    value = str2double(str);
    if ~isempty(value) && isreal(value)
        handles.currentMeasurement.x = value;
    else
        set(hObject, 'String', '');
    end
end


% --- Executes during object creation, after setting all properties.
function x_edit_CreateFcn(hObject, eventdata, handles)
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end

% --- Executes on button press in createMeasurement.
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

% --- Executes on button press in removeObject.
function removeObject_Callback(hObject, eventdata, handles)
if isa(handles.currentObject, 'Object')
    handles.map.removeObject(handles.currentObject.name);
    
    updateObjectDropDown(handles);
    set(handles.editObjectSelector, 'Value', 1);
    editObjectSelector_Callback(handles.editObjectSelector, eventdata, handles);
end
% NO GUIDATA !!!

function y_edit_Callback(hObject, eventdata, handles)
if isa(handles.currentMeasurement, 'Measurement')
    str = get(hObject, 'String');
    value = str2double(str);
    if ~isempty(value) && isreal(value)
        handles.currentMeasurement.y = value;
    else
        set(hObject, 'String', '');
    end
end

% --- Executes during object creation, after setting all properties.
function y_edit_CreateFcn(hObject, eventdata, handles)
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end

function phi_edit_Callback(hObject, eventdata, handles)
if isa(handles.currentMeasurement, 'Measurement')
    str = get(hObject, 'String');
    value = str2double(str);
    if ~isempty(value) && isreal(value)
        handles.currentMeasurement.phi = value;
    else
        set(hObject, 'String', '');
    end
end

% --- Executes during object creation, after setting all properties.
function phi_edit_CreateFcn(hObject, eventdata, handles)
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end

function h_edit_Callback(hObject, eventdata, handles)
if isa(handles.currentMeasurement, 'Measurement')
    str = get(hObject, 'String');
    value = str2double(str);
    if ~isempty(value) && isreal(value)
        handles.currentMeasurement.h = value;
    else
        set(hObject, 'String', '');
    end
end

% --- Executes during object creation, after setting all properties.
function h_edit_CreateFcn(hObject, eventdata, handles)
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end

function D_edit_Callback(hObject, eventdata, handles)
if isa(handles.currentMeasurement, 'Measurement')
    str = get(hObject, 'String');
    value = str2double(str);
    if ~isempty(value) && isreal(value)
        handles.currentMeasurement.D = value;
    else
        set(hObject, 'String', '');
    end
end

% --- Executes during object creation, after setting all properties.
function D_edit_CreateFcn(hObject, eventdata, handles)
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end

function theta_edit_Callback(hObject, eventdata, handles)
if isa(handles.currentMeasurement, 'Measurement')
    str = get(hObject, 'String');
    value = str2double(str);
    if ~isempty(value) && isreal(value)
        handles.currentMeasurement.theta = value;
    else
        set(hObject, 'String', '');
    end
end

% --- Executes during object creation, after setting all properties.
function theta_edit_CreateFcn(hObject, eventdata, handles)
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end

function sigx_edit_Callback(hObject, eventdata, handles)
if isa(handles.currentMeasurement, 'Measurement')
    str = get(hObject, 'String');
    value = str2double(str);
    if ~isempty(value) && isreal(value)
        handles.currentMeasurement.sigx = value;
    else
        set(hObject, 'String', '');
    end
end

% --- Executes during object creation, after setting all properties.
function sigx_edit_CreateFcn(hObject, eventdata, handles)
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end

function sigy_edit_Callback(hObject, eventdata, handles)
if isa(handles.currentMeasurement, 'Measurement')
    str = get(hObject, 'String');
    value = str2double(str);
    if ~isempty(value) && isreal(value)
        handles.currentMeasurement.sigy = value;
    else
        set(hObject, 'String', '');
    end
end

% --- Executes during object creation, after setting all properties.
function sigy_edit_CreateFcn(hObject, eventdata, handles)
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end

function sigphi_edit_Callback(hObject, eventdata, handles)
if isa(handles.currentMeasurement, 'Measurement')
    str = get(hObject, 'String');
    value = str2double(str);
    if ~isempty(value) && isreal(value)
        handles.currentMeasurement.sigphi = value;
    else
        set(hObject, 'String', '');
    end
end

% --- Executes during object creation, after setting all properties.
function sigphi_edit_CreateFcn(hObject, eventdata, handles)
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end

function sigh_edit_Callback(hObject, eventdata, handles)
if isa(handles.currentMeasurement, 'Measurement')
    str = get(hObject, 'String');
    value = str2double(str);
    if ~isempty(value) && isreal(value)
        handles.currentMeasurement.sigh = value;
    else
        set(hObject, 'String', '');
    end
end

% --- Executes during object creation, after setting all properties.
function sigh_edit_CreateFcn(hObject, eventdata, handles)
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end

function sigD_edit_Callback(hObject, eventdata, handles)
if isa(handles.currentMeasurement, 'Measurement')
    str = get(hObject, 'String');
    value = str2double(str);
    if ~isempty(value) && isreal(value)
        handles.currentMeasurement.sigD = value;
    else
        set(hObject, 'String', '');
    end
end

% --- Executes during object creation, after setting all properties.
function sigD_edit_CreateFcn(hObject, eventdata, handles)
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end

function sigtheta_edit_Callback(hObject, eventdata, handles)
if isa(handles.currentMeasurement, 'Measurement')
    str = get(hObject, 'String');
    value = str2double(str);
    if ~isempty(value) && isreal(value)
        handles.currentMeasurement.sigtheta = value;
    else
        set(hObject, 'String', '');
    end
end

% --- Executes during object creation, after setting all properties.
function sigtheta_edit_CreateFcn(hObject, eventdata, handles)
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end

% --- Executes on button press in mactive.
function mactive_Callback(hObject, eventdata, handles)

% --- Executes on button press in changeOrigin.
function changeOrigin_Callback(hObject, eventdata, handles)

% --- Executes on selection change in resultObjectSelector.
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
    set(handles.xigxObj, 'String', '');
    set(handles.yObj, 'String', '');
    set(handles.sigyObj, 'String', '');
end
guidata(hObject,handles);

% --- Executes during object creation, after setting all properties.
function resultObjectSelector_CreateFcn(hObject, eventdata, handles)
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end

% --- Executes on selection change in editMesurementSelector.
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

% --- Executes during object creation, after setting all properties.
function editMeasurementSelector_CreateFcn(hObject, eventdata, handles)
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
    set(hObject,'String','Select Measurement');
end

% --- Executes on selection change in editObjectSelector.
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

% --- Executes during object creation, after setting all properties.
function editObjectSelector_CreateFcn(hObject, eventdata, handles)
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end

% --- Executes on button press in mesaurementSave.
function mesaurementSave_Callback(hObject, eventdata, handles)
if isa(handles.currentMeasurement, 'Measurement')
    c = handles.currentMeasurement;
    save = handles.currentObject.getMeasurementByName(c.name);
    if isreal(c.x)
        save.x = c.x;
    else
        save.x = Null();
    end
    if isreal(c.sigx)
        save.sigx = c.sigx;
    else
        save.sigx = Null();
    end
    if isreal(c.y)
        save.y = c.y;
    else
        save.y = Null();
    end
    if isreal(c.sigy)
        save.sigy = c.sigy;
    else
        save.sigy = Null();
    end
    if isreal(c.phi)
        save.phi = c.phi;
    else
        save.phi = Null();
    end
    if isreal(c.sigphi)
        save.sigphi = c.sigphi;
    else
        save.sigphi = Null();
    end
    if isreal(c.D)
        save.D = c.D;
    else
        save.D = Null();
    end
    if isreal(c.sigD)
        save.sigD = c.sigD;
    else
        save.sigD = Null();
    end
    if isreal(c.h)
        save.h = c.h;
    else
        save.h = Null();
    end
    if isreal(c.sigh)
        save.sigh = c.sigh;
    else
        save.sigh = Null();
    end
    if isreal(c.theta)
        save.theta = c.theta;
    else
        save.theta = Null();
    end
    if isreal(c.sigtheta)
        save.sigtheta = c.sigtheta;
    else
        save.sigtheta = Null();
    end
    handles.currentObject.updateLocation;
    resultObjectSelector_Callback(handles.resultObjectSelector,eventdata,handles);
end

% --- Executes on button press in revert.
function revert_Callback(hObject, eventdata, handles)
if isa(handles.currentMeasurement, 'Measurement') && isa(handles.revertMeasurement, 'Measurement')
    handles.currentMeasurement = handles.revertMeasurement.clone();
    updateMeasurementTextBoxes(handles);
    guidata(hObject, handles);
end


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
        set(handles.phi_edit,'String',num2str(handles.currentMeasurement.phi));
    else
        set(handles.phi_edit, 'String', '');
    end
    if isreal(handles.currentMeasurement.sigphi)
        set(handles.sigphi_edit,'String',num2str(handles.currentMeasurement.sigphi));
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
        set(handles.theta_edit,'String',num2str(handles.currentMeasurement.theta));
    else
        set(handles.theta_edit, 'String', '');
    end
    if isreal(handles.currentMeasurement.sigtheta)
        set(handles.sigtheta_edit,'String',num2str(handles.currentMeasurement.sigtheta));
    else
        set(handles.sigtheta_edit, 'String', '');
    end
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

function questionMeasurementSave(handles, eventdata)
%TODO

function updateResultDropDown(handles)
selector = handles.resultObjectSelector;
objMap = handles.map.getAllObjects;
k = objMap.keys();
stringList{1} = 'Select Object';
for i = 1:int32(objMap.Count)
    cObj = objMap(char(k(i)));
    stringList{i} = cObj.name;
end
set(selector, 'String', stringList);
