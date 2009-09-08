# **********************************************************************
#
# Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
#
# This copy of Ice is licensed to you under the terms described in the
# ICE_LICENSE file included in this distribution.
#
# **********************************************************************

# Ice version 3.3.0
# Generated from file `RemoteAI.ice'

import Ice, IcePy, __builtin__

# Start of module RemoteAi
_M_RemoteAi = Ice.openModule('ice.RemoteAi')
__name__ = 'RemoteAi'

if not _M_RemoteAi.__dict__.has_key('StateEvent'):
    _M_RemoteAi.StateEvent = Ice.createTempClass()
    class StateEvent(object):
        def __init__(self, timeStamp=0, type='', name='', eventType=''):
            self.timeStamp = timeStamp
            self.type = type
            self.name = name
            self.eventType = eventType

        def __hash__(self):
            _h = 0
            _h = 5 * _h + __builtin__.hash(self.timeStamp)
            _h = 5 * _h + __builtin__.hash(self.type)
            _h = 5 * _h + __builtin__.hash(self.name)
            _h = 5 * _h + __builtin__.hash(self.eventType)
            return _h % 0x7fffffff

        def __cmp__(self, other):
            if other == None:
                return 1
            if self.timeStamp < other.timeStamp:
                return -1
            elif self.timeStamp > other.timeStamp:
                return 1
            if self.type < other.type:
                return -1
            elif self.type > other.type:
                return 1
            if self.name < other.name:
                return -1
            elif self.name > other.name:
                return 1
            if self.eventType < other.eventType:
                return -1
            elif self.eventType > other.eventType:
                return 1
            return 0

        def __str__(self):
            return IcePy.stringify(self, _M_RemoteAi._t_StateEvent)

        __repr__ = __str__

    _M_RemoteAi._t_StateEvent = IcePy.defineStruct('::RemoteAi::StateEvent', StateEvent, (), (
        ('timeStamp', (), IcePy._t_long),
        ('type', (), IcePy._t_string),
        ('name', (), IcePy._t_string),
        ('eventType', (), IcePy._t_string)
    ))

    _M_RemoteAi.StateEvent = StateEvent
    del StateEvent

if not _M_RemoteAi.__dict__.has_key('_t_StateList'):
    _M_RemoteAi._t_StateList = IcePy.defineSequence('::RemoteAi::StateList', (), _M_RemoteAi._t_StateEvent)

if not _M_RemoteAi.__dict__.has_key('AiInformation'):
    _M_RemoteAi.AiInformation = Ice.createTempClass()
    class AiInformation(Ice.Object):
        def __init__(self):
            if __builtin__.type(self) == _M_RemoteAi.AiInformation:
                raise RuntimeError('RemoteAi.AiInformation is an abstract class')

        def ice_ids(self, current=None):
            return ('::Ice::Object', '::RemoteAi::AiInformation')

        def ice_id(self, current=None):
            return '::RemoteAi::AiInformation'

        def ice_staticId():
            return '::RemoteAi::AiInformation'
        ice_staticId = staticmethod(ice_staticId)

        #
        # Operation signatures.
        #
        # def RecordEvent(self, event, current=None):
        # def GetInfo(self, seconds, current=None):

        def __str__(self):
            return IcePy.stringify(self, _M_RemoteAi._t_AiInformation)

        __repr__ = __str__

    _M_RemoteAi.AiInformationPrx = Ice.createTempClass()
    class AiInformationPrx(Ice.ObjectPrx):

        def RecordEvent(self, event, _ctx=None):
            return _M_RemoteAi.AiInformation._op_RecordEvent.invoke(self, ((event, ), _ctx))

        def GetInfo(self, seconds, _ctx=None):
            return _M_RemoteAi.AiInformation._op_GetInfo.invoke(self, ((seconds, ), _ctx))

        def checkedCast(proxy, facetOrCtx=None, _ctx=None):
            return _M_RemoteAi.AiInformationPrx.ice_checkedCast(proxy, '::RemoteAi::AiInformation', facetOrCtx, _ctx)
        checkedCast = staticmethod(checkedCast)

        def uncheckedCast(proxy, facet=None):
            return _M_RemoteAi.AiInformationPrx.ice_uncheckedCast(proxy, facet)
        uncheckedCast = staticmethod(uncheckedCast)

    _M_RemoteAi._t_AiInformationPrx = IcePy.defineProxy('::RemoteAi::AiInformation', AiInformationPrx)

    _M_RemoteAi._t_AiInformation = IcePy.defineClass('::RemoteAi::AiInformation', AiInformation, (), True, None, (), ())
    AiInformation.ice_type = _M_RemoteAi._t_AiInformation

    AiInformation._op_RecordEvent = IcePy.Operation('RecordEvent', Ice.OperationMode.Normal, Ice.OperationMode.Normal, False, (), (((), _M_RemoteAi._t_StateEvent),), (), None, ())
    AiInformation._op_GetInfo = IcePy.Operation('GetInfo', Ice.OperationMode.Idempotent, Ice.OperationMode.Idempotent, False, (), (((), IcePy._t_long),), (), _M_RemoteAi._t_StateList, ())

    _M_RemoteAi.AiInformation = AiInformation
    del AiInformation

    _M_RemoteAi.AiInformationPrx = AiInformationPrx
    del AiInformationPrx

# End of module RemoteAi
