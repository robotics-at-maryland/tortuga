# Copyright 2004-2008 Roman Yakovenko.
# Distributed under the Boost Software License, Version 1.0. (See
# accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)
#
# Authors:
#   Allen Bierbaum
#
import pygccxml.declarations as pd
from pyplusplus.module_builder.call_policies import *
import pyplusplus.module_builder
import pygccxml.declarations.type_traits as tt
import pygccxml.declarations.cpptypes as cpptypes
import pyplusplus.code_creators as code_creators
import pyplusplus.decl_wrappers as decl_wrappers
import copy
import re

# TODO:
# - extend add_member_function to include method to add

def set_recursive_default(val):
   pd.scopedef_t.RECURSIVE_DEFAULT = val

def set_allow_empty_mdecl_default(val):
   pd.scopedef_t.ALLOW_EMPTY_MDECL_WRAPPER = val

def decl_from_typedef(decl):
   """ decl: decl or mdecl with typedef.  Return the "real" decl. """
   typedef_decl = decl
   if isinstance(typedef_decl, pd.mdecl_wrapper_t):
      typedef_decl = decl[0]   
   assert isinstance(typedef_decl, pd.typedef_t)
   return typedef_decl.type.declaration

   
def finalize(cls, finalize_pure_virtuals=False):
   """ Attempt to finalize a class by not exposing virtual methods.
         Still exposes in the case of pure virtuals otherwise the class
         could not be instantiated.
   """
   if isinstance(cls, pd.mdecl_wrapper_t):
      for x in cls:
         finalize(x)
   else:
      matcher = pd.virtuality_type_matcher( pd.VIRTUALITY_TYPES.VIRTUAL )
      if finalize_pure_virtuals:
         matcher = matcher | pd.virtuality_type_matcher( pd.VIRTUALITY_TYPES.PURE_VIRTUAL)
      members = cls.decls( matcher,
                           decl_type=pd.member_calldef_t,
                           allow_empty=True)
      members.set_virtuality( pd.VIRTUALITY_TYPES.NOT_VIRTUAL ) 
      
      cls.decls(pd.access_type_matcher_t(pd.ACCESS_TYPES.PROTECTED),allow_empty=True).exclude()
      cls.decls(pd.access_type_matcher_t(pd.ACCESS_TYPES.PRIVATE),allow_empty=True).exclude()
      
      wrapper_needs = cls.is_wrapper_needed()
      if len(wrapper_needs):
         print "Finalize failed for: ", cls.name
         for x in wrapper_needs:
            print "   ", x


def add_member_function(cls, methodName, newMethod):
   """ Add a member function to the class. """
   cls.add_registration_code('def("%s",%s)'%(methodName, newMethod), True)

def wrap_method(cls, methodName, newMethod):
   """ Wrap a class method with a new method.
         ex: c.wrapmethod(c,"doSomething","doSomethingWrapper")
   """
   cls[methodName].exclude()
   add_member_function(cls, methodName, newMethod)

def add_method(moduleBuilder, methodName, method):
   """  Add a method to the module builder. """
   code_text = 'boost::python::def("%s",%s);'%(methodName, method)
   moduleBuilder.add_registration_code(code_text)   

def is_const_ref(type):
   """ Extra trait tester method to check if something is a const reference. """
   is_const = tt.is_const(type) or (hasattr(type,'base') and tt.is_const(type.base))
   is_ref = tt.is_reference(type) or (hasattr(type,'base') and tt.is_reference(type.base))
   return (is_ref and is_const)
   #[Roman]If you create unit tests for this code, I will add it to type traits module

def exclude_protected(cls):
   """ Exclude all protected declarations. """
   cls.decls(pd.access_type_matcher_t(pd.ACCESS_TYPES.PROTECTED),allow_empty=True).exclude()

def wrap_const_ref_params(cls):
   """ Find all member functions of cls and if they take a const& to a class
         that does not have a destructor, then create a thin wrapper for them.
         This works around an issue with boost.python where it needs a destructor.
   """
   #[Roman] Obviously, this will only work, if the function does not need other
   #wrapper, I think, this is a new use case for Matthias "arguments policies"
   #functionality.
   calldefs = cls.calldefs()

   if None == calldefs:
      return

   for c in calldefs:
      # Skip constructors
      if isinstance(c, pd.constructor_t):
         continue

      # Find arguments that need replacing
      args_to_replace = []   # List of indices to args to replace with wrapping
      args = c.arguments
      for i in range(len(args)):
         arg = args[i]
         if is_const_ref(arg.type):
            naked_arg = tt.remove_cv(tt.remove_reference(arg.type))
            if tt.is_class(naked_arg):
               class_type = naked_arg.declaration
               if not tt.has_public_destructor(class_type):
                  print "Found indestructible const& arg: [%s]:[%s] "%(str(c), str(arg))
                  args_to_replace.append(i)

      # Now replace arguments
      if len(args_to_replace):
         if isinstance(c, pd.operator_t) and c.symbol in ["<","==","!=","="]:
            c.exclude()
            continue

         new_args = copy.copy(args)   # Make new copy of args so we don't modify the existing method
         for i in args_to_replace:
            old_arg_type = args[i].type
            if tt.is_reference(old_arg_type) and tt.is_const(old_arg_type.base):
               new_args[i].type = cpptypes.reference_t(tt.remove_const(old_arg_type.base))
            elif tt.is_const(old_arg):
               new_args[i].type = tt.remove_const(old_arg_type)

         wrapper_name = "%s_const_ref_wrapper"%c.name
         args_str = [str(a) for a in new_args]
         arg_names_str = [str(a.name) for a in new_args]
         new_sig = "static %s %s(%s& self_arg, %s)"%(c.return_type,
                                                     wrapper_name,
                                                     pd.full_name(cls),
                                                     ",".join(args_str))
         new_method = """%s
         { return self_arg.%s(%s); }
         """%(new_sig,c.name,",".join(arg_names_str))

         # Add it all
         c.exclude()
         
         #[Roman] you can use cls.add_declaration_code, this could simplify the
         #wrapper you created, because it will generate the code within the source
         #file, the class is generated
         cls.add_declaration_code(new_method)
         
         cls.add_registration_code('def("%s", &%s)'%(c.name, wrapper_name), 
                                   works_on_instance=True)


