function  LoadCppController()

if libisloaded('Control') == 0
    addpath('C:\Documents and Settings\Joseph Gland\My Documents\AUV competition\Controller Code\control\scritps')
    loadlibrary('Control',@interface)
end