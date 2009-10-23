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
namebox = handles.objectName_edit;
name = get(namebox, 'String');
if(~(isempty(name) || isa(handles.map.getObject(name),'Object')))
    handles.map.addObject(name);
    set(namebox, 'String', '');
    updateObjectDropDown(handles);
end
stringList = get(handles.editObjectSelector,'String');
n = size(stringList);
for i = 1:n(1)
    if(strcmp(stringList{i},name))
        set(handles.editObjectSelector, 'Value', i)
    end
end
handles.currentObject = handles.map.getObject(name);
updateMeasurementDropDown(handles);
guidata(hObject, handles);
% hObject    handle to createObject (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)



function objectName_edit_Callback(hObject, eventdata, handles)
% hObject    handle to objectName_edit (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of objectName_edit as text
%        str2double(get(hObject,'String')) returns contents of objectName_edit as a double


% --- Executes during object creation, after setting all properties.
function objectName_edit_CreateFcn(hObject, eventdata, handles)
% hObject    handle to objectName_edit (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end



function x_edit_Callback(hObject, eventdata, handles)
% hObject    handle to x_edit (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of x_edit as text
%        str2double(get(hObject,'String')) returns contents of x_edit as a double


% --- Executes during object creation, after setting all properties.
function x_edit_CreateFcn(hObject, eventdata, handles)
% hObject    handle to x_edit (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end


% --- Executes on button press in createMeasurement.
function createMeasurement_Callback(hObject, eventdata, handles)
m = Measurement();
m.name = getTime();
handles.currentObject.addMeasurement(m);
updateMeasurementDropDown(handles);
% hObject    handle to createMeasurement (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)


% --- Executes on button press in removeObject.
function removeObject_Callback(hObject, eventdata, handles)

% hObject    handle to removeObject (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)



function y_edit_Callback(hObject, eventdata, handles)
% hObject    handle to y_edit (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of y_edit as text
%        str2double(get(hObject,'String')) returns contents of y_edit as a double


% --- Executes during object creation, after setting all properties.
function y_edit_CreateFcn(hObject, eventdata, handles)
% hObject    handle to y_edit (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end



function phi_edit_Callback(hObject, eventdata, handles)
% hObject    handle to phi_edit (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of phi_edit as text
%        str2double(get(hObject,'String')) returns contents of phi_edit as a double


% --- Executes during object creation, after setting all properties.
function phi_edit_CreateFcn(hObject, eventdata, handles)
% hObject    handle to phi_edit (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end



function h_edit_Callback(hObject, eventdata, handles)
% hObject    handle to h_edit (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of h_edit as text
%        str2double(get(hObject,'String')) returns contents of h_edit as a double


% --- Executes during object creation, after setting all properties.
function h_edit_CreateFcn(hObject, eventdata, handles)
% hObject    handle to h_edit (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end



function D_edit_Callback(hObject, eventdata, handles)
% hObject    handle to D_edit (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of D_edit as text
%        str2double(get(hObject,'String')) returns contents of D_edit as a double


% --- Executes during object creation, after setting all properties.
function D_edit_CreateFcn(hObject, eventdata, handles)
% hObject    handle to D_edit (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end



function theta_edit_Callback(hObject, eventdata, handles)
% hObject    handle to theta_edit (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of theta_edit as text
%        str2double(get(hObject,'String')) returns contents of theta_edit as a double


% --- Executes during object creation, after setting all properties.
function theta_edit_CreateFcn(hObject, eventdata, handles)
% hObject    handle to theta_edit (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end



function sigx_edit_Callback(hObject, eventdata, handles)
% hObject    handle to sigx_edit (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of sigx_edit as text
%        str2double(get(hObject,'String')) returns contents of sigx_edit as a double


% --- Executes during object creation, after setting all properties.
function sigx_edit_CreateFcn(hObject, eventdata, handles)
% hObject    handle to sigx_edit (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end



function sigy_edit_Callback(hObject, eventdata, handles)
% hObject    handle to sigy_edit (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of sigy_edit as text
%        str2double(get(hObject,'String')) returns contents of sigy_edit as a double


% --- Executes during object creation, after setting all properties.
function sigy_edit_CreateFcn(hObject, eventdata, handles)
% hObject    handle to sigy_edit (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end



function sigphi_edit_Callback(hObject, eventdata, handles)
% hObject    handle to sigphi_edit (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of sigphi_edit as text
%        str2double(get(hObject,'String')) returns contents of sigphi_edit as a double


% --- Executes during object creation, after setting all properties.
function sigphi_edit_CreateFcn(hObject, eventdata, handles)
% hObject    handle to sigphi_edit (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end



function sigh_edit_Callback(hObject, eventdata, handles)
% hObject    handle to sigh_edit (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of sigh_edit as text
%        str2double(get(hObject,'String')) returns contents of sigh_edit as a double


% --- Executes during object creation, after setting all properties.
function sigh_edit_CreateFcn(hObject, eventdata, handles)
% hObject    handle to sigh_edit (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end



function sigD_edit_Callback(hObject, eventdata, handles)
% hObject    handle to sigD_edit (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of sigD_edit as text
%        str2double(get(hObject,'String')) returns contents of sigD_edit as a double


% --- Executes during object creation, after setting all properties.
function sigD_edit_CreateFcn(hObject, eventdata, handles)
% hObject    handle to sigD_edit (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end



function sigtheta_edit_Callback(hObject, eventdata, handles)
% hObject    handle to sigtheta_edit (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of sigtheta_edit as text
%        str2double(get(hObject,'String')) returns contents of sigtheta_edit as a double


% --- Executes during object creation, after setting all properties.
function sigtheta_edit_CreateFcn(hObject, eventdata, handles)
% hObject    handle to sigtheta_edit (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end


% --- Executes on button press in mactive.
function mactive_Callback(hObject, eventdata, handles)
% hObject    handle to mactive (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hint: get(hObject,'Value') returns toggle state of mactive


% --- Executes on button press in changeOrigin.
function changeOrigin_Callback(hObject, eventdata, handles)
% hObject    handle to changeOrigin (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)


% --- Executes on selection change in resultObjectSelector.
function resultObjectSelector_Callback(hObject, eventdata, handles)
% hObject    handle to resultObjectSelector (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: contents = get(hObject,'String') returns resultObjectSelector contents as cell array
%        contents{get(hObject,'Value')} returns selected item from resultObjectSelector


% --- Executes during object creation, after setting all properties.
function resultObjectSelector_CreateFcn(hObject, eventdata, handles)
% hObject    handle to resultObjectSelector (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: popupmenu controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end


% --- Executes on selection change in editMesurementSelector.
function editMeasurementSelector_Callback(hObject, eventdata, handles)
mNameList = get(hObject,'String');
currName = mNameList{get(hObject,'Value')};
if(~strcmp(currName,'Select Measurement'))
    handles.currentMeasurement = handles.currentObject.getMeasurementByName(currName);
end
guidata(hObject,handles);
% hObject    handle to editMesurementSelector (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: contents = get(hObject,'String') returns editMesurementSelector contents as cell array
%        contents{get(hObject,'Value')} returns selected item from editMesurementSelector


% --- Executes during object creation, after setting all properties.
function editMeasurementSelector_CreateFcn(hObject, eventdata, handles)
% hObject    handle to editMesurementSelector (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: popupmenu controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
    set(hObject,'String','Select Measurement');
end


% --- Executes on selection change in editObjectSelector.
function editObjectSelector_Callback(hObject, eventdata, handles)
objNameList = get(hObject,'String');
currName = objNameList{get(hObject,'Value')};
handles.currentObject = handles.map.getObject(currName);
updateMeasurementDropDown(handles);
guidata(hObject, handles);
% hObject    handle to editObjectSelector (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: contents = get(hObject,'String') returns editObjectSelector contents as cell array
%        contents{get(hObject,'Value')} returns selected item from editObjectSelector


% --- Executes during object creation, after setting all properties.
function editObjectSelector_CreateFcn(hObject, eventdata, handles)
% hObject    handle to editObjectSelector (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: popupmenu controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end


% --- Executes on button press in mesaurementSave.
function mesaurementSave_Callback(hObject, eventdata, handles)
% hObject    handle to mesaurementSave (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)


% --- Executes on button press in revert.
function revert_Callback(hObject, eventdata, handles)
% hObject    handle to revert (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

function updateObjectDropDown(handles)
objectDropDown = handles.editObjectSelector;
objMap = handles.map.getAllObjects();
k = objMap.keys();
n = 1;
for i = 1:int32(objMap.Count)
    currObj = objMap(char(k(i)));
    if(~strcmp(currObj.name, 'Origin'))
        stringList{n} = currObj.name;
        n = n+1;
    end
end
set(objectDropDown,'String',stringList);

function updateMeasurementDropDown(handles)
mDropDown = handles.editMeasurementSelector;
mMap = handles.currentObject.getAllMeasurements();
k = mMap.keys();
stringList{1} = 'Select Measurement';
for i = 1:int32(mMap.Count)
    currMes = mMap(char(k(i)));
    stringList{i} = currMes.name;
end
set(mDropDown,'String',stringList);
