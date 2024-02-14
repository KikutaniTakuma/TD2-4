#include <Windows.h>
#include <memory>
#include "World/World.h"

int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {
	std::unique_ptr<Framework> game = std::make_unique<World>();

	game->Execution();

	return 0;
}