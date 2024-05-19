// ActiveEndpoint.cpp : このファイルには 'main' 関数が含まれています。プログラム実行の開始と終了がそこで行われます。
//

#include <iostream>
#include <Windows.h>
#include <stdio.h>
#include <ctype.h>
#include "EndpointRegistry.h"

#define _LD if(Er->debug)

//
//
//
class PassPrameters
{
public:
	using Keywords = EndpointRegistry::Keywords;

	PassPrameters();
	~PassPrameters();

	//
	// return how many arguments are passed
	//
	int AnalisysArguments(int argc, char* argv[], EndpointRegistry* Er)
	{
		int count = 2; // Default value

		// Store default values
		Er->role = Keywords::DEFAULT_ROLE;
		Er->flow = Keywords::DEFAULT_FLOW;

		for (int i = 1; i < argc; i++)
		{
			//
			// Convert the character(s) of arguments to upper case
			//
			int capitalKeyword = toupper(argv[i][0]);
			int secondKeyword = toupper(argv[i][1]);

			TRY:

			switch (capitalKeyword)
			{
				//
				// Our keywords to imprement Windows APO magic world are as below.
				// 
				// capture
				// render--default
				// multimedia--default
				// communication
				//

			case 'C':
				if (secondKeyword == 'O')
				{
					_LD std::cout << "communication" << std::endl;
					Er->role = Keywords::Communication;
					count++;
				}
				else if (secondKeyword == 'A')
				{
					_LD std::cout << "capture" << std::endl;
					Er->flow = Keywords::Capture;
					count++;
				}
				else
				{
					_LD std::cout << "Unknown" << std::endl;
					Er->flow = Er->role = Keywords::Unknown;
					count = 0;
				}
				break;

			case 'R':
				_LD std::cout << "render" << std::endl;
				Er->flow = Keywords::Render;
				break;

			case 'M':
				_LD std::cout << "multimedia" << std::endl;
				Er->role = Keywords::Multimedia;
				break;

			case 'D':
				_LD std::cout << "debug" << std::endl;
				Er->debug++;
				break;

			case '-':
				_LD std::cout << "-" << std::endl;
				capitalKeyword = secondKeyword;
				goto TRY;
				break;

			default:
				_LD std::cout << "Unknown" << std::endl;
				count = 0;
				break;
			}

			// Debug
			_LD std::cout << "argv[" << i << "] = " << argv[i] << std::endl;
		}

		return count;
	}

private:

};

PassPrameters::PassPrameters()
{
}

PassPrameters::~PassPrameters()
{
}

//
//
//
void Usage()
{
	printf("ActiveEndpoint [capture|render|multimedia|communication]\n");
}

int main(int argc, char* argv[])
{
    WCHAR DeviceGuid[MAX_PATH];
	INT ParamCount = 0;

    //std::cout << "Hello Active Endpoint!\n";

	EndpointRegistry* Er = new EndpointRegistry();

	PassPrameters* Pp = new PassPrameters();
	ParamCount = Pp->AnalisysArguments(argc, argv, Er);
	if (ParamCount == 0)
	{
		Usage();
		::exit(1);
	}
	
	DeviceGuid[0] = 0;
    Er->ActiveEndpoint(DeviceGuid);

    printf("Active Endpoint for (%s, %s): %ws\n",
		static_cast<int>(Er->flow) == 0 ? "Capture" : "Render",
		static_cast<int>(Er->role) == 2 ? "Multimedia" : "Communication",
		DeviceGuid);

    return 0;
}
