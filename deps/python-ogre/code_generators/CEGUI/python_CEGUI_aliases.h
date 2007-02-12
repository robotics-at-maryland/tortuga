typedef ::CEGUI::Event::Connection Connection;
typedef ::CEGUI::Event::Subscriber Subscriber;
typedef ::CEGUI::EventSet::Iterator EventIterator;

typedef ::CEGUI::FontManager::FontIterator FontIterator ;
typedef ::CEGUI::Imageset::ImageIterator ImageIterator;	
typedef ::CEGUI::ImagesetManager::ImagesetIterator ImagesetIterator;
typedef ::CEGUI::PropertySet::Iterator PropertyIterator;
typedef ::CEGUI::SchemeManager::SchemeIterator SchemeIterator;
typedef ::CEGUI::WindowFactoryManager::WindowFactoryIterator WindowFactoryIterator;
typedef ::CEGUI::WindowFactoryManager::TypeAliasIterator TypeAliasIterator;
typedef ::CEGUI::WindowFactoryManager::FalagardMappingIterator FalagardMappingIterator;
typedef ::CEGUI::WindowManager::WindowIterator WindowIterator;

typedef std::pair<float, float> StdPairFloatFloat;
typedef std::map<std::string, std::string> MapStringString;
typedef std::vector<std::string> VectorString;
   
typedef CEGUI::Singleton<CEGUI::Logger> SingletonLogger;
typedef CEGUI::Singleton<CEGUI::FontManager> SingletonFontManager;
typedef CEGUI::Singleton<CEGUI::GlobalEventSet> SingletonGlobalEventSet;
typedef CEGUI::Singleton<CEGUI::ImagesetManager> SingletonImagesetManager;
typedef CEGUI::Singleton<CEGUI::MouseCursor> SingletonMouseCursor;
typedef CEGUI::Singleton<CEGUI::SchemeManager> SingletonSchemeManager;
typedef CEGUI::Singleton<CEGUI::System> SingletonSystem;
typedef CEGUI::Singleton<CEGUI::WidgetLookManager> SingletonWidgetLookManager;
typedef CEGUI::Singleton<CEGUI::WindowFactoryManager> SingletonWindowFactoryManager;
typedef CEGUI::Singleton<CEGUI::WindowManager> SingletonWindowManager;
typedef CEGUI::Singleton<CEGUI::WindowRendererManager> SingletonWindowRendererManager;
typedef CEGUI::RefCounted<CEGUI::BoundSlot> RefCountedBoundSlot;