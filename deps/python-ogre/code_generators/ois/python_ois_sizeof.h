OIS::ParamList t;
sizeof ( t );
sizeof (ParamList);
sizeof ( Mouse * );
sizeof ( Keyboard *  );
sizeof ( JoyStick * );
sizeof ( Mouse  );
sizeof ( Keyboard );
sizeof ( JoyStick );

typedef std::map<std::string, std::string> ParamList1;
sizeof ( ParamList1);

// Named Vale Pair list
sizeof ( std::map<std::string, std::string, std::less<std::string>, std::allocator<std::pair<std::string const, std::string> > > );

// force paramlist
sizeof ( std::map<std::string, std::string>);
sizeof ( std::multimap<std::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::basic_string<char,std::char_traits<char>, std::allocator<char> >, std::less<std::basic_string<char, std::char_traits<char>, std::allocator<char> > >, std::allocator< std::pair< std::basic_string<char, std::char_traits<char>, std::allocator<char> > const , std::basic_string<char, std::char_traits<char>, std::allocator<char> > > > >);
sizeof ( class std::multimap<class std::basic_string<char,struct std::char_traits<char>,class std::allocator<char> >,class std::basic_string<char,struct std::char_traits<char>,class std::allocator<char> >,struct std::less<class std::basic_string<char,struct std::char_traits<char>,class std::allocator<char> > >,class std::allocator<struct std::pair<class std::basic_string<char,struct std::char_traits<char>,class std::allocator<char> > const ,class std::basic_string<char,struct std::char_traits<char>,class std::allocator<char> > > > > );
sizeof ( std::multimap<std::string, std::string, std::less<std::string>, std::allocator<std::pair<std::string, std::string> > >);