class TemplateWrapper:
   """ Proxy for a template instance.  Returned from the TemplateBuilder
       to allow access to the template at a later time.
       
       NOTE: DO NOT USE DIRECTLY.  ONLY USE AS RETURNED FROM TEMPLATE BUILDER.
       
       TODO: If used a form that allowed multiple templates to be specified
             ex: TemplateWrapper("OSG::vector", arguments=[["float","3"],["int","4"]]
             then how would we handle naming?  Automatic or must be specified?
   """
   def __init__(self, templateType, finalName = None):   #, arguments=None):
      """
      templateType: Either a base type ("OSG::vector") or a full template
                    type ("OSG::vector<float>")
      finalName: Name to rename the decl to after finding it.
      """
      self.mTemplateType = templateType
      self.mFinalName = finalName
      if not finalName:
         self.mTypedefName = self.cleanTemplateName(self.mTemplateType)
      else:
         self.mTypedefName = "alias_%s"%self.mFinalName
      self.mDecl = None

   # Properties
   decl = property(lambda x: x.mDecl)
   
   def process(self, templateBuilder):
      """ Process the wrapper to make it ready for usage by user.
          templateBuilder - The module builder used to parse the template.
      """
      # Look up the decl from the alias db
      # XXX: I don't know if this is the best way to look up the decl, but it seems to work
      try:
         self.mDecl = templateBuilder.mAliasDB[self.mTypedefName].declaration.type.declaration
      except:
         # If that didn't work, just try to get it from the first declaration
         self.mDecl = templateBuilder.mAliasDB[self.mTypedefName].declaration
         
      # Another method
      # decl_name = templateBuilder.mAliasDB[self.mTypedefName].declaration.name
      # decl = ns.decl(decl_name)
      
      if self.mFinalName:
         self.mDecl.rename(self.mFinalName)
      
   
   def cleanTemplateName(self, templateName):
      """ Build a clean template name. """
      clean_re = re.compile('[:<>\s,]')
      return clean_re.sub('_', templateName)


class TemplateBuilder:
   """ Template builder helper class.
       This class is meant to simplify the use of templates with py++.
       
       Usage:
          
          tb = TemplateBuilder()
          vec3f_t = tb.Template("OSG::vector<float,3>")
          
          header_contents = tb.buildAutogenContents()
          # Add contents to some file that is included by module builder
          
          mb = moduble_builder_t([myheaders])
          tb.process(mb)
          
          vec3f = vec3f_t.decl
          vec3f.method("getSize").exclude()          
          
          mb.create()
   """
   
   def __init__(self):
      """ Initialize template builder. """
      # List of tuples: (templateTypename, typedefName)
      #       ex: ("MyClass<float>","myclass_float")
      self.mTemplates = []
   
   def Template(self, *args, **kw):
      """Create and add a template wrapper.
         Returns a template wrapper that can be used to get the decl later.
      """
      temp_wrapper = TemplateWrapper(*args, **kw)
      self.mTemplates.append(temp_wrapper)
      return temp_wrapper

   def processTemplates(self, mbuilder):
      self.mAliasDB = {}
      global_typedefs = mbuilder.global_ns.typedefs()
      for td in global_typedefs:
         self.mAliasDB[td.name] = td.type
      
      for t in self.mTemplates:
         t.process(self)
      
      
   def buildAutogenContents(self):
      """ Build up the contents of a file to instantiate the needed templates. """
      if len(self.mTemplates) == 0:
         return None
      
      content = "/** Autogenerated temporary file for template instantiation. */\n"
      for t in self.mTemplates:
         template_type = t.mTemplateType
         typedef_name = t.mTypedefName
         content += """
            typedef %(template_type)s %(typedef_name)s;
            inline unsigned __instantiate_%(typedef_name)s()
            { return unsigned(sizeof(%(typedef_name)s)); }
         """ % vars()      
      
      return content


