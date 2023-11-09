#include "World01.h"
#include "World02.h"
#include "World03.h"
#include "World04.h"
#include "World05.h"
#include "World06.h"
#include "Core/Core.h"
#include "Framework/Framework.h"
#include "Renderer/Renderer.h"
#include <string>

using namespace std;

int main(int argc, char* argv[])
{
	INFO_LOG("Initialize Engine...")

	//nc::MemoryTracker::Initialize();
	nc::seedRandom((unsigned int)time(nullptr));
	nc::setFilePath("assets");

	ENGINE.Initialize();

	auto world = make_unique<nc::World06>();
	world->Initialize();

	// main loop
	bool quit = false;
	while (!quit)
	{
		// update
		ENGINE.Update();
		quit = ENGINE.IsQuit();

		world->Update(ENGINE.GetTime().GetDeltaTime());

		// draw
		world->Draw(*ENGINE.GetSystem<nc::Renderer>());
	}

	world->Shutdown();
	ENGINE.Shutdown();

	/*string string1 = "Hello, welcome";
	cout << string1 << endl;
	string string2 = nc::StringUtils::ToUpper(string1);
	cout << "ToUpper: " << string2 << endl;

	string string3 = "Hello, WELCOME";
	cout << string3 << endl;
	string string4 = nc::StringUtils::ToLower(string3);
	cout << "ToLower: " << string4 << endl;

	cout << string1 << " & " << string3 << endl;
	cout << nc::StringUtils::IsEqualIgnoreCase(string1, string3) << endl;

	cout << nc::StringUtils::CreateUnique(string1) << endl;
	cout << nc::StringUtils::CreateUnique(string1) << endl;*/

	return 0;
}