# Copyright 2004 Roman Yakovenko.
# Distributed under the Boost Software License, Version 1.0. (See
# accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

"""
This module is a collection of unrelated algorithms, that works on code creators
tree.
"""
import math
from pygccxml import declarations
from pyplusplus import code_creators
    

class missing_call_policies:

    @staticmethod
    def _selector( creator ):
        if not isinstance( creator, code_creators.declaration_based_t ):
            return False
        if not isinstance( creator.declaration, declarations.calldef_t ):
            return False
        if isinstance( creator.declaration, declarations.constructor_t ):
            return False
        return hasattr(creator, 'call_policies') and not creator.call_policies

    @staticmethod    
    def print_( extmodule ):
        creators = filter( missing_call_policies._selector
                           , code_creators.make_flatten_generator( extmodule.creators ) )
        for creator in creators:
            print creator.declaration.__class__.__name__, ': ', declarations.full_name( creator.declaration )
            print '  *** MISSING CALL POLICY', creator.declaration.function_type().decl_string
            print 

    @staticmethod    
    def exclude( extmodule ):
        creators = filter( missing_call_policies._selector
                           , code_creators.make_flatten_generator( extmodule.creators ) )
        for creator in creators:
            creator.parent.remove_creator( creator )
    

def split_sequence(seq, bucket_size):
    #split sequence to buclets, where every will contain maximum bucket_size items
    seq_len = len( seq )
    if seq_len <= bucket_size:
        return [ seq ] 
    buckets = []
    num_of_buckets = int( math.ceil( float( seq_len ) / bucket_size ) )
    for i in range(num_of_buckets):
        from_ = i * bucket_size
        to = min( ( i + 1) * bucket_size, seq_len )
        buckets.append( seq[ from_ : to ] )
    return buckets
        