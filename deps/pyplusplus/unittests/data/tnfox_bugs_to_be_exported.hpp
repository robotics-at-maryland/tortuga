#include <string>
#include <iostream>

namespace FX {

struct FXWinShellLink
{
	struct Header
	{
		unsigned int length;
		char guid[16];
		union Flags
		{
			struct impl
			{
				unsigned int hasItemIdList : 1;
				unsigned int pointsToFileOrDir : 1;
				unsigned int hasDescription : 1;
				unsigned int hasRelativePath : 1;
				unsigned int hasWorkingDir : 1;
				unsigned int hasCmdLineArgs : 1;
				unsigned int hasCustomIcon : 1;
				unsigned int useWorkingDir : 1;		// Seems to need to be set to enable working dir
				unsigned int unused : 24;
			};
			unsigned int raw;
		} flags;
    };
};

} // namespace

