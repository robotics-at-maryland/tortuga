def aliases( Version ):
    return {
    "allocator<char>"
    : "AllocatorChar"
     , "_Facetptr<std::ctype<char> >"
    : "FacetptrChar"
     , "_Facetptr<std::ctype<wchar_t> >"
    : "FacetptrWChar"
     , "_Facetptr<std::ctype<unsigned short> >"
    : "FacetptrUnsignedShort"
    , "_Ranit<char, int, char const*, char const&>"
    : "RanitEtc"
    ,"iterator<std::random_access_iterator_tag, char, int, char const*, char const&>"
    : "iteratorRamdom"
    , "iterator<std::output_iterator_tag, void, void, void, void>"
    : "iteratorOutput"
    , "codecvt<wchar_t, char, int>"
    : "codecvtWchar"
    , "iterator_traits<unsigned long long>"
    : "iteratortraitsUnsignedLongLong"
    , "iterator_traits<long long>"
    : "iteratortraitsLongLong"
    , "iterator_traits<unsigned long>"
    : "iteratortraitsUnsignedLong"
    , "iterator_traits<long>"
    : "iteratortraitsLong"
    , "iterator_traits<unsigned>"
    : "iteratortraitsUnsigned"
    , "iterator_traits<int>"
    : "iteratortraitsInt"
    , "iterator_traits<unsigned short>"
    : "iteratortraitsUnsignedShort"
    , "iterator_traits<short>"
    : "iteratortraitsShort"
    , "iterator_traits<unsigned char>"
    : "iteratortraitsUnsignedChar"
    , "iterator_traits<signed char>"
    : "iteratortraitsSignedChar"
    , "iterator_traits<char>"
    : "iteratortraitsChar"
    , "iterator_traits<bool>"
    : "iteratortraitsBool"
     }

def header_files( Version ):
    return [ 'ode/ode.h',
             'ode/odecpp.h',
             'ode/odecpp_collision.h',
             'return_pointee_value.hpp'
             ]

def huge_classes( Version ):
    return []

