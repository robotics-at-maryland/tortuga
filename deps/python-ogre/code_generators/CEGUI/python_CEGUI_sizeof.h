sizeof ( ::std::pair<float, float> );
sizeof ( ::CEGUI::Event::Connection ) ;
sizeof ( ::CEGUI::Event::Subscriber) ;

sizeof ( ::CEGUI::FontManager::FontIterator);
sizeof ( ::CEGUI::Imageset::ImageIterator);	
sizeof ( ::CEGUI::ImagesetManager::ImagesetIterator);
sizeof ( ::CEGUI::PropertySet::Iterator);
sizeof ( ::CEGUI::SchemeManager::SchemeIterator);
sizeof ( ::CEGUI::WindowFactoryManager::WindowFactoryIterator);
sizeof ( ::CEGUI::WindowFactoryManager::TypeAliasIterator);
sizeof ( ::CEGUI::WindowFactoryManager::FalagardMappingIterator);
sizeof ( ::CEGUI::WindowManager::WindowIterator);
sizeof ( ::CEGUI::EventSet::Iterator );
sizeof ( ::CEGUI::String::iterator );

sizeof ( ::std::pair<float, float> );

// seems that doing an sizeof ( ::CEGUI::EventSet::Iterator ); doesn't expose the class :(
sizeof(  CEGUI::ConstBaseIterator<std::map<CEGUI::String, CEGUI::Imageset*, CEGUI::String::FastLessCompare, std::allocator<std::pair<const CEGUI::String, CEGUI::Imageset*> > > > );
sizeof(  CEGUI::ConstBaseIterator<std::map<CEGUI::String, CEGUI::Property*, CEGUI::String::FastLessCompare, std::allocator<std::pair<const CEGUI::String, CEGUI::Property*> > > > );
sizeof(  CEGUI::ConstBaseIterator<std::map<CEGUI::String, CEGUI::Scheme*, CEGUI::String::FastLessCompare, std::allocator<std::pair<const CEGUI::String, CEGUI::Scheme*> > > > );
sizeof(  CEGUI::ConstBaseIterator<std::map<CEGUI::String, CEGUI::Window*, CEGUI::String::FastLessCompare, std::allocator<std::pair<const CEGUI::String, CEGUI::Window*> > > > );
sizeof(  CEGUI::ConstBaseIterator<std::map<CEGUI::String, CEGUI::WindowFactoryManager::FalagardWindowMapping, CEGUI::String::FastLessCompare, std::allocator<std::pair<const CEGUI::String, CEGUI::WindowFactoryManager::FalagardWindowMapping> > > > );
sizeof(  CEGUI::ConstBaseIterator<std::map<CEGUI::String, CEGUI::Event*, CEGUI::String::FastLessCompare, std::allocator<std::pair<const CEGUI::String, CEGUI::Event*> > > > );
sizeof(  CEGUI::ConstBaseIterator<std::map<CEGUI::String, CEGUI::WindowFactoryManager::AliasTargetStack, CEGUI::String::FastLessCompare, std::allocator<std::pair<const CEGUI::String, CEGUI::WindowFactoryManager::AliasTargetStack> > > > );
sizeof(  CEGUI::ConstBaseIterator<std::map<CEGUI::String, CEGUI::WindowFactory*, CEGUI::String::FastLessCompare, std::allocator<std::pair<const CEGUI::String, CEGUI::WindowFactory*> > > > );
sizeof(  CEGUI::ConstBaseIterator<std::map<CEGUI::String, CEGUI::Font*, CEGUI::String::FastLessCompare, std::allocator<std::pair<const CEGUI::String, CEGUI::Font*> > > > );
sizeof(  CEGUI::ConstBaseIterator<std::map<CEGUI::String, CEGUI::Image, CEGUI::String::FastLessCompare, std::allocator<std::pair<const CEGUI::String, CEGUI::Image> > > > );
